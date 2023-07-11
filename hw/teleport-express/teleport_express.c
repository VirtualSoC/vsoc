/**
 * @file express_gpu.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief
 * @version 0.1
 * @date 2020-10-20
 *
 * @copyright Copyright (c) 2020
 *
 */
// #define STD_DEBUG_LOG
#include "hw/teleport-express/teleport_express.h"

#include "hw/teleport-express/teleport_express_call.h"
#include "hw/teleport-express/teleport_express_distribute.h"
#include "hw/teleport-express/teleport_express_register.h"

#include "hw/teleport-express/express_log.h"

// #define express_printf null_printf

bool teleport_express_should_stop = 0;

/**
 * @brief 当vring有数据来的之后的回调函数，在aio线程中运行
 *
 * @param vdev
 * @param vq
 */
static void teleport_express_output_handle(VirtIODevice *vdev, VirtQueue *vq)
{

    Teleport_Express *g = TELEPORT_EXPRESS(vdev);
    if (g->distribute_thread_run == 0)
    {
        guest_null_ptr_init(vq);
        express_printf("start handle thread\n");
        g->distribute_thread_run = 1;
        qemu_thread_create(&g->distribute_thread, "teleport-express-distribute", call_distribute_thread,
                           vdev, QEMU_THREAD_JOINABLE);
    }
    else if (g->distribute_thread_run == 1)
    {
        //分发线程还没跑起来，就不处理了
        return;
    }
    else
    {
#ifdef DISTRIBUTE_WHEN_VM_EXIT
        int running_flag = qatomic_cmpxchg(&atomic_distribute_thread_running, 0, 1);
        if (running_flag == 0)
        {
            //没有在处理环上数据，那我就来处理
            int pop_flag = 1;
            int recycle_flag = 1;

            int recycle_cnt = 0;
            int pop_cnt = 0;
            int need_irq = 0;

            //我先处理着，但是分发线程也得赶紧醒来接着我处理
            wake_up_distribute();

            while (pop_flag != 0 || recycle_flag != 0)
            {
                //一旦分发线程跑起来了，就赶紧回虚拟机里去，避免长时间操作，影响vCPU运行，进而造成间歇性卡顿
                if (qatomic_read(&atomic_distribute_thread_running) == 2)
                {
                    running_flag = 1;
                    break;
                }

                pop_flag = 1;
                recycle_flag = 1;

                virtqueue_data_distribute_and_recycle(vq, &pop_flag, &recycle_flag, &need_irq);
                if (pop_flag != 0)
                {
                    pop_cnt += 1;
                }
                if (recycle_flag != 0)
                {
                    recycle_cnt += 1;
                }
            }
            if (need_irq != 0)
            {
                // printf("direct notify\n");
                virtio_notify(VIRTIO_DEVICE(vdev), vq);
            }

            // printf("handle ok pop_cnt %d recycle_cnt %d ", pop_cnt, recycle_cnt);

            if (running_flag == 1)
            {
                // printf("other thread continue\n");
            }

            qatomic_set(&atomic_distribute_thread_running, 0);
        }
        else
        {
            //分发线程的处理流程运行中，那我就不管了
        }
#else
        //不再尝试唤醒分发线程，而是直接我自己上手处理数据，以减少延迟
        wake_up_distribute();
#endif
    }
    return;
}

static void teleport_express_input_handle_cb(VirtIODevice *vdev, VirtQueue *vq)
{

    Teleport_Express *g = TELEPORT_EXPRESS(vdev);

    if(g->input_thread_run == 0){
        qemu_thread_create(&g->input_thread, "teleport-express-input", input_sync_thread,
                           vdev, QEMU_THREAD_JOINABLE);
        g->input_thread_run = 1;
    }

    if (qatomic_cmpxchg(&(g->register_input_vq_locker), 0, 1) == 0)
    {
        register_input_buffer_call(vdev, vq);
        qatomic_set(&(g->register_input_vq_locker), 0);
    }

    return;
}

// /**
//  * @brief aio线程处理数据时的回调函数，在这里调用实际的处理函数
//  *
//  * @param opaque 传递的参数，实际就是express-GPU
//  */
// static void teleport_express_output_handle_bh(void *opaque)
// {
//     Teleport_Express *g = opaque;
//     teleport_express_output_handle(&g->parent_obj, g->data_queue);
// }

