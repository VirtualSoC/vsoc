//
// Created by gaodi on 2020/9/22.
//

#ifndef QEMU_TELEPORT_EXPRESS_PCI_H
#define QEMU_TELEPORT_EXPRESS_PCI_H


#include "hw/teleport-express/teleport_express.h"
#include "hw/virtio/virtio-pci.h"
#include "hw/qdev-properties.h"


#define TYPE_TELEPORT_EXPRESS_PCI "teleport"
#define TELEPORT_EXPRESS_PCI(obj) OBJECT_CHECK(Teleport_Express_PCI, (obj), TYPE_TELEPORT_EXPRESS_PCI)

#define MAX_EXPRESS_DEVICE_NUM 256

typedef struct Teleport_Express_PCI{
    VirtIOPCIProxy parent_obj;
    Teleport_Express teleport_express;
    bool express_device_enable[MAX_EXPRESS_DEVICE_NUM];
    bool enalbe_opengl_debug;
    bool enable_independ_window;
} Teleport_Express_PCI;


#endif //QEMU_MY_GPU_PCI_H
