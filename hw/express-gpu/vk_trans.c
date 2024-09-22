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

    switch (call->id)
    {

    case FUNID_vkCreateInstance:

    {
        LOGD("get call vkCreateDevice!");
        const VkInstanceCreateInfo* pCreateInfo;
        const VkAllocationCallbacks* pAllocator;
    
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        LOGI("get vk param number %d", para_num);

        int need_free = 0;
        char *_ptr;
        _ptr = call_para_to_ptr(all_para[0], &need_free);
        VkInstanceCreateInfo* local_pCreateInfo = _ptr;
        LOGI("got vkCreateinfo with %lld %d %s %d %s",(long long)local_pCreateInfo->sType, local_pCreateInfo->enabledLayerCount, local_pCreateInfo->ppEnabledLayerNames, local_pCreateInfo->enabledExtensionCount, local_pCreateInfo->ppEnabledExtensionNames);


    }
    break;
    }

}
