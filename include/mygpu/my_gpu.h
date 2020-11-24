//
// Created by gaodi on 2020/9/22.
//

#ifndef QEMU_MY_GPU_H
#define QEMU_MY_GPU_H

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

#define TYPE_MY_GPU "my-gpu"


//用于MY_GPU驱动加载的非常重要的ID，需要与驱动保持一致
#define MY_GPU_DEVICE_ID 32




#define MY_GPU(obj) \
    OBJECT_CHECK(MYGPU, (obj), TYPE_MY_GPU)



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

} MYGPU;


#endif //QEMU_MY_GPU_H
