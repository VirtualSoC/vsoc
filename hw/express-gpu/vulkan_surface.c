#include "hw/express-gpu/vulkan_surface.h"
#include "hw/express-gpu/egl_surface.h"
#include "hw/express-gpu/express_vk_handle_mapping.h"
#include "hw/express-gpu/express_gpu_render.h"

#include <vulkan/vulkan.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <stdio.h>

// swapchain -> Hardware_Buffer*[]
static GHashTable *swapchain_buffer_map = NULL;
// device -> VkCommandPool
static GHashTable *device_command_pool_map = NULL;

static void ensure_swapchain_map() {
    if (!swapchain_buffer_map) {
        swapchain_buffer_map = g_hash_table_new(g_direct_hash, g_direct_equal);
    }
}

static void ensure_command_pool_map() {
    if (!device_command_pool_map) {
        device_command_pool_map = g_hash_table_new(g_direct_hash, g_direct_equal);
    }
}

static VkCommandPool get_command_pool() {
    ensure_command_pool_map();
    // 这里简化处理，使用第一个device的command pool
    // 实际应该根据当前device获取对应的command pool
    GList *keys = g_hash_table_get_keys(device_command_pool_map);
    if (!keys) {
        LOGE("[vulkan_surface] No command pool available!");
        return VK_NULL_HANDLE;
    }
    VkCommandPool pool = (VkCommandPool)(uintptr_t)g_hash_table_lookup(device_command_pool_map, keys->data);
    g_list_free(keys);
    return pool;
}

static void create_command_pool(VkDevice device) {
    ensure_command_pool_map();
    if (g_hash_table_lookup(device_command_pool_map, (gpointer)device)) {
        return; // command pool already exists
    }

    VkCommandPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = 0 // 使用第一个queue family
    };

    VkCommandPool commandPool;
    VkResult res = vkCreateCommandPool(device, &poolInfo, NULL, &commandPool);
    if (res != VK_SUCCESS) {
        LOGE("[vulkan_surface] Failed to create command pool: %d", res);
        return;
    }

    g_hash_table_insert(device_command_pool_map, (gpointer)device, (gpointer)(uintptr_t)commandPool);
}

void vulkan_surface_create_swapchain(VkDevice device, VkSurfaceKHR surface, VkSwapchainKHR guest_swapchain, uint32_t minImageCount, uint32_t imageFormat, uint32_t width, uint32_t height, uint32_t presentMode) {
    // 确保有command pool
    create_command_pool(device);
    
    // 这里只做host swapchain创建和映射，image分配在GetSwapchainImagesKHR
    VkSwapchainCreateInfoKHR sci = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = minImageCount,
        .imageFormat = imageFormat,
        .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        .imageExtent = { width, height },
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE
    };
    VkSwapchainKHR host_swapchain = VK_NULL_HANDLE;
    VkResult res = vkCreateSwapchainKHR(device, &sci, NULL, &host_swapchain);
    if (res != VK_SUCCESS) {
        LOGE("[vulkan_surface] vkCreateSwapchainKHR failed: %d", res);
        return;
    }
    insert_mapping(EXPRESS_VK_OBJECT_TYPE_SWAPCHAIN_KHR, (uint64_t)(uintptr_t)guest_swapchain, (uint64_t)(uintptr_t)host_swapchain);
    ensure_swapchain_map();
    g_hash_table_insert(swapchain_buffer_map, (gpointer)host_swapchain, NULL); // image数组后续填充
}

void vulkan_surface_register_swapchain_images(VkDevice device, VkSwapchainKHR swapchain, uint64_t *guestImages, uint32_t count) {
    ensure_swapchain_map();
    VkImage *images = malloc(sizeof(VkImage) * count);
    VkResult res = vkGetSwapchainImagesKHR(device, swapchain, &count, images);
    if (res != VK_SUCCESS) {
        LOGE("[vulkan_surface] vkGetSwapchainImagesKHR failed: %d\n", res);
        free(images);
        return;
    }
    Hardware_Buffer **buffers = malloc(sizeof(Hardware_Buffer*) * count);
    for (uint32_t i = 0; i < count; ++i) {
        Hardware_Buffer *buf = calloc(1, sizeof(Hardware_Buffer));
        buf->backend_type = HARDWARE_BUFFER_BACKEND_VULKAN;
        buf->vk_image = (void*)images[i];
        buf->vk_device = device;
        buf->vk_format = NULL; // 可选
        buf->vk_image_index = i;
        // 其他字段可按需补充
        insert_mapping(EXPRESS_VK_OBJECT_TYPE_IMAGE, guestImages[i], (uint64_t)(uintptr_t)images[i]);
        buffers[i] = buf;
    }
    g_hash_table_replace(swapchain_buffer_map, (gpointer)swapchain, buffers);
    free(images);
}

