#ifndef HANDLE_MAPPING_H
#define HANDLE_MAPPING_H

#include <vulkan/vulkan.h>

typedef enum ExpressVkObjectType {
    EXPRESS_VK_OBJECT_TYPE_INSTANCE,
    EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE,
    EXPRESS_VK_OBJECT_TYPE_DEVICE,
    EXPRESS_VK_OBJECT_TYPE_QUEUE,
    EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER,
    EXPRESS_VK_OBJECT_TYPE_DEVICE_MEMORY,
    EXPRESS_VK_OBJECT_TYPE_BUFFER,
    EXPRESS_VK_OBJECT_TYPE_BUFFER_VIEW,
    EXPRESS_VK_OBJECT_TYPE_IMAGE,
    EXPRESS_VK_OBJECT_TYPE_IMAGE_VIEW,
    EXPRESS_VK_OBJECT_TYPE_SHADER_MODULE,
    EXPRESS_VK_OBJECT_TYPE_DESCRIPTOR_POOL,
    EXPRESS_VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT,
    EXPRESS_VK_OBJECT_TYPE_DESCRIPTOR_SET,
    EXPRESS_VK_OBJECT_TYPE_SAMPLER,
    EXPRESS_VK_OBJECT_TYPE_PIPELINE,
    EXPRESS_VK_OBJECT_TYPE_PIPELINE_CACHE,
    EXPRESS_VK_OBJECT_TYPE_PIPELINE_LAYOUT,
    EXPRESS_VK_OBJECT_TYPE_RENDER_PASS,
    EXPRESS_VK_OBJECT_TYPE_FRAMEBUFFER,
    EXPRESS_VK_OBJECT_TYPE_COMMAND_POOL,
    EXPRESS_VK_OBJECT_TYPE_FENCE,
    EXPRESS_VK_OBJECT_TYPE_SEMAPHORE,
    EXPRESS_VK_OBJECT_TYPE_EVENT,
    EXPRESS_VK_OBJECT_TYPE_QUERY_POOL,
    EXPRESS_VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION,
    EXPRESS_VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE,
    EXPRESS_VK_OBJECT_TYPE_SURFACE_KHR,
    EXPRESS_VK_OBJECT_TYPE_SWAPCHAIN_KHR,
    EXPRESS_VK_OBJECT_TYPE_DISPLAY_KHR,
    EXPRESS_VK_OBJECT_TYPE_DISPLAY_MODE_KHR,
    EXPRESS_VK_OBJECT_TYPE_PRIVATE_DATA_SLOT,
    EXPRESS_VK_OBJECT_TYPE_VALIDATION_CACHE_EXT,
    EXPRESS_VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT,
    EXPRESS_VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT,
    EXPRESS_VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_NV,
    EXPRESS_VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NV,
    EXPRESS_VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR,
    EXPRESS_VK_OBJECT_TYPE_CU_MODULE_NVX,
    EXPRESS_VK_OBJECT_TYPE_CU_FUNCTION_NVX,
    EXPRESS_VK_OBJECT_TYPE_MICROMAP_EXT,
    EXPRESS_VK_OBJECT_TYPE_SURFACE,
    EXPRESS_VK_OBJECT_TYPE_NATIVE_WINDOW,

    EXPRESS_VK_OBJECT_TYPE_MAX_ENUM
} ExpressVkObjectType;

int insert_mapping(ExpressVkObjectType type, uint64_t guest_id, uint64_t host_id);
uint64_t lookup_mapping(ExpressVkObjectType type, uint64_t guest_id);
void set_memory_map(uint64_t host_mem, void* mappedPtr);
void* get_memory_map(uint64_t host_mem);
int remove_mapping(ExpressVkObjectType type, uint64_t guest_id);

void set_device_pd(uint64_t host_dev, VkPhysicalDevice phys);
VkPhysicalDevice get_device_pd(uint64_t host_dev);

