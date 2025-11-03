#ifndef VULKAN_SURFACE_H
#define VULKAN_SURFACE_H
#include <stdint.h>
#include <vulkan/vulkan.h>
#include "hw/express-gpu/egl_surface.h"
#include <stdbool.h>

#ifndef __APPLE__
#include <vulkan/vulkan_win32.h>
#endif

void vulkan_surface_create_swapchain(VkDevice device, VkSurfaceKHR surface, VkSwapchainKHR guest_swapchain, uint32_t minImageCount, uint32_t imageFormat, uint32_t width, uint32_t height, uint32_t presentMode);
void vulkan_surface_register_swapchain_images(VkDevice device, VkSwapchainKHR swapchain, uint64_t *guestImages, uint32_t count);
void vulkan_surface_present_images(VkQueue queue, VkPresentInfoKHR *presentInfo, uint64_t* buffer_ids);

bool vulkan_image_read_pixels(Hardware_Buffer *gbuffer, void *dst, size_t size);
bool vulkan_image_write_pixels(Hardware_Buffer *gbuffer, void *src, size_t size);

Hardware_Buffer *create_gbuffer_from_vulkan(int width, int height, uint64_t gbuffer_id, 
                                          void *vk_image, void *vk_device_memory, 
                                          void *vk_device, void *vk_format, uint64_t vk_buffer_handle);

#ifndef __APPLE__                                          
extern PFN_vkGetMemoryWin32HandleKHR pfn_vkGetMemoryWin32HandleKHR;

// 初始化函数声明
void init_interop_once(VkDevice device);
#endif

#endif // VULKAN_SURFACE_H 