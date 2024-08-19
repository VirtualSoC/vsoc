/**
 * vSoC camera device
 * 
 * Copyright (c) 2023 Chengen Huang <diandian86@gmail.com>
 * Copyright (c) 2023 Jiaxing Qiu <jx.qiu@outlook.com>
 */

// #define STD_DEBUG_LOG

#include "hw/express-camera/express_camera.h"

#define CAMERA_FUN_GET_CAMERA_COUNT 1
#define CAMERA_FUN_START_STREAM 2
#define CAMERA_FUN_STOP_STREAM 3
#define CAMERA_FUN_QUEUE_BUFFER 4
#define CAMERA_FUN_QUEUE_BUFFER_HW 5
#define CAMERA_FUN_GET_PROP 6
#define CAMERA_FUN_MAXID CAMERA_FUN_GET_PROP

#define CAMERA_LOG_LEVEL AV_LOG_WARNING

#ifdef __WIN32__
// Directshow only works on windows
#define SYSTEM_LIBRARY_NAME "dshow"
#else
#define SYSTEM_LIBRARY_NAME "avfoundation"
#endif

static GArray *g_camera_list = NULL;
static GHashTable *g_camera_thread_contexts_map = NULL;
static int g_camera_count = 0;

typedef struct Resolution {
    int w;
    int h;
} Resolution;

// cctv video resolution table
// https://clarionuk.com/resources/cctv-video-resolutions/
static const Resolution g_res_table[] = {
    // {.w=4000, .h=3000},
    {.w=3840, .h=2160},
    // {.w=3072, .h=2048},
    // {.w=2592, .h=1944},
    // {.w=2688, .h=1520},
    {.w=2560, .h=1440},
    // {.w=2048, .h=1536},
    {.w=1920, .h=1080},
    // {.w=1600, .h=1200},
    // {.w=1280, .h=1024},
    {.w=1280, .h=960},
    {.w=1280, .h=720},
    {.w=720, .h=480},
    {.w=640, .h=480},
    {.w=320, .h=240},
};

static const int g_fps_table[] = {60, 45, 30};

static const int g_res_table_len = sizeof(g_res_table) / sizeof(g_res_table[0]);
static const int g_fps_table_len = sizeof(g_fps_table) / sizeof(g_fps_table[0]);

static OMX_COLOR_FORMATTYPE pixel_format_v4l2_to_omx(uint32_t format) {
    switch (format) {
        case V4L2_PIX_FMT_RGBA32: return OMX_COLOR_Format32BitRGBA8888;
        case V4L2_PIX_FMT_ABGR32: return OMX_COLOR_Format32bitBGRA8888;
        case V4L2_PIX_FMT_RGB565: return OMX_COLOR_Format16bitRGB565;
        default: {
            LOGE("unknown v4l2 pixel format %d", format);
            return OMX_COLOR_FormatUnused;
        }
    }
}
/**
 * opens the camera device. returns AVFormatContext* 
 * which can then be used to find camera stream info.
 * the returned pointer needs to be freed by the caller using avformat_free_context().
 */