// 只提供 map_handle_<Type> 声明，用于 guest→host 句柄映射
VkInstance map_handle_VkInstance(VkInstance boxed);
VkPhysicalDevice map_handle_VkPhysicalDevice(VkPhysicalDevice boxed);
VkDevice map_handle_VkDevice(VkDevice boxed);
VkQueue map_handle_VkQueue(VkQueue boxed);
VkCommandBuffer map_handle_VkCommandBuffer(VkCommandBuffer boxed);
VkBuffer map_handle_VkBuffer(VkBuffer boxed);
VkBufferView map_handle_VkBufferView(VkBufferView boxed);
VkImage map_handle_VkImage(VkImage boxed);
VkImageView map_handle_VkImageView(VkImageView boxed);
VkShaderModule map_handle_VkShaderModule(VkShaderModule boxed);
VkDescriptorPool map_handle_VkDescriptorPool(VkDescriptorPool boxed);
VkDescriptorSetLayout map_handle_VkDescriptorSetLayout(VkDescriptorSetLayout boxed);
VkDescriptorSet map_handle_VkDescriptorSet(VkDescriptorSet boxed);
VkSampler map_handle_VkSampler(VkSampler boxed);
VkPipeline map_handle_VkPipeline(VkPipeline boxed);
VkPipelineCache map_handle_VkPipelineCache(VkPipelineCache boxed);
VkPipelineLayout map_handle_VkPipelineLayout(VkPipelineLayout boxed);
VkRenderPass map_handle_VkRenderPass(VkRenderPass boxed);
VkFramebuffer map_handle_VkFramebuffer(VkFramebuffer boxed);
VkCommandPool map_handle_VkCommandPool(VkCommandPool boxed);
VkFence map_handle_VkFence(VkFence boxed);
VkSemaphore map_handle_VkSemaphore(VkSemaphore boxed);
VkEvent map_handle_VkEvent(VkEvent boxed);
VkQueryPool map_handle_VkQueryPool(VkQueryPool boxed);
VkSamplerYcbcrConversion map_handle_VkSamplerYcbcrConversion(VkSamplerYcbcrConversion boxed);
VkDescriptorUpdateTemplate map_handle_VkDescriptorUpdateTemplate(VkDescriptorUpdateTemplate boxed);
VkSurfaceKHR map_handle_VkSurfaceKHR(VkSurfaceKHR boxed);
VkSwapchainKHR map_handle_VkSwapchainKHR(VkSwapchainKHR boxed);
VkDisplayKHR map_handle_VkDisplayKHR(VkDisplayKHR boxed);
VkDisplayModeKHR map_handle_VkDisplayModeKHR(VkDisplayModeKHR boxed);
VkValidationCacheEXT map_handle_VkValidationCacheEXT(VkValidationCacheEXT boxed);
VkDebugReportCallbackEXT map_handle_VkDebugReportCallbackEXT(VkDebugReportCallbackEXT boxed);
VkDebugUtilsMessengerEXT map_handle_VkDebugUtilsMessengerEXT(VkDebugUtilsMessengerEXT boxed);
VkAccelerationStructureNV map_handle_VkAccelerationStructureNV(VkAccelerationStructureNV boxed);
VkIndirectCommandsLayoutNV map_handle_VkIndirectCommandsLayoutNV(VkIndirectCommandsLayoutNV boxed);
VkAccelerationStructureKHR map_handle_VkAccelerationStructureKHR(VkAccelerationStructureKHR boxed);
VkCuModuleNVX map_handle_VkCuModuleNVX(VkCuModuleNVX boxed);
VkCuFunctionNVX map_handle_VkCuFunctionNVX(VkCuFunctionNVX boxed);
VkMicromapEXT map_handle_VkMicromapEXT(VkMicromapEXT boxed);
VkDeviceMemory map_handle_VkDeviceMemory(VkDeviceMemory boxed);


#endif // HANDLE_MAPPING_H
