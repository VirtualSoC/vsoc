/**
 * @file egl_trans.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief
 * @version 0.1
 * @date 2020-11-25
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "hw/express-gpu/vk_trans.h"
#include "hw/express-gpu/express_gpu.h"

#include "hw/express-mem/express_sync.h"
#include "hw/express-gpu/express_vk_decode_from_stream.h"
#include "hw/express-gpu/express_vk_handle_mapping.h"
#include "hw/express-gpu/vk_helper.h"
#include "hw/express-gpu/vulkan_surface.h"

#ifdef __WIN32__

#include <vulkan/vulkan_win32.h>

PFN_vkGetMemoryWin32HandleKHR pfn_vkGetMemoryWin32HandleKHR = NULL;

static bool g_is_intel_gpu = false;

void init_interop_once(VkDevice device) {
    static bool initialized = false;
    if (initialized) return;
    
    pfn_vkGetMemoryWin32HandleKHR = (PFN_vkGetMemoryWin32HandleKHR)
        vkGetDeviceProcAddr(device, "vkGetMemoryWin32HandleKHR");
    
    if (pfn_vkGetMemoryWin32HandleKHR) {
        LOGD("[Interop] vkGetMemoryWin32HandleKHR loaded successfully");
    } else {
        LOGW("[Interop] vkGetMemoryWin32HandleKHR not available, fallback to CPU copy");
    }
    
    initialized = true;
}
#endif

static __thread void *g_gl_context = NULL;

VkResult destroy_vulkan_object_other(
    uint64_t guest_dev,
    uint64_t guest_obj,
    VkObjectType obj_type1,
    VkObjectType obj_type2,
    void (*destroy_func)(void*, void*, const VkAllocationCallbacks*),
    const VkAllocationCallbacks* pAllocator) {
    
    void* realDev = (void*)(uintptr_t)lookup_mapping(obj_type1, guest_dev);
    void* realObj = (void*)(uintptr_t)lookup_mapping(obj_type2, guest_obj);
    if (realDev == (void*)(uintptr_t)UINT64_MAX || realObj == (void*)(uintptr_t)UINT64_MAX) {
        LOGE("Failed to destroy Vulkan object");
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    destroy_func(realDev, realObj, pAllocator);
    if (!remove_mapping(obj_type2, guest_obj)) {
        LOGE("Failed to remove mapping for Vulkan object %llu", (unsigned long long)guest_obj);
    } else {
        LOGD("Successfully removed mapping for Vulkan object %llu", (unsigned long long)guest_obj);
    }
    return VK_SUCCESS;
}
VkResult destroy_vulkan_object_essential(
    uint64_t guest_dev,
    VkObjectType obj_type,
    void (*destroy_func)(void*, const VkAllocationCallbacks*),
    const VkAllocationCallbacks* pAllocator) {
    
    void* realDev = (void*)(uintptr_t)lookup_mapping(obj_type, guest_dev);
    if (realDev == (void*)(uintptr_t)UINT64_MAX) {
    LOGE("Failed to destroy Vulkan object, invalid mapping for guest_dev %llu",
        (unsigned long long)guest_dev);
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    destroy_func(realDev, pAllocator);
    if (!remove_mapping(obj_type, guest_dev)) {
        LOGE("Failed to remove mapping for Vulkan object %llu", (unsigned long long)guest_dev);
    } else {
        LOGD("Successfully removed mapping for Vulkan object %llu", (unsigned long long)guest_dev);
    }
    return VK_SUCCESS;
}

VkResult destroy_vulkan_object_device(
    uint64_t guest_dev,
    uint64_t guest_obj,
    VkObjectType obj_type,
    void (*destroy_func)(VkDevice, void*, const VkAllocationCallbacks*),
    const VkAllocationCallbacks* pAllocator) {
    
    VkDevice realDev = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_dev);
    void* realObj = (void*)(uintptr_t)lookup_mapping(obj_type, guest_obj);
    if (realDev == (VkDevice)(uintptr_t)UINT64_MAX || realObj == (void*)(uintptr_t)UINT64_MAX) {
        LOGE("Failed to destroy Vulkan object, invalid mapping for guest_dev");
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    destroy_func(realDev, realObj, pAllocator);
    if (!remove_mapping(obj_type, guest_obj)) {
        LOGE("Failed to remove mapping for Vulkan object %llu", (unsigned long long)guest_obj);
    }
    return VK_SUCCESS;
}

void checkHostVisible(VkPhysicalDevice physicalDevice, uint32_t memoryTypeIndex) {
    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProps);

    if (memoryTypeIndex >= memProps.memoryTypeCount) {
        LOGD("Invalid memoryTypeIndex %u (max %u)",
               memoryTypeIndex, memProps.memoryTypeCount - 1);
        return;
    }

    VkMemoryPropertyFlags flags =
        memProps.memoryTypes[memoryTypeIndex].propertyFlags;

    LOGD("MemoryType %u flags: 0x%08x",
           memoryTypeIndex, flags);

    if (flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
        LOGD("  -> HOST_VISIBLE is PRESENT");
    } else {
        LOGD("  -> HOST_VISIBLE is NOT present");
    }

    if (flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
        LOGD("  -> HOST_COHERENT is PRESENT");
    } else {
        LOGD("  -> HOST_COHERENT is NOT present");
    }
}

void transitionImageLayoutForSampling(VkDevice device, VkImage image, VkImageLayout format) {
    // 创建临时命令缓冲区
    VkCommandPool commandPool = getOrCreateCommandPool(device); // 你需要实现

    LOGD("going to update image layout for sampling image %llx", (unsigned long long)image);
    
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;
    
    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
    
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    
    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    
    // 创建图像内存屏障
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED; // 或者你当前的布局
    barrier.newLayout = format;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    
    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;
    
    // 从UNDEFINED转换
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    
    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, NULL,
        0, NULL,
        1, &barrier
    );
    
    vkEndCommandBuffer(commandBuffer);
    
    // 提交命令缓冲区
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    
    VkQueue graphicsQueue = getGraphicsQueue(device); // 你需要实现
    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);
    
    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    
    LOGD("Host: Image layout transition completed");
}

static VkExtensionProperties* g_cached_instance_extensions = NULL;
static uint32_t g_cached_instance_extension_count = 0;
static int g_instance_extensions_cached = 0;

// 辅助函数：检查扩展是否被主机支持
static int is_extension_supported(const char* ext_name) {
    for (uint32_t i = 0; i < g_cached_instance_extension_count; i++) {
        if (strcmp(g_cached_instance_extensions[i].extensionName, ext_name) == 0) {
            return 1;
        }
    }
    return 0;
}

// 辅助函数：确保扩展缓存已加载
static VkResult ensure_instance_extensions_cached() {
    if (g_instance_extensions_cached) {
        return VK_SUCCESS;
    }
    
    VkResult result = vkEnumerateInstanceExtensionProperties(NULL, &g_cached_instance_extension_count, NULL);
    if (result != VK_SUCCESS) {
        LOGE("Failed to query instance extension count: %d", result);
        return result;
    }
    
    g_cached_instance_extensions = (VkExtensionProperties*)malloc(g_cached_instance_extension_count * sizeof(VkExtensionProperties));
    if (!g_cached_instance_extensions) {
        LOGE("Failed to allocate memory for extension cache");
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    result = vkEnumerateInstanceExtensionProperties(NULL, &g_cached_instance_extension_count, g_cached_instance_extensions);
    if (result != VK_SUCCESS) {
        LOGE("Failed to enumerate instance extensions: %d", result);
        free(g_cached_instance_extensions);
        g_cached_instance_extensions = NULL;
        g_cached_instance_extension_count = 0;
        return result;
    }
    
    g_instance_extensions_cached = 1;
    LOGD("Cached %u instance extensions", g_cached_instance_extension_count);
    return VK_SUCCESS;
}

void vk_decode_invoke(Render_Thread_Context *context, Teleport_Express_Call *call)

{
    Render_Thread_Context *egl_context = (Render_Thread_Context *)context;

    if (unlikely(egl_context == NULL))
    {
        call->callback(call, 0);
        return;
    }

    Call_Para all_para[MAX_PARA_NUM];

    unsigned char ret_local_buf[1024 * 4];

    unsigned char *no_ptr_buf = NULL;
    
    uint64_t fun_id = GET_FUN_ID(call->id);
    LOGD("get vk call with id %lld", fun_id);

    switch (fun_id)
    {

    case FUNID_vkCreateInstance:
    {
        LOGD("get call FUNID_vkCreateInstance!");

        const VkInstanceCreateInfo* pCreateInfo = malloc(sizeof(VkInstanceCreateInfo));
        const VkAllocationCallbacks* pAllocator = NULL;
        VkInstance pInstance;
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("get vk param number %d instance is %lld %lld", para_num, pInstance, &pInstance);

        int need_free = 0;
        char *stream_ptr;
        stream_ptr = call_para_to_ptr(all_para[0], &need_free);
        uint8_t ** stream_ptr_ptr = (uint8_t **)&stream_ptr;

        decode_from_stream_VkInstanceCreateInfo(VK_STRUCTURE_TYPE_MAX_ENUM, (VkInstanceCreateInfo*)(pCreateInfo), stream_ptr_ptr); 

        LOGD("got vkCreateinfo with %lld %d %s %d %s",(long long)pCreateInfo->sType, pCreateInfo->enabledLayerCount, pCreateInfo->ppEnabledLayerNames, pCreateInfo->enabledExtensionCount, pCreateInfo->ppEnabledExtensionNames);
        LOGD("appcation name is %s", pCreateInfo->pApplicationInfo->pApplicationName);
        LOGD("application create info is %lld %d",(long long)pCreateInfo->pApplicationInfo, pCreateInfo->pApplicationInfo->sType);

#ifdef __APPLE__
        ((VkInstanceCreateInfo*)pCreateInfo)->flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
        VkAllocationCallbacks* guest_allocator = (VkAllocationCallbacks*)(**stream_ptr_ptr);
        *stream_ptr_ptr += 8;

        if(guest_allocator) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, pAllocator, stream_ptr_ptr);
        }

        uint64_t guest_instance = *(uint64_t*)(*stream_ptr_ptr);
        *stream_ptr_ptr += sizeof(uint64_t);

        // 确保扩展缓存已加载
        VkResult cache_result = ensure_instance_extensions_cached();
        if (cache_result != VK_SUCCESS) {
            LOGE("Failed to cache instance extensions, proceeding without filtering");
        }

        // Filter guest extensions - 只保留主机支持的扩展
        uint32_t origExtCount = pCreateInfo->enabledExtensionCount;
        const char* const* origExts = pCreateInfo->ppEnabledExtensionNames;
        
        const char** filteredExts = NULL;
        uint32_t filteredExtCount = 0;
        
        if (g_instance_extensions_cached && origExtCount > 0) {
            filteredExts = (const char**)malloc(sizeof(char*) * origExtCount);
            for (uint32_t i = 0; i < origExtCount; i++) {
                if (is_extension_supported(origExts[i])) {
                    filteredExts[filteredExtCount++] = origExts[i];
                    LOGD("Extension accepted: %s", origExts[i]);
                } else {
                    LOGD("Extension filtered out (not supported by host): %s", origExts[i]);
                }
            }
        } else {
            // 如果缓存失败，保留所有扩展
            filteredExtCount = origExtCount;
            filteredExts = (const char**)origExts;
        }

        // 获取GLFW需要的扩展
        uint32_t glfwExtCount = 0;
        const char** glfwExts = glfwGetRequiredInstanceExtensions(&glfwExtCount);

        // 合并过滤后的guest扩展和GLFW扩展
#ifdef __APPLE__
        uint32_t totalExtCount = filteredExtCount + glfwExtCount + 1;
#else
        uint32_t totalExtCount = filteredExtCount + glfwExtCount;
#endif
        const char** mergedExts = malloc(sizeof(char*) * totalExtCount);
        
        for (uint32_t i = 0; i < filteredExtCount; i++) {
            mergedExts[i] = filteredExts[i];
        }
        for (uint32_t i = 0; i < glfwExtCount; i++) {
            mergedExts[filteredExtCount + i] = glfwExts[i];
            LOGD("glfw ext %d %s", i, glfwExts[i]);
        }
#ifdef __APPLE__
        mergedExts[totalExtCount-1] = "VK_KHR_portability_enumeration";
#endif
        for (uint32_t i = 0; i < totalExtCount; i++) {
            LOGD("Final enabled extension %d: %s", i, mergedExts[i]);
        }

        ((VkInstanceCreateInfo*)pCreateInfo)->enabledExtensionCount   = totalExtCount;
        ((VkInstanceCreateInfo*)pCreateInfo)->ppEnabledExtensionNames = mergedExts;

        VkResult result = vkCreateInstance(pCreateInfo, pAllocator, &pInstance);

        // 清理
        free(mergedExts);
        if (g_instance_extensions_cached && filteredExts != origExts) {
            free(filteredExts);
        }

        if (result == VK_SUCCESS) {
            LOGD("got result %d instance %lld %lld size %d guest %lld", result, pInstance, &pInstance, sizeof(VkInstance), guest_instance);
            insert_mapping(EXPRESS_VK_OBJECT_TYPE_INSTANCE, guest_instance, (uint64_t)(uintptr_t)pInstance);
            LOGD("map result is %lld", lookup_mapping(EXPRESS_VK_OBJECT_TYPE_INSTANCE, guest_instance));
        } else {
            LOGE("vkCreateInstance failed with %d", result);
        }
        write_to_guest_mem(all_para[1].data, &result, 0, sizeof(VkResult));

        //create gl context and bind
        g_gl_context = get_native_opengl_context(0);
        egl_makeCurrent(g_gl_context);
    }
    break;

    case FUNID_vkCreateAndroidSurfaceKHR: {
        LOGD("Host: vkCreateAndroidSurfaceKHR request");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("get vk param number %d instance is", para_num);

        char *stream_ptr;

        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t* ptr = (uint8_t*)stream;

        uint64_t guest_inst = *(uint64_t*)(ptr);
        ptr += sizeof(uint64_t);

        uint64_t guest_window_ptr = *(uint64_t*)(ptr);
        ptr += sizeof(uint64_t);

        // uint64_t guest_hostSurf_addr = *(uint64_t*)(ptr);
        // ptr += sizeof(uint64_t);
        VkSurfaceKHR guestSurface = VK_NULL_HANDLE;
        void*   guest_surface_ptr  = all_para[1].data;
        read_from_guest_mem(guest_surface_ptr, &guestSurface, 0, sizeof(VkSurfaceKHR));

        //if (need_free) free(stream);

        VkInstance hostInst = (VkInstance)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_INSTANCE, guest_inst);
        LOGD("Host: mapped guestInst %llu → hostInst %p",
            (unsigned long long)guest_inst, (void*)hostInst);

        GLFWwindow* win = (GLFWwindow*)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_NATIVE_WINDOW, guest_window_ptr);
        if (!win) {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            win = glfwCreateWindow(1, 1, "Guest Window", NULL, NULL);
            insert_mapping(EXPRESS_VK_OBJECT_TYPE_NATIVE_WINDOW,
                        guest_window_ptr,
                        (uint64_t)(uintptr_t)win);
            LOGD("Host: created GLFW window %p for guest window %llu",
                win, (unsigned long long)guest_window_ptr);
        }

        VkSurfaceKHR hostSurface = VK_NULL_HANDLE;
        VkResult res = glfwCreateWindowSurface(hostInst, win, NULL, &hostSurface);
        if (res != VK_SUCCESS) {
            LOGE("Host: vkCreateAndroidSurfaceKHR failed %d", res);
            return;
        }
        
        LOGD("Host: created hostSurface %lld %d", (long long)hostSurface, res);

        insert_mapping(EXPRESS_VK_OBJECT_TYPE_SURFACE, (uint64_t)guestSurface, (uint64_t)(uintptr_t)hostSurface);
    }
    break;

    case FUNID_vkCreateSurfaceOHOS: {
        LOGD("Host: FUNID_vkCreateSurfaceOHOS request");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("get vk param number %d instance is", para_num);

        char *stream_ptr;

        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t* ptr = (uint8_t*)stream;

        uint64_t guest_inst = *(uint64_t*)(ptr);
        ptr += sizeof(uint64_t);

        uint64_t guest_window_ptr = *(uint64_t*)(ptr);
        ptr += sizeof(uint64_t);

        // uint64_t guest_hostSurf_addr = *(uint64_t*)(ptr);
        // ptr += sizeof(uint64_t);
        VkSurfaceKHR guestSurface = VK_NULL_HANDLE;
        void*   guest_surface_ptr  = all_para[1].data;
        read_from_guest_mem(guest_surface_ptr, &guestSurface, 0, sizeof(VkSurfaceKHR));

        //if (need_free) free(stream);
THREAD_CONTROL_BEGIN
        VkInstance hostInst = (VkInstance)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_INSTANCE, guest_inst);
        LOGD("Host: mapped guestInst %llu → hostInst %p",
            (unsigned long long)guest_inst, (void*)hostInst);

        GLFWwindow* win = (GLFWwindow*)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_NATIVE_WINDOW, guest_window_ptr);
        VkSurfaceKHR hostSurface = VK_NULL_HANDLE;
        VkResult res;
            
        if (!win) {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

            #ifdef __APPLE__
                // macOS 特殊处理：启用 Retina 支持
                glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
            #endif

            win = glfwCreateWindow(720, 1280, "Guest Window", NULL, NULL);
            insert_mapping(EXPRESS_VK_OBJECT_TYPE_NATIVE_WINDOW,
                        guest_window_ptr,
                        (uint64_t)(uintptr_t)win);
            LOGD("Host: created GLFW window %p for guest window %llu",
                win, (unsigned long long)guest_window_ptr);
        }

        res = glfwCreateWindowSurface(hostInst, win, NULL, &hostSurface);

        if (res != VK_SUCCESS) {
            LOGE("Host: vkCreateSurfaceOHOS failed %d", res);
            return;
        }
        
        LOGD("Host: created hostSurface %lld %d", (long long)hostSurface, res);

        insert_mapping(EXPRESS_VK_OBJECT_TYPE_SURFACE, (uint64_t)guestSurface, (uint64_t)(uintptr_t)hostSurface);
THREAD_CONTROL_END        
    }
    break;

    case FUNID_vkCreateSwapchainKHR: {
        LOGD("Host: vkCreateSwapchainKHR request %lld", (long long)vkCreateSwapchainKHR);

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);

        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t* ptr = (uint8_t*)stream;

        uint64_t guest_device        = *(uint64_t*)ptr; ptr += sizeof(uint64_t);
        uint64_t guest_surface       = *(uint64_t*)ptr; ptr += sizeof(uint64_t);
        uint32_t minImageCount       = *(uint32_t*)ptr; ptr += sizeof(uint32_t);
        uint32_t imageFormat         = *(uint32_t*)ptr; ptr += sizeof(uint32_t);
        uint32_t width               = *(uint32_t*)ptr; ptr += sizeof(uint32_t);
        uint32_t height              = *(uint32_t*)ptr; ptr += sizeof(uint32_t);
        uint32_t presentMode         = *(uint32_t*)ptr; ptr += sizeof(uint32_t);

        void*   guest_swapchain_ptr  = all_para[1].data;
        VkSwapchainKHR guestSwapchain = VK_NULL_HANDLE; //ztodo：这里直接改成发送值应该会更快
        read_from_guest_mem(guest_swapchain_ptr, &guestSwapchain, 0, sizeof(VkSwapchainKHR));

        LOGD("Host: vkCreateSwapchainKHR guest_device %llu guest_surface %llu minImageCount %d imageFormat %d width %d height %d presentMode %d",
            (unsigned long long)guest_device,
            (unsigned long long)guest_surface,
            minImageCount, imageFormat, width, height, presentMode);

        //if (need_free) free(stream);
        VkDevice hostDevice  = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkSurfaceKHR hostSurface = (VkSurfaceKHR)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_SURFACE, guest_surface);
        
        vulkan_surface_create_swapchain(hostDevice, hostSurface, guestSwapchain, minImageCount, imageFormat, width, height, presentMode);
    }
    break;

    case FUNID_vkGetSwapchainImagesKHR: {
        LOGD("Host: vkGetSwapchainImagesKHR");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char*     stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t*  ptr    = (uint8_t*)stream;
        uint64_t guest_device    = *(uint64_t*)ptr; ptr += sizeof(uint64_t);
        uint64_t guest_swapchain = *(uint64_t*)ptr; ptr += sizeof(uint64_t);
        uint32_t count           = *(uint32_t*)ptr; ptr += sizeof(uint32_t);
        VkDevice       realDev       = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkSwapchainKHR realSwapchain = (VkSwapchainKHR)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_SWAPCHAIN_KHR, guest_swapchain);
        LOGD("Host: vkGetSwapchainImagesKHR guest_device %llu guest_swapchain %llu count %d real swapchain %lld",
            (unsigned long long)guest_device,
            (unsigned long long)guest_swapchain,
            count, (long long)realSwapchain);
        
        uint64_t* guestImages = malloc(sizeof(uint64_t) * count);
        int* buffer_width = malloc(sizeof(int) * count);
        int* buffer_height = malloc(sizeof(int) * count);
        read_from_guest_mem(
            all_para[1].data,
            guestImages,
            0,
            sizeof(uint64_t) * count);
        
        uint64_t* guestBuffers = malloc(sizeof(uint64_t) * count);
        read_from_guest_mem(
            all_para[2].data,
            guestBuffers,
            0,
            sizeof(uint64_t) * count);

        read_from_guest_mem(
            all_para[3].data,
            buffer_width,
            0,
            sizeof(int) * count);
        
        read_from_guest_mem(
            all_para[4].data,
            buffer_height,
            0,
            sizeof(int) * count);
        
        LOGD("Host: vkGetSwapchainImagesKHR guestImages %lld guestBuffers %lld", (long long)guestImages[0], (long long)guestBuffers[0]);
        
        VkImage* images = malloc(sizeof(VkImage) * count);
        VkResult res = vkGetSwapchainImagesKHR(realDev, realSwapchain, &count, images);
        if (res != VK_SUCCESS) {
            LOGE("vkGetSwapchainImagesKHR failed: %d", res);
        } else {
            for (uint32_t i = 0; i < count; i++) {
                LOGD("count is %d, i is %d, guestImages[i] is %lld, guestBuffers[i] is %lld, images[i] is %lld",
                    count, i, guestImages[i], guestBuffers[i], (uint64_t)(uintptr_t)images[i]);

                insert_mapping(
                    EXPRESS_VK_OBJECT_TYPE_IMAGE,
                    guestImages[i],
                    (uint64_t)(uintptr_t)images[i]);
                LOGD("Host: vkGetSwapchainImagesKHR guest %llu mapped to host %lld",guestImages[i], (uint64_t)(uintptr_t)images[i]);

                Hardware_Buffer *gbuffer = get_gbuffer_from_global_map(guestBuffers[i]);
                if (gbuffer == NULL) {
                    gbuffer = create_gbuffer_from_vulkan(
                        buffer_width[i],
                        buffer_height[i],
                        guestBuffers[i],
                        images[i], //这里用guest还得host的再议
                        NULL,
                        realDev,
                        NULL,
                        0
                    );
                    if (gbuffer != NULL) {
                        add_gbuffer_to_global(gbuffer);
                    }
                }
            }
        }

        // vulkan_surface_register_swapchain_images(realDev, realSwapchain, guestImages, count);
        //if (need_free) free(stream);
        free(guestImages);
        free(guestBuffers);
        free(buffer_width);
        free(buffer_height);
    }
    break;

    case FUNID_vkEnumeratePhysicalDevices: 
    {
        LOGD("Host: vkEnumeratePhysicalDevices request");

            
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("get vk param number %d", para_num);

        // int need_free = 0;
        // char *stream_ptr;
        // stream_ptr = call_para_to_ptr(all_para[0], &need_free);
        // uint8_t ** stream_ptr_ptr = (uint8_t **)&stream_ptr;

        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        uint64_t guest_inst = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        uint32_t count = 0;
        read_from_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        // uint64_t guest_count_ptr = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        // uint64_t guest_devs_ptr  = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);

        VkInstance instance = (VkInstance)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_INSTANCE, guest_inst);
        LOGD("before and after map instance %lld %lld", guest_inst, (uint64_t)(uintptr_t)instance);

        LOGD("Host: vkEnumeratePhysicalDevices count %d", count);

        VkResult result;
        if (count == 0) {
            result = vkEnumeratePhysicalDevices(instance, &count, NULL);
            LOGD("Host: vkEnumeratePhysicalDevices count after call %d", count);
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        } else {
            VkPhysicalDevice* devices = (VkPhysicalDevice*)malloc(count * sizeof(VkPhysicalDevice));
            if (!devices) {
                result = VK_ERROR_OUT_OF_HOST_MEMORY;
            } else {
                uint64_t* guest_devs = malloc(count * sizeof(uint64_t));
                read_from_guest_mem(all_para[2].data, guest_devs, 0, count * sizeof(uint64_t));
                result = vkEnumeratePhysicalDevices(instance, &count, devices);
                for (uint32_t i = 0; i < count; ++i) {
                    uint64_t host_dev  = (uint64_t)(uintptr_t)devices[i];
                    insert_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE,
                                guest_devs[i], host_dev);
                    // write_to_guest_mem(all_para[2].data, devices, 0, count * sizeof(VkPhysicalDevice));
                    LOGD("Host: vkEnumeratePhysicalDevices device guest and host %d %lld %lld", i, guest_devs[i], host_dev);
                }
                free(devices);
            }
        }
        write_to_guest_mem(all_para[3].data, &result, 0, sizeof(VkResult));
        
    }
    break;

    case FUNID_vkCreateDevice: {
        LOGD("Host: vkCreateDevice request");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host: vkCreateDevice para count = %d", para_num);

        int need_free = 0;
        char*     stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr    = (uint8_t**)&stream;

        VkDeviceCreateInfo* pCreateInfo = malloc(sizeof(VkDeviceCreateInfo));
        decode_from_stream_VkDeviceCreateInfo(
            VK_STRUCTURE_TYPE_MAX_ENUM,
            pCreateInfo,
            ptr);

        VkAllocationCallbacks guestAllocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(
                VK_STRUCTURE_TYPE_MAX_ENUM,
                &guestAllocStruct,
                ptr);
            pAllocator = &guestAllocStruct;
        }

        uint64_t guest_phys = *(uint64_t*)(*ptr);  *ptr += sizeof(uint64_t);
        uint64_t guest_dev  = *(uint64_t*)(*ptr);  *ptr += sizeof(uint64_t);

        // filter extensions
        uint32_t availCount = 0;
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_phys);
        vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &availCount, NULL);
        VkExtensionProperties* availProps = malloc(sizeof(VkExtensionProperties) * availCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &availCount, availProps);
        for(int i = 0; i < availCount; i++) {
            LOGD("Host: Available device extension %d: %s", i, availProps[i].extensionName);
        }

        uint32_t origCount = pCreateInfo->enabledExtensionCount;
        const char* const* origExts = pCreateInfo->ppEnabledExtensionNames;
        const char** newExts = malloc(sizeof(char*) * (origCount + 1));
        uint32_t newCount = 0;

        for (uint32_t i = 0; i < origCount; i++) {
            const char* ext = origExts[i];
            if (has_device_extension(availProps, availCount, ext)) {
                newExts[newCount++] = ext;
            }
        }
        if (!has_device_extension(availProps, availCount, VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
        } else {
            bool found = false;
            for (uint32_t i = 0; i < newCount; i++) {
                if (strcmp(newExts[i], VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                newExts[newCount++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
            }
        }

        #ifdef __APPLE__
            const char* required_interop_exts[] = {
                "VK_KHR_external_memory",
                "VK_EXT_metal_objects",
                "VK_EXT_external_memory_metal"  // macOS 使用 Metal 互操作
            };
            int num_exts = 3;
        #else
            const char* required_interop_exts[] = {
                "VK_KHR_external_memory",
                "VK_KHR_external_memory_win32"
            };
            int num_exts = 2;
        #endif
        
        for (int i = 0; i < num_exts; i++) {
            const char* ext = required_interop_exts[i];
            if (has_device_extension(availProps, availCount, ext)) {
                // 检查是否已添加
                bool already_added = false;
                for (uint32_t j = 0; j < newCount; j++) {
                    if (strcmp(newExts[j], ext) == 0) {
                        already_added = true;
                        break;
                    }
                }
                if (!already_added) {
                    newExts[newCount++] = ext;
                    LOGD("Host: Added interop extension: %s", ext);
                }
            } else {
                LOGW("Host: Interop extension not available: %s", ext);
            }
        }

        pCreateInfo->enabledExtensionCount   = newCount;
        pCreateInfo->ppEnabledExtensionNames = newExts;

        LOGD("Enabled extensions count: %d", newCount);
        for (uint32_t i = 0; i < newCount; i++) {
            LOGD("  Extension[%d]: %s", i, newExts[i]);
        }

        free(availProps);


        VkPhysicalDevice realPD = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(
                EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE,
                guest_phys);

        VkDevice realDevice;
        VkResult result = vkCreateDevice(
            realPD,
            pCreateInfo,
            pAllocator,
            &realDevice);

        if (result == VK_SUCCESS) {
            insert_mapping(
                EXPRESS_VK_OBJECT_TYPE_DEVICE,
                guest_dev,
                (uint64_t)(uintptr_t)realDevice);
            set_device_pd((uint64_t)(uintptr_t)realDevice, realPD);

            VkQueue graphicsQueue;
            vkGetDeviceQueue(realDevice, 0, 0, &graphicsQueue);
            set_device_graphics_queue((uint64_t)(uintptr_t)realDevice, (uint64_t)(uintptr_t)graphicsQueue);

            LOGD("Host: mapped guest Dev %llu -> host %p",
                (unsigned long long)guest_dev,
                (void*)realDevice);
        } else {
            LOGE("vkCreateDevice failed: %d", result);
        }

        write_to_guest_mem(
            all_para[1].data,
            &result,
            0,
            sizeof(VkResult));

        // //if (need_free) free(stream);
#ifdef __WIN32__
        init_interop_once(realDevice);
#endif
        free(pCreateInfo);
        free(newExts);
    }
    break;

    case FUNID_vkGetDeviceQueue: {
        LOGD("Host: vkGetDeviceQueue");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkGetDeviceQueue para_num=%d", para_num);

        int need_free = 0;
        char*      stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t**  ptr    = (uint8_t**)&stream;

        uint64_t guest_dev_handle  = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);

        uint32_t queueFamilyIndex  = *(uint32_t*)(*ptr);
        *ptr += sizeof(uint32_t);

        uint32_t queueIndex        = *(uint32_t*)(*ptr);
        *ptr += sizeof(uint32_t);

        uint64_t guest_queue_handle = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);

        VkDevice realDevice = (VkDevice)(uintptr_t)
            lookup_mapping(
                EXPRESS_VK_OBJECT_TYPE_DEVICE,
                guest_dev_handle);

        VkQueue realQueue;
        vkGetDeviceQueue(
            realDevice,
            queueFamilyIndex,
            queueIndex,
            &realQueue);

        insert_mapping(
            EXPRESS_VK_OBJECT_TYPE_QUEUE,
            guest_queue_handle,
            (uint64_t)(uintptr_t)realQueue);
        
        //ztodo:判断一下是否是图形队列
        // set_device_graphics_queue((uint64_t)(uintptr_t)realDevice, (uint64_t)(uintptr_t)realQueue);

        LOGD("guest queue %llu mapped to host %p",
            (unsigned long long)guest_queue_handle,
            (void*)realQueue);

        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkCreateRenderPass: {
        LOGD("Host: vkCreateRenderPass");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("vkCreateRenderPass para count = %d", para_num);

        int need_free = 0;
        char*     stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr    = (uint8_t**)&stream;

        VkRenderPassCreateInfo* pInfo = malloc(sizeof(VkRenderPassCreateInfo));
        decode_from_stream_VkRenderPassCreateInfo(
            VK_STRUCTURE_TYPE_MAX_ENUM,
            pInfo,
            ptr);

        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(
                VK_STRUCTURE_TYPE_MAX_ENUM,
                &allocStruct,
                ptr);
            pAllocator = &allocStruct;
        }

        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_rp  = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);

        VkDevice realDev = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_dev);

        VkRenderPass realRp;
        VkResult result = vkCreateRenderPass(
            realDev,
            pInfo,
            pAllocator,
            &realRp);

        if (result == VK_SUCCESS) {
            insert_mapping(
                EXPRESS_VK_OBJECT_TYPE_RENDER_PASS,
                guest_rp,
                (uint64_t)(uintptr_t)realRp);
            LOGD("Mapped RenderPass guest %llu -> host %p",
                (unsigned long long)guest_rp,
                (void*)realRp);
        } else {
            LOGE("vkCreateRenderPass failed: %d", result);
        }

        //if (need_free) free(stream);
        free(pInfo);
    }
    break;

    case FUNID_vkCreateImageView: {
        LOGD("Host: vkCreateImageView");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("vkCreateImageView para count = %d", para_num);

        int need_free = 0;
        char*     stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr    = (uint8_t**)&stream;

        VkImageViewCreateInfo* pInfo = malloc(sizeof(VkImageViewCreateInfo));
        decode_from_stream_VkImageViewCreateInfo(
            VK_STRUCTURE_TYPE_MAX_ENUM,
            pInfo,
            ptr);
        
        LOGD("info image is %d %lld %d %d %d", pInfo->sType, (long long)pInfo->image, pInfo->viewType, pInfo->format, pInfo->components.r);

        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(
                VK_STRUCTURE_TYPE_MAX_ENUM,
                &allocStruct,
                ptr);
            pAllocator = &allocStruct;
        }

        uint64_t guest_dev  = *(uint64_t*)(*ptr);  *ptr += sizeof(uint64_t);
        uint64_t guest_iv   = *(uint64_t*)(*ptr);  *ptr += sizeof(uint64_t);

        VkDevice realDev = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_dev);

        LOGD("Host: vkCreateImageView realDev %p", (void*)realDev);

        VkImageView realIv;
        VkResult result = vkCreateImageView(
            realDev,
            pInfo,
            pAllocator,
            &realIv);

        if (result == VK_SUCCESS) {
            insert_mapping(
                EXPRESS_VK_OBJECT_TYPE_IMAGE_VIEW,
                guest_iv,
                (uint64_t)(uintptr_t)realIv);
            set_imageview_to_image((uint64_t)(uintptr_t)realIv, (uint64_t)(uintptr_t)pInfo->image);
            LOGD("Mapped ImageView guest %llu -> host %p",
                (unsigned long long)guest_iv,
                (void*)realIv);
        } else {
            LOGE("vkCreateImageView failed: %d", result);
        }

        //if (need_free) free(stream);
        free(pInfo);
    }
    break;

    case FUNID_vkCreateFramebuffer: {
        LOGD("Host: vkCreateFramebuffer");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("vkCreateFramebuffer para count = %d", para_num);

        int need_free = 0;
        char*     stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr    = (uint8_t**)&stream;

        VkFramebufferCreateInfo* pInfo = malloc(sizeof(VkFramebufferCreateInfo));
        decode_from_stream_VkFramebufferCreateInfo(
            VK_STRUCTURE_TYPE_MAX_ENUM,
            pInfo,
            ptr);

        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(
                VK_STRUCTURE_TYPE_MAX_ENUM,
                &allocStruct,
                ptr);
            pAllocator = &allocStruct;
        }

        uint64_t guest_dev = *(uint64_t*)(*ptr);  *ptr += sizeof(uint64_t);
        uint64_t guest_fb  = *(uint64_t*)(*ptr);  *ptr += sizeof(uint64_t);

        VkDevice realDev = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_dev);

        VkFramebuffer realFb;
        VkResult result = vkCreateFramebuffer(
            realDev,
            pInfo,
            pAllocator,
            &realFb);

        if (result != VK_SUCCESS) {
            LOGE("vkCreateFramebuffer failed: %d", result);
        } else {
            insert_mapping(
                EXPRESS_VK_OBJECT_TYPE_FRAMEBUFFER,
                guest_fb,
                (uint64_t)(uintptr_t)realFb);
            LOGD("Mapped Framebuffer guest %llu -> host %p",
                (unsigned long long)guest_fb,
                (void*)realFb);
        }

        if (need_free)
            free(stream);
        free(pInfo);
    }
    break;

    case FUNID_vkCreateBuffer: {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);

        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        VkBufferCreateInfo* pInfo = malloc(sizeof(VkBufferCreateInfo));
        decode_from_stream_VkBufferCreateInfo(VK_STRUCTURE_TYPE_MAX_ENUM, pInfo, ptr);

        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }

        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_buf = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);

        VkDevice realDev = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_dev);

        VkBuffer realBuf;
        VkResult result = vkCreateBuffer(realDev, pInfo, pAllocator, &realBuf);
        LOGD("pinfo is %d", (pInfo->usage & VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT) != 0);

        if (result == VK_SUCCESS) {
            insert_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER, guest_buf, (uint64_t)(uintptr_t)realBuf);
            LOGD("Mapped Buffer guest %llu -> host %p", (unsigned long long)guest_buf, (void*)realBuf);
        } else {
            LOGE("vkCreateBuffer failed: %d", result);
        }

        //if (need_free) free(stream);
        free(pInfo);
    }
    break;

    case FUNID_vkGetBufferMemoryRequirements: {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);

        int need_free = 0;
        char*     stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr    = (uint8_t**)&stream;

        uint64_t guest_dev = *(uint64_t*)(*ptr);  *ptr += sizeof(uint64_t);
        uint64_t guest_buf = *(uint64_t*)(*ptr);  *ptr += sizeof(uint64_t);

        VkMemoryRequirements req;
        VkDevice realDev = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_dev);
        VkBuffer realBuf = (VkBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER, guest_buf);
        LOGD("Host: vkGetBufferMemoryRequirements realDev %p realBuf %p", (void*)realDev, (void*)realBuf);

        vkGetBufferMemoryRequirements(realDev, realBuf, &req);
        LOGD("Host: vkGetBufferMemoryRequirements size %d alignment %d type %d",
            req.size, req.alignment, req.memoryTypeBits);
        
        LOGD("size is %d", sizeof(VkMemoryRequirements));

        write_to_guest_mem(
            all_para[1].data,
            &req,
            0,
            sizeof(VkMemoryRequirements));

        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkAllocateMemory: {
        LOGD("Host: vkAllocateMemory");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);

        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        VkMemoryAllocateInfo* pInfo = malloc(sizeof(VkMemoryAllocateInfo));
        decode_from_stream_VkMemoryAllocateInfo(VK_STRUCTURE_TYPE_MAX_ENUM, pInfo, ptr);

        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }

        uint64_t gbuffer_id = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t buffer_width = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t buffer_height = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint64_t buffer_handle = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);

        uint64_t guest_dev  = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_mem  = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);

        VkDevice realDev = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_dev);

        /*如果应用请求的内存类型本身不带 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT，
        那么在 Host 端调用 vkMapMemory 时就会直接报 VK_ERROR_MEMORY_MAP_FAILED。
        这一点完全由真实物理设备决定，应用无法绕过。
        驱动层的兼容补偿通过在分配阶段将 memoryTypeIndex 替换为一个真正可映射的类型，保证后续所有的 vkMapMemory 调用都能成功
        不会改变vulkan语义。*/ 
        VkPhysicalDevice hostPD = get_device_pd((uint64_t)(uintptr_t)realDev);
        if (hostPD != VK_NULL_HANDLE) {
            VkPhysicalDeviceMemoryProperties memProps;
            vkGetPhysicalDeviceMemoryProperties(hostPD, &memProps);

            uint32_t reqType = pInfo->memoryTypeIndex;
            VkMemoryPropertyFlags flags =
                memProps.memoryTypes[reqType].propertyFlags;
            LOGD("Requested memoryTypeIndex=%u flags=0x%x",
                reqType, flags);

            if (!(flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
                for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
                    if (memProps.memoryTypes[i].propertyFlags &
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
                        LOGD("Override memoryTypeIndex %u -> %u (HOST_VISIBLE)",
                            reqType, i);
                        pInfo->memoryTypeIndex = i;
                        break;
                    }
                }
            }
        }

        VkDeviceMemory realMem;

        if (gbuffer_id != 0) {
            Hardware_Buffer* gbuffer = get_gbuffer_from_global_map(gbuffer_id);
            if (gbuffer == NULL) {
                gbuffer = create_gbuffer_from_vulkan(
                    buffer_width,
                    buffer_height,
                    gbuffer_id,
                    NULL,
                    NULL,
                    realDev,
                    NULL,
                    buffer_handle
                );
                if (gbuffer != NULL) {
                    add_gbuffer_to_global(gbuffer);
                }
            }
            
            VkExportMemoryAllocateInfo exportInfo = {};
            exportInfo.sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO;
            exportInfo.pNext = pInfo->pNext;

            #ifdef __APPLE__
                    exportInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_MTLTEXTURE_BIT_EXT;
            #else
                    exportInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
            #endif
            
            pInfo->pNext = &exportInfo;
            
            VkResult result = vkAllocateMemory(realDev, pInfo, pAllocator, &realMem);
            
            if (result == VK_SUCCESS) {
                insert_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE_MEMORY, guest_mem, (uint64_t)(uintptr_t)realMem);
                insert_gbuffer_memory_mapping(gbuffer_id, (uint64_t)(uintptr_t)realMem);
                LOGD("Mapped shared DeviceMemory guest %llu -> host %p, gbuffer_id=%llx",
                    (unsigned long long)guest_mem, (void*)realMem, (unsigned long long)gbuffer_id);
            } else {
                LOGE("vkAllocateMemory failed: %d", result);
            }
        } else {
            VkResult result = vkAllocateMemory(realDev, pInfo, pAllocator, &realMem);
            
            if (result == VK_SUCCESS) {
                insert_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE_MEMORY, guest_mem, (uint64_t)(uintptr_t)realMem);
                LOGD("Mapped DeviceMemory guest %llu -> host %p", (unsigned long long)guest_mem, (void*)realMem);
            } else {
                LOGE("vkAllocateMemory failed: %d", result);
            }
        }

        //if (need_free) free(stream);
        free(pInfo);
    }
    break;

    case FUNID_vkMapMemory: {
        //ztodo：如果guest是写而不是读，这一步就需要把数据write to guest，暂未实现！！！
        LOGD("Host: vkMapMemory");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);

        int need_free = 0;
        char*     stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr    = (uint8_t**)&stream;

        uint64_t guest_dev  = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_mem  = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDeviceSize offset = *(VkDeviceSize*)(*ptr); *ptr += sizeof(VkDeviceSize);
        VkDeviceSize size   = *(VkDeviceSize*)(*ptr); *ptr += sizeof(VkDeviceSize);
        VkMemoryMapFlags flags = *(VkMemoryMapFlags*)(*ptr); *ptr += sizeof(VkMemoryMapFlags);
        LOGD("size of flags is %d", sizeof(VkMemoryMapFlags));

        // void** guest_ppData;
        // read_from_guest_mem(all_para[5].data, &guest_ppData, 0, sizeof(void*));

        VkDevice realDev = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_dev);
        VkDeviceMemory realMem = (VkDeviceMemory)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE_MEMORY, guest_mem);

        void* mappedPtr = NULL;
        VkResult result = vkMapMemory(realDev, realMem, offset, size, flags, &mappedPtr);
        LOGD("real dev %p real mem %p offset %d size %d flags %d",
            (void*)realDev, (void*)realMem, offset, size, flags);
        if (result != VK_SUCCESS) {
            LOGE("vkMapMemory failed: %d", result);
        } else {
            // write_to_guest_mem(all_para[5].data, &mappedPtr, 0, sizeof(void*));
            set_memory_map((uint64_t)realMem, mappedPtr);
            LOGD("Mapped memory guest %llu -> host %p",
                (unsigned long long)guest_mem,
                mappedPtr);
        }

        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkUnmapMemory: {
        LOGD("Host: vkUnmapMemory");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);

        int need_free = 0;
        char*     stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr    = (uint8_t**)&stream;

        uint64_t guest_dev = *(uint64_t*)(*ptr);  *ptr += sizeof(uint64_t);
        uint64_t guest_mem = *(uint64_t*)(*ptr);  *ptr += sizeof(uint64_t);

        VkDevice realDev = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_dev);
        VkDeviceMemory realMem = (VkDeviceMemory)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE_MEMORY, guest_mem);

        void* hostPtr = get_memory_map((uint64_t)realMem);
        if (hostPtr) {
            // all_para[1].data 对应 addPtr(mem->map_data, mem->length)
            // all_para[1].size 存储了 mem->length
            read_from_guest_mem(
                all_para[1].data,
                hostPtr,
                0,
                all_para[1].data_len);
            LOGD("Host: synced %zu bytes to mappedPtr %p",
                (size_t)all_para[1].data_len, hostPtr);
        } else {
            LOGE("Host: no mapping found for guest_mem %llu", guest_mem);
        }

        vkUnmapMemory(realDev, realMem);

        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkBindBufferMemory: {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);

        int need_free = 0;
        char*     stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr    = (uint8_t**)&stream;

        uint64_t guest_dev        = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_buffer     = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_memory     = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDeviceSize   memoryOffset = *(VkDeviceSize*)(*ptr); *ptr += sizeof(VkDeviceSize);

        VkDevice realDev = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_dev);
        VkBuffer realBuffer = (VkBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER, guest_buffer);
        VkDeviceMemory realMemory = (VkDeviceMemory)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE_MEMORY, guest_memory);

        VkResult result = vkBindBufferMemory(
            realDev,
            realBuffer,
            realMemory,
            memoryOffset);

        if (result != VK_SUCCESS) {
            LOGE("vkBindBufferMemory failed: %d", result);
        } else {
            LOGD("vkBindBufferMemory Bound buffer %llu to memory %llu",
                (unsigned long long)guest_buffer,
                (unsigned long long)guest_memory);
        }

        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkCreateShaderModule: {
        LOGD("Host: vkCreateShaderModule");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);

        int need_free = 0;
        char*     stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr    = (uint8_t**)&stream;

        VkShaderModuleCreateInfo* pInfo = malloc(sizeof(VkShaderModuleCreateInfo));
        decode_from_stream_VkShaderModuleCreateInfo(
            VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            pInfo,
            ptr);

        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(
                VK_STRUCTURE_TYPE_MAX_ENUM,
                &allocStruct,
                ptr);
            pAllocator = &allocStruct;
        }

        uint64_t guest_dev     = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_module  = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);

        VkDevice realDev = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_dev);

        VkShaderModule realModule;
        VkResult result = vkCreateShaderModule(
            realDev,
            pInfo,
            pAllocator,
            &realModule);

        if (result == VK_SUCCESS) {
            insert_mapping(
                EXPRESS_VK_OBJECT_TYPE_SHADER_MODULE,
                guest_module,
                (uint64_t)(uintptr_t)realModule);
            LOGD("Mapped ShaderModule guest %llu -> host %p",
                (unsigned long long)guest_module,
                (void*)realModule);
        } else {
            LOGE("vkCreateShaderModule failed: %d", result);
        }
        //if (need_free) free(stream);
        free(pInfo);
    }
    break;

    case FUNID_vkCreatePipelineLayout: {
        LOGD("Host: vkCreatePipelineLayout");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);

        int need_free = 0;
        char*     stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr    = (uint8_t**)&stream;

        VkPipelineLayoutCreateInfo* pInfo = malloc(sizeof(VkPipelineLayoutCreateInfo));
        decode_from_stream_VkPipelineLayoutCreateInfo(
            VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            pInfo,
            ptr);

        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(
                VK_STRUCTURE_TYPE_MAX_ENUM,
                &allocStruct,
                ptr);
            pAllocator = &allocStruct;
        }

        uint64_t guest_dev     = *(uint64_t*)(*ptr);  *ptr += sizeof(uint64_t);
        uint64_t guest_layout  = *(uint64_t*)(*ptr);  *ptr += sizeof(uint64_t);

        VkDevice realDev = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_dev);

        VkPipelineLayout realLayout;
        VkResult result = vkCreatePipelineLayout(
            realDev,
            pInfo,
            pAllocator,
            &realLayout);

        if (result == VK_SUCCESS) {
            insert_mapping(
                EXPRESS_VK_OBJECT_TYPE_PIPELINE_LAYOUT,
                guest_layout,
                (uint64_t)(uintptr_t)realLayout);
            LOGD("Mapped PipelineLayout guest %llu -> host %p",
                (unsigned long long)guest_layout,
                (void*)realLayout);
        } else {
            LOGE("vkCreatePipelineLayout failed: %d", result);
        }

        //if (need_free) free(stream);
        free(pInfo);
    }
    break;

    case FUNID_vkCreatePipelineCache: {
        LOGD("Host: vkCreatePipelineCache");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);

        int need_free = 0;
        char*     stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr    = (uint8_t**)&stream;

        VkPipelineCacheCreateInfo* pInfo = malloc(sizeof(VkPipelineCacheCreateInfo));
        decode_from_stream_VkPipelineCacheCreateInfo(
            VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
            pInfo,
            ptr);

        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(
                VK_STRUCTURE_TYPE_MAX_ENUM,
                &allocStruct,
                ptr);
            pAllocator = &allocStruct;
        }

        uint64_t guest_dev    = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_cache  = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);

        VkDevice realDev = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_dev);

        VkPipelineCache realCache;
        VkResult result = vkCreatePipelineCache(
            realDev,
            pInfo,
            pAllocator,
            &realCache);

        if (result == VK_SUCCESS) {
            insert_mapping(
                EXPRESS_VK_OBJECT_TYPE_PIPELINE_CACHE,
                guest_cache,
                (uint64_t)(uintptr_t)realCache);
            LOGD("Mapped PipelineCache guest %llu -> host %p",
                (unsigned long long)guest_cache,
                (void*)realCache);
        } else {
            LOGE("vkCreatePipelineCache failed: %d", result);
        }

        //if (need_free) free(stream);
        free(pInfo);
    }
    break;

    case FUNID_vkCreateGraphicsPipelines: {
        LOGD("Host: vkCreateGraphicsPipelines request");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);

        int need_free = 0;
        char*     stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr    = (uint8_t**)&stream;

        uint64_t guest_dev     = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_pipelineCache   = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t createInfoCount     = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        // createInfoCount     = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        LOGD("Host: vkCreateGraphicsPipelines createInfoCount = %u guest dev %lld cache %lld", createInfoCount, (long long)guest_dev, (long long)guest_pipelineCache);

        // 2) Decode each VkGraphicsPipelineCreateInfo from the buffer
        VkGraphicsPipelineCreateInfo* infos =
            malloc(sizeof(VkGraphicsPipelineCreateInfo) * createInfoCount);
        for (uint32_t i = 0; i < createInfoCount; i++) {
            memset(&infos[i], 0, sizeof(VkGraphicsPipelineCreateInfo));
            decode_from_stream_VkGraphicsPipelineCreateInfo(
                VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                &infos[i],
                ptr);
        }
        LOGD("Decoded %u VkGraphicsPipelineCreateInfo structures", createInfoCount);

        // 3) Decode allocator pointer and callbacks at end of buffer
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct, *pAllocator = NULL;
        LOGD("Guest allocator pointer: %llu", (unsigned long long)guest_alloc_ptr);
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(
                VK_STRUCTURE_TYPE_MAX_ENUM,
                &allocStruct,
                ptr);
            pAllocator = &allocStruct;
        }

        // //if (need_free) free(stream);

        // 4) Map guest handles to host
        VkDevice       realDev    = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_dev);
        VkPipelineCache realCache = (VkPipelineCache)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PIPELINE_CACHE, guest_pipelineCache);

        // 5) Call the real Vulkan function
        VkPipeline* hostPipelines = malloc(sizeof(VkPipeline) * createInfoCount);
        VkResult result = vkCreateGraphicsPipelines(
            realDev,
            realCache,
            createInfoCount,
            infos,
            pAllocator,
            hostPipelines);

        // 6) On error, log; on success, insert mappings
        if (result != VK_SUCCESS) {
            LOGE("vkCreateGraphicsPipelines failed: %d", result);
        } else {
            for (uint32_t i = 0; i < createInfoCount; i++) {
                uint64_t guest_pipe = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
                insert_mapping(
                    EXPRESS_VK_OBJECT_TYPE_PIPELINE,
                    guest_pipe,
                    (uint64_t)(uintptr_t)hostPipelines[i]);
                LOGD("Mapped GraphicsPipeline guest %llu -> host %p",
                    (unsigned long long)guest_pipe,
                    (void*)hostPipelines[i]);
            }
            LOGD("Mapped %u VkPipelines", createInfoCount);
        }

        free(infos);
        free(hostPipelines);
    }
    break;

    case FUNID_vkCreateCommandPool: {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        VkCommandPoolCreateInfo* pInfo = malloc(sizeof(VkCommandPoolCreateInfo));
        decode_from_stream_VkCommandPoolCreateInfo(VK_STRUCTURE_TYPE_MAX_ENUM, pInfo, ptr);
        
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
        
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_pool = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkDevice realDev = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_dev);
        VkCommandPool realPool;
        
        VkResult result = vkCreateCommandPool(realDev, pInfo, pAllocator, &realPool);
        
        if (result == VK_SUCCESS) {
            insert_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_POOL, guest_pool, (uint64_t)(uintptr_t)realPool);
            LOGD("Mapped CommandPool guest %llu -> host %p", (unsigned long long)guest_pool, (void*)realPool);
        } else {
            LOGE("vkCreateCommandPool failed: %d", result);
        }
        
        //if (need_free) free(stream);
        free(pInfo);
    }
    break;

    case FUNID_vkAllocateCommandBuffers: {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        VkCommandBufferAllocateInfo* pInfo = malloc(sizeof(VkCommandBufferAllocateInfo));
        decode_from_stream_VkCommandBufferAllocateInfo(VK_STRUCTURE_TYPE_MAX_ENUM, pInfo, ptr);
        
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkDevice realDev = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_dev);
        
        VkCommandBuffer* realCmdBufs = malloc(pInfo->commandBufferCount * sizeof(VkCommandBuffer));
        LOGD("pinfo values commandBufferCount %d commandPool %p level %d",
            pInfo->commandBufferCount, (void*)pInfo->commandPool, pInfo->level);
        VkResult result = vkAllocateCommandBuffers(realDev, pInfo, realCmdBufs);
        
        if (result == VK_SUCCESS) {
            for (uint32_t i = 0; i < pInfo->commandBufferCount; ++i) {
                uint64_t guest_cmd_buf = *(uint64_t*)(*ptr);
                *ptr += sizeof(uint64_t);
                
                insert_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buf, 
                            (uint64_t)(uintptr_t)realCmdBufs[i]);
                LOGD("Mapped CommandBuffer %d guest %llu -> host %p", i, (unsigned long long)guest_cmd_buf, (void*)realCmdBufs[i]);
            }
        } else {
            LOGE("vkAllocateCommandBuffers failed: %d", result);
        }
        
        free(realCmdBufs);
        //if (need_free) free(stream);
        free(pInfo);
    }
    break;

    // Host端实现 vkBeginCommandBuffer
    case FUNID_vkBeginCommandBuffer: {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        VkCommandBufferBeginInfo* pInfo = malloc(sizeof(VkCommandBufferBeginInfo));
        decode_from_stream_VkCommandBufferBeginInfo(VK_STRUCTURE_TYPE_MAX_ENUM, pInfo, ptr);
        
        uint64_t guest_cmd_buf = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkCommandBuffer realCmdBuf = (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buf);
        
        VkResult result = vkBeginCommandBuffer(realCmdBuf, pInfo);
        
        if (result == VK_SUCCESS) {
            LOGD("BeginCommandBuffer success guest %llu -> host %p", (unsigned long long)guest_cmd_buf, (void*)realCmdBuf);
        } else {
            LOGE("vkBeginCommandBuffer failed: %d", result);
        }
        
        //if (need_free) free(stream);
        free(pInfo);
    }
    break;

    case FUNID_vkCmdPipelineBarrier: {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd_buf = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkPipelineStageFlags srcStageMask = *(uint32_t*)(*ptr);
        *ptr += sizeof(uint32_t);
        VkPipelineStageFlags dstStageMask = *(uint32_t*)(*ptr);
        *ptr += sizeof(uint32_t);
        VkDependencyFlags dependencyFlags = *(uint32_t*)(*ptr);
        *ptr += sizeof(uint32_t);
        
        uint32_t memoryBarrierCount = *(uint32_t*)(*ptr);
        *ptr += sizeof(uint32_t);
        uint32_t bufferMemoryBarrierCount = *(uint32_t*)(*ptr);
        *ptr += sizeof(uint32_t);
        uint32_t imageMemoryBarrierCount = *(uint32_t*)(*ptr);
        *ptr += sizeof(uint32_t);
        
        VkMemoryBarrier* memBarriers = NULL;
        if (memoryBarrierCount > 0) {
            memBarriers = malloc(memoryBarrierCount * sizeof(VkMemoryBarrier));
            for (uint32_t i = 0; i < memoryBarrierCount; ++i) {
                decode_from_stream_VkMemoryBarrier(VK_STRUCTURE_TYPE_MAX_ENUM, &memBarriers[i], ptr);
            }
        }
        
        VkBufferMemoryBarrier* bufBarriers = NULL;
        if (bufferMemoryBarrierCount > 0) {
            bufBarriers = malloc(bufferMemoryBarrierCount * sizeof(VkBufferMemoryBarrier));
            for (uint32_t i = 0; i < bufferMemoryBarrierCount; ++i) {
                decode_from_stream_VkBufferMemoryBarrier(VK_STRUCTURE_TYPE_MAX_ENUM, &bufBarriers[i], ptr);
                // bufBarriers[i].buffer = (VkBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER, (uint64_t)(uintptr_t)bufBarriers[i].buffer);
            }
        }
        
        VkImageMemoryBarrier* imgBarriers = NULL;
        if (imageMemoryBarrierCount > 0) {
            imgBarriers = malloc(imageMemoryBarrierCount * sizeof(VkImageMemoryBarrier));
            for (uint32_t i = 0; i < imageMemoryBarrierCount; ++i) {
                decode_from_stream_VkImageMemoryBarrier(VK_STRUCTURE_TYPE_MAX_ENUM, &imgBarriers[i], ptr);
                // imgBarriers[i].image = (VkImage)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_IMAGE, (uint64_t)(uintptr_t)imgBarriers[i].image);
            }
        }
        
        VkCommandBuffer realCmdBuf = (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buf);
        
        vkCmdPipelineBarrier(realCmdBuf, srcStageMask, dstStageMask, dependencyFlags,
                            memoryBarrierCount, memBarriers,
                            bufferMemoryBarrierCount, bufBarriers,
                            imageMemoryBarrierCount, imgBarriers);
        LOGD("vkCmdPipelineBarrier called with srcStageMask %u, dstStageMask %u, dependencyFlags %u",
            srcStageMask, dstStageMask, dependencyFlags);
        
        LOGD("CmdPipelineBarrier guest %llu -> host %p", (unsigned long long)guest_cmd_buf, (void*)realCmdBuf);
        
        if (memBarriers) free(memBarriers);
        if (bufBarriers) free(bufBarriers);
        if (imgBarriers) free(imgBarriers);
        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkCmdBeginRenderPass: {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd_buf = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkRenderPassBeginInfo* pInfo = malloc(sizeof(VkRenderPassBeginInfo));
        decode_from_stream_VkRenderPassBeginInfo(VK_STRUCTURE_TYPE_MAX_ENUM, pInfo, ptr);
        
        VkSubpassContents contents = *(uint32_t*)(*ptr);
        *ptr += sizeof(uint32_t);
        VkCommandBuffer realCmdBuf = (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buf);
        vkCmdBeginRenderPass(realCmdBuf, pInfo, contents);
        
        LOGD("CmdBeginRenderPass guest %llu -> host %p", (unsigned long long)guest_cmd_buf, (void*)realCmdBuf);
        
        //if (need_free) free(stream);
        free(pInfo);
    }
    break;

    case FUNID_vkCmdBindPipeline: {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd_buf = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkPipelineBindPoint bindPoint = *(uint32_t*)(*ptr);
        *ptr += sizeof(uint32_t);
        
        uint64_t guest_pipeline = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkCommandBuffer realCmdBuf = (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buf);
        VkPipeline realPipeline = (VkPipeline)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PIPELINE, guest_pipeline);
        
        vkCmdBindPipeline(realCmdBuf, bindPoint, realPipeline);
        
        LOGD("CmdBindPipeline guest %llu -> host %p pipeline %p", (unsigned long long)guest_cmd_buf, (void*)realCmdBuf, (void*)realPipeline);
        
        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkCmdBindVertexBuffers: {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);

        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_cmd_buf = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        uint32_t firstBinding = *(uint32_t*)(*ptr);
        *ptr += sizeof(uint32_t);
        uint32_t bindingCount = *(uint32_t*)(*ptr);
        *ptr += sizeof(uint32_t);

        VkBuffer* realBuffers = malloc(bindingCount * sizeof(VkBuffer));
        for (uint32_t i = 0; i < bindingCount; ++i) {
            uint64_t guest_buf = *(uint64_t*)(*ptr);
            *ptr += sizeof(uint64_t);
            realBuffers[i] = (VkBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER, guest_buf);
        }

        VkDeviceSize* offsets = malloc(bindingCount * sizeof(VkDeviceSize));
        for (uint32_t i = 0; i < bindingCount; ++i) {
            offsets[i] = *(VkDeviceSize*)(*ptr);
            *ptr += sizeof(VkDeviceSize);
        }

        VkCommandBuffer realCmdBuf =
            (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buf);

        vkCmdBindVertexBuffers(realCmdBuf, firstBinding, bindingCount, realBuffers, offsets);

        LOGD("CmdBindVertexBuffers guest %llu -> host %p count %d",
            (unsigned long long)guest_cmd_buf, (void*)realCmdBuf, bindingCount);

        free(realBuffers);
        free(offsets);
        // if (need_free) free(stream);
    }
    break;


    case FUNID_vkCmdDraw: {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t vertexCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t instanceCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t firstVertex = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t firstInstance = *(uint32_t*)(*ptr);
        
        VkCommandBuffer realCmd = (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        vkCmdDraw(realCmd, vertexCount, instanceCount, firstVertex, firstInstance);
        
        LOGD("CmdDraw executed cmd=%p vertices=%d", (void*)realCmd, vertexCount);
        
        //if (need_free) free(stream);
        
    }
    break;

    case FUNID_vkCmdEndRenderPass: {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr);
        
        VkCommandBuffer realCmd = (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        vkCmdEndRenderPass(realCmd);
        
        LOGD("CmdEndRenderPass executed cmd=%p", (void*)realCmd);
        
        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkEndCommandBuffer: {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr);
        
        VkCommandBuffer realCmd = (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        VkResult result = vkEndCommandBuffer(realCmd);
        
        LOGD("EndCommandBuffer executed cmd=%p result=%d", (void*)realCmd, result);
        
        //if (need_free) free(stream); 
    }
    break;

    case FUNID_vkCreateFence: {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        VkFenceCreateInfo* pInfo = malloc(sizeof(VkFenceCreateInfo));
        decode_from_stream_VkFenceCreateInfo(VK_STRUCTURE_TYPE_MAX_ENUM, pInfo, ptr);
        
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
        
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_fence = *(uint64_t*)(*ptr);
        
        VkDevice realDev = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_dev);
        VkFence realFence;
        
        VkResult result = vkCreateFence(realDev, pInfo, pAllocator, &realFence);
        if (result == VK_SUCCESS) {
            insert_mapping(EXPRESS_VK_OBJECT_TYPE_FENCE, guest_fence, (uint64_t)(uintptr_t)realFence);
            LOGD("Mapped Fence guest=%llu host=%p", (unsigned long long)guest_fence, (void*)realFence);
        } else {
            LOGD("vkCreateFence failed: %d", result);
        }
        
        //if (need_free) free(stream);
        free(pInfo);
    }
    break;

    case FUNID_vkCreateSemaphore: {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        VkSemaphoreCreateInfo* pInfo = malloc(sizeof(VkSemaphoreCreateInfo));
        decode_from_stream_VkSemaphoreCreateInfo(VK_STRUCTURE_TYPE_MAX_ENUM, pInfo, ptr);
        
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
        
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_semaphore = *(uint64_t*)(*ptr);
        
        VkDevice realDev = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_dev);
        VkSemaphore realSemaphore;
        
        VkResult result = vkCreateSemaphore(realDev, pInfo, pAllocator, &realSemaphore);
        if (result == VK_SUCCESS) {
            insert_mapping(EXPRESS_VK_OBJECT_TYPE_SEMAPHORE, guest_semaphore, (uint64_t)(uintptr_t)realSemaphore);
            LOGD("Mapped Semaphore guest=%llu host=%p", (unsigned long long)guest_semaphore, (void*)realSemaphore);
        } else {
            LOGD("vkCreateSemaphore failed: %d", result);
        }
        
        //if (need_free) free(stream);
        free(pInfo);
        break;
    }

    case FUNID_vkAcquireNextImageKHR:{
        LOGD("Host: vkAcquireNextImageKHR");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_swapchain = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t timeout = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_semaphore = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_fence = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);

        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkSwapchainKHR swapchain = (VkSwapchainKHR)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_SWAPCHAIN_KHR, guest_swapchain);
        VkSemaphore semaphore = guest_semaphore ? (VkSemaphore)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_SEMAPHORE, guest_semaphore) : VK_NULL_HANDLE;
        VkFence fence = guest_fence ? (VkFence)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_FENCE, guest_fence) : VK_NULL_HANDLE;
        
        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(device, swapchain, timeout, semaphore, fence, &imageIndex);
        
        
        LOGD("Host: vkAcquireNextImageKHR result=%d imageIndex=%d", result, imageIndex); 
    }
    break;

    case FUNID_vkResetFences: {
        LOGD("Host: vkResetFences");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t fenceCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        VkFence* fences = NULL;
        if (fenceCount > 0) {
            fences = (VkFence*)malloc(fenceCount * sizeof(VkFence));
            uint64_t* guest_fences = (uint64_t*)(*ptr);
            for (uint32_t i = 0; i < fenceCount; ++i) {
                fences[i] = (VkFence)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_FENCE, guest_fences[i]);
                LOGD("get Mapped Fence guest %llu -> host %p", (unsigned long long)guest_fences[i], (void*)fences[i]);
            }
        }
        
        VkResult result = vkResetFences(device, fenceCount, fences);
        
        if (fences) free(fences);
        
        LOGD("Host: vkResetFences result=%d fenceCount=%d", result, fenceCount);
    }
    break;

    case FUNID_vkQueueSubmit: {
        LOGD("Host: vkQueueSubmit");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_queue = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t submitCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint64_t guest_fence = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        VkQueue queue = (VkQueue)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_QUEUE, guest_queue);
        VkFence fence = guest_fence ? (VkFence)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_FENCE, guest_fence) : VK_NULL_HANDLE;
        
        VkSubmitInfo* pSubmits = NULL;
        if (submitCount > 0) {
            pSubmits = (VkSubmitInfo*)malloc(submitCount * sizeof(VkSubmitInfo));
            char* data_ptr = (char*)(*ptr);
            
            for (uint32_t i = 0; i < submitCount; ++i) {
                memcpy(&pSubmits[i], data_ptr, sizeof(VkSubmitInfo));
                data_ptr += sizeof(VkSubmitInfo);
                
                if (pSubmits[i].waitSemaphoreCount > 0) {
                    VkSemaphore* waitSems = (VkSemaphore*)malloc(pSubmits[i].waitSemaphoreCount * sizeof(VkSemaphore));
                    uint64_t* guest_sems = (uint64_t*)data_ptr;
                    for (uint32_t j = 0; j < pSubmits[i].waitSemaphoreCount; ++j) {
                        waitSems[j] = (VkSemaphore)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_SEMAPHORE, guest_sems[j]);
                    }
                    pSubmits[i].pWaitSemaphores = waitSems;
                    data_ptr += pSubmits[i].waitSemaphoreCount * sizeof(uint64_t);
                    
                    if (pSubmits[i].pWaitDstStageMask) {
                        pSubmits[i].pWaitDstStageMask = (VkPipelineStageFlags*)data_ptr;
                        data_ptr += pSubmits[i].waitSemaphoreCount * sizeof(VkPipelineStageFlags);
                    }
                }
                
                if (pSubmits[i].commandBufferCount > 0) {
                    VkCommandBuffer* cmdBufs = (VkCommandBuffer*)malloc(pSubmits[i].commandBufferCount * sizeof(VkCommandBuffer));
                    uint64_t* guest_cmds = (uint64_t*)data_ptr;
                    for (uint32_t j = 0; j < pSubmits[i].commandBufferCount; ++j) {
                        cmdBufs[j] = (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmds[j]);
                    }
                    pSubmits[i].pCommandBuffers = cmdBufs;
                    data_ptr += pSubmits[i].commandBufferCount * sizeof(uint64_t);
                }
                
                if (pSubmits[i].signalSemaphoreCount > 0) {
                    VkSemaphore* signalSems = (VkSemaphore*)malloc(pSubmits[i].signalSemaphoreCount * sizeof(VkSemaphore));
                    uint64_t* guest_sems = (uint64_t*)data_ptr;
                    for (uint32_t j = 0; j < pSubmits[i].signalSemaphoreCount; ++j) {
                        signalSems[j] = (VkSemaphore)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_SEMAPHORE, guest_sems[j]);
                    }
                    pSubmits[i].pSignalSemaphores = signalSems;
                    data_ptr += pSubmits[i].signalSemaphoreCount * sizeof(uint64_t);
                }
            }
        }
        
        VkResult result = vkQueueSubmit(queue, submitCount, pSubmits, fence);
        
        if (pSubmits) {
            for (uint32_t i = 0; i < submitCount; ++i) {
                if (pSubmits[i].pWaitSemaphores) free((void*)pSubmits[i].pWaitSemaphores);
                if (pSubmits[i].pCommandBuffers) free((void*)pSubmits[i].pCommandBuffers);
                if (pSubmits[i].pSignalSemaphores) free((void*)pSubmits[i].pSignalSemaphores);
            }
            free(pSubmits);
        }
        
        LOGD("Host: vkQueueSubmit result=%d submitCount=%d", result, submitCount);
    }
    break;

    case FUNID_vkWaitForFences:
    {
        LOGD("Host: vkWaitForFences");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t fenceCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        VkBool32 waitAll = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint64_t timeout = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        VkFence* fences = NULL;
        if (fenceCount > 0) {
            fences = (VkFence*)malloc(fenceCount * sizeof(VkFence));
            uint64_t* guest_fences = (uint64_t*)(*ptr);
            for (uint32_t i = 0; i < fenceCount; ++i) {
                fences[i] = (VkFence)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_FENCE, guest_fences[i]);
            }
        }
        
        VkResult result = vkWaitForFences(device, fenceCount, fences, waitAll, timeout);

        // write_to_guest_mem(
        //     all_para[1].data, &result, 0, sizeof(VkResult));
        
        if (fences) free(fences);
        
        LOGD("Host: vkWaitForFences result=%d fenceCount=%d waitAll=%d fence %llx", result, fenceCount, waitAll, fences ? fences[0] : 0);
        break;
    }

    case FUNID_vkQueuePresentKHR: //ztodo:目前处理的应该是单swapchain的情况？
    {
        LOGD("Host: vkQueuePresentKHR request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_queue = *(uint64_t*)(*ptr); 
        *ptr += sizeof(uint64_t);
        
        VkQueue queue = (VkQueue)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_QUEUE, guest_queue);
        
        VkPresentInfoKHR presentInfo;
        decode_from_stream_VkPresentInfoKHR(VK_STRUCTURE_TYPE_MAX_ENUM, &presentInfo, ptr);

        uint64_t* buffer_ids = malloc(presentInfo.swapchainCount * sizeof(uint64_t));
        read_from_guest_mem(
            all_para[1].data,
            buffer_ids,
            0,
            sizeof(uint64_t) * presentInfo.swapchainCount);
        
        LOGD("Host: vkQueuePresentKHR queue=%lld swapchainCount=%d buffer %llx", 
            (uint64_t)(uintptr_t)queue, presentInfo.swapchainCount, buffer_ids[0]);
        // 新逻辑：present前后做buffer管理和上屏
        
        vulkan_surface_present_images(queue, &presentInfo, buffer_ids);
        vkQueuePresentKHR(queue, &presentInfo);
        //if (need_free) free(stream);
        break;
    }

    case FUNID_vkGetImageMemoryRequirements: {
        LOGD("Host: vkGetImageMemoryRequirements request");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("get vkGetImageMemoryRequirements param count %d", para_num);

        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        uint64_t guest_device = *(uint64_t*)(*ptr);  *ptr += sizeof(uint64_t);
        uint64_t guest_image  = *(uint64_t*)(*ptr);  *ptr += sizeof(uint64_t);

        void* guest_mem_req_ptr = all_para[1].data;

        VkDevice device = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkImage image   = (VkImage)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_IMAGE, guest_image);

        VkMemoryRequirements memReq;
        vkGetImageMemoryRequirements(device, image, &memReq);

        write_to_guest_mem(guest_mem_req_ptr, &memReq, 0, sizeof(memReq));
        LOGD("Host: vkGetImageMemoryRequirements done with value size %d alignment %d",
             memReq.size, memReq.alignment);
        break;
    }

    case FUNID_vkGetImageMemoryRequirements2:
    {
        LOGD("Host: vkGetImageMemoryRequirements2 request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        VkImageMemoryRequirementsInfo2 info;
        VkMemoryRequirements2 memReqs;
        
        decode_from_stream_VkImageMemoryRequirementsInfo2(VK_STRUCTURE_TYPE_MAX_ENUM, &info, ptr);
        decode_from_stream_VkMemoryRequirements2(VK_STRUCTURE_TYPE_MAX_ENUM, &memReqs, ptr);
        
        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        vkGetImageMemoryRequirements2(device, &info, &memReqs);
        
        write_to_guest_mem(all_para[1].data, &memReqs, 0, sizeof(VkMemoryRequirements2));
    }
    break;

    case FUNID_vkGetPhysicalDeviceMemoryProperties: {
        LOGD("Host: vkGetPhysicalDeviceMemoryProperties request");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("get vkGetPhysicalDeviceMemoryProperties param count %d", para_num);

        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint64_t guest_pd = *(uint64_t*)stream;

        void* guest_props_ptr = all_para[1].data;

        VkPhysicalDevice pd = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_pd);

        VkPhysicalDeviceMemoryProperties props;
        vkGetPhysicalDeviceMemoryProperties(pd, &props);

        write_to_guest_mem(guest_props_ptr, &props, 0, sizeof(props));
        LOGD("Host: vkGetPhysicalDeviceMemoryProperties done with memoryTypeCount %d",
             props.memoryTypeCount);
        for(int i=0; i<props.memoryTypeCount; i++) {
            LOGD("  memoryType[%d] propertyFlags 0x%x heapIndex %d",
                 i, props.memoryTypes[i].propertyFlags, props.memoryTypes[i].heapIndex);
        }
        break;
    }
    
    case FUNID_vkGetPhysicalDeviceMemoryProperties2: {
        LOGD("get call GetGetPhysicalDeviceMemoryProperties2! sizeof VkPhysicalDeviceMemoryProperties2: %lu",
            sizeof(VkPhysicalDeviceMemoryProperties2));
        LOGD("sizeof memory heap: %lu",
            sizeof(VkMemoryHeap));
        LOGD("sizeof memory type: %lu",
            sizeof(VkMemoryType));

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("get vk param number %d", para_num);

        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_physicalDevice = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkPhysicalDevice real_physicalDevice = (VkPhysicalDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_physicalDevice);

        LOGD("physicalDevice = %p, properties = %p",
            guest_physicalDevice, all_para[1].data);

        VkPhysicalDeviceMemoryProperties2 props;
        props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
        props.pNext = NULL;
        vkGetPhysicalDeviceMemoryProperties2(real_physicalDevice, &props);

        write_to_guest_mem(all_para[1].data, &props, 0, sizeof(VkPhysicalDeviceMemoryProperties2));

        LOGD("vkGetPhysicalDeviceMemoryProperties2 physicalDevice = %p, memoryTypeCount = %d",
            guest_physicalDevice,
            props.memoryProperties.memoryTypeCount);

        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkGetPhysicalDeviceProperties: {
        LOGD("get call GetPhysicalDeviceProperties");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("get vk param number %d", para_num);

        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_physicalDevice = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        VkPhysicalDevice real_physicalDevice = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_physicalDevice);

        LOGD("physical_device = %p, properties_ptr = %p",
            guest_physicalDevice,
            all_para[1].data);

        VkPhysicalDeviceProperties pProps;

        vkGetPhysicalDeviceProperties(real_physicalDevice, &pProps);
        LOGD("physical_device = %p, properties = %d %d %x",
            guest_physicalDevice,
            pProps.apiVersion, pProps.driverVersion, pProps.vendorID);
        g_is_intel_gpu = (pProps.vendorID == 0x8086);

        write_to_guest_mem(all_para[1].data, &pProps, 0, sizeof(VkPhysicalDeviceProperties));

        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkGetPhysicalDeviceProperties2: {
        LOGD("get call GetPhysicalDeviceProperties2");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("get vk param number %d", para_num);

        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_physicalDevice = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        VkPhysicalDevice real_physicalDevice = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_physicalDevice);

        VkPhysicalDeviceProperties2 pProps = {0};
        pProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

        vkGetPhysicalDeviceProperties2(real_physicalDevice, &pProps);

        write_to_guest_mem(all_para[1].data, &pProps, 0, sizeof(VkPhysicalDeviceProperties2));
        LOGD("physical_device = %p, properties = %d",
            guest_physicalDevice,
            pProps.properties.apiVersion);

        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkGetPhysicalDeviceQueueFamilyProperties: {
        LOGD("Host: vkGetPhysicalDeviceQueueFamilyProperties request");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("get vkGetPhysicalDeviceQueueFamilyProperties param count %d", para_num);

        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint64_t guest_pd = *(uint64_t*)stream;

        uint32_t count = 0;
        read_from_guest_mem(all_para[1].data, &count, 0, sizeof(count));
        void* guest_props_ptr = all_para[2].data;

        // 映射
        VkPhysicalDevice pd = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_pd);

        VkQueueFamilyProperties* props = NULL;
        VkResult result;

        if (count == 0) {
            vkGetPhysicalDeviceQueueFamilyProperties(pd, &count, NULL);
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(count));
            result = VK_SUCCESS;
        } else {
            props = malloc(sizeof(VkQueueFamilyProperties) * count);
            if (!props) {
                result = VK_ERROR_OUT_OF_HOST_MEMORY;
            } else {
                vkGetPhysicalDeviceQueueFamilyProperties(pd, &count, props);
                            const VkQueueFlags allowed =
                VK_QUEUE_GRAPHICS_BIT |
                VK_QUEUE_COMPUTE_BIT |
                VK_QUEUE_TRANSFER_BIT |
                VK_QUEUE_SPARSE_BINDING_BIT |
                VK_QUEUE_PROTECTED_BIT;

                for (uint32_t i = 0; i < count; ++i) {
                    props[i].queueFlags &= allowed;
                }
                write_to_guest_mem(guest_props_ptr, props, 0,
                                   sizeof(VkQueueFamilyProperties) * count);
                free(props);
                result = VK_SUCCESS;
            }
        }

        LOGD("Host: vkGetPhysicalDeviceQueueFamilyProperties done, count=%d", count);
        break;
    }

    case FUNID_vkGetPhysicalDeviceQueueFamilyProperties2: {
        LOGD("Host: vkGetPhysicalDeviceQueueFamilyProperties2 request");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);

        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint64_t guest_pd = *(uint64_t*)stream;

        uint32_t count = 0;
        read_from_guest_mem(all_para[1].data, &count, 0, sizeof(count));
        void* guest_props_ptr = all_para[2].data;

        // 映射
        VkPhysicalDevice pd = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_pd);

        VkQueueFamilyProperties2* props = NULL;
        VkResult result;

        if (count == 0) {
            vkGetPhysicalDeviceQueueFamilyProperties2(pd, &count, NULL);
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(count));
            result = VK_SUCCESS;
        } else {
            props = malloc(sizeof(VkQueueFamilyProperties2) * count);
            for(uint32_t i = 0; i < count; ++i) {
                props[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
                props[i].pNext = NULL;
            }
            if (!props) {
                result = VK_ERROR_OUT_OF_HOST_MEMORY;
            } else {
                vkGetPhysicalDeviceQueueFamilyProperties2(pd, &count, props);
                const VkQueueFlags allowed =
                VK_QUEUE_GRAPHICS_BIT |
                VK_QUEUE_COMPUTE_BIT |
                VK_QUEUE_TRANSFER_BIT |
                VK_QUEUE_SPARSE_BINDING_BIT |
                VK_QUEUE_PROTECTED_BIT;

                for (uint32_t i = 0; i < count; ++i) {
                    props[i].queueFamilyProperties.queueFlags &= allowed;
                }
                write_to_guest_mem(guest_props_ptr, props, 0,
                                   sizeof(VkQueueFamilyProperties2) * count);
                free(props);
                result = VK_SUCCESS;
            }
        }

        LOGD("Host: vkGetPhysicalDeviceQueueFamilyProperties2 done, count=%d", count);
        break;
    }

    case FUNID_vkGetImageSubresourceLayout: {
        LOGD("Host: vkGetImageSubresourceLayout request");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("get vkGetImageSubresourceLayout param count %d", para_num);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_device = *(uint64_t*)(*ptr);  *ptr += sizeof(uint64_t);
        uint64_t guest_image  = *(uint64_t*)(*ptr);  *ptr += sizeof(uint64_t);

        VkImageSubresource subres;
        read_from_guest_mem(all_para[1].data, &subres, 0, sizeof(subres));

        void* guest_layout_ptr = all_para[2].data;

        VkDevice device = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkImage image   = (VkImage)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_IMAGE, guest_image);

        VkSubresourceLayout layout;
        vkGetImageSubresourceLayout(device, image, &subres, &layout);

        write_to_guest_mem(guest_layout_ptr, &layout, 0, sizeof(layout));
        LOGD("Host: vkGetImageSubresourceLayout done with offset %lld size %lld",
             (long long)layout.offset, (long long)layout.size);
        break;
    }

    case FUNID_vkCreateImage: {
        LOGD("Host: vkCreateImage request");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("get vkCreateImage para count %d", para_num);

        int need_free = 0;
        char* buf = call_para_to_ptr(all_para[0], &need_free);
        uint8_t* ptr = (uint8_t*)buf;

        VkImageCreateInfo createInfo;
        decode_from_stream_VkImageCreateInfo(
            VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            &createInfo,
            (uint8_t**)&ptr);
        uint64_t guest_alloc_ptr = *(uint64_t*)ptr;
        ptr += sizeof(uint64_t);

        uint64_t guest_device = *(uint64_t*)ptr;  ptr += sizeof(uint64_t);
        uint64_t guest_image  = *(uint64_t*)ptr;  ptr += sizeof(uint64_t);
        LOGD("Decoded createInfo + guest_alloc=0x%llx, device=0x%llx, image=0x%llx layout %d",
            guest_alloc_ptr, guest_device, guest_image, createInfo.initialLayout);

        VkDevice device = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);

        VkImage hostImage;
        VkResult result = vkCreateImage(device, &createInfo, NULL, &hostImage);
        LOGD("vkCreateImage %d, hostImage=0x%llx", result, (uint64_t)(uintptr_t)hostImage);

        if (result == VK_SUCCESS) {
            insert_mapping(EXPRESS_VK_OBJECT_TYPE_IMAGE,
                        guest_image,
                        (uint64_t)(uintptr_t)hostImage);
            LOGD("Mapped guest_image 0x%llx → hostImage 0x%llx",
                guest_image, (uint64_t)(uintptr_t)hostImage);
        } else {
            LOGE("vkCreateImage failed with error %d", result);
        }

        write_to_guest_mem(all_para[1].data, &result, 0, sizeof(result));

        if (need_free) free(buf);
        break;
    }

    case FUNID_vkCreateSampler: {
        LOGD("Host: vkCreateSampler request");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("get vkCreateSampler para count %d", para_num);

        int need_free = 0;
        char* buf = call_para_to_ptr(all_para[0], &need_free);
        uint8_t* ptr = (uint8_t*)buf;

        VkSamplerCreateInfo samplerInfo;
        decode_from_stream_VkSamplerCreateInfo(
            VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            &samplerInfo,
            (uint8_t**)&ptr);
        uint64_t guest_alloc_ptr = *(uint64_t*)ptr;
        ptr += sizeof(uint64_t);

        uint64_t guest_device  = *(uint64_t*)ptr; ptr += sizeof(uint64_t);
        uint64_t guest_sampler = *(uint64_t*)ptr; ptr += sizeof(uint64_t);
        LOGD("Decoded samplerInfo + guest_alloc=0x%llx, device=0x%llx, sampler=0x%llx",
            guest_alloc_ptr, guest_device, guest_sampler);

        VkDevice device = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);

        VkSampler hostSampler;
        VkResult result = vkCreateSampler(device, &samplerInfo, NULL, &hostSampler);
        LOGD("vkCreateSampler → %d, hostSampler=0x%llx", result, (uint64_t)(uintptr_t)hostSampler);

        if (result == VK_SUCCESS) {
            insert_mapping(EXPRESS_VK_OBJECT_TYPE_SAMPLER,
                        guest_sampler,
                        (uint64_t)(uintptr_t)hostSampler);
            LOGD("Mapped guest_sampler 0x%llx → hostSampler 0x%llx",
                guest_sampler, (uint64_t)(uintptr_t)hostSampler);
        } else {
            LOGE("vkCreateSampler failed with error %d", result);
        }

        write_to_guest_mem(all_para[1].data, &result, 0, sizeof(result));

        if (need_free) free(buf);
        break;
    }

    case FUNID_vkCreateDescriptorSetLayout: {
        LOGD("Host: vkCreateDescriptorSetLayout request");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("get vkCreateDescriptorSetLayout para count %d", para_num);

        int need_free = 0;
        char* buf = call_para_to_ptr(all_para[0], &need_free);
        uint8_t* ptr = (uint8_t*)buf;

        VkDescriptorSetLayoutCreateInfo layoutInfo;
        decode_from_stream_VkDescriptorSetLayoutCreateInfo(
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            &layoutInfo,
            (uint8_t**)&ptr);
        uint64_t guest_alloc_ptr = *(uint64_t*)ptr;
        ptr += sizeof(uint64_t);

        uint64_t guest_device    = *(uint64_t*)ptr; ptr += sizeof(uint64_t);
        uint64_t guest_layout    = *(uint64_t*)ptr; ptr += sizeof(uint64_t);
        LOGD("Decoded layoutInfo + guest_alloc=0x%llx, device=0x%llx, layout=0x%llx",
            guest_alloc_ptr, guest_device, guest_layout);

        VkDevice device = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);

        VkDescriptorSetLayout hostLayout;
        VkResult result = vkCreateDescriptorSetLayout(
            device, &layoutInfo, NULL, &hostLayout);
        LOGD("vkCreateDescriptorSetLayout → %d, hostLayout=0x%llx",
            result, (uint64_t)(uintptr_t)hostLayout);

        if (result == VK_SUCCESS) {
            insert_mapping(EXPRESS_VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT,
                        guest_layout,
                        (uint64_t)(uintptr_t)hostLayout);
            LOGD("Mapped guest_layout 0x%llx → hostLayout 0x%llx",
                guest_layout, (uint64_t)(uintptr_t)hostLayout);
        } else {
            LOGE("vkCreateDescriptorSetLayout failed with error %d", result);
        }

        // write_to_guest_mem(all_para[1].data, &result, 0, sizeof(result));

        if (need_free) free(buf);
        break;
    }

    case FUNID_vkBindImageMemory: {
        LOGD("Host: vkBindImageMemory request");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("get vkBindImageMemory para count %d", para_num);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_device = *(uint64_t*)(*ptr);  *ptr += sizeof(uint64_t);
        uint64_t guest_image  = *(uint64_t*)(*ptr);  *ptr += sizeof(uint64_t);
        uint64_t guest_mem    = *(uint64_t*)(*ptr);  *ptr += sizeof(uint64_t);
        uint64_t offset       = *(uint64_t*)(*ptr);  *ptr += sizeof(uint64_t);
        LOGD("guest_device=0x%llx, guest_image=0x%llx, guest_mem=0x%llx, offset=%llu",
             guest_device, guest_image, guest_mem, offset);

        VkDevice device = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkImage  image  = (VkImage)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_IMAGE, guest_image);
        VkDeviceMemory mem = (VkDeviceMemory)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE_MEMORY, guest_mem);

        VkResult result = vkBindImageMemory(device, image, mem, offset);
        LOGD("vkBindImageMemory returned %d", result);

        break;
    }

    case FUNID_vkFreeDescriptorSets: {
        LOGD("Host: vkFreeDescriptorSets request");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("get vkFreeDescriptorSets para count %d", para_num);

        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        uint64_t guest_device = *(uint64_t*)(*ptr);  *ptr += sizeof(uint64_t);
        uint64_t guest_pool   = *(uint64_t*)(*ptr);  *ptr += sizeof(uint64_t);
        uint32_t count        = *(uint32_t*)(*ptr);  *ptr += sizeof(uint32_t);
        LOGD("guest_device=0x%llx, guest_pool=0x%llx, count=%d",
             guest_device, guest_pool, count);

        VkDevice device = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        LOGD("current guest_pool is 0x%llx, host device is 0x%llx",
             guest_pool, (uint64_t)(uintptr_t)device);
        VkDescriptorPool pool = (VkDescriptorPool)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DESCRIPTOR_POOL, guest_pool);

        VkDescriptorSet* hostSets = NULL;
        if (count > 0) {
            int need_free = 0;
            // char* ret_ptr = call_para_to_ptr(all_para[1], &need_free);
            // uint64_t* guest_sets = (uint64_t*)ret_ptr;
            hostSets = malloc(count * sizeof(VkDescriptorSet));
            uint64_t* guest_sets = malloc(count * sizeof(uint64_t));
            for (uint32_t i = 0; i < count; ++i) {
                guest_sets[i] = *(uint64_t*)(*ptr);
                *ptr += sizeof(uint64_t);
                hostSets[i] = (VkDescriptorSet)(uintptr_t)
                    lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DESCRIPTOR_SET, guest_sets[i]);
                LOGD("  mapped guest_set[%u]=0x%llx to host 0x%llx",
                     i, guest_sets[i], (uint64_t)(uintptr_t)hostSets[i]);
            }
            if (guest_sets) free(guest_sets);
            // if (need_free) free(ret_ptr);
        }

        VkResult result = vkFreeDescriptorSets(device, pool, count, hostSets);
        LOGD("vkFreeDescriptorSets returned %d", result);
        if (hostSets) free(hostSets);
        LOGD("Host: vkFreeDescriptorSets not implemented yet, skipping actual call");
        
        break;
    }

    case FUNID_vkCreateDescriptorPool:
    {
        LOGD("Host: vkCreateDescriptorPool request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("get vk param number %d", para_num);

        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** stream_ptr = (uint8_t**)&stream;

        VkDescriptorPoolCreateInfo* pCreateInfo = (VkDescriptorPoolCreateInfo*)malloc(sizeof(VkDescriptorPoolCreateInfo));
        decode_from_stream_VkDescriptorPoolCreateInfo(
            VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            pCreateInfo,
            stream_ptr);

        VkAllocationCallbacks* guest_allocator = (VkAllocationCallbacks*)*(uint64_t*)(*stream_ptr);
        *stream_ptr += sizeof(uint64_t);

        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_allocator) {
            VkAllocationCallbacks* temp_allocator = (VkAllocationCallbacks*)malloc(sizeof(VkAllocationCallbacks));
            decode_from_stream_VkAllocationCallbacks(
                VK_STRUCTURE_TYPE_MAX_ENUM,
                temp_allocator,
                stream_ptr);

            free(temp_allocator);
        }
        
        uint64_t guest_device = *(uint64_t*)(*stream_ptr);
        *stream_ptr += sizeof(uint64_t);

        uint64_t guest_descriptor_pool = *(uint64_t*)(*stream_ptr);
        *stream_ptr += sizeof(uint64_t);

        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        LOGD("Host: device mapping guest=%llx host=%llx", guest_device, (uint64_t)(uintptr_t)device);

        VkDescriptorPool descriptor_pool;
        VkResult result = vkCreateDescriptorPool(device, pCreateInfo, pAllocator, &descriptor_pool);
        
        if (result == VK_SUCCESS) {
            LOGD("Host: vkCreateDescriptorPool success, guest=%llx host=%llx", 
                guest_descriptor_pool, (uint64_t)(uintptr_t)descriptor_pool);

            insert_mapping(EXPRESS_VK_OBJECT_TYPE_DESCRIPTOR_POOL, 
                        guest_descriptor_pool, 
                        (uint64_t)(uintptr_t)descriptor_pool);
        } else {
            LOGD("Host: vkCreateDescriptorPool failed with result=%d", result);
        }

        free(pCreateInfo);
        if (need_free) {
            free(stream);
        }

        break;
    }

    case FUNID_vkFlushMappedMemoryRanges:
    {
        LOGD("Host: vkFlushMappedMemoryRanges request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        uint32_t memoryRangeCount = *(uint32_t*)(*ptr);
        *ptr += sizeof(uint32_t);
        
        VkMappedMemoryRange* pMemoryRanges = NULL;
        if (memoryRangeCount > 0) {
            pMemoryRanges = (VkMappedMemoryRange*)malloc(
                memoryRangeCount * sizeof(VkMappedMemoryRange));
            
            for (uint32_t i = 0; i < memoryRangeCount; ++i) {
                decode_from_stream_VkMappedMemoryRange(VK_STRUCTURE_TYPE_MAX_ENUM, 
                                                    &pMemoryRanges[i], ptr);
            }
        }
        for (uint32_t i = 0; i < memoryRangeCount; ++i) {
            void* hostPtr = get_memory_map((uint64_t)pMemoryRanges[i].memory);
            if (hostPtr) {
                // all_para[1].data 对应 addPtr(mem->map_data, mem->length)
                // all_para[1].size 存储了 mem->length
                read_from_guest_mem(
                    all_para[1+i].data,
                    hostPtr,
                    0,
                    all_para[1+i].data_len);
                LOGD("Host: synced %zu bytes to mappedPtr %p",
                    (size_t)all_para[1+i].data_len, hostPtr);
            } else {
                LOGE("Host: no mapping found for guest_mem %llu", pMemoryRanges[i].memory);
            }
        }
        
        VkResult result = vkFlushMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
        
        if (result != VK_SUCCESS) {
            LOGE("Host: vkFlushMappedMemoryRanges failed with result %d", result);
        }
        
        if (pMemoryRanges) free(pMemoryRanges);
        //if (need_free) free(stream);    
    }
    break;

    case FUNID_vkFreeCommandBuffers:
    {
        LOGD("Host: vkFreeCommandBuffers request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_device = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        uint64_t guest_command_pool = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        uint32_t commandBufferCount = *(uint32_t*)(*ptr);
        *ptr += sizeof(uint32_t);
        
        VkDevice device = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        VkCommandPool commandPool = (VkCommandPool)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_POOL, guest_command_pool);
        
        VkCommandBuffer* pCommandBuffers = malloc(commandBufferCount * sizeof(VkCommandBuffer));
        if (!pCommandBuffers) {
            break;
        }
        
        for (uint32_t i = 0; i < commandBufferCount; ++i) {
            uint64_t guest_cmd_buffer = *(uint64_t*)(*ptr);
            *ptr += sizeof(uint64_t);
            
            pCommandBuffers[i] = (VkCommandBuffer)(uintptr_t)
                lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buffer);

            remove_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buffer);
        }
        
        vkFreeCommandBuffers(device, commandPool, commandBufferCount, pCommandBuffers);
        
        free(pCommandBuffers);
    }
    break;

    case FUNID_vkResetCommandBuffer: {
        LOGD("Host: vkResetCommandBuffer");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_cmd_buf = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkCommandBufferResetFlags flags = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        VkCommandBuffer cmd_buf = (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buf);
        VkResult result = vkResetCommandBuffer(cmd_buf, flags);
        LOGD("Host: vkResetCommandBuffer result=%d", result);
        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkResetCommandPool: {
        LOGD("Host: vkResetCommandPool");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_pool = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkCommandPoolResetFlags flags = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkCommandPool pool = (VkCommandPool)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_POOL, guest_pool);
        VkResult result = vkResetCommandPool(device, pool, flags);
        LOGD("Host: vkResetCommandPool result=%d", result);
        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkResetDescriptorPool: {
        LOGD("Host: vkResetDescriptorPool");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_pool = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDescriptorPoolResetFlags flags = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkDescriptorPool pool = (VkDescriptorPool)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DESCRIPTOR_POOL, guest_pool);
        VkResult result = vkResetDescriptorPool(device, pool, flags);
        LOGD("Host: vkResetDescriptorPool result=%d", result);
        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkResetEvent: {
        LOGD("Host: vkResetEvent");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_event = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkEvent event = (VkEvent)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_EVENT, guest_event);
        VkResult result = vkResetEvent(device, event);
        LOGD("Host: vkResetEvent result=%d", result);
        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkResetQueryPool: {
        LOGD("Host: vkResetQueryPool");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_pool = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t firstQuery = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t queryCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkQueryPool pool = (VkQueryPool)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_QUERY_POOL, guest_pool);
        vkResetQueryPool(device, pool, firstQuery, queryCount);
        LOGD("Host: vkResetQueryPool device=%p pool=%p firstQuery=%u queryCount=%u", (void*)device, (void*)pool, firstQuery, queryCount);
        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkAllocateDescriptorSets:
    {
        LOGD("Host: vkAllocateDescriptorSets request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("get vk param number %d", para_num);
        
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_device = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);

        VkDescriptorSetAllocateInfo allocate_info;
        decode_from_stream_VkDescriptorSetAllocateInfo(VK_STRUCTURE_TYPE_MAX_ENUM, &allocate_info, ptr);

        LOGD("get descripter set count %d", allocate_info.descriptorSetCount);

        VkDescriptorSet* host_descriptor_sets = (VkDescriptorSet*)malloc(
            allocate_info.descriptorSetCount * sizeof(VkDescriptorSet));
        
        VkResult result = vkAllocateDescriptorSets(device, &allocate_info, host_descriptor_sets);
        
        if (result == VK_SUCCESS) {
            uint64_t* guest_descriptor_sets = (uint64_t*)malloc(
                allocate_info.descriptorSetCount * sizeof(uint64_t));
            read_from_guest_mem(all_para[1].data, guest_descriptor_sets, 0, 
                            allocate_info.descriptorSetCount * sizeof(uint64_t));
            
            for (uint32_t i = 0; i < allocate_info.descriptorSetCount; ++i) {
                uint64_t host_desc_set = (uint64_t)(uintptr_t)host_descriptor_sets[i];
                insert_mapping(EXPRESS_VK_OBJECT_TYPE_DESCRIPTOR_SET, 
                            guest_descriptor_sets[i], host_desc_set);
                LOGD("Host: mapped descriptor set %d: guest %lld -> host %lld", 
                    i, guest_descriptor_sets[i], host_desc_set);
            }
            
            free(guest_descriptor_sets);
        } else {
            LOGE("Host: vkAllocateDescriptorSets failed with result %d", result);
        }
        
        // free(host_layouts);
        free(host_descriptor_sets);
        
        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkUpdateDescriptorSets:
    {
        LOGD("Host: vkUpdateDescriptorSets request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("get vk param number %d", para_num);
        
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_device = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);

        uint32_t descriptorWriteCount = *(uint32_t*)(*ptr);
        *ptr += sizeof(uint32_t);
        uint32_t descriptorCopyCount = *(uint32_t*)(*ptr);
        *ptr += sizeof(uint32_t);
        
        LOGD("Host: vkUpdateDescriptorSets write count %d, copy count %d", 
            descriptorWriteCount, descriptorCopyCount);

        VkWriteDescriptorSet* pDescriptorWrites = NULL;
        if (descriptorWriteCount > 0) {
            pDescriptorWrites = (VkWriteDescriptorSet*)malloc(
                descriptorWriteCount * sizeof(VkWriteDescriptorSet));
            
            for (uint32_t i = 0; i < descriptorWriteCount; ++i) {
                decode_from_stream_VkWriteDescriptorSet(VK_STRUCTURE_TYPE_MAX_ENUM, 
                                                    &pDescriptorWrites[i], ptr);
                // LOGD("Host: vkUpdateDescriptorSets copy %d: %llx",
                //     i, (long long)pDescriptorWrites[i].pImageInfo[0].imageView);
                // if (pDescriptorWrites[i].descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
                    // pDescriptorWrites[i].descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE) {
                
                    // 遍历这个write中的所有图像
                //     for (uint32_t j = 0; j < pDescriptorWrites[i].descriptorCount; ++j) {
                //         VkImageView imageView = pDescriptorWrites[i].pImageInfo[j].imageView;
                        
                //         // 通过ImageView找到对应的Image
                //         VkImage hostImage = getImageFromImageView(imageView); // 你需要实现这个函数
                        
                //         if (hostImage != VK_NULL_HANDLE) {
                //             LOGD("Host: Found image for layout transition, imageView=%llx", 
                //                 (long long)imageView);
                            
                //             // 执行布局转换
                //             // transitionImageLayoutForSampling(device, hostImage, pDescriptorWrites[i].pImageInfo[j].imageLayout);

                //             VkDescriptorImageInfo imageInfo = pDescriptorWrites[i].pImageInfo[j];
                //         }
                //     }
                // }          
            }
            LOGD("Host: vkUpdateDescriptorSets descriptor writes prepared");

        }

        VkCopyDescriptorSet* pDescriptorCopies = NULL;
        if (descriptorCopyCount > 0) {
            pDescriptorCopies = (VkCopyDescriptorSet*)malloc(
                descriptorCopyCount * sizeof(VkCopyDescriptorSet));
            
            for (uint32_t i = 0; i < descriptorCopyCount; ++i) {
                decode_from_stream_VkCopyDescriptorSet(VK_STRUCTURE_TYPE_MAX_ENUM, 
                                                    &pDescriptorCopies[i], ptr);
                // LOGD("Host: vkUpdateDescriptorSets copy %d: %llx",
                //     i, (long long)pDescriptorCopies[i].pImageInfo.imageView);
            }
        }

        vkUpdateDescriptorSets(device, descriptorWriteCount, pDescriptorWrites, 
                            descriptorCopyCount, pDescriptorCopies);
        
        LOGD("Host: vkUpdateDescriptorSets completed successfully");

        if (pDescriptorWrites) free(pDescriptorWrites);
        if (pDescriptorCopies) free(pDescriptorCopies);
        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkCmdBindDescriptorSets:
    {
        LOGD("Host: vkCmdBindDescriptorSets request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkPipelineBindPoint bindPoint = *(VkPipelineBindPoint*)(*ptr); *ptr += sizeof(uint32_t);
        uint64_t guest_layout = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t firstSet = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t setCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t dynamicOffsetCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        VkPipelineLayout layout = (VkPipelineLayout)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PIPELINE_LAYOUT, guest_layout);
        
        VkDescriptorSet* descriptorSets = NULL;
        uint32_t* dynamicOffsets = NULL;
        
        if (setCount > 0 && para_num > 1) {
            uint64_t* guest_sets = (uint64_t*)malloc(setCount * sizeof(uint64_t));
            read_from_guest_mem(all_para[1].data, guest_sets, 0, setCount * sizeof(uint64_t));
            
            descriptorSets = (VkDescriptorSet*)malloc(setCount * sizeof(VkDescriptorSet));
            for (uint32_t i = 0; i < setCount; ++i) {
                descriptorSets[i] = (VkDescriptorSet)(uintptr_t)
                    lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DESCRIPTOR_SET, guest_sets[i]);
            }
            free(guest_sets);
            
            if (para_num > 2) {
                // Read dynamic offset count from guest memory structure
                if (dynamicOffsetCount > 0) {
                    dynamicOffsets = (uint32_t*)malloc(dynamicOffsetCount * sizeof(uint32_t));
                    read_from_guest_mem(all_para[2].data, dynamicOffsets, 0, dynamicOffsetCount * sizeof(uint32_t));
                }
            }
        } else if(dynamicOffsetCount > 0) {
            // If no descriptor sets, but dynamic offsets are provided
            dynamicOffsets = (uint32_t*)malloc(dynamicOffsetCount * sizeof(uint32_t));
            read_from_guest_mem(all_para[1].data, dynamicOffsets, 0, dynamicOffsetCount * sizeof(uint32_t));
        }
        LOGD("Host: vkCmdBindDescriptorSets commandBuffer=%p bindPoint=%d layout=%p firstSet=%d setCount=%d dynamicOffsetCount=%d",
            (void*)commandBuffer, bindPoint, (void*)layout, firstSet, setCount, dynamicOffsetCount);
        
        vkCmdBindDescriptorSets(commandBuffer, bindPoint, layout, firstSet, setCount,
                            descriptorSets, dynamicOffsetCount, dynamicOffsets);
        
        if (descriptorSets) free(descriptorSets);
        if (dynamicOffsets) free(dynamicOffsets);
    }
    break;

    case FUNID_vkCmdCopyImage:
    {
        LOGD("Host: vkCmdCopyImage request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_src = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkImageLayout srcLayout = *(VkImageLayout*)(*ptr); *ptr += sizeof(uint32_t);
        uint64_t guest_dst = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkImageLayout dstLayout = *(VkImageLayout*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t regionCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        VkImage srcImage = (VkImage)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_IMAGE, guest_src);
        VkImage dstImage = (VkImage)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_IMAGE, guest_dst);
        
        VkImageCopy* regions = NULL;
        if (regionCount > 0 && para_num > 1) {
            regions = (VkImageCopy*)malloc(regionCount * sizeof(VkImageCopy));
            read_from_guest_mem(all_para[1].data, regions, 0, regionCount * sizeof(VkImageCopy));
        }
        
        vkCmdCopyImage(commandBuffer, srcImage, srcLayout, dstImage, dstLayout, regionCount, regions);
        
        if (regions) free(regions);
    }
    break;
    case FUNID_vkFreeMemory:
    {
        LOGD("Host: vkFreeMemory request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_memory = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkDeviceMemory memory = (VkDeviceMemory)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE_MEMORY, guest_memory);
        
        vkFreeMemory(device, memory, NULL);
        uint64_t gbuffer_id = lookup_memory_gbuffer_mapping((uint64_t)(uintptr_t)memory);
        if (gbuffer_id != 0) {
            remove_gbuffer_memory_mapping(gbuffer_id);
        }
        
        remove_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE_MEMORY, guest_memory);
        
    }
    break;

    case FUNID_vkGetPhysicalDeviceFormatProperties:
    {
        LOGD("Host: vkGetPhysicalDeviceFormatProperties request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkFormat format = *(VkFormat*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_device);
        
        VkFormatProperties properties;

        // ztodo:不确定对VK_FORMAT_D32_SFLOAT_S8_UINT支持的bug是我本人电脑的问题还是pc都有的问题
        // 我的1660ti查询的时候会返回支持VK_FORMAT_D32_SFLOAT_S8_UINT，但实际会导致卡死和驱动丢失
        VkFormat supported_format = VK_FORMAT_D32_SFLOAT;
        if(g_is_intel_gpu) {
            supported_format = VK_FORMAT_D24_UNORM_S8_UINT;
        }
        if (format == supported_format || 
            format == VK_FORMAT_R8G8B8A8_UNORM ||
            format == VK_FORMAT_B8G8R8A8_UNORM || 
            format == VK_FORMAT_R8G8B8A8_SRGB ||
            format == VK_FORMAT_B8G8R8A8_SRGB) {
            
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &properties);
            
            // 对于颜色格式，不需要过滤特性
            if (format == VK_FORMAT_D32_SFLOAT) {
                const VkFormatFeatureFlags allowed =
                    VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT |
                    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
                    VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                    VK_FORMAT_FEATURE_TRANSFER_DST_BIT;

                properties.linearTilingFeatures  &= allowed;
                properties.optimalTilingFeatures &= allowed;
                properties.bufferFeatures        &= allowed;
            }
        // 颜色格式保留所有特性
        
        LOGD("Host: format %d is supported with features: 0x%08X", format, properties.optimalTilingFeatures);
    } else {
        // 其他格式仍然返回0（如果需要）
        properties.linearTilingFeatures  = 0;
        properties.optimalTilingFeatures = 0;
        properties.bufferFeatures        = 0;

        LOGD("Host: format %d is not supported", format);
    }

        // 写入返回值到 guest 内存
        write_to_guest_mem(all_para[1].data, &properties, 0, sizeof(VkFormatProperties));
    }
    break;

    case FUNID_vkGetPhysicalDeviceFormatProperties2: {
        LOGD("get call GetPhysicalDeviceFormatProperties2");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("get vk param number %d", para_num);

        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_physicalDevice = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkPhysicalDevice real_physicalDevice = (VkPhysicalDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_physicalDevice);
        VkFormat format = (VkFormat)(*ptr); *ptr += sizeof(uint32_t);

        LOGD("host: physicalDevice = %p, format = %u, pFormatProperties = %p",
            guest_physicalDevice, format, all_para[1].data);

        VkFormatProperties2 props;
        vkGetPhysicalDeviceFormatProperties2(real_physicalDevice, format, &props);
        VkFormatFeatureFlags allowed =
            VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT |
            VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
            VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
            VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
            VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
            VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
            VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
            VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
            VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
            VK_FORMAT_FEATURE_BLIT_SRC_BIT |
            VK_FORMAT_FEATURE_BLIT_DST_BIT |
            VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
            VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
            VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
            VK_FORMAT_FEATURE_DISJOINT_BIT |
            VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
            VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
            VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
            VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
            VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
            VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;

        VkFormatProperties* p = &props.formatProperties;
        p->linearTilingFeatures &= allowed;
        p->optimalTilingFeatures &= allowed;
        p->bufferFeatures &= allowed;
        

        write_to_guest_mem(all_para[1].data, &props, 0, sizeof(VkFormatProperties2));

        LOGD("get PhysicalDeviceFormatProperties2: "
              "linearTilingFeatures = %x, optimalTilingFeatures = %x, bufferFeatures = %x",
              p->linearTilingFeatures, p->optimalTilingFeatures, p->bufferFeatures);

        //if (need_free) free(stream);
    }
    break;    

    case FUNID_vkGetPhysicalDeviceImageFormatProperties: {
        LOGD("get call vkGetPhysicalDeviceImageFormatProperties");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("get vk param number %d", para_num);

        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t* ptr = (uint8_t*)stream;  // 修改这里：不需要二级指针

        // 正确读取参数
        uint64_t guest_physicalDevice = *(uint64_t*)ptr; 
        ptr += sizeof(uint64_t);
        
        VkPhysicalDevice real_physicalDevice = (VkPhysicalDevice)(uintptr_t)lookup_mapping(
            EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_physicalDevice);
        
        VkFormat format = *(VkFormat*)ptr;  // 修改：先解引用
        ptr += sizeof(uint32_t);
        
        VkImageType type = *(VkImageType*)ptr;  // 修改：先解引用
        ptr += sizeof(uint32_t);
        
        VkImageTiling tiling = *(VkImageTiling*)ptr;  // 修改：先解引用
        ptr += sizeof(uint32_t);
        
        VkImageUsageFlags usage = *(VkImageUsageFlags*)ptr;  // 修改：先解引用
        ptr += sizeof(uint32_t);
        
        VkImageCreateFlags flags = *(VkImageCreateFlags*)ptr;  // 修改：先解引用
        ptr += sizeof(uint32_t);

        VkImageFormatProperties pProps;
        VkResult result = vkGetPhysicalDeviceImageFormatProperties(
            real_physicalDevice, format, type, tiling, usage, flags, &pProps);

        if (result == VK_SUCCESS) {
            write_to_guest_mem(all_para[1].data, &pProps, 0, sizeof(VkImageFormatProperties));
            LOGD("Succeeded to get image format properties: maxExtent(%u, %u, %u), maxMipLevels=%u, maxArrayLayers=%u", 
                pProps.maxExtent.width, pProps.maxExtent.height, pProps.maxExtent.depth,
                pProps.maxMipLevels, pProps.maxArrayLayers);
        } else {
            LOGW("Failed to get image format properties: %d", result);
        }

        //if (need_free) free(stream);

    }
    break;    

    case FUNID_vkGetPhysicalDeviceImageFormatProperties2: {
        LOGD("get call vkGetPhysicalDeviceImageFormatProperties2");

        const VkPhysicalDeviceImageFormatInfo2* formatInfo = malloc(sizeof(VkPhysicalDeviceImageFormatInfo2));
        if (!formatInfo) {
            LOGE("Failed to allocate memory for VkPhysicalDeviceImageFormatInfo2");
        }
        else{
            int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
            LOGD("get vk param number %d", para_num);

            int need_free = 0;
            char* stream = call_para_to_ptr(all_para[0], &need_free);
            uint8_t** ptr = (uint8_t**)&stream;
            
            uint64_t guest_physicalDevice = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
            VkPhysicalDevice real_physicalDevice = (VkPhysicalDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_physicalDevice);
            decode_from_stream_VkPhysicalDeviceImageFormatInfo2(VK_STRUCTURE_TYPE_MAX_ENUM, formatInfo, ptr);

            LOGD("physicalDevice=%p, pImageFormatProperties=%p",guest_physicalDevice, all_para[1].data);
            
            VkImageFormatProperties2 pProps;
            VkResult result = vkGetPhysicalDeviceImageFormatProperties2(real_physicalDevice, formatInfo, &pProps);

            if (result == VK_SUCCESS) {
                write_to_guest_mem(all_para[1].data, &pProps, 0, sizeof(VkImageFormatProperties2));
                LOGD("Succeeded to get image format properties: %d %d %d %d", result, 
                    pProps.imageFormatProperties.maxExtent.width, 
                    pProps.imageFormatProperties.maxExtent.height, 
                    pProps.imageFormatProperties.maxExtent.depth);
            } else {
                LOGW("Failed to get image format properties: %d", result);
            }

            //if (need_free) free(stream);
            free(formatInfo);
        }
        
    }
    break;

    case FUNID_vkInvalidateMappedMemoryRanges:
    {
        LOGD("Host: vkInvalidateMappedMemoryRanges request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t rangeCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkDevice device = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        VkMappedMemoryRange* ranges = NULL;
        if (rangeCount > 0 && para_num > 1) {
            VkMappedMemoryRange* guest_ranges = (VkMappedMemoryRange*)malloc(rangeCount * sizeof(VkMappedMemoryRange));
            read_from_guest_mem(all_para[1].data, guest_ranges, 0, rangeCount * sizeof(VkMappedMemoryRange));
            
            ranges = (VkMappedMemoryRange*)malloc(rangeCount * sizeof(VkMappedMemoryRange));
            for (uint32_t i = 0; i < rangeCount; ++i) {
                ranges[i] = guest_ranges[i];
                uint64_t guest_memory = (uint64_t)(uintptr_t)guest_ranges[i].memory;
                ranges[i].memory = (VkDeviceMemory)(uintptr_t)
                    lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE_MEMORY, guest_memory);
            }
            free(guest_ranges);
        }
        
        VkResult result = vkInvalidateMappedMemoryRanges(device, rangeCount, ranges);
        
        write_to_guest_mem(all_para[2].data, &result, 0, sizeof(VkResult));
        
        if (ranges) free(ranges);
        
    }
    break;

    case FUNID_vkBindBufferMemory2:
    {
        LOGD("Host: vkBindBufferMemory2 request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint32_t bindInfoCount = *(uint32_t*)(*ptr);
        *ptr += sizeof(uint32_t);
        
        VkBindBufferMemoryInfo* pBindInfos = malloc(bindInfoCount * sizeof(VkBindBufferMemoryInfo));
        for (uint32_t i = 0; i < bindInfoCount; ++i) {
            decode_from_stream_VkBindBufferMemoryInfo(VK_STRUCTURE_TYPE_MAX_ENUM, &pBindInfos[i], ptr);
        }
        
        uint64_t guest_device = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        VkResult result = vkBindBufferMemory2(device, bindInfoCount, pBindInfos);
        if (result != VK_SUCCESS) {
            LOGE("vkBindBufferMemory2 failed: %d", result);
        }
        
        free(pBindInfos);
    }
    break;

    case FUNID_vkCmdBeginQuery:
    {
        LOGD("Host: vkCmdBeginQuery request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_pool = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t query = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t flags = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        VkQueryPool queryPool = (VkQueryPool)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_QUERY_POOL, guest_pool);
        
        vkCmdBeginQuery(commandBuffer, queryPool, query, flags);
    }
    break;

    case FUNID_vkCmdCopyQueryPoolResults:
    {
        LOGD("Host: vkCmdCopyQueryPoolResults request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_pool = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t firstQuery = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t queryCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint64_t guest_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDeviceSize dstOffset = *(VkDeviceSize*)(*ptr); *ptr += sizeof(VkDeviceSize);
        VkDeviceSize stride = *(VkDeviceSize*)(*ptr); *ptr += sizeof(VkDeviceSize);
        uint32_t flags = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        VkQueryPool queryPool = (VkQueryPool)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_QUERY_POOL, guest_pool);
        VkBuffer dstBuffer = (VkBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER, guest_buffer);
        
        vkCmdCopyQueryPoolResults(commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
    }
    break;

    case FUNID_vkCmdDispatchIndirect:
    {
        LOGD("Host: vkCmdDispatchIndirect request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDeviceSize offset = *(VkDeviceSize*)(*ptr); *ptr += sizeof(VkDeviceSize);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        VkBuffer buffer = (VkBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER, guest_buffer);
        LOGD("Host: vkCmdDispatchIndirect commandBuffer=%p buffer=%p offset=%llu",
            (void*)commandBuffer, (void*)buffer, offset);
        
        vkCmdDispatchIndirect(commandBuffer, buffer, offset);
    }
    break;

    case FUNID_vkCmdDrawIndexedIndirect:
    {
        LOGD("Host: vkCmdDrawIndexedIndirect request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDeviceSize offset = *(VkDeviceSize*)(*ptr); *ptr += sizeof(VkDeviceSize);
        uint32_t drawCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t stride = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        VkBuffer buffer = (VkBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER, guest_buffer);
        
        vkCmdDrawIndexedIndirect(commandBuffer, buffer, offset, drawCount, stride);
    }
    break;

    case FUNID_vkCmdDrawIndirect:
    {
        LOGD("Host: vkCmdDrawIndirect request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDeviceSize offset = *(VkDeviceSize*)(*ptr); *ptr += sizeof(VkDeviceSize);
        uint32_t drawCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t stride = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        VkBuffer buffer = (VkBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER, guest_buffer);
        
        vkCmdDrawIndirect(commandBuffer, buffer, offset, drawCount, stride);
    }
    break;

    case FUNID_vkCmdEndQuery:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_pool = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t query = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        VkQueryPool queryPool = (VkQueryPool)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_QUERY_POOL, guest_pool);
        LOGD("Host: vkCmdEndQuery commandBuffer=%p queryPool=%p query=%u",
            (void*)commandBuffer, (void*)queryPool, query);
        
        vkCmdEndQuery(commandBuffer, queryPool, query);
    }
    break;

    case FUNID_vkCmdFillBuffer:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDeviceSize dstOffset = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDeviceSize size = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t data = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        VkBuffer dstBuffer = (VkBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER, guest_buffer);
        
        vkCmdFillBuffer(commandBuffer, dstBuffer, dstOffset, size, data);
        LOGD("Host: vkCmdFillBuffer commandBuffer=%p dstBuffer=%p dstOffset=%llu size=%llu data=%u",
            (void*)commandBuffer, (void*)dstBuffer, dstOffset, size, data);
    }
    break;

    case FUNID_vkCmdResetEvent:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_event = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkPipelineStageFlags stageMask = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        VkEvent event = (VkEvent)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_EVENT, guest_event);
        
        vkCmdResetEvent(commandBuffer, event, stageMask);
        LOGD("Host: vkCmdResetEvent commandBuffer=%p event=%p stageMask=%u",
            (void*)commandBuffer, (void*)event, stageMask);
    }
    break;

    case FUNID_vkCmdSetEvent:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_event = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkPipelineStageFlags stageMask = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        VkEvent event = (VkEvent)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_EVENT, guest_event);
        
        vkCmdSetEvent(commandBuffer, event, stageMask);
        LOGD("Host: vkCmdSetEvent commandBuffer=%p event=%p stageMask=%u",
            (void*)commandBuffer, (void*)event, stageMask);
    }
    break;

    case FUNID_vkCmdUpdateBuffer:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDeviceSize dstOffset = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDeviceSize dataSize = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        VkBuffer dstBuffer = (VkBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER, guest_buffer);
        
        // void* pData = all_para[1].data;
        void* pData = malloc(dataSize);
        read_from_guest_mem(all_para[1].data, pData, 0, dataSize);
        
        vkCmdUpdateBuffer(commandBuffer, dstBuffer, dstOffset, dataSize, pData);

        free(pData);
        LOGD("Host: vkCmdUpdateBuffer commandBuffer=%p dstBuffer=%p dstOffset=%llu dataSize=%llu",
            (void*)commandBuffer, (void*)dstBuffer, dstOffset, dataSize);
    }
    break;

    case FUNID_vkCmdWaitEvents:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t eventCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkEvent* pEvents = NULL;
        if (eventCount > 0) {
            pEvents = (VkEvent*)malloc(eventCount * sizeof(VkEvent));
            for (uint32_t i = 0; i < eventCount; ++i) {
                uint64_t guest_event = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
                pEvents[i] = (VkEvent)(uintptr_t)
                    lookup_mapping(EXPRESS_VK_OBJECT_TYPE_EVENT, guest_event);
            }
        }
        
        VkPipelineStageFlags srcStageMask = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        VkPipelineStageFlags dstStageMask = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t memoryBarrierCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t bufferMemoryBarrierCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t imageMemoryBarrierCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkMemoryBarrier* pMemoryBarriers = NULL;
        if (memoryBarrierCount > 0) {
            pMemoryBarriers = (VkMemoryBarrier*)malloc(memoryBarrierCount * sizeof(VkMemoryBarrier));
            for (uint32_t i = 0; i < memoryBarrierCount; ++i) {
                decode_from_stream_VkMemoryBarrier(VK_STRUCTURE_TYPE_MAX_ENUM, &pMemoryBarriers[i], ptr);
            }
        }
        
        VkBufferMemoryBarrier* pBufferMemoryBarriers = NULL;
        if (bufferMemoryBarrierCount > 0) {
            pBufferMemoryBarriers = (VkBufferMemoryBarrier*)malloc(bufferMemoryBarrierCount * sizeof(VkBufferMemoryBarrier));
            for (uint32_t i = 0; i < bufferMemoryBarrierCount; ++i) {
                decode_from_stream_VkBufferMemoryBarrier(VK_STRUCTURE_TYPE_MAX_ENUM, &pBufferMemoryBarriers[i], ptr);
            }
        }
        
        VkImageMemoryBarrier* pImageMemoryBarriers = NULL;
        if (imageMemoryBarrierCount > 0) {
            pImageMemoryBarriers = (VkImageMemoryBarrier*)malloc(imageMemoryBarrierCount * sizeof(VkImageMemoryBarrier));
            for (uint32_t i = 0; i < imageMemoryBarrierCount; ++i) {
                decode_from_stream_VkImageMemoryBarrier(VK_STRUCTURE_TYPE_MAX_ENUM, &pImageMemoryBarriers[i], ptr);
            }
        }
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        vkCmdWaitEvents(commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask,
                    memoryBarrierCount, pMemoryBarriers,
                    bufferMemoryBarrierCount, pBufferMemoryBarriers,
                    imageMemoryBarrierCount, pImageMemoryBarriers);
        LOGD("Host: vkCmdWaitEvents commandBuffer=%p eventCount=%u srcStageMask=%u dstStageMask=%u",
            (void*)commandBuffer, eventCount, srcStageMask, dstStageMask);
        
        if (pEvents) free(pEvents);
        if (pMemoryBarriers) free(pMemoryBarriers);
        if (pBufferMemoryBarriers) free(pBufferMemoryBarriers);
        if (pImageMemoryBarriers) free(pImageMemoryBarriers);
    }
    break;

    case FUNID_vkCmdWriteTimestamp:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkPipelineStageFlagBits pipelineStage = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint64_t guest_pool = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t query = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        VkQueryPool queryPool = (VkQueryPool)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_QUERY_POOL, guest_pool);
        
        vkCmdWriteTimestamp(commandBuffer, pipelineStage, queryPool, query);
        LOGD("Host: vkCmdWriteTimestamp commandBuffer=%p pipelineStage=%u queryPool=%p query=%u",
            (void*)commandBuffer, pipelineStage, (void*)queryPool, query);
    }
    break;

    case FUNID_vkCmdBlitImage:
    {
        LOGD("Host: vkCmdBlitImage request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmdBuf = *(uint64_t*)(*ptr); *ptr += 8;
        uint64_t guest_srcImg = *(uint64_t*)(*ptr); *ptr += 8;
        VkImageLayout srcLayout = *(VkImageLayout*)(*ptr); *ptr += 4;
        uint64_t guest_dstImg = *(uint64_t*)(*ptr); *ptr += 8;
        VkImageLayout dstLayout = *(VkImageLayout*)(*ptr); *ptr += 4;
        uint32_t regionCount = *(uint32_t*)(*ptr); *ptr += 4;
        VkFilter filter = *(VkFilter*)(*ptr); *ptr += 4;
        
        VkCommandBuffer cmdBuf = (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmdBuf);
        VkImage srcImg = (VkImage)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_IMAGE, guest_srcImg);
        VkImage dstImg = (VkImage)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_IMAGE, guest_dstImg);
        
        VkImageBlit* regions = NULL;
        if (regionCount > 0) {
            regions = (VkImageBlit*)(*ptr);
        }
        
        vkCmdBlitImage(cmdBuf, srcImg, srcLayout, dstImg, dstLayout, regionCount, regions, filter);
    }
    break;

    case FUNID_vkCmdCopyBufferToImage:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_commandBuffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_srcBuffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_dstImage = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkImageLayout dstImageLayout = *(VkImageLayout*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t regionCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_commandBuffer);
        VkBuffer srcBuffer = (VkBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER, guest_srcBuffer);
        VkImage dstImage = (VkImage)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_IMAGE, guest_dstImage);
        
        VkBufferImageCopy* pRegions = NULL;
        if (regionCount > 0) {
            pRegions = (VkBufferImageCopy*)malloc(regionCount * sizeof(VkBufferImageCopy));
            for (uint32_t i = 0; i < regionCount; ++i) {
                decode_from_stream_VkBufferImageCopy(VK_STRUCTURE_TYPE_MAX_ENUM, &pRegions[i], ptr);
            }
        }
        
        vkCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
        
        if (pRegions) free(pRegions);
        LOGD("Host: vkCmdCopyBufferToImage completed");
    }
    break;

    case FUNID_vkCmdPushConstants:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_commandBuffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_layout = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkShaderStageFlags stageFlags = *(VkShaderStageFlags*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t offset = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t size = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_commandBuffer);
        VkPipelineLayout layout = (VkPipelineLayout)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PIPELINE_LAYOUT, guest_layout);
        
        void* pValues = malloc(size);
        memcpy(pValues, *ptr, size);
        *ptr += size;
        
        vkCmdPushConstants(commandBuffer, layout, stageFlags, offset, size, pValues);
        
        free(pValues);
        LOGD("Host: vkCmdPushConstants completed");
    }
    break;

    case FUNID_vkCreateBufferView:
    {
        LOGD("Host: vkCreateBufferView request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        VkBufferViewCreateInfo createInfo;
        decode_from_stream_VkBufferViewCreateInfo(VK_STRUCTURE_TYPE_MAX_ENUM, &createInfo, ptr);
        
        VkAllocationCallbacks* guest_allocator = (VkAllocationCallbacks*)*(uint64_t*)(*ptr);
        *ptr += 8;
        VkAllocationCallbacks allocator_local;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_allocator) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocator_local, ptr);
            pAllocator = &allocator_local;
        }
        
        uint64_t guest_view = *(uint64_t*)(*ptr);
        
        VkBufferView bufferView;
        VkResult result = vkCreateBufferView(device, &createInfo, pAllocator, &bufferView);
        
        if (result == VK_SUCCESS) {
            insert_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER_VIEW, guest_view, (uint64_t)(uintptr_t)bufferView);
            LOGD("Host: vkCreateBufferView success, guest=%lld host=%lld", guest_view, (uint64_t)(uintptr_t)bufferView);
        } else {
            LOGE("Host: vkCreateBufferView failed with error %d", result);
        }
    }
    break;

    case FUNID_vkCreateEvent:
    {
        LOGD("Host: vkCreateEvent request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        VkEventCreateInfo createInfo;
        decode_from_stream_VkEventCreateInfo(VK_STRUCTURE_TYPE_MAX_ENUM, &createInfo, ptr);
        
        VkAllocationCallbacks* guest_allocator = (VkAllocationCallbacks*)*(uint64_t*)(*ptr);
        *ptr += 8;
        VkAllocationCallbacks allocator_local;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_allocator) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocator_local, ptr);
            pAllocator = &allocator_local;
        }
        
        uint64_t guest_event = *(uint64_t*)(*ptr);
        
        VkEvent event;
        VkResult result = vkCreateEvent(device, &createInfo, pAllocator, &event);
        
        if (result == VK_SUCCESS) {
            insert_mapping(EXPRESS_VK_OBJECT_TYPE_EVENT, guest_event, (uint64_t)(uintptr_t)event);
            LOGD("Host: vkCreateEvent success, guest=%lld host=%lld", guest_event, (uint64_t)(uintptr_t)event);
        } else {
            LOGE("Host: vkCreateEvent failed with error %d", result);
        }
    }
    break;

    case FUNID_vkGetEventStatus:
    {
        LOGD("Host: vkGetEventStatus request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_event = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkEvent event = (VkEvent)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_EVENT, guest_event);
        
        VkResult result = vkGetEventStatus(device, event);
        
        write_to_guest_mem(all_para[1].data, &result, 0, sizeof(VkResult));
        LOGD("Host: vkGetEventStatus result=%d", result);
    }
    break;

    case FUNID_vkGetFenceStatus:
    {
        LOGD("Host: vkGetFenceStatus request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_fence = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkFence fence = (VkFence)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_FENCE, guest_fence);
        
        VkResult result = vkGetFenceStatus(device, fence);
        
        write_to_guest_mem(all_para[1].data, &result, 0, sizeof(VkResult));
        LOGD("Host: vkGetFenceStatus result=%d", result);
    }
    break;

    case FUNID_vkGetPipelineCacheData:
    {
        LOGD("Host: vkGetPipelineCacheData request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_cache = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkPipelineCache pipelineCache = (VkPipelineCache)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PIPELINE_CACHE, guest_cache);
        
        size_t dataSize;
        read_from_guest_mem(all_para[1].data, &dataSize, 0, sizeof(size_t));

        if(dataSize == 0) {
            VkResult result = vkGetPipelineCacheData(device, pipelineCache, &dataSize, NULL);
            if(result == VK_SUCCESS) {
                write_to_guest_mem(all_para[1].data, &dataSize, 0, sizeof(size_t));
                LOGD("Host: vkGetPipelineCacheData dataSize %d", (int)dataSize);
            } else {
                LOGE("Host: vkGetPipelineCacheData failed with error %d", result);
            }
        } else {
            void* pData = NULL;
            pData = malloc(dataSize);        
            VkResult result = vkGetPipelineCacheData(device, pipelineCache, &dataSize, pData);

            if (result == VK_SUCCESS) {
                LOGD("Host: vkGetPipelineCacheData success, dataSize=%zu", dataSize);
                write_to_guest_mem(all_para[2].data, pData, 0, dataSize);
                free(pData);
            } else {
                LOGE("Host: vkGetPipelineCacheData failed with error %d", result);
            }      
        }
    }
    break;

    case FUNID_vkGetDeviceMemoryCommitment:
    {
        LOGD("Host: vkGetDeviceMemoryCommitment request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_memory = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkDeviceMemory memory = (VkDeviceMemory)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE_MEMORY, guest_memory);
        
        VkDeviceSize committedSize;
        vkGetDeviceMemoryCommitment(device, memory, &committedSize);
        
        write_to_guest_mem(all_para[1].data, &committedSize, 0, sizeof(VkDeviceSize));
        LOGD("Host: vkGetDeviceMemoryCommitment committedSize=%llu", committedSize);
    }
    break;

    case FUNID_vkEnumerateInstanceExtensionProperties:
    {
        LOGD("Host: vkEnumerateInstanceExtensionProperties request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint32_t has_layer = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        const char* layer_name = NULL;
        if (has_layer) {
            layer_name = (const char*)(*ptr);
            *ptr += strlen(layer_name) + 1;
        }
        
        uint32_t count = 0;
        read_from_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        
        VkResult result;
        
        // 如果查询的是默认层（NULL），使用缓存
        if (!layer_name) {
            result = ensure_instance_extensions_cached();
            if (result != VK_SUCCESS) {
                break;
            }
            
            if (count == 0) {
                write_to_guest_mem(all_para[1].data, &g_cached_instance_extension_count, 0, sizeof(uint32_t));
                LOGD("Host: Returning cached extension count=%u", g_cached_instance_extension_count);
            } else {
                uint32_t copy_count = (count < g_cached_instance_extension_count) ? count : g_cached_instance_extension_count;
                // write_to_guest_mem(all_para[1].data, &copy_count, 0, sizeof(uint32_t));
                write_to_guest_mem(all_para[2].data, g_cached_instance_extensions, 0, copy_count * sizeof(VkExtensionProperties));
                result = (copy_count < g_cached_instance_extension_count) ? VK_INCOMPLETE : VK_SUCCESS;
                LOGD("Host: Returning %u cached extensions", copy_count);
            }
        } else {
            // 对于特定层的查询，不使用缓存，直接调用原始API
            if (count == 0) {
                result = vkEnumerateInstanceExtensionProperties(layer_name, &count, NULL);
                if(result != VK_SUCCESS) {
                    LOGE("vkEnumerateInstanceExtensionProperties failed with error %d", result);
                } else {
                    LOGD("vkEnumerateInstanceExtensionProperties count=%u", count);
                    write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
                }
            } else {
                VkExtensionProperties* properties = (VkExtensionProperties*)malloc(count * sizeof(VkExtensionProperties));
                if (!properties) {
                    result = VK_ERROR_OUT_OF_HOST_MEMORY;
                } else {
                    result = vkEnumerateInstanceExtensionProperties(layer_name, &count, properties);
                    if (result == VK_SUCCESS || result == VK_INCOMPLETE) {
                        write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
                        write_to_guest_mem(all_para[2].data, properties, 0, count * sizeof(VkExtensionProperties));
                    } else {
                        LOGE("vkEnumerateInstanceExtensionProperties failed with error %d", result);
                    }
                    free(properties);
                }
            }
        }
        
        LOGD("Host: vkEnumerateInstanceExtensionProperties result=%d", result);
    }
    break;


    case FUNID_vkEnumerateDeviceExtensionProperties:
    {
        LOGD("Host: vkEnumerateDeviceExtensionProperties request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_dev = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t has_layer = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        const char* layer_name = NULL;
        if (has_layer) {
            layer_name = (const char*)(*ptr);
            *ptr += strlen(layer_name) + 1;
        }
        
        VkPhysicalDevice device = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_dev);
        
        uint32_t count = 0;
        read_from_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        
        VkResult result;
        if (count == 0) {
            result = vkEnumerateDeviceExtensionProperties(device, layer_name, &count, NULL);
            if(result != VK_SUCCESS) {
                LOGE("vkEnumerateDeviceExtensionProperties failed with error %d", result);
            } else {
                write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
                LOGD("vkEnumerateDeviceExtensionProperties count=%u", count);
            }
        } else {
            VkExtensionProperties* properties = (VkExtensionProperties*)malloc(count * sizeof(VkExtensionProperties));
            if (!properties) {
                result = VK_ERROR_OUT_OF_HOST_MEMORY;
            } else {
                result = vkEnumerateDeviceExtensionProperties(device, layer_name, &count, properties);
                if (result == VK_SUCCESS) {
                    write_to_guest_mem(all_para[2].data, properties, 0, count * sizeof(VkExtensionProperties));
                }
                free(properties);
            }
        }
        LOGD("Host: vkEnumerateDeviceExtensionProperties result=%d count=%u", result, count);
    }
    break;

    case FUNID_vkEnumerateInstanceLayerProperties:
    {
        LOGD("Host: vkEnumerateInstanceLayerProperties request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        
        uint32_t count = 0;
        read_from_guest_mem(all_para[0].data, &count, 0, sizeof(uint32_t));
        
        VkResult result;
        if (count == 0) {
            result = vkEnumerateInstanceLayerProperties(&count, NULL);
            if(result != VK_SUCCESS) {
                LOGE("vkEnumerateInstanceLayerProperties failed with error %d", result);
            } else {
                write_to_guest_mem(all_para[0].data, &count, 0, sizeof(uint32_t));
                LOGD("vkEnumerateInstanceLayerProperties count=%u", count);
            }
        } else {
            VkLayerProperties* properties = (VkLayerProperties*)malloc(count * sizeof(VkLayerProperties));
            if (!properties) {
                result = VK_ERROR_OUT_OF_HOST_MEMORY;
            } else {
                result = vkEnumerateInstanceLayerProperties(&count, properties);
                if (result == VK_SUCCESS) {
                    write_to_guest_mem(all_para[1].data, properties, 0, count * sizeof(VkLayerProperties));
                    LOGD("vkEnumerateInstanceLayerProperties count=%u", count);
                } else {
                    LOGE("vkEnumerateInstanceLayerProperties failed with error %d", result);
                }
                free(properties);
            }
        }
    }
    break;

    case FUNID_vkEnumerateDeviceLayerProperties:
    {
        LOGD("Host: vkEnumerateDeviceLayerProperties request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_dev = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        VkPhysicalDevice device = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_dev);
        
        uint32_t count = 0;
        read_from_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        
        VkResult result;
        if (count == 0) {
            result = vkEnumerateDeviceLayerProperties(device, &count, NULL);
            if (result != VK_SUCCESS) {
                LOGE("vkEnumerateDeviceLayerProperties failed with error %d", result);
            } else {
                write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
                LOGD("vkEnumerateDeviceLayerProperties count=%u", count);
            }
        } else {
            VkLayerProperties* properties = (VkLayerProperties*)malloc(count * sizeof(VkLayerProperties));
            if (!properties) {
                result = VK_ERROR_OUT_OF_HOST_MEMORY;
            } else {
                result = vkEnumerateDeviceLayerProperties(device, &count, properties);
                if (result == VK_SUCCESS) {
                    write_to_guest_mem(all_para[2].data, properties, 0, count * sizeof(VkLayerProperties));
                    LOGD("vkEnumerateDeviceLayerProperties count=%u", count);
                } else {
                    LOGE("vkEnumerateDeviceLayerProperties failed with error %d", result);
                }
                free(properties);
            }
        }
    }
    break;

    case FUNID_vkEnumerateInstanceVersion:
    {
        LOGD("Host: vkEnumerateInstanceVersion request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        
        uint32_t version;
        VkResult result = vkEnumerateInstanceVersion(&version);
        if (result != VK_SUCCESS) {
            LOGE("vkEnumerateInstanceVersion failed with error %d", result);
        } else {
            write_to_guest_mem(all_para[0].data, &version, 0, sizeof(uint32_t));
            LOGD("vkEnumerateInstanceVersion success, version=%u", version);
        }
    }
    break;

    case FUNID_vkQueueBindSparse:
    {
        LOGD("Host: vkQueueBindSparse request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        // Decode queue
        uint64_t guest_queue = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkQueue queue = (VkQueue)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_QUEUE, guest_queue);
        
        // Decode bindInfoCount
        uint32_t bindInfoCount = *(uint32_t*)(*ptr);
        *ptr += sizeof(uint32_t);
        
        // Decode bind infos
        VkBindSparseInfo* pBindInfo = NULL;
        if (bindInfoCount > 0) {
            pBindInfo = (VkBindSparseInfo*)malloc(bindInfoCount * sizeof(VkBindSparseInfo));
            for (uint32_t i = 0; i < bindInfoCount; ++i) {
                decode_from_stream_VkBindSparseInfo(VK_STRUCTURE_TYPE_MAX_ENUM, &pBindInfo[i], ptr);
            }
        }
        
        // Decode fence
        uint64_t guest_fence = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkFence fence = VK_NULL_HANDLE;
        if (guest_fence != 0) {
            fence = (VkFence)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_FENCE, guest_fence);
        }
        
        VkResult result = vkQueueBindSparse(queue, bindInfoCount, pBindInfo, fence);
        if (result != VK_SUCCESS) {
            LOGE("Host: vkQueueBindSparse failed with error %d", result);
        } else {
            LOGD("Host: vkQueueBindSparse success, queue=%p bindInfoCount=%u fence=%p",
                (void*)queue, bindInfoCount, (void*)fence);
        }
        
        if (pBindInfo) free(pBindInfo);
        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkQueueWaitIdle:
    {
        LOGD("Host: vkQueueWaitIdle request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_queue = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkQueue queue = (VkQueue)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_QUEUE, guest_queue);
        
        VkResult result = vkQueueWaitIdle(queue);
        write_to_guest_mem(all_para[1].data, &result, 0, sizeof(VkResult));
        if (result != VK_SUCCESS) {
            LOGE("Host: vkQueueWaitIdle failed with error %d", result);
        } else {
            LOGD("Host: vkQueueWaitIdle success, queue=%p", (void*)queue);
        }
        
        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkTrimCommandPool:
    {
        LOGD("Host: vkTrimCommandPool request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_pool = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint32_t flags = *(uint32_t*)(*ptr);
        *ptr += sizeof(uint32_t);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkCommandPool commandPool = (VkCommandPool)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_POOL, guest_pool);
        
        vkTrimCommandPool(device, commandPool, flags);
        
        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkGetPhysicalDeviceFeatures:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        VkPhysicalDevice device = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_device);
        
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(device, &features);
        
        write_to_guest_mem(all_para[1].data, &features, 0, sizeof(VkPhysicalDeviceFeatures));
        LOGD("Host: vkGetPhysicalDeviceFeatures device=%p", (void*)device);
    }
    break;

    case FUNID_vkGetQueryPoolResults:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_queryPool = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t firstQuery = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t queryCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        size_t dataSize = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDeviceSize stride = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkQueryResultFlags flags = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkDevice device = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkQueryPool queryPool = (VkQueryPool)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_QUERY_POOL, guest_queryPool);
        
        void* data = malloc(dataSize);
        VkResult result = vkGetQueryPoolResults(device, queryPool, firstQuery, queryCount, 
                                            dataSize, data, stride, flags);
        write_to_guest_mem(all_para[2].data, &result, 0, sizeof(VkResult));

        if (result != VK_SUCCESS) {
            LOGE("Host: vkGetQueryPoolResults failed with error %d", result);
        } else {
            write_to_guest_mem(all_para[1].data, data, 0, dataSize);
            LOGD("Host: vkGetQueryPoolResults success, firstQuery=%u queryCount=%u dataSize=%zu stride=%zu flags=%u",
                firstQuery, queryCount, dataSize, stride, flags);
        }
        free(data);
    }
    break;

    case FUNID_vkGetBufferMemoryRequirements2:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        VkBufferMemoryRequirementsInfo2 info;
        decode_from_stream_VkBufferMemoryRequirementsInfo2(VK_STRUCTURE_TYPE_MAX_ENUM, &info, ptr);
        
        VkDevice device = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        VkMemoryRequirements2 requirements = {};
        requirements.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
        LOGD("Host: before vkGetBufferMemoryRequirements2 device=%p buffer=%p size=%zu alignment=%zu memoryTypeBits=%u",
            (void*)device, (void*)info.buffer, requirements.memoryRequirements.size,
            requirements.memoryRequirements.alignment, requirements.memoryRequirements.memoryTypeBits);
        vkGetBufferMemoryRequirements2(device, &info, &requirements);
        
        write_to_guest_mem(all_para[1].data, &requirements, 0, sizeof(VkMemoryRequirements2));
        LOGD("Host: vkGetBufferMemoryRequirements2 device=%p buffer=%p size=%zu alignment=%zu memoryTypeBits=%u",
            (void*)device, (void*)info.buffer, requirements.memoryRequirements.size,
            requirements.memoryRequirements.alignment, requirements.memoryRequirements.memoryTypeBits);
    }
    break;

    case FUNID_vkGetDeviceQueue2:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device;
        memcpy(&guest_device, *ptr, 8); *ptr += 8;
        
        VkDeviceQueueInfo2 queueInfo;
        decode_from_stream_VkDeviceQueueInfo2(VK_STRUCTURE_TYPE_MAX_ENUM, &queueInfo, ptr);
        
        uint64_t guest_queue;
        memcpy(&guest_queue, *ptr, 8); *ptr += 8;
        
        VkDevice device = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        VkQueue queue;
        vkGetDeviceQueue2(device, &queueInfo, &queue);
        
        insert_mapping(EXPRESS_VK_OBJECT_TYPE_QUEUE, guest_queue, (uint64_t)(uintptr_t)queue);

        LOGD("GetDeviceQueue2 queueFamilyIndex=%u queueIndex=%u", 
            queueInfo.queueFamilyIndex, queueInfo.queueIndex);
    }
    break;

    case FUNID_vkMergePipelineCaches:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_dstCache = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t srcCacheCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkDevice device = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkPipelineCache dstCache = (VkPipelineCache)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PIPELINE_CACHE, guest_dstCache);
        
        VkPipelineCache* srcCaches = (VkPipelineCache*)malloc(srcCacheCount * sizeof(VkPipelineCache));
        uint64_t* guest_srcCaches = (uint64_t*)malloc(srcCacheCount * sizeof(uint64_t));
        read_from_guest_mem(all_para[1].data, guest_srcCaches, 0, srcCacheCount * sizeof(uint64_t));
        
        for (uint32_t i = 0; i < srcCacheCount; ++i) {
            srcCaches[i] = (VkPipelineCache)(uintptr_t)
                lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PIPELINE_CACHE, guest_srcCaches[i]);
        }
        
        VkResult result = vkMergePipelineCaches(device, dstCache, srcCacheCount, srcCaches);
        if (result != VK_SUCCESS) {
            LOGE("Host: vkMergePipelineCaches failed with error %d", result);
        } else {
            LOGD("Host: vkMergePipelineCaches success, dstCache=%p srcCacheCount=%u",
                (void*)dstCache, srcCacheCount);
        }
        
        free(srcCaches);
        free(guest_srcCaches);
    }
    break;

    case FUNID_vkCreateQueryPool:
    {
        LOGD("get call FUNID_vkCreateQueryPool!");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("get vk param number %d", para_num);
        
        int need_free = 0;
        char* stream_ptr = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** stream_ptr_ptr = (uint8_t**)&stream_ptr;
        
        uint64_t guest_device = *(uint64_t*)(*stream_ptr_ptr);
        *stream_ptr_ptr += sizeof(uint64_t);
        
        VkQueryPoolCreateInfo* pCreateInfo = (VkQueryPoolCreateInfo*)malloc(sizeof(VkQueryPoolCreateInfo));
        decode_from_stream_VkQueryPoolCreateInfo(VK_STRUCTURE_TYPE_MAX_ENUM, pCreateInfo, stream_ptr_ptr);
        
        VkAllocationCallbacks* guest_allocator = (VkAllocationCallbacks*)(*(uint64_t*)(*stream_ptr_ptr));
        *stream_ptr_ptr += 8;
        
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_allocator) {
            pAllocator = (VkAllocationCallbacks*)malloc(sizeof(VkAllocationCallbacks));
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, (VkAllocationCallbacks*)pAllocator, stream_ptr_ptr);
        }
        
        uint64_t guest_querypool = *(uint64_t*)(*stream_ptr_ptr);
        *stream_ptr_ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkQueryPool queryPool;
        
        VkResult result = vkCreateQueryPool(device, pCreateInfo, pAllocator, &queryPool);
        
        if (result == VK_SUCCESS) {
            LOGD("got result %d map querypool %llx guest %llx", result, (uint64_t)(uintptr_t)queryPool, guest_querypool);
            insert_mapping(EXPRESS_VK_OBJECT_TYPE_QUERY_POOL, guest_querypool, (uint64_t)(uintptr_t)queryPool);
        } else {
            LOGE("Host: vkCreateQueryPool failed with error %d", result);
        }
        
        if (pCreateInfo) free(pCreateInfo);
        if (pAllocator) free((void*)pAllocator);
        if (need_free) free(stream_ptr);
    }
    break;

    case FUNID_vkBindImageMemory2:
    {
        LOGD("Host: vkBindImageMemory2 request");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_device = *(uint64_t*)(*ptr); 
        *ptr += sizeof(uint64_t);
        
        uint32_t bindInfoCount = *(uint32_t*)(*ptr);
        *ptr += sizeof(uint32_t);
        
        VkDevice device = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        VkBindImageMemoryInfo* pBindInfos = malloc(bindInfoCount * sizeof(VkBindImageMemoryInfo));
        
        for (uint32_t i = 0; i < bindInfoCount; ++i) {
            decode_from_stream_VkBindImageMemoryInfo(VK_STRUCTURE_TYPE_MAX_ENUM,
                                                    &pBindInfos[i], ptr);
        }
        
        VkResult result = vkBindImageMemory2(device, bindInfoCount, pBindInfos);
        if (result != VK_SUCCESS) {
            LOGE("Host: vkBindImageMemory2 failed with error %d", result);
        } else {
            LOGD("Host: vkBindImageMemory2 success, bindInfoCount=%u", bindInfoCount);
        }
        
        free(pBindInfos);
    }
    break;

    case FUNID_vkDestroyBuffer: {
        LOGD("Host: vkDestroyBuffer request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroyBuffer para count = %d", para_num);
    
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroyBuffer failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;
    
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_buf = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
    
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
    
        VkResult result = destroy_vulkan_object_device(
            guest_dev,
            guest_buf,
            EXPRESS_VK_OBJECT_TYPE_BUFFER,
            (void (*)(VkDevice, void*, const VkAllocationCallbacks*))vkDestroyBuffer,
            pAllocator
        );
    
        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroyBuffer failed for guest buffer %llu", (unsigned long long)guest_buf);
        } else {
            LOGD("Host: vkDestroyBuffer completed for guest buffer %llu", (unsigned long long)guest_buf);
        }
    
        //if (need_free) free(stream);
    }
    break;
    
    case FUNID_vkDestroyBufferView: {
        LOGD("Host: vkDestroyBufferView request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroyBufferView para count = %d", para_num);
    
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroyBufferView failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;
    
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_buf_view = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
    
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
    
        VkResult result = destroy_vulkan_object_device(
            guest_dev,
            guest_buf_view,
            EXPRESS_VK_OBJECT_TYPE_BUFFER_VIEW,
            (void (*)(VkDevice, void*, const VkAllocationCallbacks*))vkDestroyBufferView,
            pAllocator
        );
    
        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroyBufferView failed for guest buffer view %llu", (unsigned long long)guest_buf_view);
        } else {
            LOGD("Host: vkDestroyBufferView completed for guest buffer view %llu", (unsigned long long)guest_buf_view);
        }
    
        //if (need_free) free(stream);
     }
     break;
    
    case FUNID_vkDestroyCommandPool: {
        LOGD("Host: vkDestroyCommandPool request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroyCommandPool para count = %d", para_num);
    
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroyCommandPool failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;
    
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_cmd_pool = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
    
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
    
        VkResult result = destroy_vulkan_object_device(
            guest_dev,
            guest_cmd_pool,
            EXPRESS_VK_OBJECT_TYPE_COMMAND_POOL,
            (void (*)(VkDevice, void*, const VkAllocationCallbacks*))vkDestroyCommandPool,
            pAllocator
        );
    
        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroyCommandPool failed for guest command pool %llu", (unsigned long long)guest_cmd_pool);
        } else {
            LOGD("Host: vkDestroyCommandPool completed for guest command pool %llu", (unsigned long long)guest_cmd_pool);
        }
    
        //if (need_free) free(stream);
     }
    break;

    case FUNID_vkDestroyDescriptorPool: {
        LOGD("Host: vkDestroyDescriptorPool request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroyDescriptorPool para count = %d", para_num);
    
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroyDescriptorPool failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;
    
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_desc_pool = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
    
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
    
        VkResult result = destroy_vulkan_object_device(
            guest_dev,
            guest_desc_pool,
            EXPRESS_VK_OBJECT_TYPE_DESCRIPTOR_POOL,
            (void (*)(VkDevice, void*, const VkAllocationCallbacks*))vkDestroyDescriptorPool,
            pAllocator
        );
    
        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroyDescriptorPool failed for guest descriptor pool %llu", (unsigned long long)guest_desc_pool);
        } else {
            LOGD("Host: vkDestroyDescriptorPool completed for guest descriptor pool %llu", (unsigned long long)guest_desc_pool);
        }
    
        //if (need_free) free(stream);
    }
    break;
    
    case FUNID_vkDestroyDescriptorSetLayout: {
        LOGD("Host: vkDestroyDescriptorSetLayout request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroyDescriptorSetLayout para count = %d", para_num);
    
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroyDescriptorSetLayout failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;
    
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_desc_set_layout = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
    
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
    
        VkResult result = destroy_vulkan_object_device(
            guest_dev,
            guest_desc_set_layout,
            EXPRESS_VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT,
            (void (*)(VkDevice, void*, const VkAllocationCallbacks*))vkDestroyDescriptorSetLayout,
            pAllocator
        );
    
        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroyDescriptorSetLayout failed for guest descriptor set layout %llu", (unsigned long long)guest_desc_set_layout);
        } else {
            LOGD("Host: vkDestroyDescriptorSetLayout completed for guest descriptor set layout %llu", (unsigned long long)guest_desc_set_layout);
        }
    
        //if (need_free) free(stream);
    }
    break;
    
    case FUNID_vkDestroyDescriptorUpdateTemplate: {
        LOGD("Host: vkDestroyDescriptorUpdateTemplate request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroyDescriptorUpdateTemplate para count = %d", para_num);
    
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroyDescriptorUpdateTemplate failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;
    
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_desc_update_template = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
    
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
    
        VkResult result = destroy_vulkan_object_device(
            guest_dev,
            guest_desc_update_template,
            EXPRESS_VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE,
            (void (*)(VkDevice, void*, const VkAllocationCallbacks*))vkDestroyDescriptorUpdateTemplate,
            pAllocator
        );
    
        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroyDescriptorUpdateTemplate failed for guest descriptor update template %llu", (unsigned long long)guest_desc_update_template);
        } else {
            LOGD("Host: vkDestroyDescriptorUpdateTemplate completed for guest descriptor update template %llu", (unsigned long long)guest_desc_update_template);
        }
    
        //if (need_free) free(stream);
    }
    break;

    //note: device
    case FUNID_vkDestroyDevice: {
        LOGD("Host: vkDestroyDevice request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroyDevice para count = %d", para_num);
    
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroyDevice failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;
    
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
    
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
    
        VkResult result = destroy_vulkan_object_essential(
            guest_dev,
            EXPRESS_VK_OBJECT_TYPE_DEVICE,
            (void (*)(void*, const VkAllocationCallbacks*))vkDestroyDevice,
            pAllocator
        );
    
        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroyDevice failed for guest device %llu", (unsigned long long)guest_dev);
        } else {
            LOGD("Host: vkDestroyDevice completed for guest device %llu", (unsigned long long)guest_dev);
        }
    
        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkDestroyEvent: {
        LOGD("Host: vkDestroyEvent request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroyEvent para count = %d", para_num);
    
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroyEvent failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;
    
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_event = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
    
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
    
        VkResult result = destroy_vulkan_object_device(
            guest_dev,
            guest_event,
            EXPRESS_VK_OBJECT_TYPE_EVENT,
            (void (*)(VkDevice, void*, const VkAllocationCallbacks*))vkDestroyEvent,
            pAllocator
        );
    
        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroyEvent failed for guest event %llu", (unsigned long long)guest_event);
        } else {
            LOGD("Host: vkDestroyEvent completed for guest event %llu", (unsigned long long)guest_event);
        }
    
        //if (need_free) free(stream);
    }
    break;
    
    case FUNID_vkDestroyFence: {
        LOGD("Host: vkDestroyFence request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroyFence para count = %d", para_num);
    
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroyFence failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;
    
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_fence = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
    
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
    
        VkResult result = destroy_vulkan_object_device(
            guest_dev,
            guest_fence,
            EXPRESS_VK_OBJECT_TYPE_FENCE,
            (void (*)(VkDevice, void*, const VkAllocationCallbacks*))vkDestroyFence,
            pAllocator
        );
    
        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroyFence failed for guest fence %llu", (unsigned long long)guest_fence);
        } else {
            LOGD("Host: vkDestroyFence completed for guest fence %llu", (unsigned long long)guest_fence);
        }
    
        //if (need_free) free(stream);
    }
    break;
    
    case FUNID_vkDestroyFramebuffer: {
        LOGD("Host: vkDestroyFramebuffer request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroyFramebuffer para count = %d", para_num);
    
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroyFramebuffer failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;
    
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_framebuffer = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
    
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
    
        VkResult result = destroy_vulkan_object_device(
            guest_dev,
            guest_framebuffer,
            EXPRESS_VK_OBJECT_TYPE_FRAMEBUFFER,
            (void (*)(VkDevice, void*, const VkAllocationCallbacks*))vkDestroyFramebuffer,
            pAllocator
        );
    
        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroyFramebuffer failed for guest framebuffer %llu", (unsigned long long)guest_framebuffer);
        } else {
            LOGD("Host: vkDestroyFramebuffer completed for guest framebuffer %llu", (unsigned long long)guest_framebuffer);
        }
    
        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkDestroyImage: {
        LOGD("Host: vkDestroyImage request");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroyImage para count = %d", para_num);

        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroyImage failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_image = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);

        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }

        VkResult result = destroy_vulkan_object_device(
            guest_dev,
            guest_image,
            EXPRESS_VK_OBJECT_TYPE_IMAGE,
            (void (*)(VkDevice, void*, const VkAllocationCallbacks*))vkDestroyImage,
            pAllocator
        );

        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroyImage failed for guest image %llu", (unsigned long long)guest_image);
        } else {
            LOGD("Host: vkDestroyImage completed for guest image %llu", (unsigned long long)guest_image);
        }

        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkDestroyImageView: {
        LOGD("Host: vkDestroyImageView request");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroyImageView para count = %d", para_num);

        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroyImageView failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_image_view = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);

        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }

        VkResult result = destroy_vulkan_object_device(
            guest_dev,
            guest_image_view,
            EXPRESS_VK_OBJECT_TYPE_IMAGE_VIEW,
            (void (*)(VkDevice, void*, const VkAllocationCallbacks*))vkDestroyImageView,
            pAllocator
        );

        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroyImageView failed for guest image view %llu", (unsigned long long)guest_image_view);
        } else {
            LOGD("Host: vkDestroyImageView completed for guest image view %llu", (unsigned long long)guest_image_view);
        }

        //if (need_free) free(stream);
    }
    break;

    //note: instance
    case FUNID_vkDestroyInstance: {
        LOGD("Host: vkDestroyInstance request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroyInstance para count = %d", para_num);
    
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroyInstance failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;
    
        uint64_t guest_instance = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
    
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }

        VkResult result = destroy_vulkan_object_essential(
            guest_instance,
            EXPRESS_VK_OBJECT_TYPE_INSTANCE,
            (void (*)(void*, const VkAllocationCallbacks*))vkDestroyInstance,
            pAllocator
        );
    
        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroyInstance failed for guest instance %llu", (unsigned long long)guest_instance);
        } else {
            LOGD("Host: vkDestroyInstance completed for guest instance %llu", (unsigned long long)guest_instance);
        }
    
        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkDestroyPipeline: {
        LOGD("Host: vkDestroyPipeline request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroyPipeline para count = %d", para_num);
    
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroyPipeline failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;
    
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_pipeline = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
    
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
    
        VkResult result = destroy_vulkan_object_device(
            guest_dev,
            guest_pipeline,
            EXPRESS_VK_OBJECT_TYPE_PIPELINE,
            (void (*)(VkDevice, void*, const VkAllocationCallbacks*))vkDestroyPipeline,
            pAllocator
        );
    
        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroyPipeline failed for guest pipeline %llu", (unsigned long long)guest_pipeline);
        } else {
            LOGD("Host: vkDestroyPipeline completed for guest pipeline %llu", (unsigned long long)guest_pipeline);
        }
    
        //if (need_free) free(stream);
    }
    break;
    
    case FUNID_vkDestroyPipelineCache: {
        LOGD("Host: vkDestroyPipelineCache request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroyPipelineCache para count = %d", para_num);
    
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroyPipelineCache failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;
    
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_pipeline_cache = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
    
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
    
        VkResult result = destroy_vulkan_object_device(
            guest_dev,
            guest_pipeline_cache,
            EXPRESS_VK_OBJECT_TYPE_PIPELINE_CACHE,
            (void (*)(VkDevice, void*, const VkAllocationCallbacks*))vkDestroyPipelineCache,
            pAllocator
        );
    
        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroyPipelineCache failed for guest pipeline cache %llu", (unsigned long long)guest_pipeline_cache);
        } else {
            LOGD("Host: vkDestroyPipelineCache completed for guest pipeline cache %llu", (unsigned long long)guest_pipeline_cache);
        }
    
        //if (need_free) free(stream);
    }
    break;
    
    case FUNID_vkDestroyPipelineLayout: {
        LOGD("Host: vkDestroyPipelineLayout request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroyPipelineLayout para count = %d", para_num);
    
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroyPipelineLayout failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;
    
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_pipeline_layout = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
    
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
    
        VkResult result = destroy_vulkan_object_device(
            guest_dev,
            guest_pipeline_layout,
            EXPRESS_VK_OBJECT_TYPE_PIPELINE_LAYOUT,
            (void (*)(VkDevice, void*, const VkAllocationCallbacks*))vkDestroyPipelineLayout,
            pAllocator
        );
    
        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroyPipelineLayout failed for guest pipeline layout %llu", (unsigned long long)guest_pipeline_layout);
        } else {
            LOGD("Host: vkDestroyPipelineLayout completed for guest pipeline layout %llu", (unsigned long long)guest_pipeline_layout);
        }
    
        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkDestroyPrivateDataSlot: {
        LOGD("Host: vkDestroyPrivateDataSlot request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroyPrivateDataSlot para count = %d", para_num);
    
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroyPrivateDataSlot failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;
    
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_private_data_slot = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
    
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
    
        VkResult result = destroy_vulkan_object_device(
            guest_dev,
            guest_private_data_slot,
            EXPRESS_VK_OBJECT_TYPE_PRIVATE_DATA_SLOT,
            (void (*)(VkDevice, void*, const VkAllocationCallbacks*))vkDestroyPrivateDataSlot,
            pAllocator
        );
    
        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroyPrivateDataSlot failed for guest private data slot %llu", (unsigned long long)guest_private_data_slot);
        } else {
            LOGD("Host: vkDestroyPrivateDataSlot completed for guest private data slot %llu", (unsigned long long)guest_private_data_slot);
        }
    
        //if (need_free) free(stream);
    }
    break;
    
    case FUNID_vkDestroyQueryPool: {
        LOGD("Host: vkDestroyQueryPool request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroyQueryPool para count = %d", para_num);
    
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroyQueryPool failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;
    
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_query_pool = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
    
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
    
        VkResult result = destroy_vulkan_object_device(
            guest_dev,
            guest_query_pool,
            EXPRESS_VK_OBJECT_TYPE_QUERY_POOL,
            (void (*)(VkDevice, void*, const VkAllocationCallbacks*))vkDestroyQueryPool,
            pAllocator
        );
    
        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroyQueryPool failed for guest query pool %llu", (unsigned long long)guest_query_pool);
        } else {
            LOGD("Host: vkDestroyQueryPool completed for guest query pool %llu", (unsigned long long)guest_query_pool);
        }
    
        //if (need_free) free(stream);
    }
    break;
    
    case FUNID_vkDestroyRenderPass: {
        LOGD("Host: vkDestroyRenderPass request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroyRenderPass para count = %d", para_num);
    
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroyRenderPass failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;
    
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_render_pass = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
    
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
    
        VkResult result = destroy_vulkan_object_device(
            guest_dev,
            guest_render_pass,
            EXPRESS_VK_OBJECT_TYPE_RENDER_PASS,
            (void (*)(VkDevice, void*, const VkAllocationCallbacks*))vkDestroyRenderPass,
            pAllocator
        );
    
        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroyRenderPass failed for guest render pass %llu", (unsigned long long)guest_render_pass);
        } else {
            LOGD("Host: vkDestroyRenderPass completed for guest render pass %llu", (unsigned long long)guest_render_pass);
        }
    
        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkDestroySampler: {
        LOGD("Host: vkDestroySampler request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroySampler para count = %d", para_num);
    
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroySampler failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;
    
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_sampler = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
    
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
    
        VkResult result = destroy_vulkan_object_device(
            guest_dev,
            guest_sampler,
            EXPRESS_VK_OBJECT_TYPE_SAMPLER,
            (void (*)(VkDevice, void*, const VkAllocationCallbacks*))vkDestroySampler,
            pAllocator
        );
    
        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroySampler failed for guest sampler %llu", (unsigned long long)guest_sampler);
        } else {
            LOGD("Host: vkDestroySampler completed for guest sampler %llu", (unsigned long long)guest_sampler);
        }
    
        //if (need_free) free(stream);
    }
    break;
    
    case FUNID_vkDestroySamplerYcbcrConversion: {
        LOGD("Host: vkDestroySamplerYcbcrConversion request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroySamplerYcbcrConversion para count = %d", para_num);
    
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroySamplerYcbcrConversion failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;
    
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_ycbcr_conversion = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
    
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
    
        VkResult result = destroy_vulkan_object_device(
            guest_dev,
            guest_ycbcr_conversion,
            EXPRESS_VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION,
            (void (*)(VkDevice, void*, const VkAllocationCallbacks*))vkDestroySamplerYcbcrConversion,
            pAllocator
        );
    
        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroySamplerYcbcrConversion failed for guest sampler YCbCr conversion %llu", (unsigned long long)guest_ycbcr_conversion);
        } else {
            LOGD("Host: vkDestroySamplerYcbcrConversion completed for guest sampler YCbCr conversion %llu", (unsigned long long)guest_ycbcr_conversion);
        }
    
        //if (need_free) free(stream);
    }
    break;
    
    case FUNID_vkDestroySemaphore: {
        LOGD("Host: vkDestroySemaphore request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroySemaphore para count = %d", para_num);
    
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroySemaphore failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;
    
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_semaphore = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
    
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
    
        VkResult result = destroy_vulkan_object_device(
            guest_dev,
            guest_semaphore,
            EXPRESS_VK_OBJECT_TYPE_SEMAPHORE,
            (void (*)(VkDevice, void*, const VkAllocationCallbacks*))vkDestroySemaphore,
            pAllocator
        );
    
        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroySemaphore failed for guest semaphore %llu", (unsigned long long)guest_semaphore);
        } else {
            LOGD("Host: vkDestroySemaphore completed for guest semaphore %llu", (unsigned long long)guest_semaphore);
        }
    
        //if (need_free) free(stream);
    }
    break;

    case FUNID_vkDestroyShaderModule: {
        LOGD("Host: vkDestroyShaderModule request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroyShaderModule para count = %d", para_num);
    
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroyShaderModule failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;
    
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_shader_module = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
    
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
    
        VkResult result = destroy_vulkan_object_device(
            guest_dev,
            guest_shader_module,
            EXPRESS_VK_OBJECT_TYPE_SHADER_MODULE,
            (void (*)(VkDevice, void*, const VkAllocationCallbacks*))vkDestroyShaderModule,
            pAllocator
        );
    
        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroyShaderModule failed for guest shader module %llu", (unsigned long long)guest_shader_module);
        } else {
            LOGD("Host: vkDestroyShaderModule completed for guest shader module %llu", (unsigned long long)guest_shader_module);
        }
    
        //if (need_free) free(stream);
    }
    break;
    
    //note: surface binded to instance
    case FUNID_vkDestroySurfaceKHR: {
        LOGD("Host: vkDestroySurfaceKHR request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroySurfaceKHR para count = %d", para_num);
    
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroySurfaceKHR failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;
    
        uint64_t guest_instance = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_surface = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
    
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
    
        VkResult result = destroy_vulkan_object_other(
            guest_instance,
            guest_surface,
            EXPRESS_VK_OBJECT_TYPE_INSTANCE,
            EXPRESS_VK_OBJECT_TYPE_SURFACE_KHR,
            (void (*)(void*, void*, const VkAllocationCallbacks*))vkDestroySurfaceKHR,
            pAllocator
        );
    
        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroySurfaceKHR failed for guest surface %llu", (unsigned long long)guest_surface);
        } else {
            LOGD("Host: vkDestroySurfaceKHR completed for guest surface %llu", (unsigned long long)guest_surface);
        }
    
        //if (need_free) free(stream);
    }
    break;
    
    case FUNID_vkDestroySwapchainKHR: {
        LOGD("Host: vkDestroySwapchainKHR request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGD("Host vkDestroySwapchainKHR para count = %d", para_num);
    
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        if (!stream) {
            LOGE("Host: vkDestroySwapchainKHR failed, stream is NULL");
            break;
        }
        uint8_t** ptr = (uint8_t**)&stream;
    
        uint64_t guest_dev = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_swapchain = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
    
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct;
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocStruct, ptr);
            pAllocator = &allocStruct;
        }
    
        VkResult result = destroy_vulkan_object_device(
            guest_dev,
            guest_swapchain,
            EXPRESS_VK_OBJECT_TYPE_SWAPCHAIN_KHR,
            (void (*)(VkDevice, void*, const VkAllocationCallbacks*))vkDestroySwapchainKHR,
            pAllocator
        );
    
        if (result != VK_SUCCESS) {
            LOGE("Host: vkDestroySwapchainKHR failed for guest swapchain %llu", (unsigned long long)guest_swapchain);
        } else {
            LOGD("Host: vkDestroySwapchainKHR completed for guest swapchain %llu", (unsigned long long)guest_swapchain);
        }
    
        //if (need_free) free(stream);
    }
    break;

