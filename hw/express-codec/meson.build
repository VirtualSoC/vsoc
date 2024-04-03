express_codec = ss.source_set()

express_codec.add(when: 'CONFIG_EXPRESS_CODEC',
               if_true: files(
                    'express_codec.c',
                    'dcodec_component.c',
                    'dcodec_audio.c',
                    'dcodec_vdec.c',
                    'dcodec_venc.c',
                    'colorspace.c',
                    'device_cuda.c',
               ))

avcodec = cc.find_library('avcodec')
avdevice = cc.find_library('avdevice')
avfilter = cc.find_library('avfilter')
avformat = cc.find_library('avformat')
avutil = cc.find_library('avutil')
swresample = cc.find_library('swresample')
swscale = cc.find_library('swscale')

express_codec.add(avcodec)
express_codec.add(avdevice)
express_codec.add(avfilter)
express_codec.add(avformat)
express_codec.add(avutil)
express_codec.add(swresample)
express_codec.add(swscale)

softmmu_ss.add_all(express_codec)
