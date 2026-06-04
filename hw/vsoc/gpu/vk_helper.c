#include "hw/vsoc/gpu/vk_helper.h"
#include <string.h>

bool has_device_extension(const VkExtensionProperties* props, uint32_t propCount, const char* name) {
    for (uint32_t i = 0; i < propCount; i++) {
        if (strcmp(props[i].extensionName, name) == 0) {
            return true;
        }
    }
    return false;
}