void vulkan_surface_present_images(VkQueue queue, VkPresentInfoKHR *presentInfo, 
                                   uint64_t* buffer_ids) {
    for (uint32_t i = 0; i < presentInfo->swapchainCount; ++i) {
        VkSwapchainKHR swapchain = presentInfo->pSwapchains[i];
        uint32_t imageIndex = presentInfo->pImageIndices[i];
        
        Hardware_Buffer* gbuffer = get_gbuffer_from_global_map(buffer_ids[i]);
        if (!gbuffer) {
            LOGE("[vulkan_surface] No gbuffer found for buffer id %llx", buffer_ids[i]);
            continue;
        }

        if (!gbuffer->needs_copy && gbuffer->vk_shared_image) {
            // ===== 零拷贝路径：GPU blit =====
            VkImage src_image = (VkImage)gbuffer->vk_image;  // swapchain image
            VkImage dst_image = (VkImage)gbuffer->vk_shared_image;  // 共享image
            VkDevice device = (VkDevice)gbuffer->vk_device;
            
            // 现场创建临时command buffer
            VkCommandPoolCreateInfo pool_info = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
                .queueFamilyIndex = 0  // 假设queue family 0支持graphics
            };
            VkCommandPool cmd_pool;
            vkCreateCommandPool(device, &pool_info, NULL, &cmd_pool);
            
            VkCommandBufferAllocateInfo alloc_info = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .commandPool = cmd_pool,
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = 1
            };
            VkCommandBuffer cmd;
            vkAllocateCommandBuffers(device, &alloc_info, &cmd);
            
            VkCommandBufferBeginInfo begin_info = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
            };
            vkBeginCommandBuffer(cmd, &begin_info);
            
            // Transition src image layout
            VkImageMemoryBarrier barrier1 = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
                .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = src_image,
                .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
            };
            vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                                0, NULL, 0, NULL, 1, &barrier1);
            
            // Transition dst image layout
            VkImageMemoryBarrier barrier2 = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = 0,
                .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = dst_image,
                .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
            };
            vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                                0, NULL, 0, NULL, 1, &barrier2);
            
            // Blit
            VkImageBlit blit = {
                .srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
                .srcOffsets = {{0, 0, 0}, {gbuffer->width, gbuffer->height, 1}},
                .dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
                .dstOffsets = {{0, 0, 0}, {gbuffer->width, gbuffer->height, 1}}
            };
            vkCmdBlitImage(cmd, src_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                          dst_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          1, &blit, VK_FILTER_NEAREST);
            
            // Transition dst to GENERAL for GL access
            VkImageMemoryBarrier barrier3 = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                .newLayout = VK_IMAGE_LAYOUT_GENERAL,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = dst_image,
                .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
            };
            vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0,
                                0, NULL, 0, NULL, 1, &barrier3);
            
            vkEndCommandBuffer(cmd);
            
            // Submit
            VkSubmitInfo submit = {
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .commandBufferCount = 1,
                .pCommandBuffers = &cmd
            };
            vkQueueSubmit(queue, 1, &submit, VK_NULL_HANDLE);
            vkQueueWaitIdle(queue);
            
            // 清理临时资源
            vkDestroyCommandPool(device, cmd_pool, NULL);
            
            LOGI("[vulkan_surface] Zero-copy presented image %d -> GL texture %u", 
                 imageIndex, gbuffer->data_texture);
                 
        } else {
            // ===== 传统CPU拷贝路径 =====
            void* pixels = malloc(gbuffer->width * gbuffer->height * 4);
            if (vulkan_image_read_pixels(gbuffer, pixels, 
                                        gbuffer->width * gbuffer->height * 4)) {
                glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 
                               gbuffer->width, gbuffer->height, 
                               GL_RGBA, GL_UNSIGNED_BYTE, pixels);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            free(pixels);
            
            LOGW("[vulkan_surface] CPU-copy presented image %d", imageIndex);
        }
    }
}


