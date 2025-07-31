#ifndef EXPRESS_DISPLAY_H
#define EXPRESS_DISPLAY_H

#include "hw/teleport-express/express_device_common.h"
#include "hw/teleport-express/teleport_express_call.h"

#include "hw/express-gpu/egl_surface.h"

typedef struct Display_Info
{
    char name[32];
    // guest对应的虚拟显示器的大小
    int pixel_width;
    int pixel_height;
    uint64_t refresh_rate_bits;
} __attribute__((packed, aligned(4))) Display_Info;

typedef struct Display_Status
{
    int refresh_rate;
    int power_status;
    uint32_t backlight;
} __attribute__((packed, aligned(4))) Display_Status;

typedef struct GBuffer_Layer{
     int x;
     int y;
     int z;
     int width;
     int height;
     int blend_type;
     int transform_type;
     int crop_x;
     int crop_y;
     int crop_width;
     int crop_height;
     int write_sync_id;
     int read_sync_id;
     //SetLayerVisibleRegion暂时先不支持
     uint64_t gbuffer_id;
} __attribute__((packed, aligned(4))) GBuffer_Layer;

typedef struct GBuffer_Layers{
     int layer_num;
     struct GBuffer_Layer layer[0];
} __attribute__((packed, aligned(4))) GBuffer_Layers;

typedef struct
{
    Thread_Context thread_context;

    // offscreen graphics context
    GLFWwindow *window;

    GLuint programID;
    GLuint drawVAO;

    // const display attributes
    uint64_t unique_id;
    Display_Info info;
    Display_Status status;
    int is_open;

    // QEMU的主窗口的长宽
    int window_width;
    int window_height;

    // 显示的内容的实际位置和长宽
    int content_x;
    int content_y;
    int content_w;
    int content_h;

    int transform_uniform;
    int transform_type;
    int flip_type; // todo: unify flip and transform

    int fps_counter;
    uint64_t last_fps_timestamp;
} Display_Context;

#define FUNID_Terminate (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, 0))

#define FUNID_Get_Display_Count (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, SYNC_FUN_ID(1)))

#define FUNID_Set_Sync_Flag (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, 6))

#define FUNID_Wait_Sync (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, 7))

#define FUNID_Show_Window (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, 10))

#define FUNID_Commit_Composer_Layer (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, 11))

#define FUNID_Get_Display_Mods (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, SYNC_FUN_ID(12)))

#define FUNID_Set_Display_Status (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, 13))

#define FUNID_Get_Display_Status (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, SYNC_FUN_ID(14)))

#define FUNID_Show_Window_FLIP_V (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, 15))

#define FUNID_Snapshot_Load (DEVICE_FUN_ID(EXPRESS_DISPLAY_DEVICE_ID, 16))

#define PARA_NUM_Commit_Composer_Layer 1
#define PARA_NUM_Set_Sync_Flag 1
#define PARA_NUM_Wait_Sync 1
#define PARA_NUM_Get_Display_Mods 1
#define PARA_NUM_Set_Display_Status 1
#define PARA_NUM_Get_Display_Status 1

uint64_t get_display_count(void);
void get_display_info(int displayIndex, int *width, int *height,
                      int *refreshRate);

void save_display_context(QEMUFile *f);
void load_display_context(QEMUFile *f);

#endif