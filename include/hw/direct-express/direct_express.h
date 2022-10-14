#ifndef QEMU_DIRECT_EXPRESS_H
#define QEMU_DIRECT_EXPRESS_H

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

#define TYPE_DIRECT_EXPRESS "direct-express"


//用于EXPRESS_GPU驱动加载的非常重要的ID，需要与驱动保持一致
#define DIRECT_EXPRESS_DEVICE_ID 32




#define DIRECT_EXPRESS(obj) \
    OBJECT_CHECK(Direct_Express, (obj), TYPE_DIRECT_EXPRESS)



typedef struct
{
    VirtIODevice parent_obj;

    //设备的传输通道
    VirtQueue *data_queue;

    //aio线程处理数据的句柄
    QEMUBH *data_bh;

    //渲染线程
    QemuThread render_thread;

    int thread_run;

    //暂时没用到
    QemuMutex thr_mutex;
    QemuCond thr_cond;

} Direct_Express;


#endif //QEMU_DIRECT_EXPRESS_H
