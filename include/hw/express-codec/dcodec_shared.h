/**
 * WARNING: The contents of this file should be **IDENTICAL**
 * between host and guest modules!
*/

#ifndef DCODEC_SHARED_H
#define DCODEC_SHARED_H

#ifndef CHECK
#define CHECK(e) if (!(e)) LOGE("CHECK(0x%x) failed!", (e));
#define CHECK_ZERO(e) if (e) LOGE("CHECK_ZERO(0x%x) failed!", (e));
#define CHECK_EQ(a, b) if ((a) != (b)) LOGE("CHECK_EQ(0x%" PRIx64 ", 0x%" PRIx64 ") failed!", (int64_t)(a), (int64_t)(b));
#define CHECK_LT(a, b) if ((a) >= (b)) LOGE("CHECK_LT(0x%" PRIx64 ", 0x%" PRIx64 ") failed!", (int64_t)(a), (int64_t)(b));
#define CHECK_LE(a, b) if ((a) > (b)) LOGE("CHECK_LE(0x%" PRIx64 ", 0x%" PRIx64 ") failed!", (int64_t)(a), (int64_t)(b));
#define CHECK_GT(a, b) if ((a) <= (b)) LOGE("CHECK_GT(0x%" PRIx64 ", 0x%" PRIx64 ") failed!", (int64_t)(a), (int64_t)(b));
#define CHECK_GE(a, b) if ((a) < (b)) LOGE("CHECK_GE(0x%" PRIx64 ", 0x%" PRIx64 ") failed!", (int64_t)(a), (int64_t)(b));
#endif

// function ids
#define DCODEC_FUN_InitComponent 1
#define DCODEC_FUN_ResetComponent 2
#define DCODEC_FUN_DestroyComponent 3
#define DCODEC_FUN_SendCommand 4
#define DCODEC_FUN_GetParameter 5
#define DCODEC_FUN_SetParameter 6
#define DCODEC_FUN_ProcessThisBuffer 7
#define DCODEC_FUN_SetSync 8
#define DCODEC_FUN_WaitSync 9
#define DCODEC_MAX_FUNID DCODEC_FUN_WaitSync

// device files
#define EXPRESS_CODEC_DEVICE_NAME "/dev/express_codec"

#ifndef EXPRESS_CODEC_DEVICE_ID
#define EXPRESS_CODEC_DEVICE_ID ((uint64_t)13)
#endif

#ifndef GET_DEVICE_ID
#define GET_DEVICE_ID(id) ((uint32_t)((id) >> 32))
#define GET_FUN_ID(id) ((uint32_t)((id)&0xffffff))

#define FUN_NEED_SYNC(id) (((id) >> 24) & 0x1)
#define FUN_HAS_HOST_SYNC(id) (((id) >> 24) & 0x2)
#define FUN_HAS_LONG_WAIT(id) (((id) >> 24) & 0x4)

#define SYNC_FUN_ID(id) ((uint64_t)(1L << 24) | id)
#define HOST_SYNC_FUN_ID(id) ((uint64_t)(1L << 25) | id)
#define LONG_WAIT_FUN_ID(id) ((uint64_t)(1L << 26) | id)

#define DEVICE_FUN_ID(device_id, id) (((uint64_t)device_id << 32) | id)
#endif

// ioctls
#define IOC_MAGIC 0xE6
#define IOC_ENABLE_CALLBACK _IOW(IOC_MAGIC, 1, int)
#define IOC_DISABLE_CALLBACK _IOW(IOC_MAGIC, 2, int)
#define IOC_MAXNR 2

// decoders have one input port and one output port
#define CODEC_INPUT_PORT_INDEX 0
#define CODEC_OUTPUT_PORT_INDEX 1
#define CODEC_MAX_PORT_INDEX CODEC_OUTPUT_PORT_INDEX

#define CODEC_AUDIO_INPUT_BUFFER_COUNT 4
#define CODEC_AUDIO_OUTPUT_BUFFER_COUNT 4
#define CODEC_AUDIO_INPUT_BUFFER_SIZE (32 * 1024)
#define CODEC_AUDIO_OUTPUT_BUFFER_SIZE (32 * 1024)
#define CODEC_VIDEO_INPUT_BUFFER_COUNT 4
#define CODEC_VIDEO_OUTPUT_BUFFER_COUNT 4

// dcodec-exclusive event types that unifies EmptyBufferDone/FillBufferDone/Notify events
// for usage, consult dcodec_return_buffer
#define OMX_EventEmptyBufferDone ((OMX_EVENTTYPE)0x7f100000)
#define OMX_EventFillBufferDone ((OMX_EVENTTYPE)0x7f100001)

// dcodec-exclusive codec parameter types
#define OMX_IndexParamVideoDcodecDefinition ((OMX_INDEXTYPE)0x7f100002)
#define OMX_IndexParamAudioDcodecDefinition ((OMX_INDEXTYPE)0x7f100003) // currently unused

typedef struct OMX_VIDEO_DCODECDEFINITIONTYPE {
    uint32_t nPortIndex;
    uint32_t nFrameWidth;
    uint32_t nFrameHeight;
    uint32_t eColorFormat;
    uint32_t xFramerate;
    uint8_t bLowLatency;
} __attribute__((packed, aligned(4))) OMX_VIDEO_DCODECDEFINITIONTYPE;

enum BufferType {
    // input buffer to be emptied by the codec
    CODEC_BUFFER_TYPE_INPUT = 0x1,

