#ifndef EXPRESS_DISPLAY_H
#define EXPRESS_DISPLAY_H

#include "hw/teleport-express/express_device_common.h"
#include "hw/teleport-express/teleport_express_call.h"

#define MAX_OUT_BUF_LEN 4096

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

typedef struct Display_Info
{
    int pixel_width;
    int pixel_height;
    int phy_width;
    int phy_height;
    uint64_t refresh_rate_bits;
} __attribute__((packed, aligned(4))) Display_Info;

typedef struct Display_Status
{
    int refresh_rate;
    int power_status;
    uint32_t backlight;
} __attribute__((packed, aligned(4))) Display_Status;


extern Display_Info express_display_info;

#define FUNID_Terminate (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, 0))

#define FUNID_Terminate_Gbuffer (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, SYNC_FUN_ID(1)))

#define FUNID_Alloc_Gbuffer (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, HOST_SYNC_FUN_ID(2)))

#define FUNID_Gbuffer_Download (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, SYNC_FUN_ID(3)))

#define FUNID_Gbuffer_Upload (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, 4))

#define FUNID_Sync_Gbuffer (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, SYNC_FUN_ID(5)))


#define FUNID_Show_Window (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, 10))

#define FUNID_Commit_Composer_Layer (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, 11))

#define FUNID_Get_Display_Mods (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, SYNC_FUN_ID(12)))

#define FUNID_Set_Display_Status (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, 13))

#define FUNID_Get_Display_Status (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, SYNC_FUN_ID(14)))


#define PARA_NUM_Terminate_Gbuffer 1

#define PARA_NUM_Commit_Composer_Layer 1

#define PARA_NUM_Gbuffer_Download 1

#define PARA_NUM_Gbuffer_Upload 1

#define PARA_NUM_Alloc_Gbuffer 2

#define PARA_NUM_Get_Display_Mods 1

#define PARA_NUM_Set_Display_Status 1
#define PARA_NUM_Get_Display_Status 1


extern int display_is_open;

void guest_upload_gbuffer_data(Gralloc_Gbuffer_Info info);
void guest_download_gbuffer_data(Gralloc_Gbuffer_Info info);
void alloc_gbuffer_with_gralloc(Gralloc_Gbuffer_Info info, Guest_Mem *mem_data);

#endif