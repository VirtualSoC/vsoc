/**
 * @file express_sync.c
 * @author gaodi (gaodi.sec@qq.com)
 * @author Jiaxing Qiu (jx.qiu@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-6-10
 *
 * @copyright Copyright (c) 2022
 *
 */

// #define STD_DEBUG_LOG
#include "hw/vsoc/express_log.h"
#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/express_event.h"
#include "hw/vsoc/mem/express_sync.h"
#include "hw/vsoc/gpu/express_gpu_main_window.h"
#include "hw/vsoc/gpu/express_gpu_snapshot.h"

#define MAX_SYNC_NUM 512

typedef struct Sync_Flag_Data {
    volatile uint32_t host_event_cnt; // only set by host, used for guest polling
    volatile uint32_t guest_waits; // only set by guest, used by host to decide irq strategy
	uint32_t sync_status_id[MAX_SYNC_NUM]; // bitmap, to save DMA space
} __attribute__((packed, aligned(4))) Sync_Flag_Data;

GLsync gpu_sync_id[MAX_SYNC_NUM * 32];

typedef struct Sync_Context
{
    Device_Context device_context;
    Guest_Mem *guest_buffer;
    Sync_Flag_Data *sync_data;
    bool need_sync;
} Sync_Context;

static Sync_Context static_sync_context;

#ifdef _WIN32
HANDLE sync_event = NULL;
#else
void *sync_event = NULL;
#endif

int sync_wait_cnt = 0;

#define SYNC_FLAG_SIGNAL(flag_data, sync_id) (flag_data->sync_status_id[sync_id / 32] & (1L << (sync_id % 32)))

#define SET_SYNC_FLAG(flag_data, sync_id)                                                                                           \
    do                                                                                                                              \
    {                                                                                                                               \
        int temp_sync_status = flag_data->sync_status_id[sync_id / 32];                                                             \
        qatomic_cmpxchg(&(flag_data->sync_status_id[sync_id / 32]), temp_sync_status, (temp_sync_status | (1L << (sync_id % 32)))); \
    } while (!SYNC_FLAG_SIGNAL(flag_data, sync_id))

#ifdef ENABLE_SNAPSHOT

void save_sync_flag_data(QEMUFile *f, Sync_Flag_Data *data)
{
    // qemu_put_be32(f, data->host_event_cnt);
    for (int i = 0; i < MAX_SYNC_NUM; i++)
    {
        qemu_put_be32(f, data->sync_status_id[i]);
        // LOGI("saving sync flag data %d %d", data->sync_status_id[i], data->host_event_cnt);
    }
}

void save_sync_context(QEMUFile *f){
    
    qemu_put_be32(f, static_sync_context.need_sync);
    save_guest_mem(f, static_sync_context.guest_buffer);
    // save_sync_flag_data(f, static_sync_context.sync_data);
    // LOGI("saveing sync flag data guest waitting cnt %d", static_sync_context.sync_data->host_event_cnt);
    qemu_put_be32(f, static_sync_context.device_context.irq_enabled);

    if(static_sync_context.device_context.irq_call == NULL){
        qemu_put_be32(f, 0);
    } else {
        qemu_put_be32(f, 1);
        save_teleport_express_call(f, static_sync_context.device_context.irq_call);
    }
    

    LOGI("saving sync context of irq enabled %d", static_sync_context.device_context.irq_enabled);
}

void load_sync_flag_data(QEMUFile *f, Sync_Flag_Data *data)
{
    
    LOGI("loading sync flag data num");
    // memset(data, 0, sizeof(data));

    // int a = qemu_get_be32(f);
    // data->host_event_cnt = a;
    // // data->sync_status_id = (uint32_t*)g_malloc0(MAX_SYNC_NUM * sizeof(uint32_t));
    // for (int i = 0; i < MAX_SYNC_NUM; i++)
    // {
    //     data->sync_status_id[i] = qemu_get_be32(f);
    //     LOGI("loading sync flag data %d", data->sync_status_id[i]);
    // }
}

void load_sync_context(QEMUFile *f){
    
    static_sync_context.need_sync = qemu_get_be32(f);
    static_sync_context.guest_buffer = load_guest_mem(f, 0);
    // static_sync_context.sync_data = g_malloc0(sizeof(Sync_Flag_Data));
    int null_flag = 0;
    LOGI("before load sync flag data %lld scatter data %d %d", static_sync_context.sync_data, static_sync_context.guest_buffer->scatter_data->len, static_sync_context.guest_buffer->scatter_data->data);
    static_sync_context.sync_data = (Sync_Flag_Data *)get_direct_ptr(static_sync_context.guest_buffer, &null_flag);
    // LOGI("after load sync flag data %lld %d", static_sync_context.sync_data, static_sync_context.sync_data->host_event_cnt);
    load_sync_flag_data(f, static_sync_context.sync_data);

    // static_sync_context.sync_data = g_malloc0(sizeof(Sync_Flag_Data));

    static_sync_context.device_context.irq_enabled = qemu_get_be32(f);

    Express_Device_Info *device_info = get_express_device_info(EXPRESS_SYNC_DEVICE_ID);

    LOGI("device context and info %d %d", static_sync_context.device_context, device_info->device_id);
    static_sync_context.device_context.device_info = device_info;

    int has_call = qemu_get_be32(f);
    if(has_call) {
        static_sync_context.device_context.irq_call = load_teleport_express_call(f);
    }

    LOGI("loading sync context of irq enabled %d", static_sync_context.device_context.irq_enabled);

}

