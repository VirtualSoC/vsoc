express_camera = ss.source_set()

if targetos == 'windows'
express_camera.add(when: 'CONFIG_EXPRESS_CAMERA',
               if_true: files(
                    'express_camera.c'
               ))

elif targetos == 'darwin'
express_camera.add(when: 'CONFIG_EXPRESS_CAMERA',
               if_true: files(
                    'express_camera.c',
                    'camera_utils.m'
               ))
endif

if targetos == 'windows'
avcodec = cc.find_library('libavcodec')
avdevice = cc.find_library('libavdevice')
avformat = cc.find_library('libavformat')
avutil = cc.find_library('libavutil')
jpeg = cc.find_library('libjpeg')

elif targetos == 'darwin'
avcodec = cc.find_library('libavcodec',dirs:'/opt/homebrew/Cellar/ffmpeg/6.0/lib',static:false)
avdevice = cc.find_library('libavdevice',dirs:'/opt/homebrew/Cellar/ffmpeg/6.0/lib',static:false)
avformat = cc.find_library('libavformat',dirs:'/opt/homebrew/Cellar/ffmpeg/6.0/lib',static:false)
avutil = cc.find_library('libavutil',dirs:'/opt/homebrew/Cellar/ffmpeg/6.0/lib',static:false)
jpeg = cc.find_library('libjpeg',dirs:'/opt/homebrew/Cellar/jpeg-turbo/3.0.0/lib',static:false)
CoreFoundation = dependency('appleframeworks', modules: 'CoreFoundation')
CoreMedia = dependency('appleframeworks', modules: 'CoreMedia')
AVFoundation = dependency('appleframeworks', modules: 'AVFoundation')
express_camera.add(CoreFoundation)
express_camera.add(CoreMedia)
express_camera.add(AVFoundation)
endif

express_camera.add(when: 'CONFIG_EXPRESS_CAMERA', if_true: avcodec)
express_camera.add(when: 'CONFIG_EXPRESS_CAMERA', if_true: avformat)
express_camera.add(when: 'CONFIG_EXPRESS_CAMERA', if_true: avutil)
express_camera.add(when: 'CONFIG_EXPRESS_CAMERA', if_true: avdevice)
express_camera.add(when: 'CONFIG_EXPRESS_CAMERA', if_true: jpeg)

softmmu_ss.add_all(express_camera)