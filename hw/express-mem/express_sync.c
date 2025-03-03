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

#include "hw/express-mem/express_sync.h"
#include "hw/express-gpu/express_gpu_main_window.h"
#include "hw/teleport-express/express_event.h"
#include "hw/express-gpu/express_gpu_snapshot.h"


typedef struct Sync_Flag_Data
{
    volatile uint32_t guest_waitting_cnt;
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


void save_sync_flag_data(QEMUFile *f, Sync_Flag_Data *data)
{
    qemu_put_be32(f, data->guest_waitting_cnt);
    for (int i = 0; i < MAX_SYNC_NUM; i++)
    {
        qemu_put_be32(f, data->sync_status_id[i]);
        LOGI("saving sync flag data %d %d", data->sync_status_id[i], data->guest_waitting_cnt);
    }
}

void save_sync_context(QEMUFile *f){
    
    qemu_put_be32(f, static_sync_context.need_sync);
    save_guest_mem(f, static_sync_context.guest_buffer);
    // save_sync_flag_data(f, static_sync_context.sync_data);
    LOGI("saveing sync flag data guest waitting cnt %d", static_sync_context.sync_data->guest_waitting_cnt);
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
    // data->guest_waitting_cnt = a;
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
    LOGI("after load sync flag data %lld %d", static_sync_context.sync_data, static_sync_context.sync_data->guest_waitting_cnt);
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


void signal_express_sync(int sync_id, bool need_gpu_sync)
{
    LOGD("set sync %d", sync_id);

    if (sync_id >= MAX_SYNC_NUM * 32 || sync_id < 0)
    {
        LOGE("invalid sync id %d!", sync_id);
        return;
    }
    if (static_sync_context.sync_data != NULL)
    {
        if (need_gpu_sync)
        {
            if (gpu_sync_id[sync_id] != NULL)
            {
                glDeleteSync(gpu_sync_id[sync_id]);
            }
            gpu_sync_id[sync_id] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
            glFlush();
        }
        SET_SYNC_FLAG(static_sync_context.sync_data, sync_id);

        if (qatomic_xchg(&sync_wait_cnt, 0) != 0)
        {
#ifdef _WIN32
            SetEvent(sync_event);
#else
            set_event(sync_event);
#endif
        }
        int old_waitting_cnt = 0;
        if ((old_waitting_cnt = qatomic_xchg(&static_sync_context.sync_data->guest_waitting_cnt, 0)) != 0)
        {
            // LOGD("going to set sync irq");
            while (set_express_device_irq((Device_Context *)&static_sync_context, old_waitting_cnt, sizeof(Sync_Context)) == IRQ_NOT_READY) {
                g_usleep(100);
            }
        }
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
    int64_t start_time = g_get_real_time();
    if (static_sync_context.sync_data != NULL)
    {
        while (!SYNC_FLAG_SIGNAL(static_sync_context.sync_data, sync_id))
        {
#ifdef _WIN32
            LOGD("wait success");
            qatomic_add(&sync_wait_cnt, 1);
            DWORD ret = WaitForSingleObject(sync_event, 1);
            if (ret == WAIT_FAILED)
            {
                LOGI("wait for sync failed! error code: %lld", (int64_t)GetLastError());
                break;
            }
#else
            qatomic_add(&sync_wait_cnt, 1);
            int ret = wait_event(sync_event, 1);
            if (ret == -1)
            {
                LOGI("wait for sync failed!");
                break;
            }
#endif
            if (sync_wait_cnt != 0 && sync_wait_cnt % 1000 == 0) {
                // helps debugging deadlocks
                LOGI("still waiting for sync %d after %d ms...", sync_id, sync_wait_cnt); //特定的 sync_id 在 wait_for_express_sync 中等待了较长时间，但始终未收到对应的 signal 信号
            }
        }

        if (need_gpu_sync)
        {
            if (gpu_sync_id[sync_id] != NULL)
            {
                glWaitSync(gpu_sync_id[sync_id], 0, GL_TIMEOUT_IGNORED);

                // a second wait_sync on the same id will skip gpu sync
                glDeleteSync(gpu_sync_id[sync_id]);
                gpu_sync_id[sync_id] = NULL;
            }
        }
    } else {
        LOGE("error! wait_for_express_sync sync %d failed!", sync_id);
    }
    LOGD("sync %d ok", sync_id);

    int64_t end_time = g_get_real_time();
    if (end_time - start_time > 50 * 1000)
    {
        LOGW("warning! wait_for_express_sync sync %d spend too long time (%lldms)", sync_id, (end_time - start_time) / 1000);
    }
}

static void sync_buffer_register(Guest_Mem *data, uint64_t thread_id, uint64_t process_id, uint64_t unique_id)
{
    if (static_sync_context.guest_buffer != NULL)
    {
        free_copied_guest_mem(static_sync_context.guest_buffer);
    }
    
    static_sync_context.guest_buffer = data;

    int null_flag = 0;
    static_sync_context.sync_data = (Sync_Flag_Data *)get_direct_ptr(data, &null_flag);

    // LOGI("sync register buffer %llu", (unsigned long long)static_sync_context.sync_data);

    if (null_flag != 0 && static_sync_context.sync_data == NULL)
    {
        Scatter_Data *guest_data = data->scatter_data;
        LOGE("error! sync_buffer_register cannot get direct_ptr mem num %d first mem size %lld", data->num, guest_data->len);
    }
}

static Device_Context *get_sync_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    LOGD("going to get sync context");
    if (sync_event == NULL)
    {
#ifdef _WIN32
        sync_event = CreateEvent(NULL, FALSE, FALSE, NULL);
#else
        sync_event = create_event(0,0);
#endif
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
