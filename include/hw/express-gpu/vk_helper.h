#ifndef VULKAN_HELPER_H
#define VULKAN_HELPER_H

#include <vulkan/vulkan.h>
#include <stdbool.h>

bool has_device_extension(const VkExtensionProperties* props, uint32_t propCount, const char* name);

#endif // VULKAN_HELPER_H