/**
 * @brief guest往queue中添加数据后，kick这边后的回调的函数。
 * 为了保证虚拟机快速恢复，因此需要将任务快速抛到aio线程中，由aio线程去处理数据
 *
 * @param vdev
 * @param vq
 */
static void teleport_express_output_handle_cb(VirtIODevice *vdev, VirtQueue *vq)
{
    // Teleport_Express *g = TELEPORT_EXPRESS(vdev);
    // qemu_bh_schedule(g->data_bh);
    teleport_express_output_handle(vdev, vq);
}

static void teleport_express_realize(DeviceState *qdev, Error **errp)
{

    VirtIODevice *vdev = VIRTIO_DEVICE(qdev);
    Teleport_Express *g = TELEPORT_EXPRESS(qdev);

    //初始化使用virtio的gpu设备
    virtio_init(VIRTIO_DEVICE(g), TELEPORT_EXPRESS_DEVICE_ID, 0);

    //为该设备添加1024大小的queue，并且设置收到queue返回消息后的回调函数
    //最大为1024大小，也就是不弄indirect table的话最大只有1024个页，
    //弄indirect table时单个空间最大可以放一个额外的1024大小的table，
    //一个参数占用一个空间，因此单个参数的数据被限制在1024*1024个不连续页面，
    //当然实际限制一次数据传输在256Mb左右
    virtio_add_queue(vdev, 1024, teleport_express_output_handle_cb);
    virtio_add_queue(vdev, 1024, teleport_express_input_handle_cb);

    g->out_data_queue = virtio_get_queue(vdev, 0);
    g->in_data_queue = virtio_get_queue(vdev, 1);

    //在aio线程处理中处理数据的函数
    // g->data_bh = qemu_bh_new(teleport_express_output_handle_bh, g);

    virtio_add_feature(&vdev->host_features, VIRTIO_RING_F_INDIRECT_DESC);

    express_printf("express gpu realized\n");
}

static uint64_t
teleport_express_get_features(VirtIODevice *vdev, uint64_t features,
                              Error **errp)
{
    // 设备独特的特性，下面是virtio-GPU的例子
    //    VirtIOGPUBase *g = VIRTIO_GPU_BASE(vdev);
    //
    //    if (virtio_gpu_virgl_enabled(g->conf)) {
    //        features |= (1 << VIRTIO_GPU_F_VIRGL);
    //    }
    //    if (virtio_gpu_edid_enabled(g->conf)) {
    //        features |= (1 << VIRTIO_GPU_F_EDID);
    //    }
    //
    return features;
}

// static void
// teleport_express_set_features(VirtIODevice *vdev, uint64_t features)
// {
//     //这个不会被调用到
//     //    static const uint32_t virgl = (1 << VIRTIO_GPU_F_VIRGL);
//     //    VirtIOGPUBase *g = VIRTIO_GPU_BASE(vdev);
//     //
//     //    g->use_virgl_renderer = ((features & virgl) == virgl);
//     //    trace_virtio_gpu_features(g->use_virgl_renderer);

//     return;
// }

static void teleport_express_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    VirtioDeviceClass *vdc = VIRTIO_DEVICE_CLASS(klass);

    //    vdc->unrealize = virtio_gpu_base_device_unrealize;
    vdc->get_features = teleport_express_get_features;
    vdc->set_features = NULL;

    set_bit(DEVICE_CATEGORY_DISPLAY, dc->categories);
    dc->hotpluggable = false;

    vdc->realize = teleport_express_realize;
}

static void teleport_express_register_types(void)
{
    static InterfaceInfo interfaces[] = {
        {INTERFACE_CONVENTIONAL_PCI_DEVICE},
        {},
    };
    static const TypeInfo express_info = {
        .name = TYPE_TELEPORT_EXPRESS,
        .parent = TYPE_VIRTIO_DEVICE,
        .instance_size = sizeof(Teleport_Express),
        .class_init = teleport_express_class_init,
        .interfaces = interfaces,
    };

    type_register_static(&express_info);
}
type_init(teleport_express_register_types)
