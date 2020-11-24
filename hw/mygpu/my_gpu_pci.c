/**
 * @file my_gpu_pci.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief 
 * @version 0.1
 * @date 2020-9-22
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "mygpu/my_gpu_pci.h"

//
//static Property my_gpu_pci_base_properties[] = {
//        DEFINE_MY_GPU_PCI_PROPERTIES(VirtIOPCIProxy),
//        DEFINE_PROP_END_OF_LIST(),
//};



static void my_gpu_pci_instance_init(Object *obj){


    MyGPUPCI *dev = MY_GPU_PCI(obj);

    virtio_instance_init_common(obj, &dev->mgpu, sizeof(dev->mgpu),
                                TYPE_MY_GPU);


    //VIRTIO_GPU_PCI_BASE(obj)->vgpu = VIRTIO_GPU_BASE(&dev->vdev);



}



static void my_gpu_pci_realize(VirtIOPCIProxy *vpci_dev, Error **errp){

    MyGPUPCI *vgpu = MY_GPU_PCI(vpci_dev);
    MYGPU *g = &(vgpu->mgpu);
    DeviceState *vdev = DEVICE(g);
    Error *local_error = NULL;

    qdev_set_parent_bus(vdev, BUS(&vpci_dev->bus));
    virtio_pci_force_virtio_1(vpci_dev);
    object_property_set_bool(OBJECT(vdev), true, "realized", &local_error);

    if (local_error) {
        error_propagate(errp, local_error);
        return;
    }

//    for (i = 0; i < g->conf.max_outputs; i++) {
//        object_property_set_link(OBJECT(g->scanout[i].con),
//                                 OBJECT(vpci_dev),
//                                 "device", errp);
//    }

}



static void my_gpu_pci_class_init(ObjectClass *klass, void *data){
    DeviceClass *dc = DEVICE_CLASS(klass);
    VirtioPCIClass *k = VIRTIO_PCI_CLASS(klass);
    PCIDeviceClass *pcidev_k = PCI_DEVICE_CLASS(klass);

    set_bit(DEVICE_CATEGORY_DISPLAY, dc->categories);
//    device_class_set_props(dc, my_gpu_pci_base_properties);
    dc->hotpluggable = false;
    k->realize = my_gpu_pci_realize;
    pcidev_k->class_id = PCI_CLASS_DISPLAY_OTHER;
//    pcidev_k->vendor_id = PCI_VENDOR_ID_REDHAT_QUMRANET;
//    pcidev_k->device_id = 0x10e1;
    pcidev_k->revision = 0x1;

}






static void my_gpu_pci_register_types(void)
{

    const TypeInfo my_gpu_pci_info = {
            .name          = TYPE_MY_GPU_PCI,
            .parent        = TYPE_VIRTIO_PCI,
            .instance_size = sizeof(MyGPUPCI),
            .instance_init = my_gpu_pci_instance_init,
            .class_init    = my_gpu_pci_class_init,
            .interfaces = (InterfaceInfo[]) {
                    { INTERFACE_PCIE_DEVICE },
                    { INTERFACE_CONVENTIONAL_PCI_DEVICE },
                    { }
            },
    };

    type_register_static(&my_gpu_pci_info);
}


type_init(my_gpu_pci_register_types);
