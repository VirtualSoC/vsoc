softmmu_ss.add(when: 'CONFIG_TELEPORT_EXPRESS_PCI', if_true: files('teleport_express_pci.c'))

teleport_express = ss.source_set()

teleport_express.add(when: 'CONFIG_TELEPORT_EXPRESS',
               if_true: files(
                   'express_log.c',
                   'teleport_express.c',
                   'teleport_express_call.c',
                   'teleport_express_distribute.c',
                   'teleport_express_register.c',
                   'express_handle_thread.c',
                   'express_device_ctrl.c',
                   'express_event.c'
               ))

softmmu_ss.add_all(teleport_express)