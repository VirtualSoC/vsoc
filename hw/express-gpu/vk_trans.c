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
        LOGI("Successfully removed mapping for Vulkan object %llu", (unsigned long long)guest_obj);
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
        LOGI("Successfully removed mapping for Vulkan object %llu", (unsigned long long)guest_dev);
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
        LOGI("Invalid memoryTypeIndex %u (max %u)",
               memoryTypeIndex, memProps.memoryTypeCount - 1);
        return;
    }

    VkMemoryPropertyFlags flags =
        memProps.memoryTypes[memoryTypeIndex].propertyFlags;

    LOGI("MemoryType %u flags: 0x%08x",
           memoryTypeIndex, flags);

    if (flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
        LOGI("  -> HOST_VISIBLE is PRESENT");
    } else {
        LOGI("  -> HOST_VISIBLE is NOT present");
    }

    if (flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
        LOGI("  -> HOST_COHERENT is PRESENT");
    } else {
        LOGI("  -> HOST_COHERENT is NOT present");
    }
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
    LOGI("get vk call with id %lld", fun_id);

    switch (fun_id)
    {

    case FUNID_vkCreateInstance: //ztodo：理论上传过来的所有性质都要filter一遍才行吧

    {
        LOGI("get call FUNID_vkCreateInstance!");

        const VkInstanceCreateInfo* pCreateInfo = malloc(sizeof(VkInstanceCreateInfo));
        const VkAllocationCallbacks* pAllocator = NULL; //ztodo:暂时全部用null
        VkInstance pInstance;
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("get vk param number %d instance is %lld %lld", para_num, pInstance, &pInstance);

        int need_free = 0;
        char *stream_ptr;
        stream_ptr = call_para_to_ptr(all_para[0], &need_free);
        uint8_t ** stream_ptr_ptr = (uint8_t **)&stream_ptr;

        // VkInstanceCreateInfo* pCreateInfo = (VkInstanceCreateInfo*)_ptr;
        decode_from_stream_VkInstanceCreateInfo(VK_STRUCTURE_TYPE_MAX_ENUM, (VkInstanceCreateInfo*)(pCreateInfo), stream_ptr_ptr); 

        LOGI("got vkCreateinfo with %lld %d %s %d %s",(long long)pCreateInfo->sType, pCreateInfo->enabledLayerCount, pCreateInfo->ppEnabledLayerNames, pCreateInfo->enabledExtensionCount, pCreateInfo->ppEnabledExtensionNames);
        LOGI("application create info is %lld %d",(long long)pCreateInfo->pApplicationInfo, pCreateInfo->pApplicationInfo->sType);

        VkAllocationCallbacks* guest_allocator = (VkAllocationCallbacks*)(**stream_ptr_ptr);
        *stream_ptr_ptr += 8;

        if(guest_allocator) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, pAllocator, stream_ptr_ptr);
        }

        uint64_t guest_instance = *(uint64_t*)(*stream_ptr_ptr);
        *stream_ptr_ptr += sizeof(uint64_t);

        uint32_t glfwExtCount = 0;
        const char** glfwExts = glfwGetRequiredInstanceExtensions(&glfwExtCount);
        uint32_t origExtCount = pCreateInfo->enabledExtensionCount;
        const char* const* origExts = pCreateInfo->ppEnabledExtensionNames;

        uint32_t totalExtCount = origExtCount + glfwExtCount;
        const char** mergedExts = malloc(sizeof(char*) * totalExtCount);
        for (uint32_t i = 0; i < origExtCount; i++) {
            mergedExts[i] = origExts[i];
        }
        for (uint32_t i = 0; i < glfwExtCount; i++) {
            mergedExts[origExtCount + i] = glfwExts[i];
            LOGI("glfw ext %d %s", i, glfwExts[i]);
        }

        ((VkInstanceCreateInfo*)pCreateInfo)->enabledExtensionCount   = totalExtCount;
        ((VkInstanceCreateInfo*)pCreateInfo)->ppEnabledExtensionNames = mergedExts;

        VkResult result = vkCreateInstance(pCreateInfo, pAllocator, &pInstance);

        free(mergedExts);

        // VkResult result = vkCreateInstance(pCreateInfo, pAllocator, &pInstance);

        if (result == VK_SUCCESS) {
            LOGI("got result %d instance %lld %lld size %d guest %lld", result, pInstance, &pInstance, sizeof(VkInstance), guest_instance);
            insert_mapping(EXPRESS_VK_OBJECT_TYPE_INSTANCE, guest_instance, (uint64_t)(uintptr_t)pInstance);
            LOGI("map result is %lld", lookup_mapping(EXPRESS_VK_OBJECT_TYPE_INSTANCE, guest_instance));
        }
        write_to_guest_mem(all_para[1].data, &result, 0, sizeof(VkResult));

        //create gl context and bind
        g_gl_context = get_native_opengl_context(0);
        egl_makeCurrent(g_gl_context);
    }
    break;

    case FUNID_vkCreateAndroidSurfaceKHR: {
        LOGI("Host: vkCreateAndroidSurfaceKHR request");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("get vk param number %d instance is", para_num);

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

        if (need_free) free(stream);

        VkInstance hostInst = (VkInstance)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_INSTANCE, guest_inst);
        LOGI("Host: mapped guestInst %llu → hostInst %p",
            (unsigned long long)guest_inst, (void*)hostInst);

        GLFWwindow* win = (GLFWwindow*)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_NATIVE_WINDOW, guest_window_ptr);
        if (!win) {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            win = glfwCreateWindow(1, 1, "Guest Window", NULL, NULL);
            insert_mapping(EXPRESS_VK_OBJECT_TYPE_NATIVE_WINDOW,
                        guest_window_ptr,
                        (uint64_t)(uintptr_t)win);
            LOGI("Host: created GLFW window %p for guest window %llu",
                win, (unsigned long long)guest_window_ptr);
        }

        VkSurfaceKHR hostSurface = VK_NULL_HANDLE;
        VkResult res = glfwCreateWindowSurface(hostInst, win, NULL, &hostSurface);
        if (res != VK_SUCCESS) {
            LOGE("Host: vkCreateAndroidSurfaceKHR failed %d", res);
            return;
        }
        
        LOGI("Host: created hostSurface %lld %d", (long long)hostSurface, res);

        insert_mapping(EXPRESS_VK_OBJECT_TYPE_SURFACE, (uint64_t)guestSurface, (uint64_t)(uintptr_t)hostSurface);
    }
    break;

    case FUNID_vkCreateSwapchainKHR: {
        LOGI("Host: vkCreateSwapchainKHR request %lld", (long long)vkCreateSwapchainKHR);

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

        LOGI("Host: vkCreateSwapchainKHR guest_device %llu guest_surface %llu minImageCount %d imageFormat %d width %d height %d presentMode %d",
            (unsigned long long)guest_device,
            (unsigned long long)guest_surface,
            minImageCount, imageFormat, width, height, presentMode);

        if (need_free) free(stream);
        VkDevice hostDevice  = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkSurfaceKHR hostSurface = (VkSurfaceKHR)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_SURFACE, guest_surface);
        
        vulkan_surface_create_swapchain(hostDevice, hostSurface, guestSwapchain, minImageCount, imageFormat, width, height, presentMode);
    }
    break;

    case FUNID_vkGetSwapchainImagesKHR: {
        LOGI("Host: vkGetSwapchainImagesKHR");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char*     stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t*  ptr    = (uint8_t*)stream;
        uint64_t guest_device    = *(uint64_t*)ptr; ptr += sizeof(uint64_t);
        uint64_t guest_swapchain = *(uint64_t*)ptr; ptr += sizeof(uint64_t);
        uint32_t count           = *(uint32_t*)ptr; ptr += sizeof(uint32_t);
        VkDevice       realDev       = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkSwapchainKHR realSwapchain = (VkSwapchainKHR)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_SWAPCHAIN_KHR, guest_swapchain);
        LOGI("Host: vkGetSwapchainImagesKHR guest_device %llu guest_swapchain %llu count %d real swapchain %lld",
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
        
        LOGI("Host: vkGetSwapchainImagesKHR guestImages %lld guestBuffers %lld", (long long)guestImages[0], (long long)guestBuffers[0]);
        
        VkImage* images = malloc(sizeof(VkImage) * count);
        VkResult res = vkGetSwapchainImagesKHR(realDev, realSwapchain, &count, images);
        if (res != VK_SUCCESS) {
            LOGE("vkGetSwapchainImagesKHR failed: %d", res);
        } else {
            for (uint32_t i = 0; i < count; i++) {

                insert_mapping(
                    EXPRESS_VK_OBJECT_TYPE_IMAGE,
                    guestImages[i],
                    (uint64_t)(uintptr_t)images[i]);
                LOGI("Host: vkGetSwapchainImagesKHR guest %llu mapped to host %lld",guestImages[i], (uint64_t)(uintptr_t)images[i]);

                Hardware_Buffer *gbuffer = get_gbuffer_from_global_map(guestBuffers[i]);
                if (gbuffer == NULL) {
                    gbuffer = create_gbuffer_from_vulkan(
                        buffer_width[i],
                        buffer_height[i],
                        guestBuffers[i],
                        images[i], //这里用guest还得host的再议
                        NULL,
                        realDev,
                        NULL
                    );
                    if (gbuffer != NULL) {
                        add_gbuffer_to_global(gbuffer);
                    }
                }
            }
        }

        // vulkan_surface_register_swapchain_images(realDev, realSwapchain, guestImages, count);
        if (need_free) free(stream);
        free(guestImages);
        free(guestBuffers);
        free(buffer_width);
        free(buffer_height);
    }
    break;

    case FUNID_vkEnumeratePhysicalDevices: 
    {
        LOGI("Host: vkEnumeratePhysicalDevices request");

            
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("get vk param number %d", para_num);

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
        LOGI("before and after map instance %lld %lld", guest_inst, (uint64_t)(uintptr_t)instance);

        LOGI("Host: vkEnumeratePhysicalDevices count %d", count);

        VkResult result;
        if (count == 0) {
            result = vkEnumeratePhysicalDevices(instance, &count, NULL);
            LOGI("Host: vkEnumeratePhysicalDevices count after call %d", count);
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
                    LOGI("Host: vkEnumeratePhysicalDevices device guest and host %d %lld %lld", i, guest_devs[i], host_dev);
                }
                free(devices);
            }
        }
        write_to_guest_mem(all_para[3].data, &result, 0, sizeof(VkResult));
        
    }
    break;

    case FUNID_vkCreateDevice: {
        LOGI("Host: vkCreateDevice request");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host: vkCreateDevice para count = %d", para_num);

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

        pCreateInfo->enabledExtensionCount   = newCount;
        pCreateInfo->ppEnabledExtensionNames = newExts;

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
            LOGI("Host: mapped guest Dev %llu -> host %p",
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

        if (need_free) free(stream);
        free(pCreateInfo);
        free(newExts);
    }
    break;

    case FUNID_vkGetDeviceQueue: {
        LOGI("Host: vkGetDeviceQueue");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkGetDeviceQueue para_num=%d", para_num);

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

        LOGI("guest queue %llu mapped to host %p",
            (unsigned long long)guest_queue_handle,
            (void*)realQueue);

        if (need_free) free(stream);
    }
    break;

    case FUNID_vkCreateRenderPass: {
        LOGI("Host: vkCreateRenderPass");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("vkCreateRenderPass para count = %d", para_num);

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
            LOGI("Mapped RenderPass guest %llu -> host %p",
                (unsigned long long)guest_rp,
                (void*)realRp);
        } else {
            LOGE("vkCreateRenderPass failed: %d", result);
        }

        if (need_free) free(stream);
        free(pInfo);
    }
    break;

    case FUNID_vkCreateImageView: {
        LOGI("Host: vkCreateImageView");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("vkCreateImageView para count = %d", para_num);

        int need_free = 0;
        char*     stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr    = (uint8_t**)&stream;

        VkImageViewCreateInfo* pInfo = malloc(sizeof(VkImageViewCreateInfo));
        decode_from_stream_VkImageViewCreateInfo(
            VK_STRUCTURE_TYPE_MAX_ENUM,
            pInfo,
            ptr);
        
        LOGI("info image is %d %lld %d %d %d", pInfo->sType, (long long)pInfo->image, pInfo->viewType, pInfo->format, pInfo->components.r);

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

        LOGI("Host: vkCreateImageView realDev %p", (void*)realDev);

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
            LOGI("Mapped ImageView guest %llu -> host %p",
                (unsigned long long)guest_iv,
                (void*)realIv);
        } else {
            LOGE("vkCreateImageView failed: %d", result);
        }

        if (need_free) free(stream);
        free(pInfo);
    }
    break;

    case FUNID_vkCreateFramebuffer: {
        LOGI("Host: vkCreateFramebuffer");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("vkCreateFramebuffer para count = %d", para_num);

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
            LOGI("Mapped Framebuffer guest %llu -> host %p",
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

        if (result == VK_SUCCESS) {
            insert_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER, guest_buf, (uint64_t)(uintptr_t)realBuf);
            LOGI("Mapped Buffer guest %llu -> host %p", (unsigned long long)guest_buf, (void*)realBuf);
        } else {
            LOGE("vkCreateBuffer failed: %d", result);
        }

        if (need_free) free(stream);
        free(pInfo);
    }
    break;

    case FUNID_vkGetBufferMemoryRequirements: {
        LOGI("Host: vkGetBufferMemoryRequirements");
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

        vkGetBufferMemoryRequirements(realDev, realBuf, &req);
        LOGI("Host: vkGetBufferMemoryRequirements size %d alignment %d type %d",
            req.size, req.alignment, req.memoryTypeBits);
        
        LOGI("size is %d", sizeof(VkMemoryRequirements));

        write_to_guest_mem(
            all_para[1].data,
            &req,
            0,
            sizeof(VkMemoryRequirements));

        if (need_free) free(stream);
    }
    break;

    case FUNID_vkAllocateMemory: {
        LOGI("Host: vkAllocateMemory");
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
            LOGI("Requested memoryTypeIndex=%u flags=0x%x",
                reqType, flags);

            if (!(flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
                for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
                    if (memProps.memoryTypes[i].propertyFlags &
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
                        LOGI("Override memoryTypeIndex %u -> %u (HOST_VISIBLE)",
                            reqType, i);
                        pInfo->memoryTypeIndex = i;
                        break;
                    }
                }
            }
        }

        VkDeviceMemory realMem;
        VkResult result = vkAllocateMemory(realDev, pInfo, pAllocator, &realMem);

        if (result == VK_SUCCESS) {
            insert_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE_MEMORY, guest_mem, (uint64_t)(uintptr_t)realMem);
            LOGI("Mapped DeviceMemory guest %llu -> host %p",
                (unsigned long long)guest_mem,
                (void*)realMem);
        } else {
            LOGE("vkAllocateMemory failed: %d", result);
        }

        if (need_free) free(stream);
        free(pInfo);
    }
    break;

    case FUNID_vkMapMemory: {
        //ztodo：如果guest是写而不是读，这一步就需要把数据write to guest，暂未实现！！！
        LOGI("Host: vkMapMemory");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);

        int need_free = 0;
        char*     stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr    = (uint8_t**)&stream;

        uint64_t guest_dev  = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_mem  = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDeviceSize offset = *(VkDeviceSize*)(*ptr); *ptr += sizeof(VkDeviceSize);
        VkDeviceSize size   = *(VkDeviceSize*)(*ptr); *ptr += sizeof(VkDeviceSize);
        VkMemoryMapFlags flags = *(VkMemoryMapFlags*)(*ptr); *ptr += sizeof(VkMemoryMapFlags);
        LOGI("size of flags is %d", sizeof(VkMemoryMapFlags));

        // void** guest_ppData;
        // read_from_guest_mem(all_para[5].data, &guest_ppData, 0, sizeof(void*));

        VkDevice realDev = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_dev);
        VkDeviceMemory realMem = (VkDeviceMemory)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE_MEMORY, guest_mem);

        void* mappedPtr = NULL;
        VkResult result = vkMapMemory(realDev, realMem, offset, size, flags, &mappedPtr);
        LOGI("real dev %p real mem %p offset %d size %d flags %d",
            (void*)realDev, (void*)realMem, offset, size, flags);
        if (result != VK_SUCCESS) {
            LOGE("vkMapMemory failed: %d", result);
        } else {
            // write_to_guest_mem(all_para[5].data, &mappedPtr, 0, sizeof(void*));
            set_memory_map((uint64_t)realMem, mappedPtr);
            LOGI("Mapped memory guest %llu -> host %p",
                (unsigned long long)guest_mem,
                mappedPtr);
        }

        if (need_free) free(stream);
    }
    break;

    case FUNID_vkUnmapMemory: {
        LOGI("Host: vkUnmapMemory");
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
            LOGI("Host: synced %zu bytes to mappedPtr %p",
                (size_t)all_para[1].data_len, hostPtr);
        } else {
            LOGE("Host: no mapping found for guest_mem %llu", guest_mem);
        }

        vkUnmapMemory(realDev, realMem);

        if (need_free) free(stream);
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
            LOGI("Bound buffer %llu to memory %llu",
                (unsigned long long)guest_buffer,
                (unsigned long long)guest_memory);
        }

        if (need_free) free(stream);
    }
    break;

    case FUNID_vkCreateShaderModule: {
        LOGI("Host: vkCreateShaderModule");
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
            LOGI("Mapped ShaderModule guest %llu -> host %p",
                (unsigned long long)guest_module,
                (void*)realModule);
        } else {
            LOGE("vkCreateShaderModule failed: %d", result);
        }
        if (need_free) free(stream);
        free(pInfo);
    }
    break;

    case FUNID_vkCreatePipelineLayout: {
        LOGI("Host: vkCreatePipelineLayout");
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
            LOGI("Mapped PipelineLayout guest %llu -> host %p",
                (unsigned long long)guest_layout,
                (void*)realLayout);
        } else {
            LOGE("vkCreatePipelineLayout failed: %d", result);
        }

        if (need_free) free(stream);
        free(pInfo);
    }
    break;

    case FUNID_vkCreatePipelineCache: {
        LOGI("Host: vkCreatePipelineCache");
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
            LOGI("Mapped PipelineCache guest %llu -> host %p",
                (unsigned long long)guest_cache,
                (void*)realCache);
        } else {
            LOGE("vkCreatePipelineCache failed: %d", result);
        }

        if (need_free) free(stream);
        free(pInfo);
    }
    break;

    case FUNID_vkCreateGraphicsPipelines: {
        LOGI("Host: vkCreateGraphicsPipelines request");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);

        int need_free = 0;
        char*     stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr    = (uint8_t**)&stream;

        uint64_t guest_dev     = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_pipelineCache   = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t createInfoCount     = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        // createInfoCount     = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        LOGI("Host: vkCreateGraphicsPipelines createInfoCount = %u guest dev %lld cache %lld", createInfoCount, (long long)guest_dev, (long long)guest_pipelineCache);

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
        LOGI("Decoded %u VkGraphicsPipelineCreateInfo structures", createInfoCount);

        // 3) Decode allocator pointer and callbacks at end of buffer
        uint64_t guest_alloc_ptr = *(uint64_t*)(*ptr);
        *ptr += sizeof(uint64_t);
        VkAllocationCallbacks allocStruct, *pAllocator = NULL;
        LOGI("Guest allocator pointer: %llu", (unsigned long long)guest_alloc_ptr);
        if (guest_alloc_ptr) {
            decode_from_stream_VkAllocationCallbacks(
                VK_STRUCTURE_TYPE_MAX_ENUM,
                &allocStruct,
                ptr);
            pAllocator = &allocStruct;
        }

        if (need_free) free(stream);

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
                LOGI("Mapped GraphicsPipeline guest %llu -> host %p",
                    (unsigned long long)guest_pipe,
                    (void*)hostPipelines[i]);
            }
            LOGI("Mapped %u VkPipelines", createInfoCount);
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
            LOGI("Mapped CommandPool guest %llu -> host %p", (unsigned long long)guest_pool, (void*)realPool);
        } else {
            LOGE("vkCreateCommandPool failed: %d", result);
        }
        
        if (need_free) free(stream);
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
        LOGI("pinfo values commandBufferCount %d commandPool %p level %d",
            pInfo->commandBufferCount, (void*)pInfo->commandPool, pInfo->level);
        VkResult result = vkAllocateCommandBuffers(realDev, pInfo, realCmdBufs);
        
        if (result == VK_SUCCESS) {
            for (uint32_t i = 0; i < pInfo->commandBufferCount; ++i) {
                uint64_t guest_cmd_buf = *(uint64_t*)(*ptr);
                *ptr += sizeof(uint64_t);
                
                insert_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buf, 
                            (uint64_t)(uintptr_t)realCmdBufs[i]);
                LOGI("Mapped CommandBuffer %d guest %llu -> host %p", i, (unsigned long long)guest_cmd_buf, (void*)realCmdBufs[i]);
            }
        } else {
            LOGE("vkAllocateCommandBuffers failed: %d", result);
        }
        
        free(realCmdBufs);
        if (need_free) free(stream);
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
            LOGI("BeginCommandBuffer success guest %llu -> host %p", (unsigned long long)guest_cmd_buf, (void*)realCmdBuf);
        } else {
            LOGE("vkBeginCommandBuffer failed: %d", result);
        }
        
        if (need_free) free(stream);
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
        LOGI("vkCmdPipelineBarrier called with srcStageMask %u, dstStageMask %u, dependencyFlags %u",
            srcStageMask, dstStageMask, dependencyFlags);
        
        LOGI("CmdPipelineBarrier guest %llu -> host %p", (unsigned long long)guest_cmd_buf, (void*)realCmdBuf);
        
        if (memBarriers) free(memBarriers);
        if (bufBarriers) free(bufBarriers);
        if (imgBarriers) free(imgBarriers);
        if (need_free) free(stream);
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
        
        LOGI("CmdBeginRenderPass guest %llu -> host %p", (unsigned long long)guest_cmd_buf, (void*)realCmdBuf);
        
        if (need_free) free(stream);
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
        
        LOGI("CmdBindPipeline guest %llu -> host %p pipeline %p", (unsigned long long)guest_cmd_buf, (void*)realCmdBuf, (void*)realPipeline);
        
        if (need_free) free(stream);
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
        
        VkCommandBuffer realCmdBuf = (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buf);
        
        vkCmdBindVertexBuffers(realCmdBuf, firstBinding, bindingCount, realBuffers, offsets);
        
        LOGI("CmdBindVertexBuffers guest %llu -> host %p count %d", (unsigned long long)guest_cmd_buf, (void*)realCmdBuf, bindingCount);
        
        free(realBuffers);
        free(offsets);
        if (need_free) free(stream);
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
        
        LOGI("CmdDraw executed cmd=%p vertices=%d", (void*)realCmd, vertexCount);
        
        if (need_free) free(stream);
        
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
        
        LOGI("CmdEndRenderPass executed cmd=%p", (void*)realCmd);
        
        if (need_free) free(stream);
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
        
        LOGI("EndCommandBuffer executed cmd=%p result=%d", (void*)realCmd, result);
        
        if (need_free) free(stream); 
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
            LOGI("Mapped Fence guest=%llu host=%p", (unsigned long long)guest_fence, (void*)realFence);
        } else {
            LOGI("vkCreateFence failed: %d", result);
        }
        
        if (need_free) free(stream);
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
            LOGI("Mapped Semaphore guest=%llu host=%p", (unsigned long long)guest_semaphore, (void*)realSemaphore);
        } else {
            LOGI("vkCreateSemaphore failed: %d", result);
        }
        
        if (need_free) free(stream);
        free(pInfo);
        break;
    }

    case FUNID_vkAcquireNextImageKHR:{
        LOGI("Host: vkAcquireNextImageKHR");
        
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
        
        
        LOGI("Host: vkAcquireNextImageKHR result=%d imageIndex=%d", result, imageIndex); 
    }
    break;

    case FUNID_vkResetFences: {
        LOGI("Host: vkResetFences");
        
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
                LOGI("get Mapped Fence guest %llu -> host %p", (unsigned long long)guest_fences[i], (void*)fences[i]);
            }
        }
        
        VkResult result = vkResetFences(device, fenceCount, fences);
        
        if (fences) free(fences);
        
        LOGI("Host: vkResetFences result=%d fenceCount=%d", result, fenceCount);
    }
    break;

    case FUNID_vkQueueSubmit: {
        LOGI("Host: vkQueueSubmit");
        
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
        
        LOGI("Host: vkQueueSubmit result=%d submitCount=%d", result, submitCount);
    }
    break;

    case FUNID_vkWaitForFences:
    {
        LOGI("Host: vkWaitForFences");
        
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
        
        if (fences) free(fences);
        
        LOGI("Host: vkWaitForFences result=%d fenceCount=%d waitAll=%d", result, fenceCount, waitAll);
        break;
    }

    case FUNID_vkQueuePresentKHR: //ztodo:目前处理的应该是单swapchain的情况？
    {
        LOGI("Host: vkQueuePresentKHR request");
        
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
        
        LOGI("Host: vkQueuePresentKHR queue=%lld swapchainCount=%d buffer %llx", 
            (uint64_t)(uintptr_t)queue, presentInfo.swapchainCount, buffer_ids[0]);
        // 新逻辑：present前后做buffer管理和上屏
        vulkan_surface_present_images(queue, &presentInfo, buffer_ids);
        if (need_free) free(stream);
        break;
    }