// Read pixels from Vulkan image to host memory
bool vulkan_image_read_pixels(Hardware_Buffer *gbuffer, void *dst, size_t size) {
    if (!gbuffer || gbuffer->backend_type != HARDWARE_BUFFER_BACKEND_VULKAN) return false;
    VkDevice device = (VkDevice)gbuffer->vk_device;
    VkImage image = (VkImage)gbuffer->vk_image;
    // 1. Create host visible staging buffer
    VkBufferCreateInfo buf_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };
    VkBuffer staging_buf;
    VkResult res = vkCreateBuffer(device, &buf_info, NULL, &staging_buf);
    if (res != VK_SUCCESS) return false;
    VkMemoryRequirements mem_req;
    vkGetBufferMemoryRequirements(device, staging_buf, &mem_req);
    VkMemoryAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = mem_req.size,
        .memoryTypeIndex = 0 // Need to find host visible type
    };
    VkPhysicalDevice pd = get_device_pd((uint64_t)(uintptr_t)device);
    VkPhysicalDeviceMemoryProperties mem_props;
    vkGetPhysicalDeviceMemoryProperties(pd, &mem_props);
    for (uint32_t i = 0; i < mem_props.memoryTypeCount; ++i) {
        if (mem_props.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
            alloc_info.memoryTypeIndex = i;
            break;
        }
    }
    VkDeviceMemory staging_mem;
    res = vkAllocateMemory(device, &alloc_info, NULL, &staging_mem);
    if (res != VK_SUCCESS) {
        vkDestroyBuffer(device, staging_buf, NULL);
        return false;
    }
    vkBindBufferMemory(device, staging_buf, staging_mem, 0);
    // 2. Create command buffer and copy image to buffer
    // Only doing single-threaded synchronization here, production should have more complex sync
    VkCommandPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = 0 // Need to find queue that supports graphics/transfer
    };
    VkCommandPool cmd_pool;
    vkCreateCommandPool(device, &pool_info, NULL, &cmd_pool);
    VkCommandBufferAllocateInfo alloc_cmd = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = cmd_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };
    VkCommandBuffer cmd_buf;
    vkAllocateCommandBuffers(device, &alloc_cmd, &cmd_buf);
    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
    };
    vkBeginCommandBuffer(cmd_buf, &begin_info);
    VkBufferImageCopy region = {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1
        },
        .imageOffset = {0, 0, 0},
        .imageExtent = {gbuffer->width, gbuffer->height, 1}
    };
    vkCmdCopyImageToBuffer(cmd_buf, image, VK_IMAGE_LAYOUT_GENERAL, staging_buf, 1, &region);
    vkEndCommandBuffer(cmd_buf);
    VkQueue queue;
    vkGetDeviceQueue(device, 0, 0, &queue);
    VkSubmitInfo submit = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &cmd_buf
    };
    vkQueueSubmit(queue, 1, &submit, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);
    // 3. Map buffer and read data
    void *mapped;
    vkMapMemory(device, staging_mem, 0, size, 0, &mapped);
    memcpy(dst, mapped, size);
    vkUnmapMemory(device, staging_mem);
    // 4. Cleanup
    vkFreeMemory(device, staging_mem, NULL);
    vkDestroyBuffer(device, staging_buf, NULL);
    vkDestroyCommandPool(device, cmd_pool, NULL);
    return true;
}

