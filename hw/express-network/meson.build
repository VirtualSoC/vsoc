express_network = ss.source_set()

express_network.add(when: 'CONFIG_EXPRESS_NETWORK',
               if_true: files(
                    'express_bridge.c',
                    'em_core.c',
                    'em_gsm.c',
                    'em_remote_call.c',
                    'em_sim_card.c',
                    'em_sms.c',
                    'em_utils.c',
                    'em_ruim.c'
               ))

softmmu_ss.add_all(express_network)


