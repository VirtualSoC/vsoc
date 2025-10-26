#include "hw/express-gpu/express_vk_handle_mapping.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "hw/teleport-express/express_log.h"

#include "hw/express-gpu/uthash.h"

typedef struct {
    uint64_t gbuffer_id;      // key: gbuffer_id
    uint64_t device_memory;   // value: host VkDeviceMemory handle
    UT_hash_handle hh;
} GBufferMemoryEntry;

static GBufferMemoryEntry* g_gbuffer_memory_map = NULL;

void insert_gbuffer_memory_mapping(uint64_t gbuffer_id, uint64_t device_memory) {
    GBufferMemoryEntry* e;
    HASH_FIND(hh, g_gbuffer_memory_map, &gbuffer_id, sizeof(gbuffer_id), e);
    if (e) {
        e->device_memory = device_memory;
        LOGI("Updated gbuffer_memory mapping: gbuffer_id=%llx -> memory=%llx",
             (unsigned long long)gbuffer_id, (unsigned long long)device_memory);
    } else {
        e = malloc(sizeof(*e));
        if (!e) {
            LOGE("Failed to allocate gbuffer_memory mapping entry");
            return;
        }
        e->gbuffer_id = gbuffer_id;
        e->device_memory = device_memory;
        HASH_ADD(hh, g_gbuffer_memory_map, gbuffer_id, sizeof(gbuffer_id), e);
        LOGI("Inserted gbuffer_memory mapping: gbuffer_id=%llx -> memory=%llx",
             (unsigned long long)gbuffer_id, (unsigned long long)device_memory);
    }
}

uint64_t lookup_gbuffer_memory_mapping(uint64_t gbuffer_id) {
    GBufferMemoryEntry* e;
    HASH_FIND(hh, g_gbuffer_memory_map, &gbuffer_id, sizeof(gbuffer_id), e);
    if (e) {
        return e->device_memory;
    }
    return 0;
}

uint64_t lookup_memory_gbuffer_mapping(uint64_t device_memory) {
    GBufferMemoryEntry* current;
    for (current = g_gbuffer_memory_map; current != NULL; current = current->hh.next) {
        if (current->device_memory == device_memory) {
            return current->gbuffer_id;
        }
    }
    return 0;
}

void remove_gbuffer_memory_mapping(uint64_t gbuffer_id) {
    GBufferMemoryEntry* e;
    HASH_FIND(hh, g_gbuffer_memory_map, &gbuffer_id, sizeof(gbuffer_id), e);
    if (e) {
        HASH_DEL(g_gbuffer_memory_map, e);
        free(e);
        LOGI("Removed gbuffer_memory mapping: gbuffer_id=%llx", 
             (unsigned long long)gbuffer_id);
    }
}

void clear_gbuffer_memory_mappings(void) {
    GBufferMemoryEntry* current;
    GBufferMemoryEntry* tmp;
    HASH_ITER(hh, g_gbuffer_memory_map, current, tmp) {
        HASH_DEL(g_gbuffer_memory_map, current);
        free(current);
    }
    g_gbuffer_memory_map = NULL;
}


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
    LOGD("lookup mapping Type: %d, Guest ID: %llx", type, (long long)guest_id);
    ExpressObjectEntry *e;
    HASH_FIND(hh, g_map, &guest_id, sizeof(guest_id), e);
    if (e && e->type == type) {
        return e->host_id;
    }
    LOGE("lookup mapping failed! Type: %d, Guest ID: %llx", type, (long long)guest_id);
    return 0;
}

void clear_mappings(void) {
    ExpressObjectEntry *current, *tmp;
    HASH_ITER(hh, g_map, current, tmp) {
        HASH_DEL(g_map, current);
        free(current);
    }
}

int remove_mapping(ExpressVkObjectType type, uint64_t guest_id) {
    ExpressObjectEntry *e;
    HASH_FIND(hh, g_map, &guest_id, sizeof(guest_id), e);
    if (e && e->type == type) {
        HASH_DEL(g_map, e);
        free(e);
        return 1;
    }
    return 0;
}

