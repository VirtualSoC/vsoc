//
// Created by gaodi on 2020/9/22.
//

#ifndef QEMU_DIRECT_EXPRESS_PCI_H
#define QEMU_DIRECT_EXPRESS_PCI_H


#include "direct-express/direct_express.h"
#include "hw/virtio/virtio-pci.h"
#include "hw/qdev-properties.h"
//#include "qemu/osdep.h"
//#include "qemu/units.h"
//#include "hw/pci/pci.h"
//#include "hw/hw.h"
//#include "hw/pci/msi.h"
//#include "qemu/timer.h"
//#include "qemu/main-loop.h" /* iothread mutex */
//#include "qemu/module.h"
//#include "qemu/queue.h"
//#include "qemu/log.h"
//#include "qapi/visitor.h"
//#include "hw/virtio/virtio.h"
//#include "hw/virtio/virtio-bus.h"
//#include "hw/virtio/virtio-pci.h"
//#include "ui/qemu-pixman.h"
//#include "ui/console.h"
//#include "qemu/typedefs.h"
//#include "hw/qdev-properties.h"
//#include "sysemu/vhost-user-backend.h"


#define TYPE_DIRECT_EXPRESS_PCI "direct-express-pci"
#define DIRECT_EXPRESS_PCI(obj) OBJECT_CHECK(Direct_Express_PCI, (obj), TYPE_DIRECT_EXPRESS_PCI)

typedef struct Direct_Express_PCI{
    VirtIOPCIProxy parent_obj;
    Direct_Express direct_express;
} Direct_Express_PCI;





// // to share between PCI and VGA 
// define DEFINE_MY_GPU_PCI_PROPERTIES(_state)                
//    DEFINE_PROP_BIT("ioeventfd", _state, flags,                 
//                    VIRTIO_PCI_FLAG_USE_IOEVENTFD_BIT, false),  
//        DEFINE_PROP_UINT32("vectors", _state, nvectors, 3)



#endif //QEMU_MY_GPU_PCI_H
