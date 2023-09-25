#pragma once

#include "OMX/OMX_Core.h"
#include "OMX/OMX_Component.h"
#include "OMX/OMX_Audio.h"
#include "OMX/OMX_AudioExt.h"
#include "OMX/OMX_Video.h"
#include "OMX/OMX_VideoExt.h"
#include "OMX/OMX_Index.h"
#include "OMX/OMX_IndexExt.h"
#include "OMX/OMX_FFMPEG_Extn.h"

#include "stdbool.h"

#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"

#include "libavutil/avstring.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
#include "libavutil/dict.h"
#include "libavutil/parseutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/avassert.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libavutil/opt.h"
#include "libavcodec/avfft.h"
#ifdef LIBAV_CONFIG_H
#include "libavresample/avresample.h"
#else
#include "libswresample/swresample.h"
#endif

#include "qemu/osdep.h"
#include "qemu/thread.h"

#include "hw/teleport-express/teleport_express_call.h"
#include "dcodec_shared.h"


enum CodecStatus {
    // the codec is awaiting input
    AWAITING_INPUT,

    // input data is available
    INPUT_DATA_AVAILABLE,

    // the codec has seen the eos buffer, and will flush all the output buffers
    INPUT_EOS_SEEN,
    
    // all buffers have been processed
    OUTPUT_EOS_SENT,
    
    // an error occurred while processing the buffers
    ERROR_SIGNALED,
};

enum {
    ERR_INPUT_QUEUE_FULL    = 2,
    ERR_NO_FRM              = 1,
    ERR_OK                  = 0,  // No errors
    ERR_OOM                 = -1, // Out of memmory
    ERR_INVALID_PARAM       = -2,
    ERR_CODEC_NOT_FOUND     = -3,
    ERR_DECODER_OPEN_FAILED = -4,
    ERR_SWR_INIT_FAILED     = -5,
    ERR_RESAMPLE_FAILED     = -6,
    ERR_DECODE_FAILED       = -7,
    ERR_EXTRADATA_FAILED    = -8,
    ERR_SWS_FAILED          = -9,
};

typedef struct DCodecComponent DCodecComponent;

struct DCodecComponent {
    Device_Context device_context;

    GQueue *input_buffers;
    GQueue *output_buffers;

    AVCodecContext *mCtx;
    struct SwrContext *mSwrCtx;
    AVFrame *mFrame;
    AVPacket *mPkt;

    enum CodecStatus mStatus;
    bool mSignalledError;

    Guest_Mem *dma_buf;

    OMX_ERRORTYPE (*reset_component)(DCodecComponent *_context);
    OMX_ERRORTYPE (*destroy_component)(DCodecComponent *context);
    OMX_ERRORTYPE (*get_parameter)(DCodecComponent *_context, OMX_IN OMX_INDEXTYPE index, OMX_PTR params);
    OMX_ERRORTYPE (*set_parameter)(DCodecComponent *_context, OMX_IN OMX_INDEXTYPE index, OMX_PTR params);

    int (*open_decoder)(DCodecComponent *_context);
    int (*empty_one_input_buffer)(DCodecComponent *_context);
    int (*fill_one_output_buffer)(DCodecComponent *_context);
    void (*fill_eos_output_buffer)(DCodecComponent *_context);
};

int dcodec_init_component(DCodecComponent *context);
int dcodec_reset_component(DCodecComponent *context);
void dcodec_deinit_component(DCodecComponent *context);
void dcodec_return_buffer_to_guest(DCodecComponent *context, BufferDesc *desc);
void dcodec_return_all_buffers_to_guest(DCodecComponent *context);
void dcodec_free_buffer_desc(void *desc);
OMX_ERRORTYPE dcodec_send_command(DCodecComponent *context, OMX_COMMANDTYPE cmd, OMX_U32 param, OMX_U64 data);
void dcodec_notify_error(DCodecComponent *context, OMX_ERRORTYPE type);
void dcodec_notify(DCodecComponent *context, OMX_EVENTTYPE event, OMX_U32 data1, OMX_U32 data2, OMX_U64 data, OMX_U32 flags);
OMX_ERRORTYPE dcodec_process_this_buffer(DCodecComponent *context, OMX_INOUT BufferDesc *desc);
int dcodec_handle_extradata(DCodecComponent *context);