// maps host VkDeviceMemory handle → host pointer
typedef struct MemoryMapEntry {
    uint64_t          host_mem;   // key is now host-side VkDeviceMemory handle
    void*             host_ptr;
    UT_hash_handle    hh;
} MemoryMapEntry;

static MemoryMapEntry* g_memory_map = NULL;

void set_memory_map(uint64_t host_mem, void* mappedPtr) {
    MemoryMapEntry* e;
    HASH_FIND(hh, g_memory_map, &host_mem, sizeof(host_mem), e);
    if (e) {
        e->host_ptr = mappedPtr;
    } else {
        e = malloc(sizeof(*e));
        e->host_mem = host_mem;
        e->host_ptr = mappedPtr;
        HASH_ADD(hh, g_memory_map, host_mem, sizeof(host_mem), e);
    }
}

void* get_memory_map(uint64_t host_mem) {
    MemoryMapEntry* e;
    HASH_FIND(hh, g_memory_map, &host_mem, sizeof(host_mem), e);
    return e ? e->host_ptr : NULL;
}

typedef struct DevPDEntry {
    uint64_t           host_dev;  // key：host-side VkDevice (pointer value)
    VkPhysicalDevice   phys;      // value：真实的 VkPhysicalDevice
    UT_hash_handle     hh;
} DevPDEntry;

static DevPDEntry* g_devpd_map = NULL;

void set_device_pd(uint64_t host_dev, VkPhysicalDevice phys) {
    DevPDEntry* e;
    HASH_FIND(hh, g_devpd_map, &host_dev, sizeof(host_dev), e);
    if (e) {
        e->phys = phys;
    } else {
        e = malloc(sizeof(*e));
        e->host_dev = host_dev;
        e->phys     = phys;
        HASH_ADD(hh, g_devpd_map, host_dev, sizeof(host_dev), e);
    }
}

VkPhysicalDevice get_device_pd(uint64_t host_dev) {
    DevPDEntry* e;
    HASH_FIND(hh, g_devpd_map, &host_dev, sizeof(host_dev), e);
    return e ? e->phys : VK_NULL_HANDLE;
}

typedef struct ImageViewEntry {
    uint64_t           host_imageview;  // key：host-side VkImageView (pointer value)
    uint64_t           host_image;      // value：host-side VkImage (pointer value)
    UT_hash_handle     hh;
} ImageViewEntry;

static ImageViewEntry* g_imageview_map = NULL;

void set_imageview_to_image(uint64_t host_imageview, uint64_t host_image) {
    ImageViewEntry* e;
    HASH_FIND(hh, g_imageview_map, &host_imageview, sizeof(host_imageview), e);
    if (e) {
        e->host_image = host_image;
    } else {
        e = malloc(sizeof(*e));
        e->host_imageview = host_imageview;
        e->host_image = host_image;
        HASH_ADD(hh, g_imageview_map, host_imageview, sizeof(host_imageview), e);
    }
    LOGD("Mapped ImageView %llx → Image %llx", (long long)host_imageview, (long long)host_image);
}

VkImage getImageFromImageView(VkImageView imageView) {
    uint64_t host_imageview = (uint64_t)(uintptr_t)imageView;
    ImageViewEntry* e;
    HASH_FIND(hh, g_imageview_map, &host_imageview, sizeof(host_imageview), e);
    if (e) {
        return (VkImage)(uintptr_t)e->host_image;
    }
    LOGE("getImageFromImageView failed! ImageView: %llx", (long long)host_imageview);
    return VK_NULL_HANDLE;
}

// Device 到 Graphics Queue 的映射
typedef struct DeviceQueueEntry {
    uint64_t           host_device;     // key：host-side VkDevice (pointer value)
    uint64_t           graphics_queue;  // value：host-side VkQueue (pointer value)
    uint64_t           command_pool;    // value：host-side VkCommandPool (pointer value)
    UT_hash_handle     hh;
} DeviceQueueEntry;

static DeviceQueueEntry* g_device_queue_map = NULL;

