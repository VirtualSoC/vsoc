/**
 * @file my_gpu.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief 
 * @version 0.1
 * @date 2020-10-20
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "mygpu/my_gpu.h"
#include "mygpu/my_gpu_render.h"

/**
 * @brief 当vring有数据来的之后的回调函数，在aio线程中运行
 * 
 * @param vdev 
 * @param vq 
 */
static void my_gpu_handle(VirtIODevice *vdev, VirtQueue *vq)
{

    MYGPU *g = MY_GPU(vdev);
    if (!g->thread_run)
    {
        printf("start render thread\n");
        qemu_thread_create(&g->render_thread, "mygpu-render", opengl_ui_thread,
                           vdev, QEMU_THREAD_JOINABLE);
        g->thread_run = 1;
    }
}

/**
 * @brief aio线程处理数据时的回调函数，在这里调用实际的处理函数
 * 
 * @param opaque 传递的参数，实际就是MYGPU
 */
static void my_gpu_handle_bh(void *opaque)
{
    MYGPU *g = opaque;
    my_gpu_handle(&g->parent_obj, g->data_queue);
}

/**
 * @brief guest往queue中添加数据后，kick这边后的回调的函数。
 * 为了保证虚拟机快速恢复，因此需要将任务快速抛到aio线程中，由aio线程去处理数据
 * 
 * @param vdev 
 * @param vq 
 */
static void my_gpu_handle_cb(VirtIODevice *vdev, VirtQueue *vq)
{
    MYGPU *g = MY_GPU(vdev);
    qemu_bh_schedule(g->data_bh);
}

static void my_gpu_realize(DeviceState *qdev, Error **errp)
{

    VirtIODevice *vdev = VIRTIO_DEVICE(qdev);
    MYGPU *g = MY_GPU(qdev);

    //初始化使用virtio的gpu设备
    virtio_init(VIRTIO_DEVICE(g), "my-gpu", MY_GPU_DEVICE_ID, 0);

    //为该设备添加1024大小的queue，并且设置收到queue返回消息后的回调函数
    //最大为1024大小，也就是不弄indirect table的话最大只有1024个页，
    //弄indirect table时单个空间最大可以放一个额外的1024大小的table，
    //一个参数占用一个空间，因此单个参数的数据被限制在1024个不连续页面
    //需要未来进行修复 @todo
    virtio_add_queue(vdev, 1024, my_gpu_handle_cb);

    g->data_queue = virtio_get_queue(vdev, 0);
    //在aio线程处理中处理数据的函数
    g->data_bh = qemu_bh_new(my_gpu_handle_bh, g);

    virtio_add_feature(&vdev->host_features, VIRTIO_RING_F_INDIRECT_DESC);

    //    qemu_mutex_init(&edu->thr_mutex);
    //    qemu_cond_init(&edu->thr_cond);
    //    qemu_thread_create(&g->gpu_thread, "gpu", gpu_thread,
    //                       edu, QEMU_THREAD_JOINABLE);
    //    printf("my gpu realized\n");
}

static uint64_t
my_gpu_get_features(VirtIODevice *vdev, uint64_t features,
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
// my_gpu_set_features(VirtIODevice *vdev, uint64_t features)
// {
//     //这个不会被调用到
//     //    static const uint32_t virgl = (1 << VIRTIO_GPU_F_VIRGL);
//     //    VirtIOGPUBase *g = VIRTIO_GPU_BASE(vdev);
//     //
//     //    g->use_virgl_renderer = ((features & virgl) == virgl);
//     //    trace_virtio_gpu_features(g->use_virgl_renderer);

//     return;
// }

static void gpu_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    VirtioDeviceClass *vdc = VIRTIO_DEVICE_CLASS(klass);

    //    vdc->unrealize = virtio_gpu_base_device_unrealize;
    vdc->get_features = my_gpu_get_features;
    vdc->set_features = NULL;

    set_bit(DEVICE_CATEGORY_DISPLAY, dc->categories);
    dc->hotpluggable = false;

    vdc->realize = my_gpu_realize;
}

static void my_gpu_register_types(void)
{
    static InterfaceInfo interfaces[] = {
        {INTERFACE_CONVENTIONAL_PCI_DEVICE},
        {},
    };
    static const TypeInfo gpu_info = {
        .name = TYPE_MY_GPU,
        .parent = TYPE_VIRTIO_DEVICE,
        .instance_size = sizeof(MYGPU),
        .class_init = gpu_class_init,
        .interfaces = interfaces,
    };

    type_register_static(&gpu_info);
}
type_init(my_gpu_register_types)
