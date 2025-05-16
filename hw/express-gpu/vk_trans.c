/**
 * @file egl_trans.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief
 * @version 0.1
 * @date 2020-11-25
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "hw/express-gpu/vk_trans.h"
#include "hw/express-gpu/express_gpu.h"

#include "hw/express-mem/express_sync.h"
#include "hw/express-gpu/express_vk_decode_from_stream.h"
#include "hw/express-gpu/express_vk_handle_mapping.h"

void vk_decode_invoke(Render_Thread_Context *context, Teleport_Express_Call *call)

{
    Render_Thread_Context *egl_context = (Render_Thread_Context *)context;

    if (unlikely(egl_context == NULL))
    {
        call->callback(call, 0);
        return;
    }

    Call_Para all_para[MAX_PARA_NUM];

    unsigned char ret_local_buf[1024 * 4];

    unsigned char *no_ptr_buf = NULL;
    
    uint64_t fun_id = GET_FUN_ID(call->id);
    LOGI("get vk call with id %lld", fun_id);

    switch (fun_id)
    {

    case FUNID_vkCreateInstance:

    {
        LOGI("get call FUNID_vkCreateInstance!");

        const VkInstanceCreateInfo* pCreateInfo = malloc(sizeof(VkInstanceCreateInfo));
        const VkAllocationCallbacks* pAllocator = NULL; //ztodo:暂时全部用null
        VkInstance pInstance;
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("get vk param number %d instance is %lld %lld", para_num, pInstance, &pInstance);

        int need_free = 0;
        char *stream_ptr;
        stream_ptr = call_para_to_ptr(all_para[0], &need_free);
        uint8_t ** stream_ptr_ptr = (uint8_t **)&stream_ptr;

        // VkInstanceCreateInfo* pCreateInfo = (VkInstanceCreateInfo*)_ptr;
        decode_from_stream_VkInstanceCreateInfo(VK_STRUCTURE_TYPE_MAX_ENUM, (VkInstanceCreateInfo*)(pCreateInfo), stream_ptr_ptr); 

        LOGI("got vkCreateinfo with %lld %d %s %d %s",(long long)pCreateInfo->sType, pCreateInfo->enabledLayerCount, pCreateInfo->ppEnabledLayerNames, pCreateInfo->enabledExtensionCount, pCreateInfo->ppEnabledExtensionNames);
        LOGI("application create info is %lld %d",(long long)pCreateInfo->pApplicationInfo, pCreateInfo->pApplicationInfo->sType);

        VkAllocationCallbacks* guest_allocator = (VkAllocationCallbacks*)(**stream_ptr_ptr);
        *stream_ptr_ptr += 8;

        if(guest_allocator) {
            decode_from_stream_VkAllocationCallbacks(VK_STRUCTURE_TYPE_MAX_ENUM, pAllocator, stream_ptr_ptr);
        }

        uint64_t guest_instance = *(uint64_t*)(*stream_ptr_ptr);
        *stream_ptr_ptr += sizeof(uint64_t);

        VkResult result = vkCreateInstance(pCreateInfo, pAllocator, &pInstance);

        if (result == VK_SUCCESS) {
            LOGI("got result %d instance %lld %lld size %d guest %lld", result, pInstance, &pInstance, sizeof(VkInstance), guest_instance);
            insert_mapping(EXPRESS_VK_OBJECT_TYPE_INSTANCE, guest_instance, (uint64_t)(uintptr_t)pInstance);
            LOGI("map result is %lld", lookup_mapping(EXPRESS_VK_OBJECT_TYPE_INSTANCE, guest_instance));
        }
        write_to_guest_mem(all_para[1].data, &result, 0, sizeof(VkResult));
    }
    break;
    
    }
    call->callback(call, 1);
}