void set_device_graphics_queue(uint64_t host_device, uint64_t graphics_queue) {
    DeviceQueueEntry* e;
    HASH_FIND(hh, g_device_queue_map, &host_device, sizeof(host_device), e);
    if (e) {
        e->graphics_queue = graphics_queue;
    } else {
        e = malloc(sizeof(*e));
        e->host_device = host_device;
        e->graphics_queue = graphics_queue;
        e->command_pool = 0; // 初始化为0，稍后创建
        HASH_ADD(hh, g_device_queue_map, host_device, sizeof(host_device), e);
    }
    LOGI("Mapped Device %llx → Graphics Queue %llx", (long long)host_device, (long long)graphics_queue);
}

void set_device_command_pool(uint64_t host_device, uint64_t command_pool) {
    DeviceQueueEntry* e;
    HASH_FIND(hh, g_device_queue_map, &host_device, sizeof(host_device), e);
    if (e) {
        e->command_pool = command_pool;
    } else {
        e = malloc(sizeof(*e));
        e->host_device = host_device;
        e->graphics_queue = 0;
        e->command_pool = command_pool;
        HASH_ADD(hh, g_device_queue_map, host_device, sizeof(host_device), e);
    }
    LOGD("Mapped Device %llx → Command Pool %llx", (long long)host_device, (long long)command_pool);
}

VkQueue getGraphicsQueue(VkDevice device) {
    uint64_t host_device = (uint64_t)(uintptr_t)device;
    DeviceQueueEntry* e;
    HASH_FIND(hh, g_device_queue_map, &host_device, sizeof(host_device), e);
    if (e && e->graphics_queue) {
        return (VkQueue)(uintptr_t)e->graphics_queue;
    }
    LOGE("getGraphicsQueue failed! Device: %llx", (long long)host_device);
    return VK_NULL_HANDLE;
}

VkCommandPool getOrCreateCommandPool(VkDevice device) {
    uint64_t host_device = (uint64_t)(uintptr_t)device;
    DeviceQueueEntry* e;
    HASH_FIND(hh, g_device_queue_map, &host_device, sizeof(host_device), e);
    
    if (e && e->command_pool) {
        return (VkCommandPool)(uintptr_t)e->command_pool;
    }
    
    // 如果没有找到command pool，需要创建一个
    if (e && e->graphics_queue) {
        // 获取queue family index（这里假设是0，你可能需要从别的地方获取）
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = 0; // 你可能需要从设备创建时保存的信息获取正确的family index
        
        VkCommandPool commandPool;
        VkResult result = vkCreateCommandPool(device, &poolInfo, NULL, &commandPool);
        if (result == VK_SUCCESS) {
            set_device_command_pool(host_device, (uint64_t)(uintptr_t)commandPool);
            LOGI("Created command pool %llx for device %llx", 
                 (long long)(uintptr_t)commandPool, (long long)host_device);
            return commandPool;
        } else {
            LOGE("Failed to create command pool for device %llx, error: %d", 
                 (long long)host_device, result);
        }
    }
    
    LOGE("getOrCreateCommandPool failed! Device: %llx", (long long)host_device);
    return VK_NULL_HANDLE;
}

// 清理函数
void clear_imageview_mappings(void) {
    ImageViewEntry *current, *tmp;
    HASH_ITER(hh, g_imageview_map, current, tmp) {
        HASH_DEL(g_imageview_map, current);
        free(current);
    }
}

void clear_device_queue_mappings(void) {
    DeviceQueueEntry *current, *tmp;
    HASH_ITER(hh, g_device_queue_map, current, tmp) {
        HASH_DEL(g_device_queue_map, current);
        free(current);
    }
}


VkInstance map_handle_VkInstance(VkInstance boxed) {
    return (VkInstance)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_INSTANCE, (uint64_t)(uintptr_t)boxed);
}

VkPhysicalDevice map_handle_VkPhysicalDevice(VkPhysicalDevice boxed) {
    return (VkPhysicalDevice)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PHYSICAL_DEVICE, (uint64_t)(uintptr_t)boxed);
}