static AVFormatContext* open_camera(CameraProp *prop) {
    const AVInputFormat *input_format = NULL;
    AVFormatContext *format_context = NULL;
    char error_msg[256] = {0};
    int ret;
    avdevice_register_all();

    input_format = av_find_input_format(SYSTEM_LIBRARY_NAME);
    if (!input_format) {
        LOGE("av_find_input_format failed");
        return NULL;
    }

    if (prop->name[0] == 0) {
        LOGE("error: camera name is empty");
        return NULL;
    }

    char ff_name[70] = {0};
    char frame_size_str[16] = {0};
    char framerate_str[16] = {0};
    char rtbufsize_str[16] = {0};
    AVDictionary *options = NULL;

#ifdef _WIN32
        snprintf(ff_name, 70, "video=%s", prop->name);
#else
        snprintf(ff_name, 64, "%s", prop->name);
#endif

    // if camera prop is configured, use prop width/height as specified
    if (prop->width > 0 && prop->height > 0) {
        sprintf(frame_size_str, "%dx%d", prop->width, prop->height);
        av_dict_set(&options, "video_size", frame_size_str, 0);

        sprintf(rtbufsize_str, "%d", prop->width * prop->height * 2);
        av_dict_set(&options, "rtbufsize", rtbufsize_str, 0);

        sprintf(framerate_str, "%d", prop->frame_interval_den / prop->frame_interval_num);
        av_dict_set(&options, "framerate", framerate_str, 0);

#ifdef __APPLE__
                av_dict_set(&options, "pixel_format", "nv12", 0);
#endif

        av_dict_set(&options, "fflags", "nobuffer", 0);
        av_dict_set(&options, "preset", "ultrafast", 0);
        av_dict_set(&options, "max_delay", "0", 0);
        av_dict_set(&options, "tune", "zerolatency", 0);

        LOGI("open_camera name '%s' %s@%sfps rtbufsize %s", prop->name, frame_size_str, framerate_str, rtbufsize_str);

        ret = avformat_open_input(&format_context, ff_name, input_format, &options);
        av_dict_free(&options);

        if (ret == 0) {
            return format_context;
        }
        av_strerror(ret, error_msg, 256);
        LOGE("avformat_open_input failed: %s %d", error_msg, ret);
    }

    // first time, or previous open failed,
    // try enumerate i=resolution / j=framerate / k=format combinations
    for (int i = 0; i < g_res_table_len; i++) {
        for (int j = 0; j < g_fps_table_len; j++) {
            sprintf(frame_size_str, "%dx%d", g_res_table[i].w, g_res_table[i].h);
            av_dict_set(&options, "video_size", frame_size_str, 0);

            sprintf(framerate_str, "%d", g_fps_table[j]);
            av_dict_set(&options, "framerate", framerate_str, 0);

#ifdef __APPLE__
            av_dict_set(&options, "pixel_format", "nv12", 0);
#endif

            LOGD("camera '%s' try format %s@%sfps", prop->name, frame_size_str, framerate_str);

            ret = avformat_open_input(&format_context, ff_name, input_format, &options);
            av_dict_free(&options);

            if (ret == 0) { // valid, store the combination
                LOGD("try camera format success!");
                prop->frame_interval_num = 1;
                prop->frame_interval_den = g_fps_table[j];

                return format_context;
            }
        }
    }

    LOGW("no valid configuration found for camera %s!", prop->name);
    return NULL;
}

