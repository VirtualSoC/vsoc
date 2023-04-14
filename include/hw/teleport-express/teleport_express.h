#ifndef QEMU_TELEPORT_EXPRESS_H
#define QEMU_TELEPORT_EXPRESS_H

// #include "qemu/units.h"
// #include "hw/pci/pci.h"
// #include "hw/hw.h"
// #include "hw/pci/msi.h"
// #include "qemu/timer.h"
// #include "qemu/main-loop.h" /* iothread mutex */
// #include "qemu/module.h"
// #include "qemu/queue.h"
// #include "qemu/log.h"
// #include "qapi/visitor.h"
// #include "hw/virtio/virtio.h"
// #include "ui/qemu-pixman.h"
// 
// #include "qemu/typedefs.h"

#include "qemu/osdep.h"
#include "hw/virtio/virtio-pci.h"

#define TYPE_TELEPORT_EXPRESS "teleport-express"


//用于EXPRESS_GPU驱动加载的非常重要的ID，需要与驱动保持一致
#define TELEPORT_EXPRESS_DEVICE_ID 32




#define TELEPORT_EXPRESS(obj) \
    OBJECT_CHECK(Teleport_Express, (obj), TYPE_TELEPORT_EXPRESS)



typedef struct
{
    VirtIODevice parent_obj;

    //设备的输出传输通道
    VirtQueue *out_data_queue;
    VirtQueue *in_data_queue;

    //分发线程
    QemuThread distribute_thread;

    //输入中断注入线程
    QemuThread input_thread;


    int distribute_thread_run;
    int input_thread_run;

    int register_input_vq_locker;


    //暂时没用到
    // QemuMutex thr_mutex;
    // QemuCond thr_cond;

} Teleport_Express;


extern bool teleport_express_should_stop;


#endif //QEMU_TELEPORT_EXPRESS_H
