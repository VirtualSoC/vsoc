softmmu_ss.add(when: 'CONFIG_DIRECT_EXPRESS_PCI', if_true: files('direct_express_pci.c'))

direct_express = ss.source_set()

direct_express.add(when: 'CONFIG_DIRECT_EXPRESS',
               if_true: files(
                   'express_log.c',
                   'direct_express.c',
                   'express_handle_thread.c',
                   'direct_express_distribute.c',
               ))

softmmu_ss.add_all(direct_express)