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
#include "libswresample/swresample.h"

#include "qemu/osdep.h"
#include "qemu/thread.h"

#include "hw/teleport-express/teleport_express_call.h"
#include "dcodec_shared.h"

#include "hw/express-gpu/express_gpu_render.h"

#ifndef _WIN32
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

typedef enum CodecStatus {
    // the codec is awaiting input
    AWAITING_INPUT,

    // input data is available
    INPUT_DATA_AVAILABLE,

    // the codec has seen the eos buffer
    INPUT_EOS_SEEN,

    // all buffers have been processed
    OUTPUT_EOS_SENT,

    // an error occurred while processing the buffers
    ERROR_SIGNALED,
} CodecStatus;

enum {
    ERR_HWACCEL_FAILED      = 3,
    ERR_INPUT_QUEUE_FULL    = 2,
    ERR_NO_FRM              = 1,
    ERR_OK                  = 0,  // No errors
    ERR_OOM                 = -1, // Out of memmory
    ERR_INVALID_PARAM       = -2,
    ERR_CODEC_NOT_FOUND     = -3,
    ERR_CODEC_OPEN_FAILED   = -4,
    ERR_SWR_INIT_FAILED     = -5,
    ERR_RESAMPLE_FAILED     = -6,
    ERR_CODING_FAILED       = -7,
    ERR_EXTRADATA_FAILED    = -8,
    ERR_SWS_FAILED          = -9,
    ERR_COLORSPACE_FAILED   = -10,
};

typedef struct DCodecComponent DCodecComponent;
typedef void (*NotifyCallbackFunc)(DCodecComponent *context, CodecCallbackData ccd);

struct DCodecComponent {
    Device_Context device_context;

    // buffer queues contain buffers that are not processed yet
    GQueue *input_buffers;
    GQueue *output_buffers;

    AVCodecContext *mCtx;
    AVFrame *mFrame;
    AVPacket *mPkt;

    // status of the codec
    // ref. enum CodecStatus for details
    CodecStatus mStatus;

    // field reserved for clients
    // this field will not be accessed by the codec
    uint64_t mAppPrivate;

    // DMA buffer used to implement callbacks
    Guest_Mem *dma_buf;
    GMutex dma_buf_mutex;

    // private function pointers
    OMX_ERRORTYPE (*reset_component)(DCodecComponent *_context);
    OMX_ERRORTYPE (*destroy_component)(DCodecComponent *context);
    OMX_ERRORTYPE (*get_parameter)(DCodecComponent *_context, OMX_IN OMX_INDEXTYPE index, OMX_PTR params);
    OMX_ERRORTYPE (*set_parameter)(DCodecComponent *_context, OMX_IN OMX_INDEXTYPE index, OMX_PTR params);

    int (*open_codec)(DCodecComponent *_context);
    int (*empty_one_input_buffer)(DCodecComponent *_context);
    int (*fill_one_output_buffer)(DCodecComponent *_context);
    void (*fill_eos_output_buffer)(DCodecComponent *_context);

    // the notify function should be thread-safe
    NotifyCallbackFunc notify;
};

int dcodec_init_component(DCodecComponent *context, NotifyCallbackFunc notify);
int dcodec_reset_component(DCodecComponent *context);
void dcodec_deinit_component(DCodecComponent *context);
void dcodec_return_buffer(DCodecComponent *context, BufferDesc *desc);
void dcodec_flush_buffers(DCodecComponent *context, int type);
void dcodec_free_buffer_desc(void *desc);
OMX_ERRORTYPE dcodec_send_command(DCodecComponent *context, OMX_COMMANDTYPE cmd, OMX_U32 param, OMX_U64 data);
void dcodec_notify_error(DCodecComponent *context, OMX_ERRORTYPE type);
void dcodec_notify_null(DCodecComponent *context, CodecCallbackData ccd);
void dcodec_notify_guest(DCodecComponent *context, CodecCallbackData ccd);
OMX_ERRORTYPE dcodec_process_this_buffer(DCodecComponent *context, OMX_INOUT BufferDesc *desc);
void dcodec_process_buffers(DCodecComponent *context);
int dcodec_handle_extradata(DCodecComponent *context);
void dcodec_fill_eos_output_buffer(DCodecComponent *_context);
OMX_COLOR_FORMATTYPE pixel_format_av_to_omx(enum AVPixelFormat format);
enum AVPixelFormat pixel_format_omx_to_av(OMX_COLOR_FORMATTYPE format);
int pixel_format_to_tex_format(const OMX_COLOR_FORMATTYPE format, int *glIntFmt,
                               GLenum *glPixFmt, GLenum *glPixType);
int pixel_format_to_swscale_param(const OMX_COLOR_FORMATTYPE format, int width,
                                  int height, uint8_t **data,
                                  int *linesize);