VkDevice map_handle_VkDevice(VkDevice boxed) {
    return (VkDevice)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE, (uint64_t)(uintptr_t)boxed);
}

VkQueue map_handle_VkQueue(VkQueue boxed) {
    return (VkQueue)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_QUEUE, (uint64_t)(uintptr_t)boxed);
}

VkCommandBuffer map_handle_VkCommandBuffer(VkCommandBuffer boxed) {
    return (VkCommandBuffer)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_BUFFER, (uint64_t)(uintptr_t)boxed);
}

VkBuffer map_handle_VkBuffer(VkBuffer boxed) {
    return (VkBuffer)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER, (uint64_t)(uintptr_t)boxed);
}

VkBufferView map_handle_VkBufferView(VkBufferView boxed) {
    return (VkBufferView)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_BUFFER_VIEW, (uint64_t)(uintptr_t)boxed);
}

VkImage map_handle_VkImage(VkImage boxed) {
    return (VkImage)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_IMAGE, (uint64_t)(uintptr_t)boxed);
}

VkImageView map_handle_VkImageView(VkImageView boxed) {
    return (VkImageView)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_IMAGE_VIEW, (uint64_t)(uintptr_t)boxed);
}

VkShaderModule map_handle_VkShaderModule(VkShaderModule boxed) {
    return (VkShaderModule)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_SHADER_MODULE, (uint64_t)(uintptr_t)boxed);
}

VkDescriptorPool map_handle_VkDescriptorPool(VkDescriptorPool boxed) {
    return (VkDescriptorPool)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DESCRIPTOR_POOL, (uint64_t)(uintptr_t)boxed);
}

VkDescriptorSetLayout map_handle_VkDescriptorSetLayout(VkDescriptorSetLayout boxed) {
    return (VkDescriptorSetLayout)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, (uint64_t)(uintptr_t)boxed);
}

VkDescriptorSet map_handle_VkDescriptorSet(VkDescriptorSet boxed) {
    return (VkDescriptorSet)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DESCRIPTOR_SET, (uint64_t)(uintptr_t)boxed);
}

VkSampler map_handle_VkSampler(VkSampler boxed) {
    return (VkSampler)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_SAMPLER, (uint64_t)(uintptr_t)boxed);
}

VkPipeline map_handle_VkPipeline(VkPipeline boxed) {
    return (VkPipeline)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PIPELINE, (uint64_t)(uintptr_t)boxed);
}

VkPipelineCache map_handle_VkPipelineCache(VkPipelineCache boxed) {
    return (VkPipelineCache)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PIPELINE_CACHE, (uint64_t)(uintptr_t)boxed);
}

VkPipelineLayout map_handle_VkPipelineLayout(VkPipelineLayout boxed) {
    return (VkPipelineLayout)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_PIPELINE_LAYOUT, (uint64_t)(uintptr_t)boxed);
}

VkRenderPass map_handle_VkRenderPass(VkRenderPass boxed) {
    return (VkRenderPass)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_RENDER_PASS, (uint64_t)(uintptr_t)boxed);
}

VkFramebuffer map_handle_VkFramebuffer(VkFramebuffer boxed) {
    return (VkFramebuffer)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_FRAMEBUFFER, (uint64_t)(uintptr_t)boxed);
}

VkCommandPool map_handle_VkCommandPool(VkCommandPool boxed) {
    return (VkCommandPool)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_COMMAND_POOL, (uint64_t)(uintptr_t)boxed);
}

VkFence map_handle_VkFence(VkFence boxed) {
    return (VkFence)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_FENCE, (uint64_t)(uintptr_t)boxed);
}

VkSemaphore map_handle_VkSemaphore(VkSemaphore boxed) {
    return (VkSemaphore)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_SEMAPHORE, (uint64_t)(uintptr_t)boxed);
}

VkEvent map_handle_VkEvent(VkEvent boxed) {
    return (VkEvent)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_EVENT, (uint64_t)(uintptr_t)boxed);
}

