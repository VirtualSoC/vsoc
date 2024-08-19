express_mem = ss.source_set()

express_mem.add(when: 'CONFIG_EXPRESS_MEM',
               if_true: files(
                    'express_mem.c',
                    'express_mem_worker.c',
                    'express_sync.c'
               ))

softmmu_ss.add_all(express_mem)