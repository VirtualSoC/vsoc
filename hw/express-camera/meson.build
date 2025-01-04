express_camera = ss.source_set()

express_camera.add(when: 'CONFIG_EXPRESS_CAMERA',
               if_true: files(
                    'express_camera.c'
               ))

if targetos == 'darwin'
     express_camera.add(when: 'CONFIG_EXPRESS_CAMERA',
                    if_true: files(
                         'camera_utils.m'
                    ))

     CoreFoundation = dependency('appleframeworks', modules: 'CoreFoundation')
     CoreMedia = dependency('appleframeworks', modules: 'CoreMedia')
     AVFoundation = dependency('appleframeworks', modules: 'AVFoundation')
     express_camera.add(CoreFoundation)
     express_camera.add(CoreMedia)
     express_camera.add(AVFoundation)
endif

softmmu_ss.add_all(express_camera)