VkQueryPool map_handle_VkQueryPool(VkQueryPool boxed) {
    return (VkQueryPool)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_QUERY_POOL, (uint64_t)(uintptr_t)boxed);
}

VkSamplerYcbcrConversion map_handle_VkSamplerYcbcrConversion(VkSamplerYcbcrConversion boxed) {
    return (VkSamplerYcbcrConversion)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION, (uint64_t)(uintptr_t)boxed);
}

VkDescriptorUpdateTemplate map_handle_VkDescriptorUpdateTemplate(VkDescriptorUpdateTemplate boxed) {
    return (VkDescriptorUpdateTemplate)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE, (uint64_t)(uintptr_t)boxed);
}

VkSurfaceKHR map_handle_VkSurfaceKHR(VkSurfaceKHR boxed) {
    return (VkSurfaceKHR)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_SURFACE_KHR, (uint64_t)(uintptr_t)boxed);
}

VkSwapchainKHR map_handle_VkSwapchainKHR(VkSwapchainKHR boxed) {
    return (VkSwapchainKHR)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_SWAPCHAIN_KHR, (uint64_t)(uintptr_t)boxed);
}

VkDisplayKHR map_handle_VkDisplayKHR(VkDisplayKHR boxed) {
    return (VkDisplayKHR)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DISPLAY_KHR, (uint64_t)(uintptr_t)boxed);
}

VkDisplayModeKHR map_handle_VkDisplayModeKHR(VkDisplayModeKHR boxed) {
    return (VkDisplayModeKHR)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DISPLAY_MODE_KHR, (uint64_t)(uintptr_t)boxed);
}

VkValidationCacheEXT map_handle_VkValidationCacheEXT(VkValidationCacheEXT boxed) {
    return (VkValidationCacheEXT)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_VALIDATION_CACHE_EXT, (uint64_t)(uintptr_t)boxed);
}

VkDebugReportCallbackEXT map_handle_VkDebugReportCallbackEXT(VkDebugReportCallbackEXT boxed) {
    return (VkDebugReportCallbackEXT)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT, (uint64_t)(uintptr_t)boxed);
}

VkDebugUtilsMessengerEXT map_handle_VkDebugUtilsMessengerEXT(VkDebugUtilsMessengerEXT boxed) {
    return (VkDebugUtilsMessengerEXT)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT, (uint64_t)(uintptr_t)boxed);
}

VkAccelerationStructureNV map_handle_VkAccelerationStructureNV(VkAccelerationStructureNV boxed) {
    return (VkAccelerationStructureNV)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_NV, (uint64_t)(uintptr_t)boxed);
}

VkIndirectCommandsLayoutNV map_handle_VkIndirectCommandsLayoutNV(VkIndirectCommandsLayoutNV boxed) {
    return (VkIndirectCommandsLayoutNV)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NV, (uint64_t)(uintptr_t)boxed);
}

VkAccelerationStructureKHR map_handle_VkAccelerationStructureKHR(VkAccelerationStructureKHR boxed) {
    return (VkAccelerationStructureKHR)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR, (uint64_t)(uintptr_t)boxed);
}

VkCuModuleNVX map_handle_VkCuModuleNVX(VkCuModuleNVX boxed) {
    return (VkCuModuleNVX)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_CU_MODULE_NVX, (uint64_t)(uintptr_t)boxed);
}

VkCuFunctionNVX map_handle_VkCuFunctionNVX(VkCuFunctionNVX boxed) {
    return (VkCuFunctionNVX)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_CU_FUNCTION_NVX, (uint64_t)(uintptr_t)boxed);
}

VkMicromapEXT map_handle_VkMicromapEXT(VkMicromapEXT boxed) {
    return (VkMicromapEXT)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_MICROMAP_EXT, (uint64_t)(uintptr_t)boxed);
}

VkDeviceMemory map_handle_VkDeviceMemory(VkDeviceMemory boxed) {
    return (VkDeviceMemory)lookup_mapping(EXPRESS_VK_OBJECT_TYPE_DEVICE_MEMORY, (uint64_t)(uintptr_t)boxed);
}

