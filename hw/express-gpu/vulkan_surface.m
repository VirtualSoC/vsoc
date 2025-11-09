#include "hw/express-gpu/vulkan_surface.h"
#include "hw/express-gpu/egl_surface.h"
#include "hw/express-gpu/express_vk_handle_mapping.h"
#include "hw/express-gpu/express_gpu_render.h"
#include "hw/express-gpu/glv3_context.h"

#include <vulkan/vulkan.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <stdio.h>

// swapchain -> Hardware_Buffer*[]
static GHashTable *swapchain_buffer_map = NULL;
// device -> VkCommandPool
static GHashTable *device_command_pool_map = NULL;

static __thread void *g_gl_context = NULL;
static bool is_init = false;

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

static GLuint g_copy_rectangle_shader = 0;

static void init_copy_shader_once(void) {
    if (g_copy_rectangle_shader != 0) return;
    
    const char* vertex_shader_src = 
        "#version 150\n"
        "in vec2 position;\n"
        "out vec2 fragCoord;\n"
        "void main() {\n"
        "    gl_Position = vec4(position, 0.0, 1.0);\n"
        "    fragCoord = (position + 1.0) * 0.5;\n"  // [-1,1] -> [0,1]
        "}\n";
    
    const char* fragment_shader_src = 
        "#version 150\n"
        "#extension GL_ARB_texture_rectangle : enable\n"
        "uniform sampler2DRect texRect;\n"
        "uniform vec2 texSize;\n"
        "in vec2 fragCoord;\n"
        "out vec4 outColor;\n"
        "void main() {\n"
        "    vec2 pixelCoord = fragCoord * texSize;\n"  // [0,1] -> [0,width/height]
        "    outColor = texture(texRect, pixelCoord);\n"
        "}\n";
    
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader_src, NULL);
    glCompileShader(vs);
    
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader_src, NULL);
    glCompileShader(fs);
    
    g_copy_rectangle_shader = glCreateProgram();
    glAttachShader(g_copy_rectangle_shader, vs);
    glAttachShader(g_copy_rectangle_shader, fs);
    glLinkProgram(g_copy_rectangle_shader);
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    LOGI("[vulkan_surface] Copy shader initialized: program=%u", g_copy_rectangle_shader);
}

static void copy_texture_rectangle_to_2d(GLuint src_rect_texture, GLuint dst_texture_2d,
                                         int width, int height) {
    init_copy_shader_once();
    
    // ===== 创建并绑定目标 FBO =====
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                          GL_TEXTURE_2D, dst_texture_2d, 0);
    
    // ===== 设置渲染状态 =====
    glViewport(0, 0, width, height);
    
    GLboolean depth_test_enabled = glIsEnabled(GL_DEPTH_TEST);
    GLboolean blend_enabled = glIsEnabled(GL_BLEND);
    GLboolean cull_face_enabled = glIsEnabled(GL_CULL_FACE);
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    
    // ===== 使用 shader 复制 =====
    glUseProgram(g_copy_rectangle_shader);
    
    glUniform1i(glGetUniformLocation(g_copy_rectangle_shader, "texRect"), 0);
    glUniform2f(glGetUniformLocation(g_copy_rectangle_shader, "texSize"), 
                (float)width, (float)height);
    
    // 绑定源纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_RECTANGLE, src_rect_texture);
    
    // 创建全屏四边形
    static const float vertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };
    
    GLuint vbo, vao;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);  // position 固定在 location 0
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    // 绘制
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    // ===== 清理 =====
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glBindTexture(GL_TEXTURE_RECTANGLE, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);
    
    if (depth_test_enabled) glEnable(GL_DEPTH_TEST);
    if (blend_enabled) glEnable(GL_BLEND);
    if (cull_face_enabled) glEnable(GL_CULL_FACE);
}

// todo:eglmakecurrent!!!!

