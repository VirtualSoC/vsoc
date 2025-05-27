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

        // 修改 pCreateInfo 指向新的扩展列表
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

        // 解包参数
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

        // 查映射
        VkDevice       hostDevice  = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkSurfaceKHR   hostSurface = (VkSurfaceKHR)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_SURFACE, guest_surface);

        // 准备 CreateInfo
        VkSwapchainCreateInfoKHR sci = {
            .sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext            = NULL,
            .flags            = 0,
            .surface          = hostSurface,
            .minImageCount    = minImageCount,
            .imageFormat      = (VkFormat)imageFormat,
            .imageColorSpace  = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
            .imageExtent      = { width, height },
            .imageArrayLayers = 1,
            .imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount   = 0,
            .pQueueFamilyIndices     = NULL,
            .preTransform            = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
            .compositeAlpha          = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode             = (VkPresentModeKHR)presentMode,
            .clipped                 = VK_TRUE,
            .oldSwapchain            = VK_NULL_HANDLE
        };

        LOGI("sci info is %d %lld %d %d %d %d %d", sci.sType, sci.surface, sci.minImageCount, sci.imageFormat, sci.imageExtent.width, sci.imageExtent.height, sci.presentMode);

        VkSwapchainKHR hostSwapchain = VK_NULL_HANDLE;
        VkResult res = vkCreateSwapchainKHR(hostDevice, &sci, NULL, &hostSwapchain);
        if (res != VK_SUCCESS) {
            LOGE("Host: vkCreateSwapchainKHR failed %d", res);
            return;
        }

        // 存映射
        insert_mapping(EXPRESS_VK_OBJECT_TYPE_SWAPCHAIN_KHR,
                    (uint64_t)(uintptr_t)guestSwapchain,
                    (uint64_t)(uintptr_t)hostSwapchain);

        LOGI("Host: created hostSwapchain guest %llu -> host %p",
            (unsigned long long)guestSwapchain,
            (void*)hostSwapchain);
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

        VkDevice       realDev       = (VkDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, guest_device);
        VkSwapchainKHR realSwapchain = (VkSwapchainKHR)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_SWAPCHAIN_KHR, guest_swapchain);
        
        LOGI("Host: vkGetSwapchainImagesKHR guest_device %llu guest_swapchain %llu count %d real swapchain %lld",
            (unsigned long long)guest_device,
            (unsigned long long)guest_swapchain,
            count, (long long)realSwapchain);
        
        uint64_t* guestImages = malloc(sizeof(uint64_t) * count);
        read_from_guest_mem(
            all_para[1].data,
            guestImages,
            0,
            sizeof(uint64_t) * count);

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
            }
        }

        free(images);
        if (need_free) free(stream);
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

        // 解包参数流
        int need_free = 0;
        char* stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr = (uint8_t**)&stream;
        uint64_t guest_inst = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        
        uint32_t count = 0;
        read_from_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        // uint64_t guest_count_ptr = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);
        // uint64_t guest_devs_ptr  = *(uint64_t*)(*ptr); *ptr += sizeof(uint64_t);

        // 映射实例句柄
        VkInstance instance = (VkInstance)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_INSTANCE, guest_inst);
        LOGI("before and after map instance %lld %lld", guest_inst, (uint64_t)(uintptr_t)instance);

        // 读取guest给出的count值（第一次调用时为0，第二次为应用层更新后的值）
        LOGI("Host: vkEnumeratePhysicalDevices count %d", count);

        VkResult result;
        if (count == 0) {
            // 应用第一次调用，仅查询数量
            result = vkEnumeratePhysicalDevices(instance, &count, NULL);
            LOGI("Host: vkEnumeratePhysicalDevices count after call %d", count);
            // 写回数量
            write_to_guest_mem(all_para[1].data, &count, 0, sizeof(uint32_t));
        } else {
            VkPhysicalDevice* devices = (VkPhysicalDevice*)malloc(count * sizeof(VkPhysicalDevice));
            if (!devices) {
                result = VK_ERROR_OUT_OF_HOST_MEMORY;
            } else {
                uint64_t* guest_devs = malloc(count * sizeof(uint64_t));
                read_from_guest_mem(all_para[2].data, guest_devs, 0, count * sizeof(uint64_t));
                result = vkEnumeratePhysicalDevices(instance, &count, devices);
                // 写回每个设备句柄并建立映射
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

        // 1. 拆包所有参数流
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("Host: vkCreateDevice para count = %d", para_num);

        int need_free = 0;
        char*     stream = call_para_to_ptr(all_para[0], &need_free);
        uint8_t** ptr    = (uint8_t**)&stream;

        // 2. 反序列化 VkDeviceCreateInfo
        VkDeviceCreateInfo* pCreateInfo = malloc(sizeof(VkDeviceCreateInfo));
        decode_from_stream_VkDeviceCreateInfo(
            VK_STRUCTURE_TYPE_MAX_ENUM,
            pCreateInfo,
            ptr);

        // 3. 读取 guest 的 allocator 指针并可选地反序列化回调
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

        // 4. 取出两个 64 位 handle：物理设备和 guest-VkDevice
        uint64_t guest_phys = *(uint64_t*)(*ptr);  *ptr += sizeof(uint64_t);
        uint64_t guest_dev  = *(uint64_t*)(*ptr);  *ptr += sizeof(uint64_t);

        // —— 在这里做扩展过滤 —— //

        // 查 host 支持的 extension
        uint32_t availCount = 0;
        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)(uintptr_t)
            lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, guest_phys);
        vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &availCount, NULL);
        VkExtensionProperties* availProps = malloc(sizeof(VkExtensionProperties) * availCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &availCount, availProps);

        // 重建扩展数组
        uint32_t origCount = pCreateInfo->enabledExtensionCount;
        const char* const* origExts = pCreateInfo->ppEnabledExtensionNames;
        // 最多增加 1 个 swapchain 扩展
        const char** newExts = malloc(sizeof(char*) * (origCount + 1));
        uint32_t newCount = 0;

        for (uint32_t i = 0; i < origCount; i++) {
            const char* ext = origExts[i];
            if (has_device_extension(availProps, availCount, ext)) {
                newExts[newCount++] = ext;
            }
        }
        // 确保有 VK_KHR_swapchain
        if (!has_device_extension(availProps, availCount, VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
            // 如果 Host 不支持 swapchain，就不添加
        } else {
            // 如果 Guest 原来没加，补上
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


        // 5. 在 host 端调用真正的 vkCreateDevice
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

        // 6. 如果成功，建立 guest->host 的映射
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

        // 7. 写回 VkResult
        write_to_guest_mem(
            all_para[1].data,
            &result,
            0,
            sizeof(VkResult));

        // 8. 清理
        if (need_free) free(stream);
        free(pCreateInfo);
        free(newExts);
    }
    break;

    case FUNID_vkGetDeviceQueue: {
        LOGI("Host: vkGetDeviceQueue");

        // 1. 拆包
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

        // 2. 查映射，拿到真实的 VkDevice
        VkDevice realDevice = (VkDevice)(uintptr_t)
            lookup_mapping(
                EXPRESS_VK_OBJECT_TYPE_DEVICE,
                guest_dev_handle);

        // 3. 调用底层 Vulkan
        VkQueue realQueue;
        vkGetDeviceQueue(
            realDevice,
            queueFamilyIndex,
            queueIndex,
            &realQueue);

        // 4. 建立 guest→host Queue 映射
        insert_mapping(
            EXPRESS_VK_OBJECT_TYPE_QUEUE,
            guest_queue_handle,
            (uint64_t)(uintptr_t)realQueue);

        LOGI("guest queue %llu mapped to host %p",
            (unsigned long long)guest_queue_handle,
            (void*)realQueue);

        // 5. 清理
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

        uint32_t attCount = pInfo->attachmentCount;
        VkImageView* hostAtts = malloc(sizeof(VkImageView) * attCount);
        for (uint32_t i = 0; i < attCount; i++) {
            uint64_t guest_view = ((uint64_t*)(pInfo->pAttachments))[i];
            hostAtts[i] = (VkImageView)(uintptr_t)
                lookup_mapping(EXPRESS_VK_OBJECT_TYPE_IMAGE_VIEW, guest_view);
        }
        pInfo->pAttachments = hostAtts;

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
        free(hostAtts);
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

    }
    call->callback(call, 1);
}