uint32_t find_memory_type(VkPhysicalDevice pd, uint32_t type_filter, 
                                 VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties mem_props;
    vkGetPhysicalDeviceMemoryProperties(pd, &mem_props);
    
    for (uint32_t i = 0; i < mem_props.memoryTypeCount; i++) {
        if ((type_filter & (1 << i)) && 
            (mem_props.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    
    // 如果找不到完全匹配的，返回第一个满足type_filter的
    for (uint32_t i = 0; i < mem_props.memoryTypeCount; i++) {
        if (type_filter & (1 << i)) {
            LOGW("[Interop] Using memory type %d (not exact match)", i);
            return i;
        }
    }
    
    LOGE("[Interop] Failed to find suitable memory type");
    return 0;
}
// 创建可导出的共享image
static bool create_shared_image_and_gl_texture(VkDevice device, VkPhysicalDevice pd,
                                              uint32_t width, uint32_t height,
                                              VkImage *out_image,
                                              VkDeviceMemory *out_memory,
                                              GLuint *out_texture) {
    // 1. 创建可导出的image
    VkExternalMemoryImageCreateInfo external_info = {
        .sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO,
        .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT
    };
    
    VkImageCreateInfo img_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = &external_info,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = VK_FORMAT_R8G8B8A8_UNORM,
        .extent = {width, height, 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };
    
    VkResult res = vkCreateImage(device, &img_info, NULL, out_image);
    if (res != VK_SUCCESS) return false;
    
    // 2. 分配可导出的memory
    VkMemoryRequirements mem_reqs;
    vkGetImageMemoryRequirements(device, *out_image, &mem_reqs);
    
    VkExportMemoryAllocateInfo export_info = {
        .sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO,
        .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT
    };
    
    VkMemoryAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = &export_info,
        .allocationSize = mem_reqs.size,
        .memoryTypeIndex = find_memory_type(pd, mem_reqs.memoryTypeBits,
                                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    };
    
    res = vkAllocateMemory(device, &alloc_info, NULL, out_memory);
    if (res != VK_SUCCESS) {
        vkDestroyImage(device, *out_image, NULL);
        return false;
    }
    
    vkBindImageMemory(device, *out_image, *out_memory, 0);
    
    // 3. 获取Win32 handle
    if (!pfn_vkGetMemoryWin32HandleKHR) return false;
    
    VkMemoryGetWin32HandleInfoKHR handle_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_GET_WIN32_HANDLE_INFO_KHR,
        .memory = *out_memory,
        .handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT
    };
    
    HANDLE win32_handle;
    res = pfn_vkGetMemoryWin32HandleKHR(device, &handle_info, &win32_handle);
    if (res != VK_SUCCESS) return false;
    
    // 4. 创建GL memory object
    GLuint memory_object;
    glCreateMemoryObjectsEXT(1, &memory_object);
    glImportMemoryWin32HandleEXT(memory_object, mem_reqs.size,
                                GL_HANDLE_TYPE_OPAQUE_WIN32_EXT,
                                win32_handle);
    
    // 5. 创建GL texture
    glCreateTextures(GL_TEXTURE_2D, 1, out_texture);
    glTextureParameteri(*out_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(*out_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(*out_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(*out_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTextureStorageMem2DEXT(*out_texture, 1, GL_RGBA8,
                            width, height, memory_object, 0);
    
    LOGI("[Interop] Created shared image and GL texture %u (%dx%d)", 
         *out_texture, width, height);
    return true;
}

// 写入host内存到Vulkan image
bool vulkan_image_write_pixels(Hardware_Buffer *gbuffer, void *src, size_t size) {
    if (!gbuffer || gbuffer->backend_type != HARDWARE_BUFFER_BACKEND_VULKAN) return false;
    VkDevice device = (VkDevice)gbuffer->vk_device;
    VkImage image = (VkImage)gbuffer->vk_image;
    // 1. 创建host visible的staging buffer
    VkBufferCreateInfo buf_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };
    VkBuffer staging_buf;
    VkResult res = vkCreateBuffer(device, &buf_info, NULL, &staging_buf);
    if (res != VK_SUCCESS) return false;
    VkMemoryRequirements mem_req;
    vkGetBufferMemoryRequirements(device, staging_buf, &mem_req);
    VkMemoryAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = mem_req.size,
        .memoryTypeIndex = 0 // 需查找host visible类型
    };
    VkPhysicalDevice pd = get_device_pd((uint64_t)(uintptr_t)device);
    VkPhysicalDeviceMemoryProperties mem_props;
    vkGetPhysicalDeviceMemoryProperties(pd, &mem_props);
    for (uint32_t i = 0; i < mem_props.memoryTypeCount; ++i) {
        if (mem_props.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
            alloc_info.memoryTypeIndex = i;
            break;
        }
    }
    VkDeviceMemory staging_mem;
    res = vkAllocateMemory(device, &alloc_info, NULL, &staging_mem);
    if (res != VK_SUCCESS) {
        vkDestroyBuffer(device, staging_buf, NULL);
        return false;
    }
    vkBindBufferMemory(device, staging_buf, staging_mem, 0);
    // 2. 映射buffer写入数据
    void *mapped;
    vkMapMemory(device, staging_mem, 0, size, 0, &mapped);
    memcpy(mapped, src, size);
    vkUnmapMemory(device, staging_mem);
    // 3. 创建command buffer并拷贝buffer到image
    VkCommandPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = 0 // 需查找支持graphics/transfer的队列
    };
    VkCommandPool cmd_pool;
    vkCreateCommandPool(device, &pool_info, NULL, &cmd_pool);
    VkCommandBufferAllocateInfo alloc_cmd = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = cmd_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };
    VkCommandBuffer cmd_buf;
    vkAllocateCommandBuffers(device, &alloc_cmd, &cmd_buf);
    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
    };
    vkBeginCommandBuffer(cmd_buf, &begin_info);
    VkBufferImageCopy region = {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1
        },
        .imageOffset = {0, 0, 0},
        .imageExtent = {gbuffer->width, gbuffer->height, 1}
    };
    vkCmdCopyBufferToImage(cmd_buf, staging_buf, image, VK_IMAGE_LAYOUT_GENERAL, 1, &region);
    vkEndCommandBuffer(cmd_buf);
    VkQueue queue;
    vkGetDeviceQueue(device, 0, 0, &queue);
    VkSubmitInfo submit = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &cmd_buf
    };
    vkQueueSubmit(queue, 1, &submit, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);
    // 4. 清理
    vkFreeMemory(device, staging_mem, NULL);
    vkDestroyBuffer(device, staging_buf, NULL);
    vkDestroyCommandPool(device, cmd_pool, NULL);
    return true;
}