    // output buffer to be filled by the codec
    CODEC_BUFFER_TYPE_OUTPUT = 0x2,

    // this indicates that the buffer stores guest memory
    // .data field of BufferDesc objects stores Guest_Mem*
    CODEC_BUFFER_TYPE_GUEST_MEM = 0x10,

    // gbuffer
    // .id field stores gbuffer id; .data field is unused
    CODEC_BUFFER_TYPE_GBUFFER = 0x20,

    // AVPacket
    // .data field stores AVPacket* . The client should be responsible for
    // freeing the AVPacket* after use
    CODEC_BUFFER_TYPE_AVPACKET = 0x40,

    // AVFrame
    // .data field stores AVFrame* . The client should be responsible for
    // freeing the AVFrame* after use
    CODEC_BUFFER_TYPE_AVFRAME = 0x80,
};

typedef struct BufferDesc {
    uint32_t type; // buffer type
    uint64_t id; // buffer id
    uint64_t header; // ptr to guest header (if it is a guest buffer)
    int sync_id; // sync id of the corresponding gbuffer (if exists)

    // the following fields are copied from header
    uint32_t nAllocLen;          /**< size of the buffer allocated, in bytes */
    uint32_t nFilledLen;         /**< number of bytes currently in the
                                buffer */
    uint32_t nOffset;            /**< start offset of valid data in bytes from
                                the start of the buffer */
    int64_t nTimeStamp;       /**< Timestamp corresponding to the sample
                                starting at the first logical sample
                                boundary in the buffer. Timestamps of
                                successive samples within the buffer may
                                be inferred by adding the duration of the
                                of the preceding buffer to the timestamp
                                of the preceding buffer.*/
    uint32_t nFlags;             /**< buffer specific flags */

#ifdef QEMU_OSDEP_H
    // host-only pointer to data. the actual interpretation of the data depends on the type of buffer
    // see enum BufferType for details
    void *data;
#endif
} __attribute__((packed, aligned(4))) BufferDesc;

#define CODEC_CALLBACK_BUFFER_LEN 16

typedef struct {
	int event;
	uint32_t data1;
	uint32_t data2;
	uint64_t data;
    uint32_t flags;
    uint32_t extra;
} __attribute__((packed, aligned(4))) CodecCallbackData;

typedef struct {
	int size;
	int host_idx;
	int guest_idx;
	CodecCallbackData callbacks[CODEC_CALLBACK_BUFFER_LEN];
} __attribute__((packed, aligned(4))) CodecDMABuffer;

#ifdef OMX_Core_h
// also used to determine if the host supports a param
static inline uint32_t get_omx_param_size(OMX_INDEXTYPE index) {
    switch ((int)index) {
        case OMX_IndexParamAudioPcm:
            return sizeof(OMX_AUDIO_PARAM_PCMMODETYPE);

        case OMX_IndexParamAudioAac:
            return sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE);

        case OMX_IndexParamAudioMp3:
            return sizeof(OMX_AUDIO_PARAM_MP3TYPE);

        case OMX_IndexParamAudioVorbis:
            return sizeof(OMX_AUDIO_PARAM_VORBISTYPE);

        case OMX_IndexParamAudioWma:
            return sizeof(OMX_AUDIO_PARAM_WMATYPE);

        case OMX_IndexParamAudioRa:
            return sizeof(OMX_AUDIO_PARAM_RATYPE);

        case OMX_IndexParamAudioFlac:
            return sizeof(OMX_AUDIO_PARAM_FLACTYPE);

        case OMX_IndexParamAudioMp2:
            return sizeof(OMX_AUDIO_PARAM_MP2TYPE);

        case OMX_IndexParamAudioAndroidAc3:
            return sizeof(OMX_AUDIO_PARAM_ANDROID_AC3TYPE);

        case OMX_IndexParamAudioAc3:
            return sizeof(OMX_AUDIO_PARAM_AC3TYPE);

        case OMX_IndexParamAudioAlac:
            return sizeof(OMX_AUDIO_PARAM_ALACTYPE);

        case OMX_IndexParamAudioApe:
            return sizeof(OMX_AUDIO_PARAM_APETYPE);

        case OMX_IndexParamAudioDts:
            return sizeof(OMX_AUDIO_PARAM_DTSTYPE);

        case OMX_IndexParamAudioFFmpeg:
            return sizeof(OMX_AUDIO_PARAM_FFMPEGTYPE);

        case OMX_IndexParamVideoDcodecDefinition:
            return sizeof(OMX_VIDEO_DCODECDEFINITIONTYPE);

        case OMX_IndexParamVideoWmv:
            return sizeof(OMX_VIDEO_PARAM_WMVTYPE);

        case OMX_IndexParamVideoRv:
            return sizeof(OMX_VIDEO_PARAM_RVTYPE);

        case OMX_IndexParamVideoAvc:
            return sizeof(OMX_VIDEO_PARAM_AVCTYPE);

        case OMX_IndexParamVideoHevc:
            return sizeof(OMX_VIDEO_PARAM_HEVCTYPE);

        case OMX_IndexParamVideoFFmpeg:
            return sizeof(OMX_VIDEO_PARAM_FFMPEGTYPE);

        case OMX_IndexParamVideoBitrate:
            return sizeof(OMX_VIDEO_PARAM_BITRATETYPE);

        default: {
            return 0;
        }
    }
}
#endif

#endif