// Created by gaodi on 2020/9/22.
//
//

#ifndef QEMU_TELEPORT_EXPRESS_PCI_H
#define QEMU_TELEPORT_EXPRESS_PCI_H


#include "hw/teleport-express/teleport_express.h"
#include "hw/virtio/virtio-pci.h"
#include "hw/qdev-properties.h"


#define TYPE_TELEPORT_EXPRESS_PCI "teleport"
#define TELEPORT_EXPRESS_PCI(obj) OBJECT_CHECK(Teleport_Express_PCI, (obj), TYPE_TELEPORT_EXPRESS_PCI)

#define MAX_EXPRESS_DEVICE_NUM 256

typedef struct Teleport_Express_PCI{ //就类似于VirtIONetPCI那样 定义了
    VirtIOPCIProxy parent_obj; //将virtio和pci设备关联起来。VirtIOPCIProxy存储virtio信息，kvm给guest注册了很多memory region，driver写这些memory region，kvm拦截，把写的值放在VirtIOPCIProxy中。
    Teleport_Express teleport_express;
    bool express_device_enable[MAX_EXPRESS_DEVICE_NUM];
    bool enalbe_opengl_debug;
    bool enable_independ_window;
    bool show_device_input_window;
    bool keep_window_scale;
    int window_width;
    int window_height;

    int display_width;
    int display_height;
    int refresh_rate;

    int phy_width;
    int phy_height;

    int gpu_debug_level;
    bool gpu_log_to_host;
    bool save_snapshot;
#ifdef __APPLE__
    bool load_snapshot;
#endif
    bool gpu_log_with_buffer;
    bool opengl_trace;

    bool scroll_is_zoom;
    bool right_click_is_two_finger;
    int scroll_ratio;

    bool finger_replay;

    bool display_switch_open;

    bool open_shader_binary;
    
    char *ruim_file;

} Teleport_Express_PCI;


#endif //QEMU_MY_GPU_PCI_H
