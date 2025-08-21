#include "hw/teleport-express/express_platform.h"
#include "hw/teleport-express/express_device.h"
#include "hw/teleport-express/express_event.h"
#include "hw/teleport-express/express_subprocess.h"

#include "hw/express-gpu/express_gpu_main_window.h"

ExpressPlatformOps g_ops;

static bool should_stop = false;

void init_express_platform(ExpressPlatformOps ops) {
    g_ops = ops;
}

void init_express_device(Express_Device_Info *info) {
    express_device_init_common(info);
}

void deinit_express_device(void) {
    should_stop = true;
    express_gpu_shutdown_notify_callback();
    // deinit
}

bool platform_device_should_stop(void) {
    return should_stop;
}

// Duplicate a Guest_Mem (deep copy scatter list) for persistence beyond handler scope
Guest_Mem *duplicate_guest_mem(Guest_Mem *orig) {
    if (!orig) return NULL;
    Guest_Mem *cpy = g_malloc(sizeof(Guest_Mem));
    cpy->num = orig->num;
    cpy->all_len = orig->all_len;
    cpy->scatter_data = g_malloc(sizeof(Scatter_Data) * cpy->num);
    memcpy(cpy->scatter_data, orig->scatter_data, sizeof(Scatter_Data) * cpy->num);
    return cpy;
}

void free_duplicated_guest_mem(Guest_Mem *mem) {
    if (mem) {
        g_free(mem->scatter_data);
        g_free(mem);
    }
}

/**
 * @brief 获取直接的guest端指针，flag表示是否获取到了，返回guest端的指针，可能为NULL，因为当初传入的指针可能真的为NULL
 *
 * @param guest_mem
 * @param flag
 * @return void*
 */
void *get_direct_ptr(Guest_Mem *guest_mem, int *flag)
{
    if (likely(guest_mem->num == 1))
    {
        Scatter_Data *guest_data = guest_mem->scatter_data;
        *flag = 1;
        //这里也可能返回NULL，所以以flag来区分
        return guest_data->data;
    }
    *flag = 0;
    return NULL;
}

void *call_para_to_ptr(Call_Para para, int *need_free) {
    size_t ptr_len = 0;
    unsigned char *ptr = NULL;

    ptr_len = para.data_len;

    int null_flag = 0;
    ptr = get_direct_ptr(para.data, &null_flag);
    if (unlikely(ptr == NULL)) {
        if (ptr_len != 0 && null_flag == 0) {
            ptr = g_malloc(ptr_len);
            *need_free = 1;
            g_ops.read_from_guest_mem(para.data, ptr, 0, para.data_len);
        }
    }

    return ptr;
}

/**
 * @brief 创建一个thread_context，并根据这个context新建一个线程
 *
 * @param context 需要初始化的线程context
 */
Thread_Context *thread_context_create(uint64_t thread_id, uint64_t device_id, uint64_t len, Express_Device_Info *info)
{
    Thread_Context *context = g_malloc0(len);
    context->device_id = device_id;
    context->thread_id = thread_id;

    context->read_loc = 0;
    context->write_loc = 0;
    // context->atomic_event_lock = 0;
    context->init = 0;
    context->thread_run = 1;

    context->context_init = info->context_init;
    context->context_destroy = info->context_destroy;
    context->call_handler = info->call_handler;

//线程缓冲区事件初始化
    context->data_event = create_event(0, 0);

    char thread_name[32];
    snprintf(thread_name, sizeof(thread_name), "%s_handle_thread", info->name);

    qemu_thread_create(&context->this_thread, thread_name, handle_thread_run, context, QEMU_THREAD_JOINABLE);

    return context;
}
