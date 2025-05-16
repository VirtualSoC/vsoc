#include "hw/express-gpu/express_vk_handle_mapping.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "hw/express-gpu/uthash.h"

typedef struct {
    ExpressVkObjectType type; // Vulkan 对象类型
    uint64_t         guest_id; // 来自 Guest 的虚拟句柄（64 位稀疏）
    uint64_t         host_id;  // 真正的 Host 句柄
    UT_hash_handle   hh;       // uthash 必需的句柄
} ExpressObjectEntry;

static ExpressObjectEntry *g_map = NULL; // 哈希表的“头指针”，初始为 NULL

// 插入／更新一条映射
int insert_mapping(ExpressVkObjectType type, uint64_t guest_id, uint64_t host_id) {
    ExpressObjectEntry *e;
    // 先尝试查找已有条目
    HASH_FIND(hh, g_map, &guest_id, sizeof(guest_id), e);
    if (e) {
        // 如果已存在，只更新 host_id
        e->host_id = host_id;
    } else {
        // 否则 new 一个新条目并插入
        e = malloc(sizeof(*e));
        if (!e) return 0;
        e->type     = type;
        e->guest_id = guest_id;
        e->host_id  = host_id;
        HASH_ADD(hh, g_map, guest_id, sizeof(guest_id), e);
    }
    return 1;
}

// 查找映射，失败返回 UINT64_MAX
uint64_t lookup_mapping(ExpressVkObjectType type, uint64_t guest_id) {
    ExpressObjectEntry *e;
    HASH_FIND(hh, g_map, &guest_id, sizeof(guest_id), e);
    if (e && e->type == type) {
        return e->host_id;
    }
    return UINT64_MAX;
}

// 清空所有映射，用于销毁时释放内存
void clear_mappings(void) {
    ExpressObjectEntry *current, *tmp;
    HASH_ITER(hh, g_map, current, tmp) {
        HASH_DEL(g_map, current);
        free(current);
    }
}

// map_handle_<Type> 实现：目前直接原样返回，可自行改写为真正的映射逻辑
VkInstance map_handle_VkInstance(VkInstance boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkPhysicalDevice map_handle_VkPhysicalDevice(VkPhysicalDevice boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkDevice map_handle_VkDevice(VkDevice boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkQueue map_handle_VkQueue(VkQueue boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkCommandBuffer map_handle_VkCommandBuffer(VkCommandBuffer boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkBuffer map_handle_VkBuffer(VkBuffer boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkBufferView map_handle_VkBufferView(VkBufferView boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkImage map_handle_VkImage(VkImage boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkImageView map_handle_VkImageView(VkImageView boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkShaderModule map_handle_VkShaderModule(VkShaderModule boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkDescriptorPool map_handle_VkDescriptorPool(VkDescriptorPool boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkDescriptorSetLayout map_handle_VkDescriptorSetLayout(VkDescriptorSetLayout boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkDescriptorSet map_handle_VkDescriptorSet(VkDescriptorSet boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkSampler map_handle_VkSampler(VkSampler boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkPipeline map_handle_VkPipeline(VkPipeline boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkPipelineCache map_handle_VkPipelineCache(VkPipelineCache boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkPipelineLayout map_handle_VkPipelineLayout(VkPipelineLayout boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkRenderPass map_handle_VkRenderPass(VkRenderPass boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkFramebuffer map_handle_VkFramebuffer(VkFramebuffer boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkCommandPool map_handle_VkCommandPool(VkCommandPool boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkFence map_handle_VkFence(VkFence boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkSemaphore map_handle_VkSemaphore(VkSemaphore boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkEvent map_handle_VkEvent(VkEvent boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkQueryPool map_handle_VkQueryPool(VkQueryPool boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkSamplerYcbcrConversion map_handle_VkSamplerYcbcrConversion(VkSamplerYcbcrConversion boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkDescriptorUpdateTemplate map_handle_VkDescriptorUpdateTemplate(VkDescriptorUpdateTemplate boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkSurfaceKHR map_handle_VkSurfaceKHR(VkSurfaceKHR boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkSwapchainKHR map_handle_VkSwapchainKHR(VkSwapchainKHR boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkDisplayKHR map_handle_VkDisplayKHR(VkDisplayKHR boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkDisplayModeKHR map_handle_VkDisplayModeKHR(VkDisplayModeKHR boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkValidationCacheEXT map_handle_VkValidationCacheEXT(VkValidationCacheEXT boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkDebugReportCallbackEXT map_handle_VkDebugReportCallbackEXT(VkDebugReportCallbackEXT boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkDebugUtilsMessengerEXT map_handle_VkDebugUtilsMessengerEXT(VkDebugUtilsMessengerEXT boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkAccelerationStructureNV map_handle_VkAccelerationStructureNV(VkAccelerationStructureNV boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkIndirectCommandsLayoutNV map_handle_VkIndirectCommandsLayoutNV(VkIndirectCommandsLayoutNV boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkAccelerationStructureKHR map_handle_VkAccelerationStructureKHR(VkAccelerationStructureKHR boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkCuModuleNVX map_handle_VkCuModuleNVX(VkCuModuleNVX boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkCuFunctionNVX map_handle_VkCuFunctionNVX(VkCuFunctionNVX boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkMicromapEXT map_handle_VkMicromapEXT(VkMicromapEXT boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

VkDeviceMemory map_handle_VkDeviceMemory(VkDeviceMemory boxed) {
    return boxed;  // TODO: 实际映射逻辑
}