void vulkan_surface_present_images(VkQueue queue, VkPresentInfoKHR *presentInfo, 
                                   uint64_t* buffer_ids) {
    if(!is_init) {
        g_gl_context = get_native_opengl_context(0);
        egl_makeCurrent(g_gl_context); 
        LOGI("[Interop] Obtained native OpenGL context: %p", g_gl_context);       
    }
    is_init = true;

    for (uint32_t i = 0; i < presentInfo->swapchainCount; ++i) {
        VkSwapchainKHR swapchain = presentInfo->pSwapchains[i];
        uint32_t imageIndex = presentInfo->pImageIndices[i];
        
        Hardware_Buffer* gbuffer = get_gbuffer_from_global_map(buffer_ids[i]);
        if (!gbuffer) {
            LOGE("[vulkan_surface] No gbuffer found for buffer id %llx", buffer_ids[i]);
            continue;
        }

        if (!gbuffer->needs_copy && gbuffer->vk_shared_image) {
            // ===== 步骤1：Vulkan blit 到共享 image =====
            VkImage src_image = (VkImage)gbuffer->vk_image;
            VkImage dst_image = (VkImage)gbuffer->vk_shared_image;
            VkDevice device = (VkDevice)gbuffer->vk_device;
            
            VkCommandPoolCreateInfo pool_info = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
                .queueFamilyIndex = 0
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
            
            // Transition src image
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
            
            // Transition dst image
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
            
            // Blit（上下翻转源图像）
            VkImageBlit blit = {
                .srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
                .srcOffsets = {
                    {0, (int32_t)gbuffer->height, 0},
                    {(int32_t)gbuffer->width, 0, 1}
                },
                .dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
                .dstOffsets = {{0, 0, 0}, {(int32_t)gbuffer->width, (int32_t)gbuffer->height, 1}}
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
            
            VkSubmitInfo submit = {
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .commandBufferCount = 1,
                .pCommandBuffers = &cmd
            };
            vkQueueSubmit(queue, 1, &submit, VK_NULL_HANDLE);
            vkQueueWaitIdle(queue);
            
            vkDestroyCommandPool(device, cmd_pool, NULL);

            glFlush();
            
            // ===== 步骤2：GL_TEXTURE_RECTANGLE → GL_TEXTURE_2D =====
            copy_texture_rectangle_to_2d(gbuffer->intermediate_texture,
                                        gbuffer->data_texture,
                                        gbuffer->width,
                                        gbuffer->height);
            
            LOGI("[vulkan_surface] Two-step zero-copy completed: image %d", i);
                 
        } else {
            // ===== 传统CPU拷贝路径 =====
            void* pixels = malloc(gbuffer->width * gbuffer->height * 4);
            if (vulkan_image_read_pixels(gbuffer, pixels, 
                                        gbuffer->width * gbuffer->height * 4)) {
                LOGI("[vulkan_surface] Read pixels from Vulkan image %d", imageIndex);
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
        .memoryTypeIndex = 0
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
    VkCommandPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = 0
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

    
    // **关键修改1：添加图像布局转换到 TRANSFER_SRC_OPTIMAL**
    VkImageMemoryBarrier barrier1 = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_GENERAL,  // 或者使用 gbuffer 中存储的当前布局
        .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    vkCmdPipelineBarrier(cmd_buf, 
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        0, 0, NULL, 0, NULL, 1, &barrier1);
    
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

    // **关键修改2：使用 TRANSFER_SRC_OPTIMAL 布局**
    vkCmdCopyImageToBuffer(cmd_buf, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, staging_buf, 1, &region);

    
    // **关键修改3：转换回原始布局**
    VkImageMemoryBarrier barrier2 = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
        .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_GENERAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    vkCmdPipelineBarrier(cmd_buf, 
        VK_PIPELINE_STAGE_TRANSFER_BIT, 
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        0, 0, NULL, 0, NULL, 1, &barrier2);
    
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

    // vkDeviceWaitIdle(device);
    
    // // 4. Cleanup
    vkFreeCommandBuffers(device, cmd_pool, 1, &cmd_buf);  // 显式释放命令缓冲区
    vkDestroyCommandPool(device, cmd_pool, NULL);
    vkFreeMemory(device, staging_mem, NULL);
    vkDestroyBuffer(device, staging_buf, NULL);
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

#include <IOSurface/IOSurface.h>
#include <OpenGL/CGLIOSurface.h>
#include <CoreFoundation/CoreFoundation.h>

static bool create_shared_image_and_gl_texture(VkDevice device, VkPhysicalDevice pd,
                                              uint32_t width, uint32_t height,
                                              VkImage *out_image,
                                              VkDeviceMemory *out_memory,
                                              GLuint *out_texture) {
    
    // if(!is_init) {
    //     g_gl_context = get_native_opengl_context(0);
    //     egl_makeCurrent(g_gl_context); 
    //     LOGI("[Interop] Obtained native OpenGL context: %p", g_gl_context);       
    // }
    // is_init = true;

    // 1. 创建 IOSurface
    CFMutableDictionaryRef properties = CFDictionaryCreateMutable(
        kCFAllocatorDefault, 0,
        &kCFTypeDictionaryKeyCallBacks,
        &kCFTypeDictionaryValueCallBacks);
    
    int w = (int)width, h = (int)height;
    CFNumberRef w_num = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &w);
    CFNumberRef h_num = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &h);
    int bytes_per_element = 4;
    CFNumberRef bpe_num = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &bytes_per_element);
    
    // BGRA 格式
    uint32_t pixel_format = 'BGRA';
    CFNumberRef fmt_num = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &pixel_format);
    
    CFDictionarySetValue(properties, kIOSurfaceWidth, w_num);
    CFDictionarySetValue(properties, kIOSurfaceHeight, h_num);
    CFDictionarySetValue(properties, kIOSurfaceBytesPerElement, bpe_num);
    CFDictionarySetValue(properties, kIOSurfacePixelFormat, fmt_num);
    
    IOSurfaceRef io_surface = IOSurfaceCreate(properties);
    
    CFRelease(properties);
    CFRelease(w_num);
    CFRelease(h_num);
    CFRelease(bpe_num);
    CFRelease(fmt_num);
    
    if (!io_surface) {
        LOGE("[Interop] Failed to create IOSurface");
        return false;
    }
    
    LOGI("[Interop] Created IOSurface: %p, size=%dx%d", io_surface, width, height);
    
    // 2. 创建 Vulkan image（使用 IOSurface 导入方式）
    // MoltenVK 的 IOSurface 导入方式需要在 vkCreateImage 阶段传入 VkImportMetalIOSurfaceInfoEXT
    
    typedef struct VkImportMetalIOSurfaceInfoEXT {
        VkStructureType sType;
        const void* pNext;
        IOSurfaceRef ioSurface;
    } VkImportMetalIOSurfaceInfoEXT;
    
    #ifndef VK_STRUCTURE_TYPE_IMPORT_METAL_IO_SURFACE_INFO_EXT
    #define VK_STRUCTURE_TYPE_IMPORT_METAL_IO_SURFACE_INFO_EXT ((VkStructureType)1000311009)
    #endif
    
    VkImportMetalIOSurfaceInfoEXT import_info = {
        .sType = VK_STRUCTURE_TYPE_IMPORT_METAL_IO_SURFACE_INFO_EXT,
        .pNext = NULL,
        .ioSurface = io_surface
    };
    
    VkImageCreateInfo img_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = &import_info,  // ✅ 正确的位置：将 IOSurface 导入信息放这里
        .imageType = VK_IMAGE_TYPE_2D,
        .format = VK_FORMAT_B8G8R8A8_UNORM,
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
    if (res != VK_SUCCESS) {
        LOGE("[Interop] vkCreateImage failed: %d", res);
        CFRelease(io_surface);
        return false;
    }
    
    // 3. 获取内存需求
    VkMemoryRequirements mem_reqs;
    vkGetImageMemoryRequirements(device, *out_image, &mem_reqs);
    
    LOGI("[Interop] Memory requirements: size=%llu, alignment=%llu, memoryTypeBits=0x%x",
         (unsigned long long)mem_reqs.size,
         (unsigned long long)mem_reqs.alignment,
         mem_reqs.memoryTypeBits);
    
    // 4. 普通分配（此时 IOSurface 已经在 vkCreateImage 时被绑定，不需要再导入）
    VkMemoryAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,  // ✅ 不再传 import_info
        .allocationSize = mem_reqs.size,
        .memoryTypeIndex = find_memory_type(pd, mem_reqs.memoryTypeBits,
                                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    };
    
    res = vkAllocateMemory(device, &alloc_info, NULL, out_memory);
    if (res != VK_SUCCESS) {
        LOGE("[Interop] vkAllocateMemory failed: %d", res);
        vkDestroyImage(device, *out_image, NULL);
        CFRelease(io_surface);
        return false;
    }
    
    res = vkBindImageMemory(device, *out_image, *out_memory, 0);
    if (res != VK_SUCCESS) {
        LOGE("[Interop] vkBindImageMemory failed: %d", res);
        vkFreeMemory(device, *out_memory, NULL);
        vkDestroyImage(device, *out_image, NULL);
        CFRelease(io_surface);
        return false;
    }
    
    LOGI("[Interop] Vulkan image and memory created successfully");
    
    // 5. 创建 OpenGL texture from IOSurface
    glGenTextures(1, out_texture);
    glBindTexture(GL_TEXTURE_RECTANGLE, *out_texture);
    
    CGLContextObj cgl_ctx = CGLGetCurrentContext();
    if (!cgl_ctx) {
        LOGE("[Interop] No current CGL context!");
        glDeleteTextures(1, out_texture);
        vkFreeMemory(device, *out_memory, NULL);
        vkDestroyImage(device, *out_image, NULL);
        CFRelease(io_surface);
        return false;
    }
    
    LOGI("[Interop] CGL context: %p", cgl_ctx);
    
    CGLError cgl_err = CGLTexImageIOSurface2D(
        cgl_ctx, 
        GL_TEXTURE_RECTANGLE,
        GL_RGBA8, 
        width, 
        height,
        GL_BGRA, 
        GL_UNSIGNED_INT_8_8_8_8_REV,
        io_surface, 
        0
    );
    
    if (cgl_err != kCGLNoError) {
        LOGE("[Interop] CGLTexImageIOSurface2D failed: %d", cgl_err);
        glDeleteTextures(1, out_texture);
        vkFreeMemory(device, *out_memory, NULL);
        vkDestroyImage(device, *out_image, NULL);
        CFRelease(io_surface);
        return false;
    }
    
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    GLenum gl_err = glGetError();
    if (gl_err != GL_NO_ERROR) {
        LOGE("[Interop] OpenGL error after texture setup: 0x%x", gl_err);
    }
    
    glBindTexture(GL_TEXTURE_RECTANGLE, 0);
    
    // 测试：往 IOSurface 写入测试数据
    IOSurfaceLock(io_surface, 0, NULL);
    void* base_addr = IOSurfaceGetBaseAddress(io_surface);
    size_t bytes_per_row = IOSurfaceGetBytesPerRow(io_surface);
    
    LOGI("[Interop] IOSurface base address: %p, bytes per row: %zu", base_addr, bytes_per_row);
    
    // 写入红色测试图案到中心区域
    if (base_addr) {
        for (int y = height/2 - 10; y < height/2 + 10; y++) {
            uint8_t* row = (uint8_t*)base_addr + y * bytes_per_row;
            for (int x = width/2 - 10; x < width/2 + 10; x++) {
                row[x*4 + 0] = 255;  // B
                row[x*4 + 1] = 0;    // G
                row[x*4 + 2] = 253;  // R
                row[x*4 + 3] = 255;  // A
            }
        }
        LOGI("[Interop] Wrote test pattern (magenta square) to IOSurface center");
    }
    
    IOSurfaceUnlock(io_surface, 0, NULL);
    
    // 强制 GL 同步
    glFlush();
    
    LOGI("[Interop] Created shared IOSurface and GL texture %u (%dx%d)", 
         *out_texture, width, height);
    return true;
}
// 写入host内存到Vulkan image

Hardware_Buffer *create_gbuffer_from_vulkan(int width, int height, uint64_t gbuffer_id, 
                                          void *vk_image, void *vk_device_memory, 
                                          void *vk_device, void *vk_format, uint64_t vk_buffer_handle) {
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
    gbuffer->vk_buffer_handle = vk_buffer_handle;

    // 尝试创建共享image和GL texture
    VkPhysicalDevice pd = get_device_pd((uint64_t)(uintptr_t)vk_device);
    VkImage shared_image = VK_NULL_HANDLE;
    VkDeviceMemory shared_memory = VK_NULL_HANDLE;
    GLuint shared_texture = 0;

    if(vk_image == NULL) {
        LOGI("not swapchain image, skip interop");
        return gbuffer;
    }

    bool interop_success = false;
    
    interop_success = create_shared_image_and_gl_texture(
        (VkDevice)vk_device, pd, width, height,
        &shared_image, &shared_memory, &shared_texture);
    
    if (interop_success) {
        gbuffer->vk_shared_image = shared_image;
        gbuffer->vk_shared_memory = shared_memory;
        gbuffer->needs_copy = false;
        
        // macOS: shared_texture 是 GL_TEXTURE_RECTANGLE，需要转换
        gbuffer->intermediate_texture = shared_texture;
        
        // 创建最终的 GL_TEXTURE_2D
        glGenTextures(1, &gbuffer->data_texture);
        glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, 
                     GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        LOGI("[vulkan_surface] Created ZERO-COPY gbuffer: intermediate_texture=%u (RECTANGLE), data_texture=%u (2D)", 
             gbuffer->intermediate_texture, gbuffer->data_texture);
        
    } else {
        // Fallback 到 CPU 拷贝
        glGenTextures(1, &gbuffer->data_texture);
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