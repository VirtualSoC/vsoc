#include "hw/express-gpu/vulkan_surface.h"
#include "hw/express-gpu/egl_surface.h"
#include "hw/express-gpu/express_vk_handle_mapping.h"
#include "hw/express-gpu/express_gpu_main_window.h"

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

void vulkan_surface_present_images(VkQueue queue, VkPresentInfoKHR *presentInfo, uint64_t* buffer_ids) {
    // Iterate through all swapchains and images to be presented
    for (uint32_t i = 0; i < presentInfo->swapchainCount; ++i) {
        VkSwapchainKHR swapchain = presentInfo->pSwapchains[i];
        uint32_t imageIndex = presentInfo->pImageIndices[i];
        Hardware_Buffer* target_gbuffer = get_gbuffer_from_global_map(buffer_ids[i]);
        if (target_gbuffer) {
            // 1. Get the target gbuffer from global map
            Hardware_Buffer* target_gbuffer = get_gbuffer_from_global_map(buffer_ids[i]);
            if (!target_gbuffer) {
                LOGE("[vulkan_surface] Failed to find gbuffer for id %llx", buffer_ids[i]);
                continue;
            }

            // 2. Read pixels from Vulkan image
            void* pixels = malloc(target_gbuffer->width * target_gbuffer->height * 4); // RGBA format
            if (vulkan_image_read_pixels(target_gbuffer, pixels, target_gbuffer->width * target_gbuffer->height * 4)) {
                // 3. Copy pixels to GL texture
                glBindTexture(GL_TEXTURE_2D, target_gbuffer->data_texture);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, target_gbuffer->width, target_gbuffer->height, 
                                GL_RGBA, GL_UNSIGNED_BYTE, pixels);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            free(pixels);
            LOGI("[vulkan_surface] Presented image %d from swapchain %lld to gbuffer %llx", 
                imageIndex, (unsigned long long)swapchain, buffer_ids[i]);
        } else {
            LOGE("[vulkan_surface] No gbuffer found for buffer id %llx", buffer_ids[i]);
        }
    }

    // Present the images
    // VkResult res = vkQueuePresentKHR(queue, presentInfo);
    // if (res != VK_SUCCESS) {
    //     LOGE("[vulkan_surface] vkQueuePresentKHR failed: %d", res);
    // }
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

    // Store Vulkan resources
    gbuffer->vk_image = vk_image;
    gbuffer->vk_device_memory = vk_device_memory;
    gbuffer->vk_device = vk_device;
    gbuffer->vk_format = vk_format;

    // Create GL texture for pixel data
    glGenTextures(1, &(gbuffer->data_texture));
    glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Allocate texture storage
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, 
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);

    LOGI("[vulkan_surface] Created gbuffer %llx with texture %d for Vulkan image", 
         gbuffer_id, gbuffer->data_texture);

    return gbuffer;
} 