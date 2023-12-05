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


#define FUNID_Terminate_Gbuffer (DEVICE_FUN_ID(EXPRESS_MEM_DEVICE_ID, SYNC_FUN_ID(1)))
#define FUNID_Alloc_Gbuffer (DEVICE_FUN_ID(EXPRESS_MEM_DEVICE_ID, HOST_SYNC_FUN_ID(2)))
#define FUNID_Gbuffer_Host_To_Guest (DEVICE_FUN_ID(EXPRESS_MEM_DEVICE_ID, SYNC_FUN_ID(3)))
#define FUNID_Gbuffer_Guest_To_Host (DEVICE_FUN_ID(EXPRESS_MEM_DEVICE_ID, 4))
#define FUNID_Mem_Signal_Sync (DEVICE_FUN_ID(EXPRESS_MEM_DEVICE_ID, 5))
#define FUNID_Mem_Wait_Sync (DEVICE_FUN_ID(EXPRESS_MEM_DEVICE_ID, 6))

#define PARA_NUM_Terminate_Gbuffer 1
#define PARA_NUM_Commit_Composer_Layer 1
#define PARA_NUM_Gbuffer_Host_To_Guest 1
#define PARA_NUM_Gbuffer_Guest_To_Host 1
#define PARA_NUM_Alloc_Gbuffer 2

void gbuffer_data_guest_to_host(Gralloc_Gbuffer_Info info);
void gbuffer_data_host_to_guest(Gralloc_Gbuffer_Info info);
void alloc_gbuffer_with_gralloc(Gralloc_Gbuffer_Info info, Guest_Mem *mem_data);
Graphic_Buffer *create_gbuffer_from_gralloc_info(Gralloc_Gbuffer_Info info, uint64_t gbuffer_id);

#endif