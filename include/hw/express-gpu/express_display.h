#ifndef EXPRESS_DISPLAY_H
#define EXPRESS_DISPLAY_H

#include "hw/teleport-express/express_device_common.h"
#include "hw/teleport-express/teleport_express_call.h"


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


#define FUNID_Terminate (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, 0))

#define FUNID_Set_Sync_Flag (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, 6))

#define FUNID_Wait_Sync (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, 7))


#define FUNID_Show_Window (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, 10))

#define FUNID_Commit_Composer_Layer (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, 11))

#define FUNID_Get_Display_Mods (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, SYNC_FUN_ID(12)))

#define FUNID_Set_Display_Status (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, 13))

#define FUNID_Get_Display_Status (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, SYNC_FUN_ID(14)))

#define FUNID_Show_Window_FLIP_V (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, 15))


#define PARA_NUM_Commit_Composer_Layer 1
#define PARA_NUM_Set_Sync_Flag 1
#define PARA_NUM_Wait_Sync 1
#define PARA_NUM_Get_Display_Mods 1
#define PARA_NUM_Set_Display_Status 1
#define PARA_NUM_Get_Display_Status 1


extern int display_is_open;

#endif