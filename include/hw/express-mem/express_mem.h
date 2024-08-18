#ifndef EXPRESS_MEM_H
#define EXPRESS_MEM_H

#include "hw/teleport-express/express_device_common.h"
#include "hw/teleport-express/teleport_express_call.h"
#include "hw/express-gpu/egl_surface.h"

typedef struct Gralloc_Gbuffer_Info
{
    uint64_t gbuffer_id;
    int width;
    int height;
    int format;
    int pixel_size;
    int size;
    int stride;
    int usage;
} __attribute__((packed, aligned(4))) Gralloc_Gbuffer_Info;

struct MemTransferTask;
typedef struct MemTransferTask MemTransferTask;
typedef void (*PreprocessCbType)(MemTransferTask *task, void *mapped_addr);
typedef void (*PostprocessCbType)(MemTransferTask *task, int retval);

struct MemTransferTask {
    // the destination of the transfer
    ExpressMemType dst_dev;

    // the source of the transfer
    ExpressMemType src_dev;

    // handle to destination data
    void *dst_data;

    // handle to source data
    void *src_data;

    // the length of the destination data
    int dst_len;

    // the length of the source data
    int src_len;

    // (optional) sync_id to signal after the transfer
    int sync_id;

    // (optional) user-provided preprocess function
    // will be called from the async thread after DMA mapping, 
    // but before the actual data transfer
    PreprocessCbType pre_cb;

    // (optional) user-provided postprocess function
    // will be called from the async thread when the transfer command is sent
    PostprocessCbType post_cb;

    // (optional) user-provided private data
    void *private_data;
};

#define FUNID_Terminate_Gbuffer (DEVICE_FUN_ID(EXPRESS_MEM_DEVICE_ID, SYNC_FUN_ID(1)))
#define FUNID_Alloc_Gbuffer (DEVICE_FUN_ID(EXPRESS_MEM_DEVICE_ID, HOST_SYNC_FUN_ID(2)))
#define FUNID_Gbuffer_Host_To_Guest (DEVICE_FUN_ID(EXPRESS_MEM_DEVICE_ID, SYNC_FUN_ID(3)))
#define FUNID_Gbuffer_Guest_To_Host (DEVICE_FUN_ID(EXPRESS_MEM_DEVICE_ID, 4))
#define FUNID_Mem_Signal_Sync (DEVICE_FUN_ID(EXPRESS_MEM_DEVICE_ID, 5))
#define FUNID_Mem_Wait_Sync (DEVICE_FUN_ID(EXPRESS_MEM_DEVICE_ID, 6))
#define FUNID_Update_Gbuffer_Location (DEVICE_FUN_ID(EXPRESS_MEM_DEVICE_ID, 7))

#define PARA_NUM_Terminate_Gbuffer 1
#define PARA_NUM_Alloc_Gbuffer 2
#define PARA_NUM_Gbuffer_Host_To_Guest 1
#define PARA_NUM_Gbuffer_Guest_To_Host 2
#define PARA_NUM_Mem_Signal_Sync 1
#define PARA_NUM_Mem_Wait_Sync 1
#define PARA_NUM_Update_Gbuffer_Location 1

const char *memtype_to_str(ExpressMemType loc);

void hg_update_bandwidth(ExpressMemType dst, ExpressMemType src, double new_bandwidth);
void update_gbuffer_virt_usage(Hardware_Buffer *gbuffer, int virt_dev, int write);
void update_gbuffer_phy_usage(Hardware_Buffer *gbuffer, ExpressMemType phy_dev, int write);
ExpressMemType mem_predict_prefetch(Hardware_Buffer *gbuffer, int virt_dev, ExpressMemType phy_dev, uint32_t *pred_block);

void mem_transfer_async(ExpressMemType dst_dev, ExpressMemType src_dev,
                        void *dst_data, void *src_data, int dst_len,
                        int src_len, int sync_id, PreprocessCbType pre_cb,
                        PostprocessCbType post_cb, void *private_data);
bool mem_transfer_is_busy(void);
void express_mem_worker(gpointer data, gpointer user_data);

void gbuffer_data_guest_to_host(Gralloc_Gbuffer_Info info, int sync_id);
void gbuffer_data_host_to_guest(Gralloc_Gbuffer_Info info);
void alloc_gbuffer_with_gralloc(Gralloc_Gbuffer_Info info, Guest_Mem *mem_data);
Hardware_Buffer *create_gbuffer_from_gralloc_info(Gralloc_Gbuffer_Info info, uint64_t gbuffer_id);

#endif