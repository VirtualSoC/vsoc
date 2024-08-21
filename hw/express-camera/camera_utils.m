/**
 * vSoC macOS camera utils
 * 
 * Copyright (c) 2023 Zijie Zhou <zijiezhou017@outlook.com>
 * Copyright (c) 2023 Jiaxing Qiu <jx.qiu@outlook.com>
 */

#include <stdio.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreMedia/CoreMedia.h>
#include <AVFoundation/AVFoundation.h>
#include "hw/express-camera/express_camera.h"
#include <libavdevice/avdevice.h>

#define DEFAULT_FRAME_WIDTH 1280
#define DEFAULT_FRAME_HEIGHT 720

void listAvfoundationDevices(AVDeviceInfoList *deviceList) {
    AVCaptureDeviceDiscoverySession *discoverySession = [AVCaptureDeviceDiscoverySession discoverySessionWithDeviceTypes:@[AVCaptureDeviceTypeExternalUnknown, AVCaptureDeviceTypeBuiltInWideAngleCamera] mediaType:AVMediaTypeVideo position:AVCaptureDevicePositionUnspecified];

    AVAuthorizationStatus status = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];

    switch (status) {
        case AVAuthorizationStatusAuthorized:
            LOGI("摄像头访问权限已授权");
            break;

        case AVAuthorizationStatusNotDetermined:
            LOGI("摄像头访问权限尚未确定，需要请求用户授权");
            // 向用户请求摄像头访问权限
            [AVCaptureDevice requestAccessForMediaType:AVMediaTypeVideo completionHandler:^(BOOL granted) {
                if (granted) {
                    LOGI("用户已授权摄像头访问权限");
                } else {
                    LOGI("用户拒绝了摄像头访问权限");
                }
            }];
            break;

        case AVAuthorizationStatusDenied:
        case AVAuthorizationStatusRestricted:
            LOGI("摄像头访问权限被拒绝或受限制");
            break;
    }

    NSArray *devices = discoverySession.devices;
    deviceList->nb_devices = 0;

    for (AVCaptureDevice *device in devices) {
        LOGI("Device Name '%s' uid %s\n", [device.localizedName UTF8String],[device.uniqueID UTF8String]);

        // select the best format combination
        AVCaptureDeviceFormat *activeFormat = device.activeFormat;
        for (AVCaptureDeviceFormat *f in device.formats) {
            CMTime frameDuration = f.videoSupportedFrameRateRanges.firstObject.minFrameDuration;
            float frameRate = frameDuration.timescale / (float)frameDuration.value;
            CMVideoDimensions videoSize = CMVideoFormatDescriptionGetDimensions(f.formatDescription);
            if (frameRate >= 30 && videoSize.width == DEFAULT_FRAME_WIDTH && videoSize.height == DEFAULT_FRAME_HEIGHT) {
                // 1280x720 is the best (defined by our guest-side config)
                activeFormat = f;
            }
        }

        // 获取帧率信息
        CMTime frameDuration = activeFormat.videoSupportedFrameRateRanges.firstObject.minFrameDuration;
        float frameRate = frameDuration.timescale / (float)frameDuration.value;
        LOGI("摄像头帧率: %.2f", frameRate);

        // 获取视频分辨率信息
        CMVideoDimensions videoSize = CMVideoFormatDescriptionGetDimensions(activeFormat.formatDescription);
        LOGI("摄像头分辨率: %dx%d", videoSize.width, videoSize.height);

        AVDeviceInfo *info = av_mallocz(sizeof(AVDeviceInfo));
        info->device_name = av_mallocz(64);
        info->device_description = av_mallocz(64);
        info->nb_media_types = 1;
        info->media_types = av_mallocz(sizeof(enum AVMediaType));
        info->media_types[0] = AVMEDIA_TYPE_VIDEO;
        snprintf(info->device_name, sizeof(info->device_name), "%s", [device.localizedName UTF8String]);
        snprintf(info->device_description, sizeof(info->device_description), "%s", [device.uniqueID UTF8String]);

        deviceList->devices = av_realloc(deviceList->devices, (deviceList->nb_devices + 1) * sizeof(AVDeviceInfo *));
        deviceList->devices[deviceList->nb_devices] = info;
        deviceList->nb_devices++;
    }
} 