express_codec = ss.source_set()

express_codec.add(when: 'CONFIG_EXPRESS_CODEC',
               if_true: files(
                    'express_codec.c',
                    'dcodec.c',
                    'dcodec_component.c',
                    'dcodec_audio.c',
                    'dcodec_video.c',
               ))

avcodec = cc.find_library('libavcodec')
avdevice = cc.find_library('libavdevice')
avfilter = cc.find_library('libavfilter')
avformat = cc.find_library('libavformat')
avutil = cc.find_library('libavutil')
swresample = cc.find_library('libswresample')
swscale = cc.find_library('libswscale')

express_codec.add(avcodec)
express_codec.add(avdevice)
express_codec.add(avfilter)
express_codec.add(avformat)
express_codec.add(avutil)
express_codec.add(swresample)
express_codec.add(swscale)

softmmu_ss.add_all(express_codec)