Hardware_Buffer *create_gbuffer_from_vulkan(int width, int height, uint64_t gbuffer_id, 
                                          void *vk_image, void *vk_device_memory, 
                                          void *vk_device, void *vk_format) {
    Hardware_Buffer *gbuffer = g_malloc0(sizeof(Hardware_Buffer));
    
    // Set basic properties
    gbuffer->usage_type = GBUFFER_TYPE_WINDOW;
    gbuffer->gbuffer_id = gbuffer_id;
    gbuffer->backend_type = HARDWARE_BUFFER_BACKEND_VULKAN;
    gbuffer->width = width;
    gbuffer->height = height;

    // Store Vulkan resources (swapchain image，不可共享)
    gbuffer->vk_image = vk_image;
    gbuffer->vk_device_memory = vk_device_memory;
    gbuffer->vk_device = vk_device;
    gbuffer->vk_format = vk_format;

    // 尝试创建共享image和GL texture
    VkPhysicalDevice pd = get_device_pd((uint64_t)(uintptr_t)vk_device);
    VkImage shared_image = VK_NULL_HANDLE;
    VkDeviceMemory shared_memory = VK_NULL_HANDLE;
    GLuint shared_texture = 0;
    
    init_interop_once((VkDevice)vk_device);
    
    if (pfn_vkGetMemoryWin32HandleKHR &&
        create_shared_image_and_gl_texture((VkDevice)vk_device, pd,
                                          width, height,
                                          &shared_image, &shared_memory,
                                          &shared_texture)) {
        // 成功：使用零拷贝模式
        gbuffer->vk_shared_image = shared_image;
        gbuffer->vk_shared_memory = shared_memory;
        gbuffer->data_texture = shared_texture;
        gbuffer->needs_copy = false;
        
        LOGI("[vulkan_surface] Created ZERO-COPY gbuffer %llx with shared texture %d", 
             gbuffer_id, shared_texture);
    } else {
        // 失败：fallback到传统模式
        glGenTextures(1, &(gbuffer->data_texture));
        glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, 
                     GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        glBindTexture(GL_TEXTURE_2D, 0);
        
        gbuffer->needs_copy = true;
        
        LOGW("[vulkan_surface] Created CPU-COPY gbuffer %llx with texture %d (fallback)", 
             gbuffer_id, gbuffer->data_texture);
    }

    return gbuffer;
} 