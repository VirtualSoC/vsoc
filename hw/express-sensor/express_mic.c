/**
 * @file express_mic.c
 * @author Cheng en Huang (diandian86@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-12-23
 *
 * @copyright Copyright (c) 2022
 *
 */

// #define STD_DEBUG_LOG

#include "hw/express-sensor/express_mic.h"
// https://github.com/mackron/miniaudio a pure header implemented audio library
#define MINIAUDIO_IMPLEMENTATION
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_NODE_GRAPH
#define MA_NO_ENGINE
#define MA_NO_GENERATION
#include "hw/express-sensor/miniaudio.h"
#include <sys/time.h>

#define MAX_AUDIO_BUFFER_SIZE 32768
typedef struct Express_Mic_Data
{
    char buf[MAX_AUDIO_BUFFER_SIZE];
    int size;
} __attribute__((packed, aligned(4))) Express_Mic_Data;

typedef struct Mic_Context
{
    Device_Context device_context;
    Express_Mic_Data data;
    struct timeval last_send_time;
    Guest_Mem *guest_buffer;
    Teleport_Express_Call *irq_call;
    ma_decoder decoder;
    ma_decoder_config decoder_config;
    ma_device dev;
    ma_device_config dev_config;
    bool from_file;
    bool need_sync;
} Mic_Context;

static Mic_Context static_mic_context = {
    .data = {
        .buf = {0},
        .size = 0},
    .from_file = false,
    .last_send_time.tv_usec = 0,
    .last_send_time.tv_sec = 0};

static void data_callback_from_file(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
    ma_decoder_read_pcm_frames(&static_mic_context.decoder, pOutput, frameCount, NULL);
    express_mic_status_changed(pOutput, frameCount * 2 * 2);
    sync_express_mic_status();
    (void)pOutput;
    (void)pInput;
}

static void data_callback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
    express_mic_status_changed(pInput, frameCount * 2 * 2);
    sync_express_mic_status();
    (void)pOutput;
}

int start_capture_from_file(char *path)
{
    ma_result result;
    static_mic_context.decoder_config = ma_decoder_config_init(ma_format_s16, 2, 44100);

    result = ma_decoder_init_file(path, &static_mic_context.decoder_config, &static_mic_context.decoder);
    if (result != MA_SUCCESS)
    {
        LOGE("Could not load file: %s", path);
        return -2;
    }
    static_mic_context.dev_config = ma_device_config_init(ma_device_type_playback);
    static_mic_context.dev_config.capture.format = static_mic_context.decoder.outputFormat;
    static_mic_context.dev_config.capture.channels = static_mic_context.decoder.outputChannels;
    static_mic_context.dev_config.sampleRate = static_mic_context.decoder.outputSampleRate;
    static_mic_context.dev_config.dataCallback = data_callback_from_file;

    result = ma_device_init(NULL, &static_mic_context.dev_config, &static_mic_context.dev);
    if (result != MA_SUCCESS)
    {
        LOGE("Failed to initialize capture device.");
        ma_decoder_uninit(&static_mic_context.decoder);
        return -2;
    }

    result = ma_device_start(&static_mic_context.dev);
    if (result != MA_SUCCESS)
    {
        ma_device_uninit(&static_mic_context.dev);
        ma_decoder_uninit(&static_mic_context.decoder);
        LOGE("Failed to start device.");
        return -3;
    }
    static_mic_context.from_file = true;
    return 0;
}

int start_capture(void)
{
    ma_result result;
    static_mic_context.dev_config = ma_device_config_init(ma_device_type_capture);
    static_mic_context.dev_config.capture.format = ma_format_s16;
    static_mic_context.dev_config.capture.channels = 2;
    static_mic_context.dev_config.sampleRate = 44100;
    static_mic_context.dev_config.dataCallback = data_callback;

    result = ma_device_init(NULL, &static_mic_context.dev_config, &static_mic_context.dev);
    if (result != MA_SUCCESS)
    {
        LOGE("Failed to initialize capture device.");
        return -2;
    }

    result = ma_device_start(&static_mic_context.dev);
    if (result != MA_SUCCESS)
    {
        ma_device_uninit(&static_mic_context.dev);
        LOGE("Failed to start device.");
        return -3;
    }
    static_mic_context.from_file = false;
    return 0;
}

void stop_capture(void)
{
    ma_device_uninit(&static_mic_context.dev);
    if (static_mic_context.from_file)
    {
        ma_decoder_uninit(&static_mic_context.decoder);
    }
}

void express_mic_status_changed(const void *buf, int size)
{
    struct timeval now;
    if (size > MAX_AUDIO_BUFFER_SIZE)
    {
        size = MAX_AUDIO_BUFFER_SIZE;
    }
    memcpy(static_mic_context.data.buf, buf, size);
    static_mic_context.data.size = size;
    static_mic_context.need_sync = true;
    gettimeofday(&now, NULL);
    LOGD("frame time : %ld , data size: %d",
           (now.tv_sec - static_mic_context.last_send_time.tv_sec) * 1000000 + now.tv_usec - static_mic_context.last_send_time.tv_usec,
           size);
    static_mic_context.last_send_time = now;
}

void sync_express_mic_status(void)
{
    if (!static_mic_context.device_context.irq_enabled)
    {
        return;
    }

    if (!static_mic_context.need_sync)
    {
        return;
    }

    write_to_guest_mem(static_mic_context.guest_buffer, &(static_mic_context.data), 0, sizeof(Express_Mic_Data));

    static_mic_context.need_sync = false;

    // printf("mic irq send ok\n");

    set_express_device_irq((Device_Context *)&static_mic_context, 0, sizeof(Express_Mic_Data));
}

static void mic_buffer_register(Guest_Mem *data, uint64_t thread_id, uint64_t process_id, uint64_t unique_id)
{
    if (static_mic_context.guest_buffer != NULL)
    {
        free_copied_guest_mem(static_mic_context.guest_buffer);
    }
    LOGI("mic register buffer");
    static_mic_context.guest_buffer = data;
}

static Device_Context *get_mic_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    return (Device_Context *)&static_mic_context;
}

static Express_Device_Info express_mic_info = {
    .enable_default = true,
    .name = "express-mic",
    .option_name = "mic",
    .driver_name = "express_mic",
    .device_id = EXPRESS_MICROPHONE_DEVICE_ID,
    .device_type = INPUT_DEVICE_TYPE,

    .get_device_context = get_mic_context,
    .buffer_register = mic_buffer_register,

};

EXPRESS_DEVICE_INIT(express_mic, &express_mic_info)