#endif

void signal_express_sync(int sync_id, bool need_gpu_sync)
{
    LOGD("set sync %d", sync_id);

    if (sync_id >= MAX_SYNC_NUM * 32 || sync_id < 0)
    {
        LOGE("invalid sync id %d!", sync_id);
        return;
    }

    if (static_sync_context.sync_data == NULL)
    {
        LOGE("error! signal_express_sync sync %d failed, sync_data is NULL!", sync_id);
        return;
    }

    if (need_gpu_sync)
    {
        GLsync gpu_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        gpu_sync = qatomic_xchg(&gpu_sync_id[sync_id], gpu_sync);
        if (gpu_sync != NULL)
        {
            glDeleteSync(gpu_sync);
        }

        glFlush();
    }

    SET_SYNC_FLAG(static_sync_context.sync_data, sync_id);

    if (qatomic_xchg(&sync_wait_cnt, 0) != 0)
    {
        set_event(sync_event);
    }

    // if guest is already waiting for something, send an IRQ to notify it
    // else set host_event_cnt and let guest poll for the event
    int ret = IRQ_NOT_READY;
    uint32_t guest_waits = qatomic_xchg(&static_sync_context.sync_data->guest_waits, 0);

    if (guest_waits > 0) {
        ret = g_ops.set_express_device_irq((Device_Context *)&static_sync_context, 0, sizeof(Sync_Flag_Data));
    }

    if (ret != IRQ_SET_OK) {
        static_sync_context.sync_data->host_event_cnt += 1;
    }
}

void wait_for_express_sync(int sync_id, bool need_gpu_sync)
{
    LOGD("wait for sync %d", sync_id);
    if (sync_id >= MAX_SYNC_NUM * 32 || sync_id < 0)
    {
        LOGE("invalid sync id %d!", sync_id);
        return;
    }
    int64_t start_time = g_get_monotonic_time();
    if (static_sync_context.sync_data != NULL)
    {
        // bool has_printed_backtrace = false;
        while (!SYNC_FLAG_SIGNAL(static_sync_context.sync_data, sync_id))
        {
            qatomic_add(&sync_wait_cnt, 1);
            int ret = wait_event(sync_event, 1);
            if (ret == -1)
            {
                LOGI("wait for sync failed!");
                break;
            }
            //特定的 sync_id 在 wait_for_express_sync 中等待了较长时间，但始终未收到对应的 signal 信号
            if (sync_wait_cnt != 0 && sync_wait_cnt % 1000 == 0) {
                // helps debugging deadlocks
                LOGI("still waiting for sync %d (gpu %d) after %d ms...", sync_id, need_gpu_sync, sync_wait_cnt);

                // if (!has_printed_backtrace) {
                //     has_printed_backtrace = true;
                //     backtrace();
                // }
            }
        }

        if (need_gpu_sync)
        {
            // a second wait_sync on the same id will skip gpu sync
            GLsync gpu_sync = qatomic_xchg(&gpu_sync_id[sync_id], NULL);
            if (gpu_sync != NULL)
            {
                glWaitSync(gpu_sync, 0, GL_TIMEOUT_IGNORED);
                glDeleteSync(gpu_sync);
            }
        }
    } else {
        LOGE("error! wait_for_express_sync %d failed!", sync_id);
    }
    LOGD("sync %d ok", sync_id);

    // int64_t end_time = g_get_monotonic_time();
    // if (end_time - start_time > 100 * 1000)
    // {
    //     LOGD("wait_for_express_sync %d spent too long time (%lldms)", sync_id, (end_time - start_time) / 1000);
    // }
}

static void sync_buffer_register(Guest_Mem *data, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, uint64_t user_id, Express_Device_Info *info)
{
    if (static_sync_context.guest_buffer != NULL)
    {
        free_duplicated_guest_mem(static_sync_context.guest_buffer);
    }
    
    static_sync_context.guest_buffer = data;

    int null_flag = 0;
    static_sync_context.sync_data = (Sync_Flag_Data *)get_direct_ptr(data, &null_flag);

    LOGI("sync register buffer %p size host %zu guest %d", static_sync_context.sync_data, sizeof(Sync_Flag_Data), data->all_len);

    if (data->all_len != sizeof(Sync_Flag_Data))
    {
        LOGE("error! guest/host sync buffer size not equal: %d != %zu", data->all_len, sizeof(Sync_Flag_Data));
    }

    if (static_sync_context.sync_data == NULL)
    {
        Scatter_Data *guest_data = data->scatter_data;
        LOGE("error! cannot get direct ptr guest_mem num %d first sg size %lld ptr %p", data->num, guest_data->iov_len, guest_data->iov_base);
    }
}

static Device_Context *get_sync_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    LOGD("going to get sync context");
    if (sync_event == NULL)
    {
        sync_event = create_event(0, 0);
    }

    return (Device_Context *)&static_sync_context;
}

static Express_Device_Info express_sync_info = {
    .enable_default = true,
    .name = "express-sync",
    .option_name = "sync",
    .driver_name = "express_sync",
    .device_id = EXPRESS_SYNC_DEVICE_ID,
    .device_type = INPUT_DEVICE_TYPE,

    .get_device_context = get_sync_context,
    .buffer_register = sync_buffer_register,

};

EXPRESS_DEVICE_INIT(express_sync, &express_sync_info)