/*
not tested yet!
    case FUNID_vkFreeCommandBuffers:
    {
        LOGI("Host: vkFreeCommandBuffers");
        
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        
        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_pool = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t bufferCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkCommandPool commandPool = (VkCommandPool)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_POOL, guest_pool);
        
        VkCommandBuffer* commandBuffers = NULL;
        if (bufferCount > 0) {
            commandBuffers = (VkCommandBuffer*)malloc(bufferCount * sizeof(VkCommandBuffer));
            uint64_t* guest_buffers = (uint64_t*)(*ptr);
            for (uint32_t i = 0; i < bufferCount; ++i) {
                commandBuffers[i] = (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_buffers[i]);
                remove_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_buffers[i]);
            }
        }
        
        vkFreeCommandBuffers(device, commandPool, bufferCount, commandBuffers);
        
        if (commandBuffers) free(commandBuffers);
        
        LOGI("Host: vkFreeCommandBuffers device=%lld pool=%lld count=%d", 
            (long long)guest_device, (long long)guest_pool, bufferCount);
    }
    break;

    case FUNID_vkQueueSubmit2: {
        LOGI("Host: vkQueueSubmit2");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_queue = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t submitCount = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint64_t guest_fence = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);

        VkQueue queue = (VkQueue)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_QUEUE, guest_queue);
        VkFence fence = guest_fence ? (VkFence)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_FENCE, guest_fence) : VK_NULL_HANDLE;

        VkSubmitInfo2* pSubmits = NULL;
        if (submitCount > 0) {
            pSubmits = (VkSubmitInfo2*)malloc(submitCount * sizeof(VkSubmitInfo2));
            for (uint32_t i = 0; i < submitCount; ++i) {
                decode_from_stream_VkSubmitInfo2(VK_STRUCTURE_TYPE_MAX_ENUM, &pSubmits[i], ptr);
            }
        }

        VkResult result = vkQueueSubmit2(queue, submitCount, pSubmits, fence);
        if (pSubmits) free(pSubmits);
        LOGI("Host: vkQueueSubmit2 result=%d submitCount=%d", result, submitCount);
        if (need_free) free(stream);
    }
    break;

    case FUNID_vkQueueWaitIdle: {
        LOGI("Host: vkQueueWaitIdle");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_queue = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkQueue queue = (VkQueue)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_QUEUE, guest_queue);
        VkResult result = vkQueueWaitIdle(queue);
        LOGI("Host: vkQueueWaitIdle result=%d", result);
        if (need_free) free(stream);
    }
    break;

    case FUNID_vkResetCommandBuffer: {
        LOGI("Host: vkResetCommandBuffer");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_cmd_buf = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkCommandBufferResetFlags flags = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        VkCommandBuffer cmd_buf = (VkCommandBuffer)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, guest_cmd_buf);
        VkResult result = vkResetCommandBuffer(cmd_buf, flags);
        LOGI("Host: vkResetCommandBuffer result=%d", result);
        if (need_free) free(stream);
    }
    break;

    case FUNID_vkResetCommandPool: {
        LOGI("Host: vkResetCommandPool");
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
        LOGI("Host: vkResetCommandPool result=%d", result);
        if (need_free) free(stream);
    }
    break;

    case FUNID_vkResetDescriptorPool: {
        LOGI("Host: vkResetDescriptorPool");
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
        LOGI("Host: vkResetDescriptorPool result=%d", result);
        if (need_free) free(stream);
    }
    break;

    case FUNID_vkResetEvent: {
        LOGI("Host: vkResetEvent");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_event = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkEvent event = (VkEvent)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_EVENT, guest_event);
        VkResult result = vkResetEvent(device, event);
        LOGI("Host: vkResetEvent result=%d", result);
        if (need_free) free(stream);
    }
    break;

    case FUNID_vkResetQueryPool: {
        LOGI("Host: vkResetQueryPool");
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
        LOGI("Host: vkResetQueryPool device=%p pool=%p firstQuery=%u queryCount=%u", (void*)device, (void*)pool, firstQuery, queryCount);
        if (need_free) free(stream);
    }
    break;

    case FUNID_vkSetEvent: {
        LOGI("Host: vkSetEvent");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_event = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkEvent event = (VkEvent)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_EVENT, guest_event);
        VkResult result = vkSetEvent(device, event);
        LOGI("Host: vkSetEvent result=%d", result);
        if (need_free) free(stream);
    }
    break;

    case FUNID_vkSetPrivateData: {
        LOGI("Host: vkSetPrivateData");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint32_t objectType = *(uint32_t*)(*ptr); *ptr += sizeof(uint32_t);
        uint64_t objectHandle = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t privateDataSlot = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t data = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkResult result = vkSetPrivateData(device, (VkObjectType)objectType, (uint64_t)objectHandle, (VkPrivateDataSlot)privateDataSlot, data);
        LOGI("Host: vkSetPrivateData result=%d", result);
        if (need_free) free(stream);
    }
    break;

    case FUNID_vkSignalSemaphore: {
        LOGI("Host: vkSignalSemaphore");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        uint64_t guest_semaphore = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkSemaphoreSignalInfo signalInfo;
        decode_from_stream_VkSemaphoreSignalInfo(VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO, &signalInfo, ptr);
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkResult result = vkSignalSemaphore(device, &signalInfo);
        LOGI("Host: vkSignalSemaphore result=%d", result);
        if (need_free) free(stream);
    }
    break;

    case FUNID_vkAcquireNextImage2KHR: {
        LOGI("Host: vkAcquireNextImage2KHR");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        VkAcquireNextImageInfoKHR* pInfo = malloc(sizeof(VkAcquireNextImageInfoKHR));
        decode_from_stream_VkAcquireNextImageInfoKHR(VK_STRUCTURE_TYPE_MAX_ENUM, pInfo, ptr);

        uint64_t guest_device = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDevice device = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);

        uint32_t imageIndex = 0;
        VkResult result = vkAcquireNextImage2KHR(device, pInfo, &imageIndex);

        write_to_guest_mem(all_para[1].data, &imageIndex, 0, sizeof(uint32_t));
        LOGI("vkAcquireNextImage2KHR result=%d imageIndex=%u", result, imageIndex);

        if (need_free) free(stream);
        free(pInfo);
    }
    break;

    case FUNID_vkAllocateDescriptorSets: {
        LOGI("Host: vkAllocateDescriptorSets");
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;

        VkDescriptorSetAllocateInfo* pInfo = malloc(sizeof(VkDescriptorSetAllocateInfo));
        decode_from_stream_VkDescriptorSetAllocateInfo(VK_STRUCTURE_TYPE_MAX_ENUM, pInfo, ptr);

        uint64_t guest_dev = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        VkDevice realDev = (VkDevice)(uintptr_t)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_dev);

        VkDescriptorSet* realSets = malloc(pInfo->descriptorSetCount * sizeof(VkDescriptorSet));
        VkResult result = vkAllocateDescriptorSets(realDev, pInfo, realSets);

        if (result == VK_SUCCESS) {
            for (uint32_t i = 0; i < pInfo->descriptorSetCount; ++i) {
                uint64_t guest_set = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
                insert_mapping(EXPRESS_VK_OBJECT_TYPE_DESCRIPTOR_SET, guest_set, (uint64_t)(uintptr_t)realSets[i]);
                LOGI("Mapped DescriptorSet %d guest %llu -> host %p", i, (unsigned long long)guest_set, (void*)realSets[i]);
            }
        } else {
            LOGE("vkAllocateDescriptorSets failed: %d", result);
        }

        free(realSets);
        if (need_free) free(stream);
        free(pInfo);
    }
    break;
*/
    case FUNID_vkDestroyBuffer: {
        LOGI("Host: vkDestroyBuffer request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroyBuffer para count = %d", para_num);
    
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
            LOGI("Host: vkDestroyBuffer completed for guest buffer %llu", (unsigned long long)guest_buf);
        }
    
        if (need_free) free(stream);
    }
    break;
    
    case FUNID_vkDestroyBufferView: {
        LOGI("Host: vkDestroyBufferView request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroyBufferView para count = %d", para_num);
    
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
            LOGI("Host: vkDestroyBufferView completed for guest buffer view %llu", (unsigned long long)guest_buf_view);
        }
    
        if (need_free) free(stream);
     }
     break;
    
    case FUNID_vkDestroyCommandPool: {
        LOGI("Host: vkDestroyCommandPool request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroyCommandPool para count = %d", para_num);
    
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
            LOGI("Host: vkDestroyCommandPool completed for guest command pool %llu", (unsigned long long)guest_cmd_pool);
        }
    
        if (need_free) free(stream);
     }
    break;

    case FUNID_vkDestroyDescriptorPool: {
        LOGI("Host: vkDestroyDescriptorPool request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroyDescriptorPool para count = %d", para_num);
    
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
            LOGI("Host: vkDestroyDescriptorPool completed for guest descriptor pool %llu", (unsigned long long)guest_desc_pool);
        }
    
        if (need_free) free(stream);
    }
    break;
    
    case FUNID_vkDestroyDescriptorSetLayout: {
        LOGI("Host: vkDestroyDescriptorSetLayout request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroyDescriptorSetLayout para count = %d", para_num);
    
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
            LOGI("Host: vkDestroyDescriptorSetLayout completed for guest descriptor set layout %llu", (unsigned long long)guest_desc_set_layout);
        }
    
        if (need_free) free(stream);
    }
    break;
    
    case FUNID_vkDestroyDescriptorUpdateTemplate: {
        LOGI("Host: vkDestroyDescriptorUpdateTemplate request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroyDescriptorUpdateTemplate para count = %d", para_num);
    
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
            LOGI("Host: vkDestroyDescriptorUpdateTemplate completed for guest descriptor update template %llu", (unsigned long long)guest_desc_update_template);
        }
    
        if (need_free) free(stream);
    }
    break;

    //note: device
    case FUNID_vkDestroyDevice: {
        LOGI("Host: vkDestroyDevice request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroyDevice para count = %d", para_num);
    
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
            LOGI("Host: vkDestroyDevice completed for guest device %llu", (unsigned long long)guest_dev);
        }
    
        if (need_free) free(stream);
    }
    break;

    case FUNID_vkDestroyEvent: {
        LOGI("Host: vkDestroyEvent request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroyEvent para count = %d", para_num);
    
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
            LOGI("Host: vkDestroyEvent completed for guest event %llu", (unsigned long long)guest_event);
        }
    
        if (need_free) free(stream);
    }
    break;
    
    case FUNID_vkDestroyFence: {
        LOGI("Host: vkDestroyFence request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroyFence para count = %d", para_num);
    
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
            LOGI("Host: vkDestroyFence completed for guest fence %llu", (unsigned long long)guest_fence);
        }
    
        if (need_free) free(stream);
    }
    break;
    
    case FUNID_vkDestroyFramebuffer: {
        LOGI("Host: vkDestroyFramebuffer request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroyFramebuffer para count = %d", para_num);
    
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
            LOGI("Host: vkDestroyFramebuffer completed for guest framebuffer %llu", (unsigned long long)guest_framebuffer);
        }
    
        if (need_free) free(stream);
    }
    break;

    case FUNID_vkDestroyImage: {
        LOGI("Host: vkDestroyImage request");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroyImage para count = %d", para_num);

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
            LOGI("Host: vkDestroyImage completed for guest image %llu", (unsigned long long)guest_image);
        }

        if (need_free) free(stream);
    }
    break;

    case FUNID_vkDestroyImageView: {
        LOGI("Host: vkDestroyImageView request");

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroyImageView para count = %d", para_num);

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
            LOGI("Host: vkDestroyImageView completed for guest image view %llu", (unsigned long long)guest_image_view);
        }

        if (need_free) free(stream);
    }
    break;

    //note: instance
    case FUNID_vkDestroyInstance: {
        LOGI("Host: vkDestroyInstance request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroyInstance para count = %d", para_num);
    
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
            LOGI("Host: vkDestroyInstance completed for guest instance %llu", (unsigned long long)guest_instance);
        }
    
        if (need_free) free(stream);
    }
    break;

    case FUNID_vkDestroyPipeline: {
        LOGI("Host: vkDestroyPipeline request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroyPipeline para count = %d", para_num);
    
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
            LOGI("Host: vkDestroyPipeline completed for guest pipeline %llu", (unsigned long long)guest_pipeline);
        }
    
        if (need_free) free(stream);
    }
    break;
    
    case FUNID_vkDestroyPipelineCache: {
        LOGI("Host: vkDestroyPipelineCache request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroyPipelineCache para count = %d", para_num);
    
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
            LOGI("Host: vkDestroyPipelineCache completed for guest pipeline cache %llu", (unsigned long long)guest_pipeline_cache);
        }
    
        if (need_free) free(stream);
    }
    break;
    
    case FUNID_vkDestroyPipelineLayout: {
        LOGI("Host: vkDestroyPipelineLayout request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroyPipelineLayout para count = %d", para_num);
    
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
            LOGI("Host: vkDestroyPipelineLayout completed for guest pipeline layout %llu", (unsigned long long)guest_pipeline_layout);
        }
    
        if (need_free) free(stream);
    }
    break;

    case FUNID_vkDestroyPrivateDataSlot: {
        LOGI("Host: vkDestroyPrivateDataSlot request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroyPrivateDataSlot para count = %d", para_num);
    
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
            LOGI("Host: vkDestroyPrivateDataSlot completed for guest private data slot %llu", (unsigned long long)guest_private_data_slot);
        }
    
        if (need_free) free(stream);
    }
    break;
    
    case FUNID_vkDestroyQueryPool: {
        LOGI("Host: vkDestroyQueryPool request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroyQueryPool para count = %d", para_num);
    
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
            LOGI("Host: vkDestroyQueryPool completed for guest query pool %llu", (unsigned long long)guest_query_pool);
        }
    
        if (need_free) free(stream);
    }
    break;
    
    case FUNID_vkDestroyRenderPass: {
        LOGI("Host: vkDestroyRenderPass request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroyRenderPass para count = %d", para_num);
    
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
            LOGI("Host: vkDestroyRenderPass completed for guest render pass %llu", (unsigned long long)guest_render_pass);
        }
    
        if (need_free) free(stream);
    }
    break;

    case FUNID_vkDestroySampler: {
        LOGI("Host: vkDestroySampler request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroySampler para count = %d", para_num);
    
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
            LOGI("Host: vkDestroySampler completed for guest sampler %llu", (unsigned long long)guest_sampler);
        }
    
        if (need_free) free(stream);
    }
    break;
    
    case FUNID_vkDestroySamplerYcbcrConversion: {
        LOGI("Host: vkDestroySamplerYcbcrConversion request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroySamplerYcbcrConversion para count = %d", para_num);
    
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
            LOGI("Host: vkDestroySamplerYcbcrConversion completed for guest sampler YCbCr conversion %llu", (unsigned long long)guest_ycbcr_conversion);
        }
    
        if (need_free) free(stream);
    }
    break;
    
    case FUNID_vkDestroySemaphore: {
        LOGI("Host: vkDestroySemaphore request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroySemaphore para count = %d", para_num);
    
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
            LOGI("Host: vkDestroySemaphore completed for guest semaphore %llu", (unsigned long long)guest_semaphore);
        }
    
        if (need_free) free(stream);
    }
    break;

    case FUNID_vkDestroyShaderModule: {
        LOGI("Host: vkDestroyShaderModule request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroyShaderModule para count = %d", para_num);
    
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
            LOGI("Host: vkDestroyShaderModule completed for guest shader module %llu", (unsigned long long)guest_shader_module);
        }
    
        if (need_free) free(stream);
    }
    break;
    
    //note: surface binded to instance
    case FUNID_vkDestroySurfaceKHR: {
        LOGI("Host: vkDestroySurfaceKHR request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroySurfaceKHR para count = %d", para_num);
    
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
            LOGI("Host: vkDestroySurfaceKHR completed for guest surface %llu", (unsigned long long)guest_surface);
        }
    
        if (need_free) free(stream);
    }
    break;
    
    case FUNID_vkDestroySwapchainKHR: {
        LOGI("Host: vkDestroySwapchainKHR request");
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host vkDestroySwapchainKHR para count = %d", para_num);
    
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
            LOGI("Host: vkDestroySwapchainKHR completed for guest swapchain %llu", (unsigned long long)guest_swapchain);
        }
    
        if (need_free) free(stream);
    }
    break;
    }
    call->callback(call, 1);
}
