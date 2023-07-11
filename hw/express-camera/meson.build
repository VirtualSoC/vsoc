express_camera = ss.source_set()

express_camera.add(when: 'CONFIG_EXPRESS_CAMERA',
               if_true: files(
                    'express_camera.c',
               ))

avcodec = cc.find_library('libavcodec')
avdevice = cc.find_library('libavdevice')
avformat = cc.find_library('libavformat')
avutil = cc.find_library('libavutil')
jpeg = cc.find_library('libjpeg')

express_camera.add(when: 'CONFIG_EXPRESS_CAMERA', if_true: avcodec)
express_camera.add(when: 'CONFIG_EXPRESS_CAMERA', if_true: avformat)
express_camera.add(when: 'CONFIG_EXPRESS_CAMERA', if_true: avutil)
express_camera.add(when: 'CONFIG_EXPRESS_CAMERA', if_true: avdevice)
express_camera.add(when: 'CONFIG_EXPRESS_CAMERA', if_true: jpeg)

softmmu_ss.add_all(express_camera)