/*
* If guest would like to find the camera capabilities before streaming. 
* This function would be called to find the basic information of the camera
*/
static void set_camera_capabilties(CameraProp *prop)
{
    AVFormatContext *format_context = NULL;
    int stream_index = -1;
    char error_msg[256] = {0};
    int ret;

    format_context = open_camera(prop);
    if (format_context == NULL) {
        return;
    }

    ret = avformat_find_stream_info(format_context, NULL);
    if (ret < 0) {
        av_strerror(ret, error_msg, 256);
        LOGE("avformat_find_stream_info failed: %s", error_msg);
        return;
    }

    stream_index = av_find_best_stream(format_context, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (stream_index == -1) {
        LOGE("no video stream found");
        return;
    }

    AVCodecParameters *codecpar = format_context->streams[stream_index]->codecpar;

    prop->width = codecpar->width;
    prop->height = codecpar->height;
    prop->max_width = prop->width;
    prop->max_height = prop->height;
    // step_width and step_height are used to specified the step size of the capture window if the resolutions are in a array of same step
    // for example, if the resolutions are 640x480, 1280x720, 1920x1080, then step_width = 640, step_height = 480
    prop->step_width = 2;
    prop->step_height = 2;
    prop->line_stride = prop->width * 2;

    LOGI("set camera %d capabilities: width %u height %u fps %d", prop->camera_id, prop->width, prop->height, prop->frame_interval_den / prop->frame_interval_num);

    // clean up av stuff
    avformat_close_input(&format_context);
    avformat_free_context(format_context);
}

/**
 * notify guest driver that there is a buffer ready for display
 * assumes guest HAL keeps the original buffer order, so the available buffer index is the same as the buffer index in queue
*/
static void camera_codec_notify(DCodecComponent *context, CodecCallbackData ccd) {
    static int ready_cnt = 1;

    LOGD("camera codec callback event %x data1 %d data2 %d ptr %" PRIx64 " flags %x extra %u", ccd.event, ccd.data1, ccd.data2, ccd.data, ccd.flags, ccd.extra);

    if (ccd.event != OMX_EventFillBufferDone) {
        return;
    }

    if (set_express_device_irq((Device_Context *)context->mAppPrivate, ready_cnt, ccd.extra) == IRQ_SET_OK) {
        ready_cnt = 1;
    }
    else {
        // if failed to set the irq, cache the ready buffer count. will retried to set it up in next frame
        LOGW("warning: camera device irq lost! This may cause deadlocks.");
        ready_cnt++;
    }
}

static void *camera_capturing_thread(void *opaque)
{
    Camera_Context *context = (Camera_Context *)opaque;
    CameraProp * prop = &g_array_index(g_camera_list, CameraProp, context->camera_id);
    AVFormatContext *format_context = NULL;
    AVPacket packet;
    int stream_index = -1;
    int ret;

    format_context = open_camera(prop);
    if (format_context == NULL) {
        LOGE("error: cannot open camera!");
        return NULL;
    }

    if (avformat_find_stream_info(format_context, NULL) < 0) {
        LOGE("error: Could not find stream information");
        return NULL;
    }

    stream_index = av_find_best_stream(format_context, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);

    if (stream_index < 0) {
        LOGE("error: Could not find video stream");
        return NULL;
    }

    AVCodecParameters *codecpar = format_context->streams[stream_index]->codecpar;
    DCodecComponent *codec = dcodec_vdec_init_component(OMX_VIDEO_CodingAutoDetect, camera_codec_notify);
    if (!codec) {
        LOGE("error: codec init failed!");
        return NULL;
    }
    codec->mAppPrivate = (uint64_t)context;
    codec->mCtx->pix_fmt = codecpar->format; // rawvideo decoder needs pix_fmt info

    // configure input port settings
    OMX_VIDEO_PARAM_FFMPEGTYPE in_def;
    in_def.nSize = sizeof(OMX_VIDEO_PARAM_FFMPEGTYPE);
    in_def.nPortIndex = CODEC_INPUT_PORT_INDEX;
    in_def.nWidth = codecpar->width;
    in_def.nHeight = codecpar->height;
    in_def.eCodecId = codecpar->codec_id;
    codec->set_parameter(codec, OMX_IndexParamVideoFFmpeg, &in_def);

    // configure output port settings
    OMX_VIDEO_DCODECDEFINITIONTYPE out_def;
    out_def.nPortIndex = CODEC_OUTPUT_PORT_INDEX;
    out_def.nFrameWidth = codecpar->width;
    out_def.nFrameHeight = codecpar->height;
    out_def.eColorFormat = context->guest_pix_fmt;
    out_def.bLowLatency = OMX_TRUE;
    codec->set_parameter(codec, OMX_IndexParamVideoDcodecDefinition, &out_def);

    codec->mCtx->pkt_timebase = format_context->streams[stream_index]->time_base;

    g_async_queue_ref(context->frame_queue);

    while (context->status == CAMERA_STATUS_STREAMING) {
        // allocate some time slice to the decoder for faster buffer retrieval
        dcodec_process_buffers(codec);

        // then read frame
        ret = av_read_frame(format_context, &packet);
        if (ret < 0) {
            if (ret != AVERROR(EAGAIN)) {
                char error_msg[256] = {0};
                av_strerror(ret, error_msg, 256);
                LOGE("av_read_frame failed with %d: %s", ret, error_msg);
            }
            usleep(100);
            continue;
        }
        if (packet.stream_index != stream_index) {
            LOGW("camera input packet stream_index %d vs. %d not equal!", packet.stream_index, stream_index);
        }

        // obtained frame from camera (through ffmpeg) 
        LOGD("obtained frame from camera!");

        // send output buffer first
        // there need to be empty buffers in the camera's bufferqueue
        BufferDesc *desc = (BufferDesc *)g_async_queue_try_pop(context->frame_queue);
        if (desc == NULL) {
            av_packet_unref(&packet);
            continue;
        }
        LOGD("processing output buffer with desc id %" PRIx64, desc->id);
        // get a buffer from bufferqueue
        dcodec_process_this_buffer(codec, desc);

        // send input packet to codec
        desc = g_malloc(sizeof(BufferDesc));
        desc->type = CODEC_BUFFER_TYPE_INPUT | CODEC_BUFFER_TYPE_AVPACKET;
        desc->id = (uint64_t)(uintptr_t)desc;
        desc->data = &packet;
        desc->nAllocLen = packet.size;
        desc->nFilledLen = packet.size;
        desc->nOffset = 0;
        desc->nTimeStamp = packet.pts;
        desc->nFlags = OMX_BUFFERFLAG_ENDOFFRAME;
        
        //decode and put the result into buffer
        dcodec_process_this_buffer(codec, desc);
        av_packet_unref(&packet);
    }

    // clean up
    avformat_close_input(&format_context);
    avformat_free_context(format_context);

    codec->destroy_component(codec);
    g_async_queue_unref(context->frame_queue);

    return NULL;
}

/*
* list all the cameras.
*/
int list_cameras(void)
{
    av_log_set_level(CAMERA_LOG_LEVEL);

    if (g_camera_list != NULL) {
        LOGW("g_camera_list not null, refreshing cameras.");
        g_array_unref(g_camera_list);
        g_camera_list = NULL;
        g_camera_count = 0;
    }

    AVDeviceInfoList *device_info_list = av_mallocz(sizeof(AVDeviceInfoList));
    avdevice_register_all();

#ifdef __APPLE__
    listAvfoundationDevices(device_info_list);
#else
    // DirectShow is the windows camera driver, only works on windows
    const AVInputFormat *input_format = av_find_input_format(SYSTEM_LIBRARY_NAME);
    if (!input_format) {
        LOGE("av_find_input_format failed");
        return 0;
    }

    int ret = avdevice_list_input_sources(input_format, NULL, NULL, &device_info_list);
    if (ret < 0) {
        LOGE("avdevice_list_input_sources failed %d", ret);
        return 0;
    }

#endif
    // allocate the g_camera_list base on the number of video devices
    g_camera_list = g_array_new(false, true, sizeof(CameraProp));
    for (int i = 0; i < device_info_list->nb_devices; ++i) {
        AVDeviceInfo *device_info = device_info_list->devices[i];
        for(int j = 0; j < device_info->nb_media_types; ++j) {
            if (device_info->media_types[j] == AVMEDIA_TYPE_VIDEO) {
                CameraProp prop;
                memset(&prop, 0, sizeof(CameraProp));
#ifdef __APPLE__
                snprintf(prop.name, sizeof(prop.name), "%s", device_info->device_name);
#else
                snprintf(prop.name, sizeof(prop.name), "%s", device_info->device_description);
#endif
                prop.camera_id = g_camera_count;

                set_camera_capabilties(&prop);

                LOGI("discovered camera id %d: %s", g_camera_count, prop.name);
                g_array_append_val(g_camera_list, prop);
                ++g_camera_count;

                break;
            }
        }
    }

    LOGI("list_cameras: found %d cameras", g_camera_count);

    return g_camera_count;
}

static void camera_output_call_handle(struct Thread_Context *context, Teleport_Express_Call *call)
{

    Camera_Context *camera_context = &(((Camera_Thread_Context *)context)->ctx);
    Call_Para all_para[6];
    get_para_from_call(call, all_para, MAX_PARA_NUM);
    int camera_id = camera_context->camera_id;

    if(camera_context == NULL) {
        LOGE("error! camera context is null!");
        return;
    }

    if (camera_context->camera_id != ((Camera_Thread_Context *)context)->camera_id) {
        LOGE("error! inconsistent camera_id between camera context and thread context!");
    }

    unsigned int fun_id = GET_FUN_ID(call->id);

    LOGD("express_camera received call id %u", fun_id);

    switch (fun_id)
    {
    case CAMERA_FUN_GET_CAMERA_COUNT:
    {
        write_to_guest_mem(all_para[0].data, &g_camera_count, 0, sizeof(int));
    }
    break;
    case CAMERA_FUN_START_STREAM:
    {
        int need_free = 0;
        char *params = (char *)call_para_to_ptr(all_para[1], &need_free);
        camera_context->guest_pix_fmt = pixel_format_v4l2_to_omx(*(uint32_t *)params);
        if (need_free) {
            g_free(params);
        }
        if (camera_context->guest_pix_fmt == 0) {
            LOGE("error! cannot start stream when camera format is unknown");
        }
        else if (camera_context->status == CAMERA_STATUS_IDLE)
        {
            LOGI("camera id %d start stream", camera_id);
            qemu_thread_create(&camera_context->stream_thread, "camera_capturing_thread", camera_capturing_thread, camera_context, QEMU_THREAD_JOINABLE);
            camera_context->status = CAMERA_STATUS_STREAMING;
        }
        else {
            LOGE("error! cannot start stream when camera is not in idle state (current %d)!", camera_context->status);
        }
    }
    break;
    case CAMERA_FUN_STOP_STREAM:
    {
        int need_free = 0;
        char *params = (char *)call_para_to_ptr(all_para[0], &need_free);

        if (camera_context->status == CAMERA_STATUS_STREAMING)
        {
            LOGI("camera id %d stop stream", camera_id);
            camera_context->status = CAMERA_STATUS_IDLE;
            qemu_thread_join(&camera_context->stream_thread);
        }
        else {
            LOGE("error! cannot stop stream when camera is not streaming!");
        }

        while (g_async_queue_length(camera_context->frame_queue) != 0) {
            g_async_queue_pop(camera_context->frame_queue);
        }
        camera_context->guest_pix_fmt = 0;

        if (need_free) {
            g_free(params);
        }
    }
    break;
    case CAMERA_FUN_GET_PROP:
    {
        int need_free = 0;
        char *params = (char *)call_para_to_ptr(all_para[0], &need_free);
        CameraProp *prop = &g_array_index(g_camera_list, CameraProp, camera_id);

        write_to_guest_mem(all_para[1].data, prop, 0, sizeof(CameraProp));

        if (need_free) {
            g_free(params);
        }
    }
    break;
    case CAMERA_FUN_QUEUE_BUFFER:
    {
        int need_free = 0;
        char *params = (char *)call_para_to_ptr(all_para[0], &need_free);

        BufferDesc *desc = g_malloc0(sizeof(BufferDesc));
        desc->type = CODEC_BUFFER_TYPE_OUTPUT | CODEC_BUFFER_TYPE_GUEST_MEM;
        desc->id = *(uint64_t *)params;
        desc->data = copy_guest_mem_from_call(call, 2);
        desc->nAllocLen = all_para[1].data_len;

        g_async_queue_push(camera_context->frame_queue, (gpointer)desc);

        if (need_free) {
            g_free(params);
        }
    }
    break;
    case CAMERA_FUN_QUEUE_BUFFER_HW:
    {
        int need_free = 0;
        char *params = (char *)call_para_to_ptr(all_para[0], &need_free);
        CameraProp *prop = &g_array_index(g_camera_list, CameraProp, camera_id);

        BufferDesc *desc = g_malloc0(sizeof(BufferDesc));
        desc->type = CODEC_BUFFER_TYPE_OUTPUT | CODEC_BUFFER_TYPE_GBUFFER;
        desc->id = *(uint64_t *)params;
        desc->sync_id = *(int *)(params + 8);
        desc->nAllocLen = av_image_get_buffer_size(pixel_format_omx_to_av(camera_context->guest_pix_fmt), prop->width, prop->height, 1);

        // guest dequeue buffer, host camera queue buffer into frame_queue. 
        LOGD("guest dequeue buffer, host camera queue buffer into frame_queue with id %" PRIx64, desc->id);

        g_async_queue_push(camera_context->frame_queue, (gpointer)desc);

        if (need_free) {
            g_free(params);
        }
    }
    break;
    default:
    {
        LOGE("unknown camera function!");
    }
    break;
    }

    call->callback(call, 1);
}

static Thread_Context *get_camera_thread_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    if (g_camera_thread_contexts_map == NULL)
    {
        // first time thread safety?
        g_camera_thread_contexts_map = g_hash_table_new(g_direct_hash, g_direct_equal);
        list_cameras();
    }

    Thread_Context *context = (Thread_Context *)g_hash_table_lookup(g_camera_thread_contexts_map, GUINT_TO_POINTER(unique_id));
    // 没有context就新建线程
    if (context == NULL)
    {
        LOGI("create new thread context");
        context = thread_context_create(thread_id, device_id, sizeof(Camera_Thread_Context), info);
        Camera_Thread_Context *c_context = (Camera_Thread_Context *)context;
        c_context->camera_id = (int)unique_id;

        c_context->ctx.camera_id = (int)unique_id;
        c_context->ctx.status = CAMERA_STATUS_IDLE;
        c_context->ctx.frame_queue = g_async_queue_new_full(dcodec_free_buffer_desc);

        g_hash_table_insert(g_camera_thread_contexts_map, GUINT_TO_POINTER(unique_id), (gpointer)context);
    }
    return context;
}

