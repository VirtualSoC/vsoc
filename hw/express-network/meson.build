


express_network = ss.source_set()

express_network.add(when: 'CONFIG_EXPRESS_NETWORK',
               if_true: files(
                    'express_bridge.c',
               ))




softmmu_ss.add_all(express_network)


