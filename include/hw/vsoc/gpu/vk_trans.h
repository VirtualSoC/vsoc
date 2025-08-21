#include <vulkan/vulkan.h>

#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/gpu/express_gpu.h"


#define FUNID_vkCreateInstance 1000
#define FUNID_vkDestroyInstance 1001
#define FUNID_vkEnumeratePhysicalDevices 1002
#define FUNID_vkGetPhysicalDeviceFeatures 1003
#define FUNID_vkGetPhysicalDeviceFormatProperties 1004
#define FUNID_vkGetPhysicalDeviceImageFormatProperties 1005
#define FUNID_vkGetPhysicalDeviceProperties 1006
#define FUNID_vkGetPhysicalDeviceQueueFamilyProperties 1007
#define FUNID_vkGetPhysicalDeviceMemoryProperties 1008
#define FUNID_vkGetInstanceProcAddr 1009
#define FUNID_vkGetDeviceProcAddr 1010

void vk_decode_invoke(Render_Thread_Context *context, Teleport_Express_Call *call);