static bool remove_camera_thread_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    Thread_Context *context = (Thread_Context *)g_hash_table_lookup(g_camera_thread_contexts_map, GUINT_TO_POINTER(unique_id));

    if (context != NULL) {
        Camera_Thread_Context *c_context = (Camera_Thread_Context *)context;
        g_async_queue_unref(c_context->ctx.frame_queue);
    }

    // g_hash_table_remove(g_camera_thread_contexts_map, GUINT_TO_POINTER(unique_id));
    return true;
}

static Device_Context *get_camera_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    Camera_Thread_Context * c_context = (Camera_Thread_Context *)g_hash_table_lookup(g_camera_thread_contexts_map, GUINT_TO_POINTER(unique_id));
    if (c_context)
        return &(c_context->ctx.device_context);
    else
        return NULL;
}

static Express_Device_Info express_camera_info = {
    .enable_default = true,
    .name = "express-camera",
    .option_name = "camera",
    .driver_name = "express_camera",
    .device_id = EXPRESS_CAMERA_DEVICE_ID,
    .device_type = INPUT_DEVICE_TYPE | OUTPUT_DEVICE_TYPE,

    .get_device_context = get_camera_context,

    .call_handle = camera_output_call_handle,
    .get_context = get_camera_thread_context,
    .remove_context = remove_camera_thread_context,

    .static_prop = NULL,
    .static_prop_size = 0,
};

EXPRESS_DEVICE_INIT(express_camera, &express_camera_info);