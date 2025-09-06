softmmu_ss.add(when: 'CONFIG_TELEPORT_EXPRESS_PCI', if_true: files('teleport_express_pci.c'))

teleport_express = ss.source_set()
vsoc_ss = ss.source_set()

teleport_express.add(when: 'CONFIG_TELEPORT_EXPRESS',
               if_true: files(
                   'express_log.c',
                   'express_platform.c',
                   'teleport_express.c',
                   'teleport_express_call.c',
                   'teleport_express_distribute.c',
                   'teleport_express_register.c',
                   'express_handle_thread.c',
                   'express_device_ctrl.c',
                   'teleport_express_hmp.c',
                   'express_ipc.c',
                   'container_utils.c',
               ))

if targetos == 'windows'
    teleport_express.add(when: 'CONFIG_TELEPORT_EXPRESS',
                if_true: files(
                    'express_event_win32.c',
                ))
    vsoc_ss.add(files('express_event_win32.c'))
else
    teleport_express.add(when: 'CONFIG_TELEPORT_EXPRESS',
                if_true: files(
                    'express_event_posix.c',
                ))
    vsoc_ss.add(files('express_event_posix.c'))
endif

softmmu_ss.add_all(teleport_express)

subdir('camera')
subdir('codec')
subdir('display')
subdir('gpu')
subdir('input')
subdir('mem')
subdir('network')
subdir('sensor')

vsoc_ss.add(files(
    'express_log.c',
    'express_ipc.c',
    'express_handle_thread.c',
    'container_utils.c',
))