//ztodo:not test yet from here
    case FUNID_vkCmdBeginRendering:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        VkRenderingInfo renderingInfo = {};
        decode_from_stream_VkRenderingInfo(VK_STRUCTURE_TYPE_MAX_ENUM, &renderingInfo, ptr);
        
        vkCmdBeginRendering(commandBuffer, &renderingInfo);
        LOGD("Host: vkCmdBeginRendering called for command buffer %llu", (unsigned long long)guest_cmd);
    }
    break;

    case FUNID_vkCmdBeginRenderPass2:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        VkRenderPassBeginInfo renderPassBegin = {};
        VkSubpassBeginInfo subpassBegin = {};
        
        decode_from_stream_VkRenderPassBeginInfo(VK_STRUCTURE_TYPE_MAX_ENUM, &renderPassBegin, ptr);
        decode_from_stream_VkSubpassBeginInfo(VK_STRUCTURE_TYPE_MAX_ENUM, &subpassBegin, ptr);
        
        vkCmdBeginRenderPass2(commandBuffer, &renderPassBegin, &subpassBegin);
        LOGD("Host: vkCmdBeginRenderPass2 called for command buffer %llu", (unsigned long long)guest_cmd);
    }
    break;

    case FUNID_vkCmdBindIndexBuffer:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDeviceSize offset = *(VkDeviceSize*)(*ptr); *ptr += sizeof(VkDeviceSize);
        VkIndexType indexType = *(VkIndexType*)(*ptr); *ptr += sizeof(VkIndexType);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        VkBuffer buffer = (VkBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER, guest_buffer);
        
        vkCmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
        LOGD("Host: vkCmdBindIndexBuffer called for command buffer %llu with buffer %llu", 
             (unsigned long long)guest_cmd, (unsigned long long)guest_buffer);
    }
    break;

    case FUNID_vkCmdBindVertexBuffers2:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t firstBinding = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t bindingCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t has_sizes = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t has_strides = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buffer);
        
        VkBuffer* buffers = (VkBuffer*)malloc(bindingCount * sizeof(VkBuffer));
        VkDeviceSize* offsets = (VkDeviceSize*)malloc(bindingCount * sizeof(VkDeviceSize));
        VkDeviceSize* sizes = has_sizes ? (VkDeviceSize*)malloc(bindingCount * sizeof(VkDeviceSize)) : NULL;
        VkDeviceSize* strides = has_strides ? (VkDeviceSize*)malloc(bindingCount * sizeof(VkDeviceSize)) : NULL;
        
        for (uint32_t i = 0; i < bindingCount; ++i) {
            uint64_t guest_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
            buffers[i] = (VkBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER, guest_buffer);
            offsets[i] = *(VkDeviceSize*)(*ptr); *ptr += sizeof(VkDeviceSize);
        }
        
        if (has_sizes) {
            for (uint32_t i = 0; i < bindingCount; ++i) {
                sizes[i] = *(VkDeviceSize*)(*ptr); *ptr += sizeof(VkDeviceSize);
            }
        }
        
        if (has_strides) {
            for (uint32_t i = 0; i < bindingCount; ++i) {
                strides[i] = *(VkDeviceSize*)(*ptr); *ptr += sizeof(VkDeviceSize);
            }
        }
        
        vkCmdBindVertexBuffers2(commandBuffer, firstBinding, bindingCount, buffers, offsets, sizes, strides);

        LOGD("Host: vkCmdBindVertexBuffers2 called for command buffer %llu with %u bindings", 
             (unsigned long long)guest_cmd_buffer, bindingCount);
        
        free(buffers);
        free(offsets);
        if (sizes) free(sizes);
        if (strides) free(strides);
    }
    break;

    case FUNID_vkCmdBlitImage2:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        VkBlitImageInfo2 blitImageInfo = {};
        decode_from_stream_VkBlitImageInfo2(VK_STRUCTURE_TYPE_MAX_ENUM, &blitImageInfo, ptr);
        
        vkCmdBlitImage2(commandBuffer, &blitImageInfo);

        LOGD("Host: vkCmdBlitImage2 called for command buffer %llu", (unsigned long long)guest_cmd);
    }
    break;

    case FUNID_vkCmdClearAttachments:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t attachmentCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t rectCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        VkClearAttachment* pAttachments = NULL;
        VkClearRect* pRects = NULL;
        
        if (attachmentCount > 0) {
            pAttachments = malloc(attachmentCount * sizeof(VkClearAttachment));
            memcpy(pAttachments, *ptr, attachmentCount * sizeof(VkClearAttachment));
            *ptr += attachmentCount * sizeof(VkClearAttachment);
        }
        
        if (rectCount > 0) {
            pRects = malloc(rectCount * sizeof(VkClearRect));
            memcpy(pRects, *ptr, rectCount * sizeof(VkClearRect));
            *ptr += rectCount * sizeof(VkClearRect);
        }
        
        vkCmdClearAttachments(commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
        
        if (pAttachments) free(pAttachments);
        if (pRects) free(pRects);
        LOGD("Host: vkCmdClearAttachments called for command buffer %llu with %u attachments and %u rects", 
             (unsigned long long)guest_cmd, attachmentCount, rectCount);
    }
    break;

    case FUNID_vkCmdClearColorImage:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_image = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t layout = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkClearColorValue clearColor;
        decode_from_stream_VkClearColorValue(VK_STRUCTURE_TYPE_MAX_ENUM, &clearColor, ptr);
        
        uint32_t rangeCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkImageSubresourceRange* ranges = malloc(rangeCount * sizeof(VkImageSubresourceRange));
        for (uint32_t i = 0; i < rangeCount; ++i) {
            decode_from_stream_VkImageSubresourceRange(VK_STRUCTURE_TYPE_MAX_ENUM, &ranges[i], ptr);
        }
        
        VkCommandBuffer cmdBuffer = (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        VkImage image = (VkImage)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_IMAGE, guest_image);
        
        vkCmdClearColorImage(cmdBuffer, image, (VkImageLayout)layout, &clearColor, rangeCount, ranges);
        LOGD("Host: vkCmdClearColorImage called for command buffer %llu with image %llu", 
             (unsigned long long)guest_cmd, (unsigned long long)guest_image);
        
        free(ranges);
    }
    break;

    case FUNID_vkCmdClearDepthStencilImage:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_image = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t layout = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkClearDepthStencilValue depthStencil;
        decode_from_stream_VkClearDepthStencilValue(VK_STRUCTURE_TYPE_MAX_ENUM, &depthStencil, ptr);
        
        uint32_t rangeCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkImageSubresourceRange* ranges = malloc(rangeCount * sizeof(VkImageSubresourceRange));
        for (uint32_t i = 0; i < rangeCount; ++i) {
            decode_from_stream_VkImageSubresourceRange(VK_STRUCTURE_TYPE_MAX_ENUM, &ranges[i], ptr);
        }
        
        VkCommandBuffer cmdBuffer = (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        VkImage image = (VkImage)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_IMAGE, guest_image);
        
        vkCmdClearDepthStencilImage(cmdBuffer, image, (VkImageLayout)layout, &depthStencil, rangeCount, ranges);
        LOGD("Host: vkCmdClearDepthStencilImage called for command buffer %llu with image %llu", 
             (unsigned long long)guest_cmd, (unsigned long long)guest_image);   

        
        free(ranges);
    }
    break;

    case FUNID_vkCmdCopyBuffer2:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        VkCopyBufferInfo2 copyInfo = {};
        decode_from_stream_VkCopyBufferInfo2(VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2, &copyInfo, ptr);
        
        VkCommandBuffer cmdBuffer = (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        vkCmdCopyBuffer2(cmdBuffer, &copyInfo);
        LOGD("Host: vkCmdCopyBuffer2 called for command buffer %llu", (unsigned long long)guest_cmd);
    }
    break;

    case FUNID_vkCmdCopyBufferToImage2:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        VkCopyBufferToImageInfo2 copyInfo;
        decode_from_stream_VkCopyBufferToImageInfo2(VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2, &copyInfo, ptr);
        
        VkCommandBuffer cmdBuffer = (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        vkCmdCopyBufferToImage2(cmdBuffer, &copyInfo);
        LOGD("Host: vkCmdCopyBufferToImage2 called for command buffer %llu", (unsigned long long)guest_cmd);
    }
    break;

    case FUNID_vkCmdCopyImage2:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        VkCopyImageInfo2 copyInfo;
        decode_from_stream_VkCopyImageInfo2(VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2, &copyInfo, ptr);
        
        VkCommandBuffer cmdBuffer = (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        vkCmdCopyImage2(cmdBuffer, &copyInfo);
        LOGD("Host: vkCmdCopyImage2 called for command buffer %llu", (unsigned long long)guest_cmd);
    }
    break;

    case FUNID_vkCmdCopyImageToBuffer2:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        VkCopyImageToBufferInfo2 copyInfo;
        decode_from_stream_VkCopyImageToBufferInfo2(VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2, &copyInfo, ptr);
        
        VkCommandBuffer cmdBuffer = (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        vkCmdCopyImageToBuffer2(cmdBuffer, &copyInfo);
        LOGD("Host: vkCmdCopyImageToBuffer2 called for command buffer %llu", (unsigned long long)guest_cmd);
    }
    break;

    case FUNID_vkCmdDispatch:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t groupCountX = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t groupCountY = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t groupCountZ = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buffer);
        
        vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);

        LOGD("Host: vkCmdDispatch called for command buffer %llu with group counts (%u, %u, %u)", 
             (unsigned long long)guest_cmd_buffer, groupCountX, groupCountY, groupCountZ);
    }
    break;

    case FUNID_vkCmdDispatchBase:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t baseGroupX = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t baseGroupY = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t baseGroupZ = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t groupCountX = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t groupCountY = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t groupCountZ = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buffer);
        
        vkCmdDispatchBase(commandBuffer, baseGroupX, baseGroupY, baseGroupZ,
                        groupCountX, groupCountY, groupCountZ);
        LOGD("in FUNID_vkCmdDispatchBase finish");
    }
    break;

    case FUNID_vkCmdDrawIndexed:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t indexCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t instanceCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t firstIndex = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        int32_t vertexOffset = *(int32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t firstInstance = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buffer);
        
        vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, 
                        firstIndex, vertexOffset, firstInstance);
        LOGD("Host: vkCmdDrawIndexed called");
    }
    break;

    case FUNID_vkCmdDrawIndexedIndirectCount:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDeviceSize offset = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_count_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDeviceSize countBufferOffset = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t maxDrawCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t stride = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buffer);
        VkBuffer buffer = (VkBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER, guest_buffer);
        VkBuffer countBuffer = (VkBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER, guest_count_buffer);
        
        vkCmdDrawIndexedIndirectCount(commandBuffer, buffer, offset, 
                                    countBuffer, countBufferOffset, 
                                    maxDrawCount, stride);
        LOGD("Host: vkCmdDrawIndexedIndirectCount called for command buffer %llu", 
             (unsigned long long)guest_cmd_buffer);
    }
    break;

    case FUNID_vkCmdDrawIndirectCount:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDeviceSize offset = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_count_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDeviceSize countBufferOffset = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t maxDrawCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t stride = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buffer);
        VkBuffer buffer = (VkBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER, guest_buffer);
        VkBuffer countBuffer = (VkBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER, guest_count_buffer);
        
        vkCmdDrawIndirectCount(commandBuffer, buffer, offset, 
                            countBuffer, countBufferOffset, 
                            maxDrawCount, stride);
        LOGD("Host: vkCmdDrawIndirectCount called for command buffer %llu", 
             (unsigned long long)guest_cmd_buffer);    
    }
    break;

    case FUNID_vkCmdEndRendering:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buffer);
        
        vkCmdEndRendering(commandBuffer);
        LOGD("Host: vkCmdEndRendering called for command buffer %llu", (unsigned long long)guest_cmd_buffer);
    }
    break;

    case FUNID_vkCmdEndRenderPass2:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buffer);
        
        VkSubpassEndInfo* guest_subpass_end = (VkSubpassEndInfo*)(*ptr);
        *ptr += 8;
        
        VkSubpassEndInfo* pSubpassEndInfo = NULL;
        if (guest_subpass_end) {
            pSubpassEndInfo = (VkSubpassEndInfo*)malloc(sizeof(VkSubpassEndInfo));
            decode_from_stream_VkSubpassEndInfo(VK_STRUCTURE_TYPE_MAX_ENUM, 
                                            pSubpassEndInfo, ptr);
        }
        
        vkCmdEndRenderPass2(commandBuffer, pSubpassEndInfo);
        
        if (pSubpassEndInfo) free(pSubpassEndInfo);
        LOGD("Host: vkCmdEndRenderPass2 called for command buffer %llu", (unsigned long long)guest_cmd_buffer);
    }
    break;

    case FUNID_vkCmdExecuteCommands:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t commandBufferCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buffer);
        
        VkCommandBuffer* pCommandBuffers = NULL;
        if (commandBufferCount > 0) {
            pCommandBuffers = (VkCommandBuffer*)malloc(commandBufferCount * sizeof(VkCommandBuffer));
            for (uint32_t i = 0; i < commandBufferCount; ++i) {
                uint64_t guest_secondary_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
                pCommandBuffers[i] = (VkCommandBuffer)(uintptr_t)
                    lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_secondary_cmd);
            }
        }
        
        vkCmdExecuteCommands(commandBuffer, commandBufferCount, pCommandBuffers);
        
        if (pCommandBuffers) free(pCommandBuffers);
        LOGD("Host: vkCmdExecuteCommands called for command buffer %llu with %u secondary command buffers", 
             (unsigned long long)guest_cmd_buffer, commandBufferCount);
    }
    break;

    case FUNID_vkCmdNextSubpass:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t contents = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        LOGD("Host: vkCmdNextSubpass called with contents %u", contents);
        
        vkCmdNextSubpass(commandBuffer, (VkSubpassContents)contents);
        LOGD("Host: vkCmdNextSubpass called for command buffer %llu with contents %u", 
             (unsigned long long)guest_cmd, contents);
    }
    break;

    case FUNID_vkCmdNextSubpass2:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        // uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        uint64_t begin_ptr = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkSubpassBeginInfo* pSubpassBeginInfo = NULL;
        if (begin_ptr) {
            pSubpassBeginInfo = malloc(sizeof(VkSubpassBeginInfo));
            decode_from_stream_VkSubpassBeginInfo(VK_STRUCTURE_TYPE_MAX_ENUM,
                                                pSubpassBeginInfo, ptr);
        }
        
        uint64_t end_ptr = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkSubpassEndInfo* pSubpassEndInfo = NULL;
        if (end_ptr) {
            pSubpassEndInfo = malloc(sizeof(VkSubpassEndInfo));
            decode_from_stream_VkSubpassEndInfo(VK_STRUCTURE_TYPE_MAX_ENUM,
                                                pSubpassEndInfo, ptr);
        }

        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        vkCmdNextSubpass2(commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);
        
        if (pSubpassBeginInfo) free(pSubpassBeginInfo);
        if (pSubpassEndInfo) free(pSubpassEndInfo);
        LOGD("Host: vkCmdNextSubpass2 called for command buffer %llu", (unsigned long long)commandBuffer);
    }
    break;

    case FUNID_vkCmdPipelineBarrier2:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t dep_ptr = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDependencyInfo* pDependencyInfo = NULL;
        if (dep_ptr) {
            pDependencyInfo = malloc(sizeof(VkDependencyInfo));
            LOGD("Host: vkCmdPipelineBarrier2 dep_ptr %llu", dep_ptr);
            decode_from_stream_VkDependencyInfo(VK_STRUCTURE_TYPE_MAX_ENUM,
                                                pDependencyInfo, ptr);
        }
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        vkCmdPipelineBarrier2(commandBuffer, pDependencyInfo);
        
        if (pDependencyInfo) free(pDependencyInfo);
        LOGD("Host: vkCmdPipelineBarrier2 called for command buffer %llu", (unsigned long long)guest_cmd);
    }
    break;

    case FUNID_vkCmdResetEvent2:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_event = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkPipelineStageFlags2 stageMask = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        VkEvent event = (VkEvent)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_EVENT, guest_event);
        
        vkCmdResetEvent2(commandBuffer, event, stageMask);
        LOGD("Host: vkCmdResetEvent2 called for command buffer %llu with event %llu", 
             (unsigned long long)guest_cmd, (unsigned long long)guest_event);
    }
    break;

    case FUNID_vkCmdResolveImage:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_src = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t src_layout = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint64_t guest_dst = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t dst_layout = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t regionCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        VkImage srcImage = (VkImage)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_IMAGE, guest_src);
        VkImage dstImage = (VkImage)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_IMAGE, guest_dst);
        
        VkImageResolve* pRegions = NULL;
        if (regionCount > 0) {
            pRegions = (VkImageResolve*)stream;
        }
        
        vkCmdResolveImage(commandBuffer, srcImage, (VkImageLayout)src_layout,
                        dstImage, (VkImageLayout)dst_layout, regionCount, pRegions);
        LOGD("Host: vkCmdResolveImage called for command buffer %llu with source image %llu and destination image %llu",
             (unsigned long long)guest_cmd, (unsigned long long)guest_src, (unsigned long)guest_dst);
    }
    break;

    case FUNID_vkCmdResolveImage2:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        uint64_t resolve_ptr = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkResolveImageInfo2* pResolveImageInfo = NULL;
        if (resolve_ptr) {
            pResolveImageInfo = malloc(sizeof(VkResolveImageInfo2));
            decode_from_stream_VkResolveImageInfo2(VK_STRUCTURE_TYPE_MAX_ENUM,
                                                pResolveImageInfo, ptr);
        }
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        vkCmdResolveImage2(commandBuffer, pResolveImageInfo);
        
        if (pResolveImageInfo) free(pResolveImageInfo);
        LOGD("Host: vkCmdResolveImage2 called for command buffer %llu", (unsigned long long)guest_cmd);
    }
    break;

    case FUNID_vkCmdSetBlendConstants:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        float* blendConstants = (float*)stream;
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        vkCmdSetBlendConstants(commandBuffer, blendConstants);
        LOGD("Host: vkCmdSetBlendConstants called for command buffer %llu", (unsigned long long)guest_cmd);
    }
    break;

    case FUNID_vkCmdSetDepthBounds:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
    
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
    
        float minDepthBounds, maxDepthBounds;
        memcpy(&minDepthBounds, *ptr, 4); *ptr += 4;
        memcpy(&maxDepthBounds, *ptr, 4); *ptr += 4;
    
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
    
        vkCmdSetDepthBounds(commandBuffer, minDepthBounds, maxDepthBounds);
        LOGD("Host: vkCmdSetDepthBounds called");
    }
    break;

    case FUNID_vkCmdSetCullMode:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkCullModeFlags cullMode = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buffer);
        
        vkCmdSetCullMode(commandBuffer, cullMode);
        LOGD("Host: vkCmdSetCullMode called for command buffer %llu with cull mode %u", 
             (unsigned long long)guest_cmd_buffer, cullMode);
    }
    break;

    case FUNID_vkCmdSetDepthBiasEnable:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkBool32 depthBiasEnable = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buffer);
        
        vkCmdSetDepthBiasEnable(commandBuffer, depthBiasEnable);
        LOGD("Host: vkCmdSetDepthBiasEnable called for command buffer %llu with depth bias enable %u", 
             (unsigned long long)guest_cmd_buffer, depthBiasEnable);
    }
    break;

    case FUNID_vkCmdSetDepthBoundsTestEnable:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkBool32 depthBoundsTestEnable = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buffer);
        
        vkCmdSetDepthBoundsTestEnable(commandBuffer, depthBoundsTestEnable);
    }
    break;

    case FUNID_vkCmdSetDepthTestEnable:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkBool32 depthTestEnable = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buffer);
        
        vkCmdSetDepthTestEnable(commandBuffer, depthTestEnable);
    }
    break;

    case FUNID_vkCmdSetDepthWriteEnable:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkBool32 depthWriteEnable = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buffer);
        
        vkCmdSetDepthWriteEnable(commandBuffer, depthWriteEnable);
    }
    break;

    case FUNID_vkCmdSetDeviceMask:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t deviceMask = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buffer);
        
        vkCmdSetDeviceMask(commandBuffer, deviceMask);
    }
    break;

    case FUNID_vkCmdSetEvent2:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_event = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t has_dependency_info = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buffer);
        VkEvent event = (VkEvent)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_EVENT, guest_event);
        
        VkDependencyInfo dependencyInfo;
        const VkDependencyInfo* pDependencyInfo = NULL;
        
        if (has_dependency_info) {
            decode_from_stream_VkDependencyInfo(VK_STRUCTURE_TYPE_DEPENDENCY_INFO, &dependencyInfo, ptr);
            pDependencyInfo = &dependencyInfo;
        }
        
        vkCmdSetEvent2(commandBuffer, event, pDependencyInfo);
        LOGD("Host: vkCmdSetEvent2 called for command buffer %llu with event %llu", 
             (unsigned long long)guest_cmd_buffer, (unsigned long long)guest_event);
    }
    break;

    case FUNID_vkCmdSetFrontFace:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkFrontFace frontFace = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buffer);
        
        vkCmdSetFrontFace(commandBuffer, frontFace);
    }
    break;

    case FUNID_vkCmdSetLineWidth:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        float lineWidth = *(float*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        vkCmdSetLineWidth(commandBuffer, lineWidth);
    }
    break;

    case FUNID_vkCmdSetPrimitiveRestartEnable:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkBool32 primitiveRestartEnable = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        vkCmdSetPrimitiveRestartEnable(commandBuffer, primitiveRestartEnable);
    }
    break;

    case FUNID_vkCmdSetPrimitiveTopology:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkPrimitiveTopology primitiveTopology = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        vkCmdSetPrimitiveTopology(commandBuffer, primitiveTopology);
    }
    break;

    case FUNID_vkCmdSetRasterizerDiscardEnable:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkBool32 rasterizerDiscardEnable = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        vkCmdSetRasterizerDiscardEnable(commandBuffer, rasterizerDiscardEnable);
    }
    break;

    case FUNID_vkCmdSetScissor:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint32_t firstScissor = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t scissorCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkRect2D* pScissors = NULL;
        if (scissorCount > 0) {
            pScissors = (VkRect2D*)malloc(scissorCount * sizeof(VkRect2D));
            memcpy(pScissors, *ptr, scissorCount * sizeof(VkRect2D));
        }
        *ptr += scissorCount * sizeof(VkRect2D);
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        vkCmdSetScissor(commandBuffer, firstScissor, scissorCount, pScissors);
        
        if (pScissors) free(pScissors);
    }
    break;

    case FUNID_vkCmdSetScissorWithCount:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint32_t scissorCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkRect2D* pScissors = NULL;
        if (scissorCount > 0) {
            pScissors = (VkRect2D*)malloc(scissorCount * sizeof(VkRect2D));
            memcpy(pScissors, *ptr, scissorCount * sizeof(VkRect2D));
        }
        *ptr += scissorCount * sizeof(VkRect2D);
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        vkCmdSetScissorWithCount(commandBuffer, scissorCount, pScissors);
        
        if (pScissors) free(pScissors);
    }
    break;

    case FUNID_vkCmdSetStencilCompareMask:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkStencilFaceFlags faceMask = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t compareMask = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        vkCmdSetStencilCompareMask(commandBuffer, faceMask, compareMask);
    }
    break;

    case FUNID_vkCmdSetStencilOp:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkStencilFaceFlags faceMask = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        VkStencilOp failOp = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        VkStencilOp passOp = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        VkStencilOp depthFailOp = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        VkCompareOp compareOp = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        vkCmdSetStencilOp(commandBuffer, faceMask, failOp, passOp, depthFailOp, compareOp);
    }
    break;

    case FUNID_vkCmdSetStencilReference:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkStencilFaceFlags face_mask = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t reference = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        vkCmdSetStencilReference(commandBuffer, face_mask, reference);
        LOGD("Host: CmdSetStencilReference");
    }
    break;

    case FUNID_vkCmdSetStencilTestEnable:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkBool32 stencil_test_enable = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        vkCmdSetStencilTestEnable(commandBuffer, stencil_test_enable);
        LOGD("Host: CmdSetStencilTestEnable");
    }
    break;

    case FUNID_vkCmdSetStencilWriteMask:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkStencilFaceFlags face_mask = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t write_mask = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        vkCmdSetStencilWriteMask(commandBuffer, face_mask, write_mask);
        LOGD("Host: CmdSetStencilWriteMask");
    }
    break;

    case FUNID_vkCmdSetViewport:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t first_viewport = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint32_t viewport_count = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        VkViewport* viewports = (VkViewport*)malloc(viewport_count * sizeof(VkViewport));
        read_from_guest_mem(all_para[1].data, viewports, 0, viewport_count * sizeof(VkViewport));
        
        vkCmdSetViewport(commandBuffer, first_viewport, viewport_count, viewports);
        
        free(viewports);
        LOGD("Host: CmdSetViewport");
    }
    break;

    case FUNID_vkCmdSetViewportWithCount:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t viewport_count = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        VkViewport* viewports = (VkViewport*)malloc(viewport_count * sizeof(VkViewport));
        read_from_guest_mem(all_para[1].data, viewports, 0, viewport_count * sizeof(VkViewport));
        
        vkCmdSetViewportWithCount(commandBuffer, viewport_count, viewports);
        
        free(viewports);
        LOGD("Host: CmdSetViewportWithCount");
    }
    break;

    case FUNID_vkCmdWaitEvents2:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t event_count = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        uint64_t* guest_events = (uint64_t*)malloc(event_count * sizeof(uint64_t));
        read_from_guest_mem(all_para[1].data, guest_events, 0, event_count * sizeof(uint64_t));
        
        VkEvent* events = (VkEvent*)malloc(event_count * sizeof(VkEvent));
        for (uint32_t i = 0; i < event_count; ++i) {
            events[i] = (VkEvent)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_EVENT, guest_events[i]);
        }
        
        VkDependencyInfo* dependency_infos = (VkDependencyInfo*)malloc(event_count * sizeof(VkDependencyInfo));
        for (uint32_t i = 0; i < event_count; ++i) {
            decode_from_stream_VkDependencyInfo(VK_STRUCTURE_TYPE_MAX_ENUM, &dependency_infos[i], ptr);
        }
        
        vkCmdWaitEvents2(commandBuffer, event_count, events, dependency_infos);
        
        free(guest_events);
        free(events);
        free(dependency_infos);
        LOGD("Host: CmdWaitEvents2");
    }
    break;

    case FUNID_vkCmdWriteTimestamp2:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkPipelineStageFlags2 stage = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_query_pool = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t query = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        VkQueryPool queryPool = (VkQueryPool)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_QUERY_POOL, guest_query_pool);
        
        vkCmdWriteTimestamp2(commandBuffer, stage, queryPool, query);
        LOGD("Host: CmdWriteTimestamp2");
    }
    break;

    case FUNID_vkCreateComputePipelines:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device, guest_cache, guest_allocator;
        uint32_t createInfoCount;
        uint8_t has_allocator;
        
        memcpy(&guest_device, *ptr, sizeof(uint64_t)); *ptr += sizeof(uint64_t);
        memcpy(&guest_cache, *ptr, sizeof(uint64_t)); *ptr += sizeof(uint64_t);
        memcpy(&createInfoCount, *ptr, sizeof(uint32_t)); *ptr += sizeof(uint32_t);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkPipelineCache pipelineCache = guest_cache ? (VkPipelineCache)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PIPELINE_CACHE, guest_cache) : VK_NULL_HANDLE;
        
        VkComputePipelineCreateInfo* pCreateInfos = (VkComputePipelineCreateInfo*)malloc(sizeof(VkComputePipelineCreateInfo) * createInfoCount);
        
        for (uint32_t i = 0; i < createInfoCount; ++i) {
            decode_from_stream_VkComputePipelineCreateInfo(VK_STRUCTURE_TYPE_MAX_ENUM, &pCreateInfos[i], ptr);
        }
        
        memcpy(&guest_allocator, *ptr, sizeof(uint64_t)); *ptr += sizeof(uint64_t);
        memcpy(&has_allocator, *ptr, sizeof(uint8_t)); *ptr += sizeof(uint8_t);
        
        VkAllocationCallbacks* pAllocator = NULL;
        VkAllocationCallbacks allocator_copy;
        if (has_allocator) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocator_copy, ptr);
            pAllocator = &allocator_copy;
        }
        
        VkPipeline* pPipelines = (VkPipeline*)malloc(sizeof(VkPipeline) * createInfoCount);
        
        VkResult result = vkCreateComputePipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
        
        if (result == VK_SUCCESS) {
            uint64_t* guest_pipelines = (uint64_t*)malloc(sizeof(uint64_t) * createInfoCount);
            read_from_guest_mem(all_para[1].data, guest_pipelines, 0, sizeof(uint64_t) * createInfoCount);
            
            for (uint32_t i = 0; i < createInfoCount; ++i) {
                insert_mapping(EXPRESS_VK_OBJECT_TYPE_PIPELINE, guest_pipelines[i], (uint64_t)(uintptr_t)pPipelines[i]);
            }
            free(guest_pipelines);
        } else {
            for (uint32_t i = 0; i < createInfoCount; ++i) {
                pPipelines[i] = VK_NULL_HANDLE;
            }
            LOGE("vkCreateComputePipelines failed with error %d", result);
        }

        free(pCreateInfos);
        free(pPipelines);
        LOGD("vkCreateComputePipelines result %d", result);
    }
    break;

    case FUNID_vkCreateDescriptorUpdateTemplate:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device, guest_allocator;
        uint8_t has_allocator;
        
        memcpy(&guest_device, *ptr, sizeof(uint64_t)); *ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        VkDescriptorUpdateTemplateCreateInfo createInfo;
        decode_from_stream_VkDescriptorUpdateTemplateCreateInfo(VK_STRUCTURE_TYPE_MAX_ENUM, &createInfo, ptr);
        
        memcpy(&guest_allocator, *ptr, sizeof(uint64_t)); *ptr += sizeof(uint64_t);
        memcpy(&has_allocator, *ptr, sizeof(uint8_t)); *ptr += sizeof(uint8_t);
        
        VkAllocationCallbacks* pAllocator = NULL;
        VkAllocationCallbacks allocator_copy;
        if (has_allocator) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocator_copy, ptr);
            pAllocator = &allocator_copy;
        }
        
        VkDescriptorUpdateTemplate template;
        VkResult result = vkCreateDescriptorUpdateTemplate(device, &createInfo, pAllocator, &template);
        
        if (result == VK_SUCCESS) {
            uint64_t guest_template;
            read_from_guest_mem(all_para[1].data, &guest_template, 0, sizeof(uint64_t));
            insert_mapping(EXPRESS_VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE, guest_template, (uint64_t)(uintptr_t)template);
        } else {
            template = VK_NULL_HANDLE;
            LOGE("vkCreateDescriptorUpdateTemplate failed with error %d", result);
        }
        LOGD("vkCreateDescriptorUpdateTemplate result %d", result);
    }
    break;

    case FUNID_vkCreatePrivateDataSlot:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device, guest_allocator;
        uint8_t has_allocator;
        
        memcpy(&guest_device, *ptr, sizeof(uint64_t)); *ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        VkPrivateDataSlotCreateInfo createInfo;
        decode_from_stream_VkPrivateDataSlotCreateInfo(VK_STRUCTURE_TYPE_MAX_ENUM, &createInfo, ptr);
        
        memcpy(&guest_allocator, *ptr, sizeof(uint64_t)); *ptr += sizeof(uint64_t);
        memcpy(&has_allocator, *ptr, sizeof(uint8_t)); *ptr += sizeof(uint8_t);
        
        VkAllocationCallbacks* pAllocator = NULL;
        VkAllocationCallbacks allocator_copy;
        if (has_allocator) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocator_copy, ptr);
            pAllocator = &allocator_copy;
        }
        
        VkPrivateDataSlot slot;
        VkResult result = vkCreatePrivateDataSlot(device, &createInfo, pAllocator, &slot);
        
        if (result == VK_SUCCESS) {
            uint64_t guest_slot;
            read_from_guest_mem(all_para[1].data, &guest_slot, 0, sizeof(uint64_t));
            insert_mapping(EXPRESS_VK_OBJECT_TYPE_PRIVATE_DATA_SLOT, guest_slot, (uint64_t)(uintptr_t)slot);
        } else {
            slot = VK_NULL_HANDLE;
            LOGE("vkCreatePrivateDataSlot failed with error %d", result);
        }

        LOGD("vkCreatePrivateDataSlot result %d", result);
    }
    break;

    case FUNID_vkCreateRenderPass2:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device, guest_allocator;
        uint8_t has_allocator;
        
        memcpy(&guest_device, *ptr, sizeof(uint64_t)); *ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        VkRenderPassCreateInfo2 createInfo;
        decode_from_stream_VkRenderPassCreateInfo2(VK_STRUCTURE_TYPE_MAX_ENUM, &createInfo, ptr);
        
        memcpy(&guest_allocator, *ptr, sizeof(uint64_t)); *ptr += sizeof(uint64_t);
        memcpy(&has_allocator, *ptr, sizeof(uint8_t)); *ptr += sizeof(uint8_t);
        
        VkAllocationCallbacks* pAllocator = NULL;
        VkAllocationCallbacks allocator_copy;
        if (has_allocator) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocator_copy, ptr);
            pAllocator = &allocator_copy;
        }
        
        VkRenderPass renderPass;
        VkResult result = vkCreateRenderPass2(device, &createInfo, pAllocator, &renderPass);
        
        if (result == VK_SUCCESS) {
            uint64_t guest_renderpass;
            read_from_guest_mem(all_para[1].data, &guest_renderpass, 0, sizeof(uint64_t));
            insert_mapping(EXPRESS_VK_OBJECT_TYPE_RENDER_PASS, guest_renderpass, (uint64_t)(uintptr_t)renderPass);
        } else {
            renderPass = VK_NULL_HANDLE;
            LOGE("vkCreateRenderPass2 failed with error %d", result);
        }   
        LOGD("vkCreateRenderPass2 result %d", result);
    }
    break;

    case FUNID_vkCreateSamplerYcbcrConversion:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device, guest_allocator;
        uint8_t has_allocator;
        
        memcpy(&guest_device, *ptr, sizeof(uint64_t)); *ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        VkSamplerYcbcrConversionCreateInfo createInfo;
        decode_from_stream_VkSamplerYcbcrConversionCreateInfo(VK_STRUCTURE_TYPE_MAX_ENUM, &createInfo, ptr);
        
        memcpy(&guest_allocator, *ptr, sizeof(uint64_t)); *ptr += sizeof(uint64_t);
        memcpy(&has_allocator, *ptr, sizeof(uint8_t)); *ptr += sizeof(uint8_t);
        
        VkAllocationCallbacks* pAllocator = NULL;
        VkAllocationCallbacks allocator_copy;
        if (has_allocator) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocator_copy, ptr);
            pAllocator = &allocator_copy;
        }
        
        VkSamplerYcbcrConversion conversion;
        VkResult result = vkCreateSamplerYcbcrConversion(device, &createInfo, pAllocator, &conversion);
        
        if (result == VK_SUCCESS) {
            uint64_t guest_conversion;
            read_from_guest_mem(all_para[1].data, &guest_conversion, 0, sizeof(uint64_t));
            insert_mapping(EXPRESS_VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION, guest_conversion, (uint64_t)(uintptr_t)conversion);
        } else {
            conversion = VK_NULL_HANDLE;
            LOGE("vkCreateSamplerYcbcrConversion failed with error %d", result);
        }

        LOGD("vkCreateSamplerYcbcrConversion result %d", result);
    }
    break;

    case FUNID_vkDeviceWaitIdle:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        VkResult result = vkDeviceWaitIdle(device);
        if (result != VK_SUCCESS) {
            LOGE("vkDeviceWaitIdle failed with error %d", result);
        } else {
            LOGD("vkDeviceWaitIdle completed successfully");
        }
        LOGD("Host: vkDeviceWaitIdle result %d", result);
    }
    break;

    case FUNID_vkCmdSetDepthBias:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        float depthBiasConstantFactor = *(float*)(*ptr); *ptr += sizeof(float);
        float depthBiasClamp = *(float*)(*ptr); *ptr += sizeof(float);
        float depthBiasSlopeFactor = *(float*)(*ptr); *ptr += sizeof(float);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buffer);
        
        vkCmdSetDepthBias(commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
        LOGD("Host: vkCmdSetDepthBias executed");
    }
    break;

    case FUNID_vkCmdCopyImageToBuffer:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_src_image = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkImageLayout srcImageLayout = *(VkImageLayout*)(*ptr); *ptr += sizeof(VkImageLayout);
        uint64_t guest_dst_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t regionCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buffer);
        VkImage srcImage = (VkImage)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_IMAGE, guest_src_image);
        VkBuffer dstBuffer = (VkBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER, guest_dst_buffer);
        
        VkBufferImageCopy* pRegions = (VkBufferImageCopy*)malloc(sizeof(VkBufferImageCopy) * regionCount);
        memcpy(pRegions, *ptr, sizeof(VkBufferImageCopy) * regionCount);
        
        vkCmdCopyImageToBuffer(commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
        
        free(pRegions);
        LOGD("Host: vkCmdCopyImageToBuffer executed with %d regions", regionCount);
    }
    break;

    case FUNID_vkCmdCopyBuffer:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_src_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_dst_buffer = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t regionCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buffer);
        VkBuffer srcBuffer = (VkBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER, guest_src_buffer);
        VkBuffer dstBuffer = (VkBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER, guest_dst_buffer);
        
        VkBufferCopy* pRegions = (VkBufferCopy*)malloc(sizeof(VkBufferCopy) * regionCount);
        memcpy(pRegions, *ptr, sizeof(VkBufferCopy) * regionCount);
        
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
        
        free(pRegions);
        LOGD("Host: vkCmdCopyBuffer executed with %d regions", regionCount);
    }
    break;

    case FUNID_vkCmdSetDepthCompareOp:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t compare_op = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd);
        
        vkCmdSetDepthCompareOp(commandBuffer, (VkCompareOp)compare_op);
        
        LOGD("Host: CmdSetDepthCompareOp complete");
    }
    break;

    case FUNID_vkGetBufferDeviceAddress:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr); 
        *ptr += sizeof(uint64_t);
        
        VkBufferDeviceAddressInfo info;
        decode_from_stream_VkBufferDeviceAddressInfo(VK_STRUCTURE_TYPE_MAX_ENUM, &info, ptr);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        VkDeviceAddress result = vkGetBufferDeviceAddress(device, &info);
        write_to_guest_mem(all_para[1].data, &result, 0, sizeof(VkDeviceAddress));
        
        LOGD("GetBufferDeviceAddress completed");
    }
    break;

    case FUNID_vkGetBufferOpaqueCaptureAddress:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr); 
        *ptr += sizeof(uint64_t);
        
        VkBufferDeviceAddressInfo info;
        decode_from_stream_VkBufferDeviceAddressInfo(VK_STRUCTURE_TYPE_MAX_ENUM, &info, ptr);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        uint64_t result = vkGetBufferOpaqueCaptureAddress(device, &info);
        write_to_guest_mem(all_para[1].data, &result, 0, sizeof(uint64_t));
        
        LOGD("GetBufferOpaqueCaptureAddress completed");
    }
    break;

    case FUNID_vkGetDeviceMemoryOpaqueCaptureAddress:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr); 
        *ptr += sizeof(uint64_t);
        
        VkDeviceMemoryOpaqueCaptureAddressInfo info;
        decode_from_stream_VkDeviceMemoryOpaqueCaptureAddressInfo(VK_STRUCTURE_TYPE_MAX_ENUM, &info, ptr);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        uint64_t result = vkGetDeviceMemoryOpaqueCaptureAddress(device, &info);
        write_to_guest_mem(all_para[1].data, &result, 0, sizeof(uint64_t));
        
        LOGD("GetDeviceMemoryOpaqueCaptureAddress completed");
    }
    break;

    case FUNID_vkGetDescriptorSetLayoutSupport:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr); 
        *ptr += sizeof(uint64_t);
        
        VkDescriptorSetLayoutCreateInfo createInfo;
        decode_from_stream_VkDescriptorSetLayoutCreateInfo(VK_STRUCTURE_TYPE_MAX_ENUM, &createInfo, ptr);
        
        VkDescriptorSetLayoutSupport support;
        decode_from_stream_VkDescriptorSetLayoutSupport(VK_STRUCTURE_TYPE_MAX_ENUM, &support, ptr);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        vkGetDescriptorSetLayoutSupport(device, &createInfo, &support);
        write_to_guest_mem(all_para[1].data, &support, 0, sizeof(VkDescriptorSetLayoutSupport));
        
        LOGD("GetDescriptorSetLayoutSupport completed");
    }
    break;

    case FUNID_vkGetDeviceBufferMemoryRequirements:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr); 
        *ptr += sizeof(uint64_t);
        
        VkDeviceBufferMemoryRequirements info;
        decode_from_stream_VkDeviceBufferMemoryRequirements(VK_STRUCTURE_TYPE_MAX_ENUM, &info, ptr);
        
        VkMemoryRequirements2 memReq;
        decode_from_stream_VkMemoryRequirements2(VK_STRUCTURE_TYPE_MAX_ENUM, &memReq, ptr);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        vkGetDeviceBufferMemoryRequirements(device, &info, &memReq);
        write_to_guest_mem(all_para[1].data, &memReq, 0, sizeof(VkMemoryRequirements2));
        
        LOGD("GetDeviceBufferMemoryRequirements completed");
    }
    break;

    case FUNID_vkGetDeviceImageMemoryRequirements:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr); 
        *ptr += sizeof(uint64_t);
        
        VkDeviceImageMemoryRequirements info;
        decode_from_stream_VkDeviceImageMemoryRequirements(VK_STRUCTURE_TYPE_MAX_ENUM, &info, ptr);
        
        VkMemoryRequirements2 memReq;
        decode_from_stream_VkMemoryRequirements2(VK_STRUCTURE_TYPE_MAX_ENUM, &memReq, ptr);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        vkGetDeviceImageMemoryRequirements(device, &info, &memReq);
        write_to_guest_mem(all_para[1].data, &memReq, 0, sizeof(VkMemoryRequirements2));
        
        LOGD("GetDeviceImageMemoryRequirements completed");
    }
    break;

    case FUNID_vkGetDeviceImageSparseMemoryRequirements:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr); 
        *ptr += sizeof(uint64_t);
        
        VkDeviceImageMemoryRequirements info;
        decode_from_stream_VkDeviceImageMemoryRequirements(VK_STRUCTURE_TYPE_MAX_ENUM, &info, ptr);
        
        uint32_t count = 0;
        read_from_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        if (count == 0) {
            vkGetDeviceImageSparseMemoryRequirements(device, &info, &count, NULL);
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        } else {
            VkSparseImageMemoryRequirements2* reqs = malloc(count * sizeof(VkSparseImageMemoryRequirements2));
            vkGetDeviceImageSparseMemoryRequirements(device, &info, &count, reqs);
            // write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
            write_to_guest_mem(all_para[2].data, reqs, 0, count * sizeof(VkSparseImageMemoryRequirements2));
            free(reqs);
        }
        
        LOGD("GetDeviceImageSparseMemoryRequirements completed");
    }
    break;

    case FUNID_vkGetDeviceGroupPeerMemoryFeatures:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr); 
        *ptr += sizeof(uint64_t);
        uint32_t heapIndex = *(uint32_t*)(*ptr); 
        *ptr += sizeof(uint32_t);
        uint32_t localDeviceIndex = *(uint32_t*)(*ptr); 
        *ptr += sizeof(uint32_t);
        uint32_t remoteDeviceIndex = *(uint32_t*)(*ptr); 
        *ptr += sizeof(uint32_t);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        VkPeerMemoryFeatureFlags flags;
        vkGetDeviceGroupPeerMemoryFeatures(device, heapIndex, localDeviceIndex, remoteDeviceIndex, &flags);
        write_to_guest_mem(all_para[1].data, &flags, 0, sizeof(VkPeerMemoryFeatureFlags));
        
        LOGD("GetDeviceGroupPeerMemoryFeatures completed");
    }
    break;

    case FUNID_vkEnumeratePhysicalDeviceGroups:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_instance = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkInstance instance = (VkInstance)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_INSTANCE, guest_instance);
        
        uint32_t count = 0;
        read_from_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        
        VkResult result;
        if (count == 0) {
            result = vkEnumeratePhysicalDeviceGroups(instance, &count, NULL);
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        } else {
            VkPhysicalDeviceGroupProperties* properties = (VkPhysicalDeviceGroupProperties*)malloc(count * sizeof(VkPhysicalDeviceGroupProperties));
            if (!properties) {
                result = VK_ERROR_OUT_OF_HOST_MEMORY;
            } else {
                result = vkEnumeratePhysicalDeviceGroups(instance, &count, properties);
                write_to_guest_mem(all_para[2].data, properties, 0, count * sizeof(VkPhysicalDeviceGroupProperties));
                free(properties);
            }
        }
        
        write_to_guest_mem(all_para[3].data, &result, 0, sizeof(VkResult));
        LOGD("EnumeratePhysicalDeviceGroups completed");
    }
    break;

    case FUNID_vkGetPhysicalDeviceExternalBufferProperties:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_device);
        
        VkPhysicalDeviceExternalBufferInfo* pExternalBufferInfo = (VkPhysicalDeviceExternalBufferInfo*)malloc(sizeof(VkPhysicalDeviceExternalBufferInfo));
        decode_from_stream_VkPhysicalDeviceExternalBufferInfo(VK_STRUCTURE_TYPE_MAX_ENUM, pExternalBufferInfo, ptr);
        
        VkExternalBufferProperties properties;
        vkGetPhysicalDeviceExternalBufferProperties(physicalDevice, pExternalBufferInfo, &properties);
        
        write_to_guest_mem(all_para[1].data, &properties, 0, sizeof(VkExternalBufferProperties));
        free(pExternalBufferInfo);
        LOGD("GetPhysicalDeviceExternalBufferProperties completed");
    }
    break;

    case FUNID_vkGetPhysicalDeviceExternalFenceProperties:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_device);
        
        VkPhysicalDeviceExternalFenceInfo* pExternalFenceInfo = (VkPhysicalDeviceExternalFenceInfo*)malloc(sizeof(VkPhysicalDeviceExternalFenceInfo));
        decode_from_stream_VkPhysicalDeviceExternalFenceInfo(VK_STRUCTURE_TYPE_MAX_ENUM, pExternalFenceInfo, ptr);
        
        VkExternalFenceProperties properties;
        vkGetPhysicalDeviceExternalFenceProperties(physicalDevice, pExternalFenceInfo, &properties);
        
        write_to_guest_mem(all_para[1].data, &properties, 0, sizeof(VkExternalFenceProperties));
        free(pExternalFenceInfo);
        LOGD("GetPhysicalDeviceExternalFenceProperties completed");
    }
    break;

    case FUNID_vkGetPhysicalDeviceExternalSemaphoreProperties:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_device);
        
        VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo = (VkPhysicalDeviceExternalSemaphoreInfo*)malloc(sizeof(VkPhysicalDeviceExternalSemaphoreInfo));
        decode_from_stream_VkPhysicalDeviceExternalSemaphoreInfo(VK_STRUCTURE_TYPE_MAX_ENUM, pExternalSemaphoreInfo, ptr);
        
        VkExternalSemaphoreProperties properties;
        vkGetPhysicalDeviceExternalSemaphoreProperties(physicalDevice, pExternalSemaphoreInfo, &properties);
        
        write_to_guest_mem(all_para[1].data, &properties, 0, sizeof(VkExternalSemaphoreProperties));
        free(pExternalSemaphoreInfo);
        LOGD("GetPhysicalDeviceExternalSemaphoreProperties completed");
    }
    break;

    case FUNID_vkGetImageSparseMemoryRequirements:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_image = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkImage image = (VkImage)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_IMAGE, guest_image);
        
        uint32_t count = 0;
        read_from_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        
        if (count == 0) {
            vkGetImageSparseMemoryRequirements(device, image, &count, NULL);
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        } else {
            VkSparseImageMemoryRequirements* requirements = (VkSparseImageMemoryRequirements*)malloc(count * sizeof(VkSparseImageMemoryRequirements));
            if (requirements) {
                vkGetImageSparseMemoryRequirements(device, image, &count, requirements);
                write_to_guest_mem(all_para[2].data, requirements, 0, count * sizeof(VkSparseImageMemoryRequirements));
                free(requirements);
            }
        }
        LOGD("GetImageSparseMemoryRequirements completed");
    }
    break;

    case FUNID_vkGetImageSparseMemoryRequirements2:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        VkImageSparseMemoryRequirementsInfo2* pInfo = (VkImageSparseMemoryRequirementsInfo2*)malloc(sizeof(VkImageSparseMemoryRequirementsInfo2));
        decode_from_stream_VkImageSparseMemoryRequirementsInfo2(VK_STRUCTURE_TYPE_MAX_ENUM, pInfo, ptr);
        
        uint32_t count = 0;
        read_from_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        
        if (count == 0) {
            vkGetImageSparseMemoryRequirements2(device, pInfo, &count, NULL);
            LOGD("GetImageSparseMemoryRequirements2 count=%u", count);
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        } else {
            VkSparseImageMemoryRequirements2* requirements = (VkSparseImageMemoryRequirements2*)malloc(count * sizeof(VkSparseImageMemoryRequirements2));
            if (requirements) {
                vkGetImageSparseMemoryRequirements2(device, pInfo, &count, requirements);
                LOGD("GetImageSparseMemoryRequirements2 fetched %u requirements", count);
                write_to_guest_mem(all_para[2].data, requirements, 0, count * sizeof(VkSparseImageMemoryRequirements2));
                free(requirements);
            }
        }
        free(pInfo);
        LOGD("GetImageSparseMemoryRequirements2 completed");
    }
    break;

    case FUNID_vkGetPhysicalDeviceFeatures2:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_device);
        
        VkPhysicalDeviceFeatures2 features;
        vkGetPhysicalDeviceFeatures2(physicalDevice, &features);
        
        write_to_guest_mem(all_para[1].data, &features, 0, sizeof(VkPhysicalDeviceFeatures2));
        LOGD("GetPhysicalDeviceFeatures2 completed");
    }
    break;

    case FUNID_vkGetPhysicalDeviceSparseImageFormatProperties:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkFormat format = *(VkFormat*)(*ptr); *ptr += sizeof(VkFormat);
        VkImageType type = *(VkImageType*)(*ptr); *ptr += sizeof(VkImageType);
        VkSampleCountFlagBits samples = *(VkSampleCountFlagBits*)(*ptr); *ptr += sizeof(VkSampleCountFlagBits);
        VkImageUsageFlags usage = *(VkImageUsageFlags*)(*ptr); *ptr += sizeof(VkImageUsageFlags);
        VkImageTiling tiling = *(VkImageTiling*)(*ptr); *ptr += sizeof(VkImageTiling);
        
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_device);
        
        uint32_t count = 0;
        read_from_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        
        if (count == 0) {
            vkGetPhysicalDeviceSparseImageFormatProperties(physicalDevice, format, type, samples, usage, tiling, &count, NULL);
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        } else {
            VkSparseImageFormatProperties* properties = (VkSparseImageFormatProperties*)malloc(count * sizeof(VkSparseImageFormatProperties));
            if (properties) {
                vkGetPhysicalDeviceSparseImageFormatProperties(physicalDevice, format, type, samples, usage, tiling, &count, properties);
                write_to_guest_mem(all_para[2].data, properties, 0, count * sizeof(VkSparseImageFormatProperties));
                free(properties);
            }
        }
        LOGD("GetPhysicalDeviceSparseImageFormatProperties completed");
    }
    break;

    case FUNID_vkGetPhysicalDeviceSparseImageFormatProperties2:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_device);
        
        VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo = (VkPhysicalDeviceSparseImageFormatInfo2*)malloc(sizeof(VkPhysicalDeviceSparseImageFormatInfo2));
        decode_from_stream_VkPhysicalDeviceSparseImageFormatInfo2(VK_STRUCTURE_TYPE_MAX_ENUM, pFormatInfo, ptr);
        
        uint32_t count = 0;
        read_from_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        
        if (count == 0) {
            vkGetPhysicalDeviceSparseImageFormatProperties2(physicalDevice, pFormatInfo, &count, NULL);
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        } else {
            VkSparseImageFormatProperties2* properties = (VkSparseImageFormatProperties2*)malloc(count * sizeof(VkSparseImageFormatProperties2));
            for(int i = 0; i < count; i++) {
                properties[i].sType = VK_STRUCTURE_TYPE_SPARSE_IMAGE_FORMAT_PROPERTIES_2;
                properties[i].pNext = NULL;
            }
            if (properties) {
                vkGetPhysicalDeviceSparseImageFormatProperties2(physicalDevice, pFormatInfo, &count, properties);
                write_to_guest_mem(all_para[2].data, properties, 0, count * sizeof(VkSparseImageFormatProperties2));
                free(properties);
            }
        }
        free(pFormatInfo);
        LOGD("GetPhysicalDeviceSparseImageFormatProperties2 completed");
    }
    break;

    case FUNID_vkGetPhysicalDeviceToolProperties:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_physical_device = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_physical_device);
        
        uint32_t count = 0;
        read_from_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        
        VkResult result;
        if (count == 0) {
            result = vkGetPhysicalDeviceToolProperties(physicalDevice, &count, NULL);
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        } else {
            VkPhysicalDeviceToolProperties* properties = 
                (VkPhysicalDeviceToolProperties*)malloc(count * sizeof(VkPhysicalDeviceToolProperties));
            result = vkGetPhysicalDeviceToolProperties(physicalDevice, &count, properties);
            write_to_guest_mem(all_para[2].data, properties, 0, count * sizeof(VkPhysicalDeviceToolProperties));
            free(properties);
        }
        
        write_to_guest_mem(all_para[3].data, &result, 0, sizeof(VkResult));
        LOGD("GetPhysicalDeviceToolProperties result: %d", result);
    }
    break;

    case FUNID_vkGetRenderAreaGranularity:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_render_pass = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkRenderPass renderPass = (VkRenderPass)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_RENDER_PASS, guest_render_pass);
        
        VkExtent2D granularity;
        vkGetRenderAreaGranularity(device, renderPass, &granularity);
        
        write_to_guest_mem(all_para[1].data, &granularity, 0, sizeof(VkExtent2D));
        LOGD("GetRenderAreaGranularity complete");
    }
    break;

    case FUNID_vkGetSemaphoreCounterValue:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_semaphore = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkSemaphore semaphore = (VkSemaphore)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_SEMAPHORE, guest_semaphore);
        
        uint64_t value;
        VkResult result = vkGetSemaphoreCounterValue(device, semaphore, &value);
        
        write_to_guest_mem(all_para[1].data, &value, 0, sizeof(uint64_t));
        write_to_guest_mem(all_para[2].data, &result, 0, sizeof(VkResult));
        LOGD("GetSemaphoreCounterValue result: %d", result);
    }
    break;

    case FUNID_vkQueueSubmit2:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_queue = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_fence = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint32_t submitCount = *(uint32_t*)(*ptr);
        *ptr += sizeof(uint32_t);
        
        VkQueue queue = (VkQueue)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_QUEUE, guest_queue);
        VkFence fence = guest_fence ? (VkFence)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_FENCE, guest_fence) : VK_NULL_HANDLE;
        
        VkSubmitInfo2* pSubmits = (VkSubmitInfo2*)malloc(submitCount * sizeof(VkSubmitInfo2));
        for (uint32_t i = 0; i < submitCount; ++i) {
            decode_from_stream_VkSubmitInfo2(VK_STRUCTURE_TYPE_MAX_ENUM, 
                                            &pSubmits[i], ptr);
        }
        
        VkResult result = vkQueueSubmit2(queue, submitCount, pSubmits, fence);
        if(result == VK_SUCCESS) {
            LOGD("QueueSubmit2 completed successfully");
        } else {
            LOGE("QueueSubmit2 failed with error: %d", result);
        }
        
        free(pSubmits);
        LOGD("QueueSubmit2 result: %d", result);
    }
    break;

    case FUNID_vkSetEvent:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_event = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkEvent event = (VkEvent)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_EVENT, guest_event);
        
        vkSetEvent(device, event);
        LOGD("SetEvent complete");
    }
    break;

    case FUNID_vkSignalSemaphore:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        VkSemaphoreSignalInfo signalInfo;
        decode_from_stream_VkSemaphoreSignalInfo(VK_STRUCTURE_TYPE_MAX_ENUM, 
                                                &signalInfo, ptr);
        
        VkResult result = vkSignalSemaphore(device, &signalInfo);
        if(result == VK_SUCCESS) {
            LOGD("SignalSemaphore completed successfully");
        } else {
            LOGE("SignalSemaphore failed with error: %d", result);
        }
        LOGD("SignalSemaphore result: %d", result);
    }
    break;

    case FUNID_vkUpdateDescriptorSetWithTemplate:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device;
        memcpy(&guest_device, *ptr, sizeof(uint64_t));
        *ptr += sizeof(uint64_t);
        
        uint64_t guest_descriptor_set;
        memcpy(&guest_descriptor_set, *ptr, sizeof(uint64_t));
        *ptr += sizeof(uint64_t);
        
        uint64_t guest_template;
        memcpy(&guest_template, *ptr, sizeof(uint64_t));
        *ptr += sizeof(uint64_t);
        
        size_t data_size;
        memcpy(&data_size, *ptr, sizeof(size_t));
        *ptr += sizeof(size_t);
        
        uint32_t entry_count;
        memcpy(&entry_count, *ptr, sizeof(uint32_t));
        *ptr += sizeof(uint32_t);
        LOGD("UpdateDescriptorSetWithTemplate entry_count=%u, data_size=%zu", 
            entry_count, data_size);
        
        VkDescriptorUpdateTemplateEntry* entries = 
            (VkDescriptorUpdateTemplateEntry*)malloc(entry_count * sizeof(VkDescriptorUpdateTemplateEntry));
        for (uint32_t i = 0; i < entry_count; ++i) {
            memcpy(&entries[i], *ptr, sizeof(VkDescriptorUpdateTemplateEntry));
            *ptr += sizeof(VkDescriptorUpdateTemplateEntry);
        }
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(
            EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkDescriptorSet descriptorSet = (VkDescriptorSet)(uintptr_t)lookup_mapping(
            EXPRESS_VK_OBJECT_TYPE_DESCRIPTOR_SET, guest_descriptor_set);
        VkDescriptorUpdateTemplate descriptorUpdateTemplate = 
            (VkDescriptorUpdateTemplate)(uintptr_t)lookup_mapping(
                EXPRESS_VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE, guest_template);
        
        void* pData_copy = malloc(data_size);
        memcpy(pData_copy, *ptr, data_size);
        
        for (uint32_t i = 0; i < entry_count; ++i) {
            VkDescriptorUpdateTemplateEntry* entry = &entries[i];
            size_t offset = entry->offset;
            
            for (uint32_t j = 0; j < entry->descriptorCount; ++j) {
                void* desc_ptr = (char*)pData_copy + offset + j * entry->stride;
                LOGD("Processing entry %u/%u at offset %zu", i+1, entry_count, offset + j * entry->stride);
                
                switch (entry->descriptorType) {
                    case VK_DESCRIPTOR_TYPE_SAMPLER:
                    case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                    case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                    case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                    case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT: {
                        VkDescriptorImageInfo* img_info = (VkDescriptorImageInfo*)desc_ptr;
                        if (img_info->sampler) {
                            uint64_t guest_sampler = (uint64_t)(uintptr_t)img_info->sampler;
                            img_info->sampler = (VkSampler)(uintptr_t)lookup_mapping(
                                EXPRESS_VK_OBJECT_TYPE_SAMPLER, guest_sampler);
                        }
                        if (img_info->imageView) {
                            uint64_t guest_image_view = (uint64_t)(uintptr_t)img_info->imageView;
                            img_info->imageView = (VkImageView)(uintptr_t)lookup_mapping(
                                EXPRESS_VK_OBJECT_TYPE_IMAGE_VIEW, guest_image_view);
                        }
                        break;
                    }
                    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
                    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
                    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC: {
                        VkDescriptorBufferInfo* buf_info = (VkDescriptorBufferInfo*)desc_ptr;
                        if (buf_info->buffer) {
                            uint64_t guest_buffer = (uint64_t)(uintptr_t)buf_info->buffer;
                            buf_info->buffer = (VkBuffer)(uintptr_t)lookup_mapping(
                                EXPRESS_VK_OBJECT_TYPE_BUFFER, guest_buffer);
                                LOGD("Mapped buffer guest %lu to host %p", guest_buffer, buf_info->buffer);
                        }
                        break;
                    }
                    case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
                    case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER: {
                        VkBufferView* buffer_view = (VkBufferView*)desc_ptr;
                        if (*buffer_view) {
                            uint64_t guest_buffer_view = (uint64_t)(uintptr_t)(*buffer_view);
                            *buffer_view = (VkBufferView)(uintptr_t)lookup_mapping(
                                EXPRESS_VK_OBJECT_TYPE_BUFFER_VIEW, guest_buffer_view);
                        }
                        break;
                    }
                    case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR: {
                        VkAccelerationStructureKHR* accel = (VkAccelerationStructureKHR*)desc_ptr;
                        if (*accel) {
                            uint64_t guest_accel = (uint64_t)(uintptr_t)(*accel);
                            *accel = (VkAccelerationStructureKHR)(uintptr_t)lookup_mapping(
                                EXPRESS_VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR, guest_accel);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
        }
        
        vkUpdateDescriptorSetWithTemplate(device, descriptorSet, 
                                        descriptorUpdateTemplate, pData_copy);
        
        free(entries);
        free(pData_copy);
        LOGD("UpdateDescriptorSetWithTemplate done");
    }
    break;

    case FUNID_vkWaitSemaphores:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t timeout = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        VkSemaphoreWaitInfo waitInfo;
        decode_from_stream_VkSemaphoreWaitInfo(VK_STRUCTURE_TYPE_MAX_ENUM, 
                                            &waitInfo, ptr);
        
        VkResult result = vkWaitSemaphores(device, &waitInfo, timeout);
        if(result == VK_SUCCESS) {
            LOGD("WaitSemaphores completed successfully");
        } else {
            LOGE("WaitSemaphores failed with error: %d", result);
        }
        LOGD("WaitSemaphores result: %d", result);
    }
    break;

    case FUNID_vkGetPrivateData:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint32_t objectType = *(uint32_t*)(*ptr);
        *ptr += sizeof(uint32_t);
        uint64_t objectHandle = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_private_data_slot = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);

        uint64_t real_handle = lookup_mapping(objectType, objectHandle);
        LOGD("going to call vkGetPrivateData");
        
        VkDevice device = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkPrivateDataSlot privateDataSlot = (VkPrivateDataSlot)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PRIVATE_DATA_SLOT, guest_private_data_slot);
        
        uint64_t data;
        vkGetPrivateData(device, (VkObjectType)objectType, real_handle, privateDataSlot, &data);
        
        write_to_guest_mem(all_para[1].data, &data, 0, sizeof(uint64_t));
        LOGD("GetPrivateData complete");
    }
    break;

    case FUNID_vkSetPrivateData:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint32_t objectType = *(uint32_t*)(*ptr);
        *ptr += sizeof(uint32_t);
        uint64_t objectHandle = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t guest_private_data_slot = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        uint64_t data = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        
        VkDevice device = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkPrivateDataSlot privateDataSlot = (VkPrivateDataSlot)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PRIVATE_DATA_SLOT, guest_private_data_slot);
        uint64_t real_handle = lookup_mapping(objectType, objectHandle);
        LOGD("going to call vkSetPrivateData");
        LOGD("info verbose: device=%p, objectType=%u, objectHandle=%lu, privateDataSlot=%p, data=%lu",
             device, objectType, real_handle, privateDataSlot, data);

        VkResult result = vkSetPrivateData(device, (VkObjectType)objectType, real_handle, privateDataSlot, data);
        if(result == VK_SUCCESS) {
            LOGD("SetPrivateData completed successfully");
        } else {
            LOGE("SetPrivateData failed with error: %d", result);
        }
        LOGD("SetPrivateData result: %d", result);
    }
    break;

    case FUNID_vkGetPhysicalDeviceSurfaceCapabilitiesKHR:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_phys_dev;
        memcpy(&guest_phys_dev, *ptr, 8); *ptr += 8;
        uint64_t guest_surface;
        memcpy(&guest_surface, *ptr, 8); *ptr += 8;
        
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_phys_dev);
        VkSurfaceKHR surface = (VkSurfaceKHR)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_SURFACE_KHR, guest_surface);
        
        VkSurfaceCapabilitiesKHR capabilities;
        VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);
        
        write_to_guest_mem(all_para[1].data, &capabilities, 0, sizeof(VkSurfaceCapabilitiesKHR));
        LOGD("GetPhysicalDeviceSurfaceCapabilitiesKHR result %d", result);
    }
    break;

    case FUNID_vkGetPhysicalDeviceSurfaceCapabilities2KHR:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_phys_dev;
        memcpy(&guest_phys_dev, *ptr, 8); *ptr += 8;
        
        VkPhysicalDeviceSurfaceInfo2KHR surfaceInfo;
        decode_from_stream_VkPhysicalDeviceSurfaceInfo2KHR(VK_STRUCTURE_TYPE_MAX_ENUM, &surfaceInfo, ptr);
        
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_phys_dev);
        
        VkSurfaceCapabilities2KHR capabilities = {VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR};
        VkResult result = vkGetPhysicalDeviceSurfaceCapabilities2KHR(physicalDevice, &surfaceInfo, &capabilities);
        
        write_to_guest_mem(all_para[1].data, &capabilities, 0, sizeof(VkSurfaceCapabilities2KHR));
        LOGD("GetPhysicalDeviceSurfaceCapabilities2KHR result %d", result);
    }
    break;

    case FUNID_vkGetPhysicalDeviceSurfaceFormatsKHR:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_phys_dev;
        memcpy(&guest_phys_dev, *ptr, 8); *ptr += 8;
        uint64_t guest_surface;
        memcpy(&guest_surface, *ptr, 8); *ptr += 8;
        
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_phys_dev);
        VkSurfaceKHR surface = (VkSurfaceKHR)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_SURFACE_KHR, guest_surface);
        
        uint32_t count = 0;
        read_from_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        
        VkResult result;
        if (count == 0) {
            result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, NULL);
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        } else {
            VkSurfaceFormatKHR* formats = (VkSurfaceFormatKHR*)malloc(count * sizeof(VkSurfaceFormatKHR));
            result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, formats);
            write_to_guest_mem(all_para[2].data, formats, 0, count * sizeof(VkSurfaceFormatKHR));
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
            free(formats);
        }
        
        LOGD("GetPhysicalDeviceSurfaceFormatsKHR count %d result %d", count, result);
    }
    break;

    case FUNID_vkGetPhysicalDeviceSurfaceFormats2KHR:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_phys_dev;
        memcpy(&guest_phys_dev, *ptr, 8); *ptr += 8;
        
        VkPhysicalDeviceSurfaceInfo2KHR surfaceInfo;
        decode_from_stream_VkPhysicalDeviceSurfaceInfo2KHR(VK_STRUCTURE_TYPE_MAX_ENUM, &surfaceInfo, ptr);
        
        uint32_t count;
        memcpy(&count, *ptr, 4); *ptr += 4;
        
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_phys_dev);
        
        VkResult result;
        if (count == 0) {
            result = vkGetPhysicalDeviceSurfaceFormats2KHR(physicalDevice, &surfaceInfo, &count, NULL);
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        } else {
            VkSurfaceFormat2KHR* formats = (VkSurfaceFormat2KHR*)malloc(count * sizeof(VkSurfaceFormat2KHR));
            for (uint32_t i = 0; i < count; i++) {
                formats[i].sType = VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR;
                formats[i].pNext = NULL;
            }
            result = vkGetPhysicalDeviceSurfaceFormats2KHR(physicalDevice, &surfaceInfo, &count, formats);
            write_to_guest_mem(all_para[2].data, formats, 0, count * sizeof(VkSurfaceFormat2KHR));
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
            free(formats);
        }
        
        write_to_guest_mem(all_para[para_num - 1].data, &result, 0, sizeof(VkResult));
        LOGD("GetPhysicalDeviceSurfaceFormats2KHR count %d result %d", count, result);
    }
    break;

    case FUNID_vkGetPhysicalDeviceSurfacePresentModesKHR:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_phys_dev;
        memcpy(&guest_phys_dev, *ptr, 8); *ptr += 8;
        uint64_t guest_surface;
        memcpy(&guest_surface, *ptr, 8); *ptr += 8;
        
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_phys_dev);
        VkSurfaceKHR surface = (VkSurfaceKHR)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_SURFACE_KHR, guest_surface);
        
        uint32_t count = 0;
        read_from_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        
        VkResult result;
        if (count == 0) {
            result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, NULL);
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        } else {
            VkPresentModeKHR* modes = (VkPresentModeKHR*)malloc(count * sizeof(VkPresentModeKHR));
            result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, modes);
            write_to_guest_mem(all_para[2].data, modes, 0, count * sizeof(VkPresentModeKHR));
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
            free(modes);
        }
        
        LOGD("GetPhysicalDeviceSurfacePresentModesKHR count %d result %d", count, result);
    }
    break;

    case FUNID_vkGetPhysicalDeviceSurfaceSupportKHR:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_phys_dev;
        memcpy(&guest_phys_dev, *ptr, 8); *ptr += 8;
        uint32_t queueFamilyIndex;
        memcpy(&queueFamilyIndex, *ptr, 4); *ptr += 4;
        uint64_t guest_surface;
        memcpy(&guest_surface, *ptr, 8); *ptr += 8;
        
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_phys_dev);
        VkSurfaceKHR surface = (VkSurfaceKHR)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_SURFACE_KHR, guest_surface);
        
        VkBool32 supported;
        VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, &supported);
        
        write_to_guest_mem(all_para[1].data, &supported, 0, sizeof(VkBool32));
        LOGD("GetPhysicalDeviceSurfaceSupportKHR queue %d supported %d result %d", queueFamilyIndex, supported, result);
    }
    break;


    case FUNID_vkGetPhysicalDeviceDisplayPropertiesKHR:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_phys_dev;
        memcpy(&guest_phys_dev, *ptr, 8); *ptr += 8;
        
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_phys_dev);
        
        uint32_t count = 0;
        read_from_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        
        VkResult result;
        if (count == 0) {
            result = vkGetPhysicalDeviceDisplayPropertiesKHR(physicalDevice, &count, NULL);
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        } else {
            uint64_t* guest_displays = (uint64_t*)malloc(count * sizeof(uint64_t));
            read_from_guest_mem(all_para[2].data, guest_displays, 0, count * sizeof(uint64_t));
            
            VkDisplayPropertiesKHR* props = (VkDisplayPropertiesKHR*)malloc(count * sizeof(VkDisplayPropertiesKHR));
            result = vkGetPhysicalDeviceDisplayPropertiesKHR(physicalDevice, &count, props);
            
            for (uint32_t i = 0; i < count; ++i) {
                uint64_t host_display = (uint64_t)(uintptr_t)props[i].display;
                insert_mapping(EXPRESS_VK_OBJECT_TYPE_DISPLAY_KHR, guest_displays[i], host_display);
            }
            
            write_to_guest_mem(all_para[3].data, props, 0, count * sizeof(VkDisplayPropertiesKHR));
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
            
            free(props);
            free(guest_displays);
        }
        
        LOGD("GetPhysicalDeviceDisplayPropertiesKHR count %d", count);
    }
    break;

    case FUNID_vkGetPhysicalDeviceDisplayProperties2KHR:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_phys_dev;
        memcpy(&guest_phys_dev, *ptr, 8); *ptr += 8;
        
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_phys_dev);
        
        uint32_t count = 0;
        read_from_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        
        VkResult result;
        if (count == 0) {
            result = vkGetPhysicalDeviceDisplayProperties2KHR(physicalDevice, &count, NULL);
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        } else {
            uint64_t* guest_displays = (uint64_t*)malloc(count * sizeof(uint64_t));
            read_from_guest_mem(all_para[2].data, guest_displays, 0, count * sizeof(uint64_t));
            
            VkDisplayProperties2KHR* props = (VkDisplayProperties2KHR*)malloc(count * sizeof(VkDisplayProperties2KHR));
            for (uint32_t i = 0; i < count; ++i) {
                props[i].sType = VK_STRUCTURE_TYPE_DISPLAY_PROPERTIES_2_KHR;
                props[i].pNext = NULL;
            }
            result = vkGetPhysicalDeviceDisplayProperties2KHR(physicalDevice, &count, props);
            
            for (uint32_t i = 0; i < count; ++i) {
                uint64_t host_display = (uint64_t)(uintptr_t)props[i].displayProperties.display;
                insert_mapping(EXPRESS_VK_OBJECT_TYPE_DISPLAY_KHR, guest_displays[i], host_display);
            }
            
            write_to_guest_mem(all_para[3].data, props, 0, count * sizeof(VkDisplayProperties2KHR));
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
            
            free(props);
            free(guest_displays);
        }
        
        LOGD("GetPhysicalDeviceDisplayProperties2KHR count %d", count);
    }
    break;

    case FUNID_vkGetPhysicalDeviceDisplayPlanePropertiesKHR:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_phys_dev;
        memcpy(&guest_phys_dev, *ptr, 8); *ptr += 8;
        
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_phys_dev);
        
        uint32_t count = 0;
        read_from_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        
        VkResult result;
        if (count == 0) {
            result = vkGetPhysicalDeviceDisplayPlanePropertiesKHR(physicalDevice, &count, NULL);
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        } else {
            uint64_t* guest_displays = (uint64_t*)malloc(count * sizeof(uint64_t));
            read_from_guest_mem(all_para[2].data, guest_displays, 0, count * sizeof(uint64_t));
            
            VkDisplayPlanePropertiesKHR* props = (VkDisplayPlanePropertiesKHR*)malloc(count * sizeof(VkDisplayPlanePropertiesKHR));
            result = vkGetPhysicalDeviceDisplayPlanePropertiesKHR(physicalDevice, &count, props);
            
            for (uint32_t i = 0; i < count; ++i) {
                if (props[i].currentDisplay != VK_NULL_HANDLE) {
                    uint64_t host_display = (uint64_t)(uintptr_t)props[i].currentDisplay;
                    insert_mapping(EXPRESS_VK_OBJECT_TYPE_DISPLAY_KHR, guest_displays[i], host_display);
                }
            }
            
            write_to_guest_mem(all_para[3].data, props, 0, count * sizeof(VkDisplayPlanePropertiesKHR));
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
            
            free(props);
            free(guest_displays);
        }
        
        LOGD("GetPhysicalDeviceDisplayPlanePropertiesKHR count %d", count);
    }
    break;

    case FUNID_vkGetPhysicalDeviceDisplayPlaneProperties2KHR:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_phys_dev;
        memcpy(&guest_phys_dev, *ptr, 8); *ptr += 8;
        
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_phys_dev);
        
        uint32_t count = 0;
        read_from_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        
        VkResult result;
        if (count == 0) {
            result = vkGetPhysicalDeviceDisplayPlaneProperties2KHR(physicalDevice, &count, NULL);
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        } else {
            uint64_t* guest_displays = (uint64_t*)malloc(count * sizeof(uint64_t));
            read_from_guest_mem(all_para[2].data, guest_displays, 0, count * sizeof(uint64_t));
            
            VkDisplayPlaneProperties2KHR* props = (VkDisplayPlaneProperties2KHR*)malloc(count * sizeof(VkDisplayPlaneProperties2KHR));
            for (uint32_t i = 0; i < count; ++i) {
                props[i].sType = VK_STRUCTURE_TYPE_DISPLAY_PLANE_PROPERTIES_2_KHR;
                props[i].pNext = NULL;
            }
            result = vkGetPhysicalDeviceDisplayPlaneProperties2KHR(physicalDevice, &count, props);
            
            for (uint32_t i = 0; i < count; ++i) {
                if (props[i].displayPlaneProperties.currentDisplay != VK_NULL_HANDLE) {
                    uint64_t host_display = (uint64_t)(uintptr_t)props[i].displayPlaneProperties.currentDisplay;
                    insert_mapping(EXPRESS_VK_OBJECT_TYPE_DISPLAY_KHR, guest_displays[i], host_display);
                }
            }
            
            write_to_guest_mem(all_para[3].data, props, 0, count * sizeof(VkDisplayPlaneProperties2KHR));
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
            
            free(props);
            free(guest_displays);
        }
        
        LOGD("GetPhysicalDeviceDisplayPlaneProperties2KHR count %d", count);
    }
    break;

    case FUNID_vkGetDisplayModePropertiesKHR:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_phys_dev;
        memcpy(&guest_phys_dev, *ptr, 8); *ptr += 8;
        uint64_t guest_display;
        memcpy(&guest_display, *ptr, 8); *ptr += 8;
        
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_phys_dev);
        VkDisplayKHR display = (VkDisplayKHR)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DISPLAY_KHR, guest_display);
        
        uint32_t count = 0;
        read_from_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        
        VkResult result;
        if (count == 0) {
            result = vkGetDisplayModePropertiesKHR(physicalDevice, display, &count, NULL);
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        } else {
            uint64_t* guest_modes = (uint64_t*)malloc(count * sizeof(uint64_t));
            read_from_guest_mem(all_para[2].data, guest_modes, 0, count * sizeof(uint64_t));
            
            VkDisplayModePropertiesKHR* props = (VkDisplayModePropertiesKHR*)malloc(count * sizeof(VkDisplayModePropertiesKHR));
            result = vkGetDisplayModePropertiesKHR(physicalDevice, display, &count, props);
            
            for (uint32_t i = 0; i < count; ++i) {
                uint64_t host_mode = (uint64_t)(uintptr_t)props[i].displayMode;
                insert_mapping(EXPRESS_VK_OBJECT_TYPE_DISPLAY_MODE_KHR, guest_modes[i], host_mode);
            }
            
            write_to_guest_mem(all_para[3].data, props, 0, count * sizeof(VkDisplayModePropertiesKHR));
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
            
            free(props);
            free(guest_modes);
        }
        
        write_to_guest_mem(all_para[para_num - 1].data, &result, 0, sizeof(VkResult));

        LOGD("GetDisplayModePropertiesKHR count %d result %d", count, result);
    }
    break;

    case FUNID_vkGetDisplayModeProperties2KHR:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_phys_dev;
        memcpy(&guest_phys_dev, *ptr, 8); *ptr += 8;
        uint64_t guest_display;
        memcpy(&guest_display, *ptr, 8); *ptr += 8;
        
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)
        lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_phys_dev);
        VkDisplayKHR display = (VkDisplayKHR)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DISPLAY_KHR, guest_display);
        
        uint32_t count = 0;
        read_from_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        
        VkResult result;
        if (count == 0) {
            result = vkGetDisplayModeProperties2KHR(physicalDevice, display, &count, NULL);
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        } else {
            uint64_t* guest_modes = (uint64_t*)malloc(count * sizeof(uint64_t));
            read_from_guest_mem(all_para[2].data, guest_modes, 0, count * sizeof(uint64_t));
            
            VkDisplayModeProperties2KHR* props = (VkDisplayModeProperties2KHR*)malloc(count * sizeof(VkDisplayModeProperties2KHR));
            for (uint32_t i = 0; i < count; ++i) {
                props[i].sType = VK_STRUCTURE_TYPE_DISPLAY_MODE_PROPERTIES_2_KHR;
                props[i].pNext = NULL;
            }
            result = vkGetDisplayModeProperties2KHR(physicalDevice, display, &count, props);
            
            for (uint32_t i = 0; i < count; ++i) {
                uint64_t host_mode = (uint64_t)(uintptr_t)props[i].displayModeProperties.displayMode;
                insert_mapping(EXPRESS_VK_OBJECT_TYPE_DISPLAY_MODE_KHR, guest_modes[i], host_mode);
            }
            
            write_to_guest_mem(all_para[3].data, props, 0, count * sizeof(VkDisplayModeProperties2KHR));
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
            
            free(props);
            free(guest_modes);
        }
        
        write_to_guest_mem(all_para[para_num - 1].data, &result, 0, sizeof(VkResult));
        LOGD("GetDisplayModeProperties2KHR count %d", count);
    }
    break;

    case FUNID_vkGetPhysicalDevicePresentRectanglesKHR:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_phys_dev;
        memcpy(&guest_phys_dev, *ptr, 8); *ptr += 8;
        uint64_t guest_surface;
        memcpy(&guest_surface, *ptr, 8); *ptr += 8;
        uint32_t rect_count;
        memcpy(&rect_count, *ptr, 4); *ptr += 4;
        
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_phys_dev);
        VkSurfaceKHR surface = (VkSurfaceKHR)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_SURFACE_KHR, guest_surface);
        
        VkResult result;
        uint32_t count = rect_count;
        
        if (count == 0) {
            result = vkGetPhysicalDevicePresentRectanglesKHR(physicalDevice, surface, &count, NULL);
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        } else {
            VkRect2D* rects = (VkRect2D*)malloc(count * sizeof(VkRect2D));
            result = vkGetPhysicalDevicePresentRectanglesKHR(physicalDevice, surface, &count, rects);
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
            if (result == VK_SUCCESS) {
                write_to_guest_mem(all_para[2].data, rects, 0, count * sizeof(VkRect2D));
            }
            free(rects);
        }
        
        LOGD("GetPhysicalDevicePresentRectanglesKHR result=%d count=%d", result, count);
    }
    break;

    case FUNID_vkCmdResetQueryPool:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_cmd_buf;
        memcpy(&guest_cmd_buf, *ptr, 8); *ptr += 8;
        uint64_t guest_query_pool;
        memcpy(&guest_query_pool, *ptr, 8); *ptr += 8;
        uint32_t firstQuery;
        memcpy(&firstQuery, *ptr, 4); *ptr += 4;
        uint32_t queryCount;
        memcpy(&queryCount, *ptr, 4); *ptr += 4;
        
        VkCommandBuffer commandBuffer = (VkCommandBuffer)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buf);
        VkQueryPool queryPool = (VkQueryPool)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_QUERY_POOL, guest_query_pool);
        
        vkCmdResetQueryPool(commandBuffer, queryPool, firstQuery, queryCount);
        
        LOGD("CmdResetQueryPool firstQuery=%d queryCount=%d", firstQuery, queryCount);
    }
    break;

    case FUNID_vkCreateDisplayModeKHR:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_physicalDevice;
        memcpy(&guest_physicalDevice, *ptr, sizeof(uint64_t));
        *ptr += sizeof(uint64_t);
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_physicalDevice);
        
        uint64_t guest_display;
        memcpy(&guest_display, *ptr, sizeof(uint64_t));
        *ptr += sizeof(uint64_t);
        VkDisplayKHR display = (VkDisplayKHR)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DISPLAY_KHR, guest_display);
        
        VkDisplayModeCreateInfoKHR createInfo;
        decode_from_stream_VkDisplayModeCreateInfoKHR(VK_STRUCTURE_TYPE_MAX_ENUM, &createInfo, ptr);
        
        uint64_t guest_allocator_ptr;
        memcpy(&guest_allocator_ptr, *ptr, 8);
        *ptr += 8;
        
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_allocator_ptr) {
            VkAllocationCallbacks allocator;
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocator, ptr);
            pAllocator = &allocator;
        }
        
        uint64_t guest_mode;
        memcpy(&guest_mode, *ptr, sizeof(uint64_t));
        *ptr += sizeof(uint64_t);
        
        VkDisplayModeKHR mode;
        VkResult result = vkCreateDisplayModeKHR(physicalDevice, display, &createInfo, pAllocator, &mode);
        
        if (result == VK_SUCCESS) {
            insert_mapping(EXPRESS_VK_OBJECT_TYPE_DISPLAY_MODE_KHR, guest_mode, (uint64_t)(uintptr_t)mode);
        }
        
        write_to_guest_mem(all_para[1].data, &result, 0, sizeof(VkResult));
        LOGD("CreateDisplayModeKHR result %d", result);
    }
    break;

    case FUNID_vkCreateDisplayPlaneSurfaceKHR:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_instance;
        memcpy(&guest_instance, *ptr, sizeof(uint64_t));
        *ptr += sizeof(uint64_t);
        VkInstance instance = (VkInstance)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_INSTANCE, guest_instance);
        
        VkDisplaySurfaceCreateInfoKHR createInfo;
        decode_from_stream_VkDisplaySurfaceCreateInfoKHR(VK_STRUCTURE_TYPE_MAX_ENUM, &createInfo, ptr);
        
        uint64_t guest_allocator_ptr;
        memcpy(&guest_allocator_ptr, *ptr, 8);
        *ptr += 8;
        
        const VkAllocationCallbacks* pAllocator = NULL;
        if (guest_allocator_ptr) {
            VkAllocationCallbacks allocator;
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, &allocator, ptr);
            pAllocator = &allocator;
        }
        
        uint64_t guest_surface;
        memcpy(&guest_surface, *ptr, sizeof(uint64_t));
        *ptr += sizeof(uint64_t);
        
        VkSurfaceKHR surface;
        VkResult result = vkCreateDisplayPlaneSurfaceKHR(instance, &createInfo, pAllocator, &surface);
        
        if (result == VK_SUCCESS) {
            insert_mapping(EXPRESS_VK_OBJECT_TYPE_SURFACE_KHR, guest_surface, (uint64_t)(uintptr_t)surface);
        }
        
        write_to_guest_mem(all_para[1].data, &result, 0, sizeof(VkResult));
        LOGD("CreateDisplayPlaneSurfaceKHR result %d", result);
    }
    break;

    case FUNID_vkGetDeviceGroupPresentCapabilitiesKHR:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device;
        memcpy(&guest_device, *ptr, sizeof(uint64_t));
        *ptr += sizeof(uint64_t);
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        VkDeviceGroupPresentCapabilitiesKHR capabilities;
        VkResult result = vkGetDeviceGroupPresentCapabilitiesKHR(device, &capabilities);
        
        write_to_guest_mem(all_para[1].data, &capabilities, 0, sizeof(VkDeviceGroupPresentCapabilitiesKHR));
        write_to_guest_mem(all_para[2].data, &result, 0, sizeof(VkResult));
        LOGD("GetDeviceGroupPresentCapabilitiesKHR result %d", result);
    }
    break;

    case FUNID_vkGetDeviceGroupSurfacePresentModesKHR:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device;
        memcpy(&guest_device, *ptr, sizeof(uint64_t));
        *ptr += sizeof(uint64_t);
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        
        uint64_t guest_surface;
        memcpy(&guest_surface, *ptr, sizeof(uint64_t));
        *ptr += sizeof(uint64_t);
        VkSurfaceKHR surface = (VkSurfaceKHR)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_SURFACE_KHR, guest_surface);
        
        VkDeviceGroupPresentModeFlagsKHR modes;
        VkResult result = vkGetDeviceGroupSurfacePresentModesKHR(device, surface, &modes);
        
        write_to_guest_mem(all_para[1].data, &modes, 0, sizeof(VkDeviceGroupPresentModeFlagsKHR));
        write_to_guest_mem(all_para[2].data, &result, 0, sizeof(VkResult));
        LOGD("GetDeviceGroupSurfacePresentModesKHR result %d", result);
    }
    break;

    case FUNID_vkGetDisplayPlaneCapabilities2KHR:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_physicalDevice;
        memcpy(&guest_physicalDevice, *ptr, sizeof(uint64_t));
        *ptr += sizeof(uint64_t);
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_physicalDevice);
        
        VkDisplayPlaneInfo2KHR planeInfo;
        decode_from_stream_VkDisplayPlaneInfo2KHR(VK_STRUCTURE_TYPE_MAX_ENUM, &planeInfo, ptr);
        
        VkDisplayPlaneCapabilities2KHR capabilities;
        VkResult result = vkGetDisplayPlaneCapabilities2KHR(physicalDevice, &planeInfo, &capabilities);
        
        write_to_guest_mem(all_para[1].data, &capabilities, 0, sizeof(VkDisplayPlaneCapabilities2KHR));
        write_to_guest_mem(all_para[2].data, &result, 0, sizeof(VkResult));
        LOGD("GetDisplayPlaneCapabilities2KHR result %d", result);
    }
    break;

    case FUNID_vkGetDisplayPlaneCapabilitiesKHR:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_physicalDevice;
        memcpy(&guest_physicalDevice, *ptr, sizeof(uint64_t));
        *ptr += sizeof(uint64_t);
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_physicalDevice);
        
        uint64_t guest_mode;
        memcpy(&guest_mode, *ptr, sizeof(uint64_t));
        *ptr += sizeof(uint64_t);
        VkDisplayModeKHR mode = (VkDisplayModeKHR)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DISPLAY_MODE_KHR, guest_mode);
        
        uint32_t planeIndex;
        memcpy(&planeIndex, *ptr, sizeof(uint32_t));
        *ptr += sizeof(uint32_t);
        
        VkDisplayPlaneCapabilitiesKHR capabilities;
        VkResult result = vkGetDisplayPlaneCapabilitiesKHR(physicalDevice, mode, planeIndex, &capabilities);
        
        write_to_guest_mem(all_para[1].data, &capabilities, 0, sizeof(VkDisplayPlaneCapabilitiesKHR));
        write_to_guest_mem(all_para[2].data, &result, 0, sizeof(VkResult));
        LOGD("GetDisplayPlaneCapabilitiesKHR result %d", result);
    }
    break;

    case FUNID_vkGetDisplayPlaneSupportedDisplaysKHR:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_physicalDevice;
        memcpy(&guest_physicalDevice, *ptr, sizeof(uint64_t));
        *ptr += sizeof(uint64_t);
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_physicalDevice);
        
        uint32_t planeIndex;
        memcpy(&planeIndex, *ptr, sizeof(uint32_t));
        *ptr += sizeof(uint32_t);
        
        uint32_t count = 0;
        read_from_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        
        VkResult result;
        if (count == 0) {
            result = vkGetDisplayPlaneSupportedDisplaysKHR(physicalDevice, planeIndex, &count, NULL);
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        } else {
            VkDisplayKHR* displays = (VkDisplayKHR*)malloc(count * sizeof(VkDisplayKHR));
            uint64_t* guest_displays = malloc(count * sizeof(uint64_t));
            read_from_guest_mem(all_para[2].data, guest_displays, 0, count * sizeof(uint64_t));
            
            result = vkGetDisplayPlaneSupportedDisplaysKHR(physicalDevice, planeIndex, &count, displays);
            
            for (uint32_t i = 0; i < count; ++i) {
                insert_mapping(EXPRESS_VK_OBJECT_TYPE_DISPLAY_KHR, guest_displays[i], (uint64_t)(uintptr_t)displays[i]);
            }
            
            free(displays);
            free(guest_displays);
        }
        
        write_to_guest_mem(all_para[para_num - 1].data, &result, 0, sizeof(VkResult));
        LOGD("GetDisplayPlaneSupportedDisplaysKHR result %d", result);
    }
    break;

    case FUNID_vkGetMemoryNativeBufferOHOS:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_memory;
        uint64_t ret_handle;
        memcpy(&guest_memory, *ptr, sizeof(uint64_t));

        uint64_t memory = (uint64_t)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE_MEMORY, guest_memory);
        
        uint64_t gbuffer_id = lookup_memory_gbuffer_mapping(memory);

        if (gbuffer_id == 0) {
            LOGE("GetMemoryNativeBufferOHOS: cannot find native buffer for memory %llx", memory);
            ret_handle = 0;
            write_to_guest_mem(all_para[1].data, &ret_handle, 0, sizeof(uint64_t));
        } else {
            Hardware_Buffer* nativeBuffer = get_gbuffer_from_global_map(gbuffer_id);
            if (nativeBuffer == NULL) {           
                LOGE("GetMemoryNativeBufferOHOS: get null native buffer for gbuffer id %llx", gbuffer_id);
                ret_handle = 0;
                write_to_guest_mem(all_para[1].data, &ret_handle, 0, sizeof(uint64_t));
                break;
            }
            ret_handle = nativeBuffer->vk_buffer_handle;
            write_to_guest_mem(all_para[1].data, &ret_handle, 0, sizeof(uint64_t));
            LOGD("GetMemoryNativeBufferOHOS ptr %llx handle %llx", memory, ret_handle);            
        }
    }
    break;

    default:
        LOGE("Unhandled Vulkan function ID: %d", fun_id);
        break;

    }
    call->callback(call, 1);
}
