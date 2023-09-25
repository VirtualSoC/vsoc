/**
 * @file express_camera.c
 * @author Chengen Huang 
 * @brief
 * @version 0.1
 * @date 2023-05-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#define STD_DEBUG_LOG

#include "hw/express-camera/express_camera.h"
#include "hw/express-gpu/express_gpu_render.h"
#include "hw/express-gpu/egl_surface.h"
#include "hw/express-gpu/glv3_context.h"
#include "hw/express-gpu/glv3_status.h"


#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libavdevice/avdevice.h"

#define CAMERA_FUN_GET_CAMERA_COUNT 1
#define CAMERA_FUN_START_STREAM 2
#define CAMERA_FUN_STOP_STREAM 3
#define CAMERA_FUN_QUEUE_BUFFER 4
#define CAMERA_FUN_QUEUE_BUFFER_HW 5
#define CAMERA_FUN_GET_PROP 6
#define CAMERA_FUN_MAXID CAMERA_FUN_GET_PROP

#define MAX_CAPTURE_FPS 30
#define SHOW_CAMERA_CAPTURE_WINDOW 1
#define CAMERA_LOG_LEVEL AV_LOG_FATAL
//#define CAMERA_LOG_LEVEL AV_LOG_DEBUG

#ifdef __WIN32__
// Directshow only works on windows
#define SYSTEM_LIBRARY_NAME "dshow"
#define HW_ACCELERATION AV_HWDEVICE_TYPE_D3D11VA
#else // todo: macOS/linux library name
#define SYSTEM_LIBRARY_NAME "avfoundation"
#endif

#ifdef __APPLE__
#include <dispatch/dispatch.h>
#define THREAD_CONTROL_BEGIN \
dispatch_sync(dispatch_get_main_queue(), ^{ 
#define THREAD_CONTROL_END \
}); 
#endif

enum Camera_Status { CAMERA_STATUS_IDLE, CAMERA_STATUS_STREAMING };
enum Buffer_Type { CAMERA_BUFFER_SW, CAMERA_BUFFER_HW };

typedef struct BufferDesc {
    enum Buffer_Type type;
    uint64_t id;
    Guest_Mem *ptr;
} BufferDesc;

typedef struct CameraProp {
    int camera_id;
    char name[64];

    unsigned int pixel_format;
    union {
		int width;
		int min_width;
	};
	union {
		int height;
		int min_height;
	};
	int max_width;
	int max_height;
	int step_width;
	int step_height;
	int line_stride;
	int frame_interval_num;
	int frame_interval_den;
} __attribute__((packed, aligned(4))) CameraProp;

typedef struct Camera_Context
{
    Device_Context device_context;

	int camera_id;
    GLFWwindow* window;
    GLuint texture_loc[3];
    GLuint color_type_loc;
    enum Camera_Status status;
    uint8_t *sw_buffer;
    QemuThread stream_thread;
    GAsyncQueue *frame_queue;
} Camera_Context;

typedef struct Camera_Thread_Context
{
    Thread_Context thread_context;
    int camera_id;
    Camera_Context ctx;
} Camera_Thread_Context;

static GArray *camera_list = NULL;
static GHashTable *camera_thread_contexts_map = NULL;
static int camera_count = 0;

//static GLuint programObject;

static void error_callback(int error, const char* description)
{
    LOGE("glfw error: %s", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    THREAD_CONTROL_BEGIN
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    THREAD_CONTROL_END
}

// using OpenGL to convert YUV to RGB
static int init_opengl(Camera_Context *context, int width, int height)
{
    //THREAD_CONTROL_BEGIN
    // inform express-gpu to create shared child window
    context->window = get_native_opengl_context(DGL_CONTEXT_FLAG_INDEPENDENT_MODE_BIT);
    // context->window = glfwCreateWindow(width, height, "Camera Capturing", NULL, NULL);
    if (!context->window) {
        LOGE("create shared child window failed!");
        //return -1;
        exit(-1);
    }
#ifdef __APPLE__
    THREAD_CONTROL_BEGIN
#endif
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_FALSE);
    glfwSetWindowSize(context->window, width, height);
#ifdef __APPLE__
    THREAD_CONTROL_END
#endif
    glfwSetErrorCallback(error_callback);
    glfwSetKeyCallback(context->window, key_callback);
    glfwMakeContextCurrent(context->window);
    glfwSwapInterval(1);
#ifdef __APPLE__
    THREAD_CONTROL_BEGIN
#endif
#ifdef SHOW_CAMERA_CAPTURE_WINDOW
    glfwShowWindow(context->window);
#else
    glfwHideWindow(context->window);
#endif
#ifdef __APPLE__
    THREAD_CONTROL_END
#endif
    glViewport(0, 0, 480, 640);
    // required, since get_native_opengl_context() creates a 1x1 window by default
    
    //glViewport(0, 0, width, height);
    
#ifdef _WIN32
    glDebugMessageCallback(d_debug_message_callback, NULL);
#endif    
    char vShaderStr[] =
    #ifdef _WIN32
        "#version 310 es\n"
    #else
        "#version 330\n"
    #endif
        "layout (location = 0) in vec2 position;\n"
        "layout (location = 1) in vec2 texCoords;\n"
        "out vec2 TexCoords;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);\n"
        "    TexCoords = texCoords;\n"
        "}\n";

    #ifdef __APPLE__
    char fShaderStr[] =
        "#version 310 es\n"
        "#version 330\n"
        "precision mediump float;                     \n"
        "in vec2 TexCoords;\n"
        "out vec4 color;\n"
        "uniform sampler2D texture_yuyv;\n" // 新的 YUYV422 采样器
        "void main(){\n"
        "   vec3 yuv;\n"
        "   vec3 rgb;\n"
        "   vec4 yuyv = texture(texture_yuyv, TexCoords);\n"
        "   yuv.x = yuyv.r;\n"
        "   yuv.y = yuyv.g - 0.5;\n"
        "   yuv.z = yuyv.a - 0.5;\n"
        "   rgb.r = yuv.x + 1.402 * yuv.z;\n"
        "   rgb.g = yuv.x - 0.39465 * yuv.y - 0.58060 * yuv.z;\n"
        "   rgb.b = yuv.x + 2.03211 * yuv.y;\n"
        "   color = vec4(rgb, 1.0);\n"
        "}\n";
    #else
    char fShaderStr[] =
        "#version 310 es\n"
        "precision mediump float;                     \n"
        "in vec2 TexCoords;\n"
        "out vec4 color;\n"
        "uniform int yuv_type;\n"
        "uniform sampler2D texture_y;\n"
        "uniform sampler2D texture_u;\n"
        "uniform sampler2D texture_v;\n"
        "const mat3 YUV2RGB = mat3(1.0, 0.0, 1.13983,\n"
        "                          1.0, -0.39465, -0.58060,\n"
        "                          1.0, 2.03211, 0.0);\n"
        "void main(){\n"
        "   vec3 yuv;\n"
        "   vec3 rgb;\n"
        "   // yuv_type 13 is AV_PIX_FMT_YUVJ422P, and AV_PIX_FMT_YUVJ420P is 12\n"
        "   if(yuv_type == 13){\n"
        "       // Sample the Y, U, and V textures at the current texcoords. \n"
        "       yuv.x = texture(texture_y, TexCoords).r;\n"
        "       yuv.y = texture(texture_u, TexCoords).r - 0.5;\n"
        "       yuv.z = texture(texture_v, TexCoords).r - 0.5;\n"
        "       // Convert YUV to RGB using BT.601 coefficients \n"
        "       rgb.r = yuv.x + 1.402 * yuv.z;\n"
        "       rgb.g = yuv.x - 0.34414 * yuv.y - 0.71414 * yuv.z;\n"
        "       rgb.b = yuv.x + 1.772 * yuv.y;\n"
        "   } else if(yuv_type == 12){\n"
        "       yuv.x = texture(texture_y, TexCoords).r;\n"
        "       yuv.y = texture(texture_u, TexCoords).r - 0.5;\n"
        "       yuv.z = texture(texture_u, TexCoords).g - 0.5;\n"
        "       rgb = yuv * YUV2RGB;\n" 
        "   }"
        "   color = vec4(rgb,1.0);\n"
        "}\n";
    #endif

    GLuint programObject = glCreateProgram();
    if (programObject == 0)
    {
        #ifdef _WIN32
        return -1;
        #else
        exit(-1);
        #endif
    }

    
    LOGI("going to compile shader!");
    GLuint vertexShader = load_shader(GL_VERTEX_SHADER, vShaderStr);
    GLuint fragmentShader = load_shader(GL_FRAGMENT_SHADER, fShaderStr);
    LOGI("loaded shader %d %d",vertexShader,fragmentShader);
    if(vertexShader == -1 || fragmentShader == -1){
        //return -1;
        exit(-1);
    }
        

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    glLinkProgram(programObject);

    GLint linked;
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        //return -1;
        exit(-1);
    }
    //THREAD_CONTROL_END
    GLfloat quadVertices[] = {// Vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
                              // Positions   // TexCoords
                              -1.0f, 1.0f, 0.0f, 1.0f,
                              -1.0f, -1.0f, 0.0f, 0.0f,
                              1.0f, -1.0f, 1.0f, 0.0f,

                              -1.0f, 1.0f, 0.0f, 1.0f,
                              1.0f, -1.0f, 1.0f, 0.0f,
                              1.0f, 1.0f, 1.0f, 1.0f};

    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    //THREAD_CONTROL_END
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));

    //开启透明度混合后，默认不开透明度的线程的绘制结果对应的texture的透明度默认为0，叠加上去后会导致透明，看不到东西
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(programObject);

    glClearColor(0, 0, 0, 1);

#ifdef _WIN32
    context->texture_loc[0] = glGetUniformLocation(programObject, "texture_y");
    context->texture_loc[1] = glGetUniformLocation(programObject, "texture_u");
    context->texture_loc[2] = glGetUniformLocation(programObject, "texture_v");
    context->color_type_loc = glGetUniformLocation(programObject, "yuv_type");
#endif

    return 0;
}

/* this function is called by ffmpeg to draw each frame and convert yuv422 to rgb color space
* input_frame: the ffmpeg frame to be drawn and converted
* output_buffer: the buffer to store the converted rgb data
*/
static int opengl_yuv2rgb(Camera_Context *context, AVFrame *input_frame, BufferDesc *desc){
    // glViewport(0,0, input_frame->width, input_frame->height);
    // in ffmpeg YUV422 frame, the Y, U and V data are stored in data[0], data[1] and data[2] respectively
    // and the linesize[0], linesize[1] and linesize[2] are the size of each line in data[0], data[1] and data[2] respectively
    // linessize[1] and linsssize[2] are the same, and linessize[0] is twice as large as linessize[1] and linessize[2]

#ifdef __APPLE__
    // uint8_t *uyvyData = frame->data[0];
    // int width = frame->width;
    // int height = frame->height;

    // for (int y = 0; y < height; y++) {
    //     for (int x = 0; x < width; x += 2) {
    //         int offset = y * frame->linesize[0] + x * 2;

    //         uint8_t u = uyvyData[offset];     // U 值
    //         uint8_t y1 = uyvyData[offset + 1]; // Y1 值
    //         uint8_t v = uyvyData[offset + 2];  // V 值
    //         uint8_t y2 = uyvyData[offset + 3]; // Y2 值

    //         // 在这里可以使用 UYVY422 数据执行相关操作
    //     }
    // }

    // 创建一个纹理
    GLuint textureYUV;
    glGenTextures(1, &textureYUV);

    // 绑定并设置 YUV 纹理
    glBindTexture(GL_TEXTURE_2D, textureYUV);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, input_frame->linesize[0], input_frame->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, input_frame->data[0]);
#else

    glUniform1i(context->color_type_loc, input_frame->format);
    GLuint texture[3];
    glActiveTexture(GL_TEXTURE1);
    glGenTextures(3, texture);

    // Set up Y texture parameters
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glUniform1i(context->texture_loc[0], 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, input_frame->linesize[0], input_frame->height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, input_frame->data[0]);

    // In YUV420, the U and V data is stored in data[1]. But in YUV422, data[1] only contains U data
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glUniform1i(context->texture_loc[1], 2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, input_frame->linesize[1], input_frame->height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, input_frame->data[1]);

    // In YUV422, U and V store seperately, so we needs to set up V texture parameters
    if(input_frame->format == AV_PIX_FMT_YUVJ422P)
    {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture[2]);
        glUniform1i(context->texture_loc[2], 3);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, input_frame->linesize[2], input_frame->height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, input_frame->data[2]);
    }
#endif

    glClear(GL_COLOR_BUFFER_BIT);


    if (desc->type == CAMERA_BUFFER_HW) {
        // draw to gbuffer data_fbo
        Graphic_Buffer *gbuffer = get_gbuffer_from_global_map(desc->id);

        if (gbuffer == NULL)
        {
            CameraProp *prop = &g_array_index(camera_list, CameraProp, context->camera_id);
            #ifdef __APPLE__
            prop->width=640;
            prop->height=480;
            #endif
            LOGI("camera %d create g_buffer with gbuffer_id %llx width %d height %d", context->camera_id, desc->id, prop->width, prop->height);
            gbuffer = create_gbuffer(prop->width, prop->height, 0 /* sampler num */,
                          GL_RGB,
                          GL_UNSIGNED_SHORT_5_6_5_REV,
                          GL_RGB565,
                          0,
                          0,
                          desc->id);

            add_gbuffer_to_global(gbuffer);
        }

        if (gbuffer->data_fbo == 0) {
            // create gbuffer data_fbo
            glGenFramebuffers(1 /* num */, &gbuffer->data_fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, gbuffer->data_fbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gbuffer->data_texture, 0);
            gbuffer->has_connected_fbo = 1;

            // check data_fbo status
            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE)
            {
                LOGE("error! camera gbuffer framebuffer not complete! status %x gl error %x ", status, glGetError());

                LOGI("data texture %d: ", gbuffer->data_texture);
                glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);
                int dims[2] = {0, 0};
                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, dims);
                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, dims + 1);
                LOGI("width %d height %d ", dims[0], dims[1]);
                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, dims);
                LOGI("internal_format %x", dims[0]);
                
                LOGI("");
                glBindTexture(GL_TEXTURE_2D, 0);
                return 1;
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, gbuffer->data_fbo);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // todo: use gbuffer sync lock instead of glFinish()
        glFinish();
    }

    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    if (desc->type == CAMERA_BUFFER_SW) {
        glReadPixels(0, 0, input_frame->width, input_frame->height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, context->sw_buffer);
    }
    else {
        // draw again, to the debug window
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glfwSwapBuffers(context->window);

    // GLfloat colorValue[4];
    // GLfloat rgbValue[3];
    // GLfloat yuvValue[3];
    // glGetUniformfv(programObject, context->color_loc, colorValue);
    // glGetUniformfv(programObject, context->rgb_loc, rgbValue);
    // glGetUniformfv(programObject, context->yuv_loc, yuvValue);
    // // 输出color的值
    // LOGI("Color value: %f, %f, %f, %f", colorValue[0], colorValue[1], colorValue[2], colorValue[3]);
    // LOGI("Color value: %f, %f, %f", rgbValue[0], rgbValue[1], rgbValue[2]);
    // LOGI("Color value: %f, %f, %f", yuvValue[0], yuvValue[1], yuvValue[2]);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        LOGE("gl error %x!", error);
        return 1;
    }
    return 0;
}

/*
* If guest would like to find the camera capabilities before streaming. 
* This function would be called to find the basic information of the camera
*/
static void set_camera_capabilties(CameraProp *prop)
{
    const AVInputFormat *input_format = NULL;
    AVFormatContext *format_context = NULL;
    AVCodecContext *codec_context = NULL;
    const AVCodec *codec = NULL;
    int stream_index = -1;
    char error_msg[256] = {0};
    int ret;
    
    avformat_network_init();
    avdevice_register_all();

    av_log_set_level(CAMERA_LOG_LEVEL);
    // dshow stands for DirectShow, this only works on Windows
    input_format = av_find_input_format(SYSTEM_LIBRARY_NAME);
    if (!input_format) {
        LOGE("av_find_input_format failed");
        return;
    }

    char ff_name[70];
    if (prop->name[0] == 0) {
        LOGE("error: Camera name is NULL");
        return;
    }

    sprintf(ff_name, "video=%s", prop->name);
#ifdef _WIN32
    ret = avformat_open_input(&format_context, ff_name, input_format, NULL);
#else

    AVDictionary *options = NULL;
    av_dict_set(&options, "framerate", "30", 0);
    av_dict_set(&options, "video_size", "640x480", 0);
    av_dict_set(&options, "pixel_format", "yuyv422", 0);
    char *inputPath = "0";

    ret = avformat_open_input(&format_context, inputPath, input_format, &options);
    av_dict_free(&options);
#endif
    if (ret < 0) {
        av_strerror(ret, error_msg, 256);
        LOGE("avformat_open_input failed: %s %d", error_msg,ret);
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
    codec = avcodec_find_decoder(format_context->streams[stream_index]->codecpar->codec_id);
    if (!codec) {
        LOGE("avcodec_find_decoder failed");
        return;
    }



    codec_context = avcodec_alloc_context3(codec);
    if (!codec_context) {
        LOGE("avcodec_alloc_context3 failed");
        return;
    }
    //codec_context->width = format_context->streams[stream_index]->codecpar->width;
    //codec_context->height = format_context->streams[stream_index]->codecpar->height;
    ret = avcodec_parameters_to_context(codec_context, format_context->streams[stream_index]->codecpar);
    if (ret < 0) {
        LOGE("error: Could not copy codec parameters to context");
        return ;
    }
#ifdef _WIN32
    codec_context->width = 640;
    codec_context->height = 480;
#endif
    if (avcodec_open2(codec_context, codec, NULL) < 0) {
        LOGE("error: Could not open codec");
        return ;
    }
    
    // TODO: support more pixel format and resolution
    // currently only support RGB24 and fixed framesize
    prop->width = codec_context->width;
    prop->height = codec_context->height;
    prop->pixel_format = V4L2_PIX_FMT_RGB565;
    prop->max_width = codec_context->width;
    prop->max_height = codec_context->height;
    // step_width and step_height are used to specified the step size of the capture window if the resolutions are in a array of same step
    // for example, if the resolutions are 640x480, 1280x720, 1920x1080, then step_width = 640, step_height = 480
    prop->step_width = 2;
    prop->step_height = 2;
    prop->line_stride = codec_context->width * 2;
    prop->frame_interval_num = 1;
    prop->frame_interval_den = MAX_CAPTURE_FPS;

    LOGI("Camera capabilities: width %u height %u", codec_context->width, codec_context->height);

    // clean up av stuff
    avcodec_close(codec_context);
    avcodec_free_context(&codec_context);
    avformat_close_input(&format_context);
    avformat_free_context(format_context);
}

static void *camera_capturing_thread(void *opaque)
{
    Camera_Context *context = (Camera_Context *)opaque;
    CameraProp * prop = &g_array_index(camera_list, CameraProp, context->camera_id);
    const AVInputFormat *input_format = NULL;
    AVFormatContext *format_context = NULL;
    AVCodecContext *codec_context = NULL;
    const AVCodec *codec = NULL;
    AVFrame *frame = NULL;
    AVPacket packet;
    AVDictionary *options = NULL;
    int stream_index = -1;
    char error_msg[256] = {0},framerate_str[8],frame_size_str[32];
    int ret,ready_cnt = 1;

    avformat_network_init();
    avdevice_register_all();

    av_log_set_level(CAMERA_LOG_LEVEL);
    // av_dict_set(&options, "list_devices", "true", 0);
    // dshow stands for DirectShow, this only works on Windows
    input_format = av_find_input_format(SYSTEM_LIBRARY_NAME);
    if (!input_format) {
        LOGE("error: Could not find input format");
    #ifdef __APPLE__
        exit(-1);
    #else
        return NULL;
    #endif
    }
    sprintf(framerate_str, "%d", MAX_CAPTURE_FPS);
    sprintf(frame_size_str, "%dx%d", prop->width, prop->height);
    av_dict_set_int(&options, "rtbufsize", 3041280 * 100, 0);
    // must set video_size and r(stands for frame rate)

#ifdef _WIN32
    av_dict_set(&options, "video_size", frame_size_str, 0);
    av_dict_set(&options, "r", framerate_str, 0);
#endif

    char ff_name[70];
    if (prop->name[0] == 0) {
        LOGE("error: Camera name is NULL");
    #ifdef __APPLE__
        exit(-1);
    #else
        return NULL;
    #endif
    }

#ifdef __APPLE__
    sprintf(ff_name, "video=%s", prop->name);
    av_dict_set(&options, "framerate", "30", 0);
    av_dict_set(&options, "video_size", "640x480", 0);
    av_dict_set(&options, "pixel_format", "yuyv422", 0);
    ret = avformat_open_input(&format_context, "0", input_format, &options);
#else
    ret = avformat_open_input(&format_context, ff_name, input_format, &options);
#endif
    if (ret < 0) {
        av_strerror(ret,error_msg,256);
        LOGE("error: Could not open input, %s",error_msg);
    #ifdef __APPLE__
        exit(-1);
    #else
        return NULL;
    #endif
    }
    if (avformat_find_stream_info(format_context, NULL) < 0) {
        LOGE("error: Could not find stream information");
    #ifdef __APPLE__
        exit(-1);
    #else
        return NULL;
    #endif
    }
    av_dump_format(format_context, 0, ff_name, 0);
    
    stream_index = av_find_best_stream(format_context, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);

    if (stream_index < 0) {
        LOGE("error: Could not find video stream");
    #ifdef __APPLE__
        exit(-1);
    #else
        return NULL;
    #endif
    }

    codec = avcodec_find_decoder(format_context->streams[stream_index]->codecpar->codec_id);
    if (!codec) {
        LOGE("error: Could not find decoder");
    #ifdef __APPLE__
        exit(-1);
    #else
        return NULL;
    #endif
    }
    
    codec_context = avcodec_alloc_context3(codec);
    if (!codec_context) {
        LOGE("error: Could not allocate codec context");
    #ifdef __APPLE__
        exit(-1);
    #else
        return NULL;
    #endif
    }
    
    ret = avcodec_parameters_to_context(codec_context, format_context->streams[stream_index]->codecpar);
    if (ret < 0) {
        LOGE("error: Could not copy codec parameters to context");
    #ifdef __APPLE__
        exit(-1);
    #else
        return NULL;
    #endif
    }

    if (avcodec_open2(codec_context, codec, NULL) < 0) {
        LOGE("error: Could not open codec");
    #ifdef __APPLE__
        exit(-1);
    #else
        return NULL;
    #endif
    }

    // allocate frame
    frame = av_frame_alloc();
    if (!frame) {
        LOGE("error: Could not allocate frame");
    #ifdef __APPLE__
        exit(-1);
    #else
        return NULL;
    #endif
    }
    //THREAD_CONTROL_BEGIN
    //init_opengl(context, codec_context->width, codec_context->height);
#ifdef __APPLE__
    codec_context->width=640;
    codec_context->height=480;
    codec_context->pix_fmt = AV_PIX_FMT_YUYV422;
    codec_context->sw_pix_fmt = AV_PIX_FMT_YUYV422;
#endif
    //LOGI("going to init_opengl with %d %d",codec_context->width,codec_context->height);
    ret = init_opengl(context, codec_context->width, codec_context->height);
    if (ret < 0) {
        LOGE("error: Could not initialize OpenGL");
    #ifdef __APPLE__
        exit(-1);
    #else
        return NULL;
    #endif
    }
    //THREAD_CONTROL_END
#ifdef __APPLE__
    THREAD_CONTROL_BEGIN
#endif
    //LOGI("going to show window!%d %d",codec_context->width,codec_context->height);
    glfwShowWindow(context->window);
#ifdef __APPLE__
    THREAD_CONTROL_END
#endif

    // TODO: support more pixel format and resolution
    // currently only support RGB24 and fixed framesize
    prop->width = codec_context->width;
    prop->height = codec_context->height;
    prop->pixel_format = V4L2_PIX_FMT_RGB565;
    prop->max_width = codec_context->width;
    prop->max_height = codec_context->height;
    // step_width and step_height are used to specified the step size of the capture window if the resolutions are in a array of same step
    // for example, if the resolutions are 640x480, 1280x720, 1920x1080, then step_width = 640, step_height = 480
    prop->step_width = 2;
    prop->step_height = 2;
    prop->line_stride = codec_context->width * 2;
    prop->frame_interval_num = 1;
    prop->frame_interval_den = MAX_CAPTURE_FPS;
    // allocate tmp buffer for RGB565
    context->sw_buffer = (uint8_t *)calloc(1,codec_context->width * codec_context->height * 2);
    g_async_queue_ref(context->frame_queue);
    // Read packets from input device
    //THREAD_CONTROL_BEGIN
    while (context->status == CAMERA_STATUS_STREAMING) {
    #ifdef __APPLE__
        THREAD_CONTROL_BEGIN
    #endif
        if (glfwWindowShouldClose(context->window) || glfwGetKey(context->window, GLFW_KEY_ESCAPE)) {
            glfwHideWindow(context->window);
        }
    #ifdef __APPLE__
        THREAD_CONTROL_END
    #endif
        ret = av_read_frame(format_context, &packet);
        if (ret < 0) {
                char error_msg[256] = {0};
                av_strerror(ret, error_msg, 256);
                LOGE("Failed to av_read_frame %d %s",ret,error_msg);
                continue;
        }
        if (packet.stream_index == stream_index) {
            // Decode packet
            ret = avcodec_send_packet(codec_context, &packet);
            if (ret < 0) {
                char error_msg[256] = {0};
                av_strerror(ret, error_msg, 256);
                LOGE("Failed to send packet for decoding %d %s",ret,error_msg);
                continue;
            }
            while (ret >= 0) {
                ret = avcodec_receive_frame(codec_context, frame);
                LOGI("linesize is %d",frame->linesize[0]);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    av_frame_unref(frame);
                    break;
                } else if (ret < 0) {
                    LOGE("Failed to decode packet");
                    av_frame_unref(frame);
                    break;
                }
                LOGI("decode packet!");
                BufferDesc *desc = (BufferDesc *)g_async_queue_try_pop(context->frame_queue);
                // Convert frame to RGB, it could used swscale libraries in FFMPEG for better compatibility
                if(desc == NULL)
                {
                    LOGD("No buffers available in queue. Is the guest too busy?");
                    av_frame_unref(frame);
                    continue;
                } 

                opengl_yuv2rgb(context, frame, desc);
                
                if (desc->type == CAMERA_BUFFER_SW) {
                    Guest_Mem *guest_mem = desc->ptr;
                    assert(guest_mem->all_len >= frame->height * frame->width * 2);
                    LOGI("write sw frame to guest gbuffer_id %" PRIx64 "", desc->id);
                    write_to_guest_mem(guest_mem, context->sw_buffer, 0, guest_mem->all_len);
                    free_copied_guest_mem(guest_mem);
                }

                // Notify guest driver that there is a buffer ready for display
                // assume guest HAL kepp the original buffers order, so the available buffer index is the same as the buffer index in queue
                if(set_express_device_irq((Device_Context *)context, ready_cnt, 0) == IRQ_SET_OK) {
                    ready_cnt = 1;
                } else {
                    // if faild to set the irq, cache the ready buffer count. will retried to set it up in next frame
                    LOGW("warning: camera device irq lost! This may cause deadlocks.");
                    ready_cnt++;
                }
                g_free(desc);
                av_frame_unref(frame);
            }
        }
        
        av_packet_unref(&packet);
    }
    //Clean up
    avcodec_free_context(&codec_context);
    avformat_close_input(&format_context);
    avformat_free_context(format_context);

    LOGI("camera release_native_opengl_context");

#ifdef __WIN32
    THREAD_CONTROL_BEGIN
#endif
    glfwHideWindow(context->window);
#ifdef __WIN32
    THREAD_CONTROL_END
#endif
    glfwMakeContextCurrent(NULL);
   

    release_native_opengl_context(context->window, DGL_CONTEXT_FLAG_INDEPENDENT_MODE_BIT);
    free(context->sw_buffer);
    g_async_queue_unref(context->frame_queue);
    


    return NULL;
}

/*
* This function is used to free the copied guest memory in the queue when stop streaming
*/
static void free_queue_entry(gpointer data)
{
    BufferDesc *desc = (BufferDesc *)data;
    if(desc != NULL)
    {
        if (desc->type == CAMERA_BUFFER_SW && desc->ptr != NULL)
            free_copied_guest_mem(desc->ptr);
        g_free(desc);
    }
    return;
}

/*
* list all the cameras.
*/
int list_cameras(void)
{
#ifdef __APPLE__
    if (camera_list != NULL) {
        LOGW("camera_list not null, refreshing cameras.");
        g_array_unref(camera_list);
        camera_list = NULL;
        camera_count = 0;
    }

    const AVInputFormat *input_format = NULL;
    AVDeviceInfoList *device_info_list = av_mallocz(sizeof(AVDeviceInfoList));
    char error_msg[256] = {0};
    int ret;
    avdevice_register_all();

    // DirectShow is the windows camera driver, only works on windows
    // input_format = av_find_input_format(SYSTEM_LIBRARY_NAME);
    // //printf("format %s\n",input_format->long_name);
    // if (!input_format) {
    //     LOGE("av_find_input_format failed");
    //     return -1;
    // }
    listAvfoundationDevices(device_info_list);

   // printf("number is %d\n",device_info_list->nb_devices);
    //ret = avdevice_list_input_sources(input_format, NULL, NULL, &device_info_list);
    // if (ret < 0) {
    //     av_strerror(ret, error_msg, 256);
    //     LOGE("avdevice_list_input_sources failed: %s %d", error_msg,ret);
    //     return -1;
    // }

    //printf("nubmer of devices is:%d\n",device_info_list->nb_devices);
    // allocate the camera_list base on the number of video devices
    camera_list = g_array_new(false, true, sizeof(CameraProp));
    for (int i = 0; i < device_info_list->nb_devices; ++i) {
        AVDeviceInfo *device_info = device_info_list->devices[i];
        CameraProp prop;
        strcpy(prop.name, device_info->device_name);
        prop.camera_id = camera_count;

        // todo: query the real capabilities of the camera
        set_camera_capabilties(&prop);

        LOGI("discovered camera id %d: %s", camera_count, prop.name);
        g_array_append_val(camera_list, prop);
        ++camera_count;

        break;
    }

    LOGI("list_cameras: found %d cameras.", camera_count);

    return camera_count;
#else
    if (camera_list != NULL) {
        LOGW("camera_list not null, refreshing cameras.");
        g_array_unref(camera_list);
        camera_list = NULL;
        camera_count = 0;
    }

    const AVInputFormat *input_format = NULL;
    AVDeviceInfoList *device_info_list = NULL;
    char error_msg[256] = {0};
    int ret;
    avdevice_register_all();

    // DirectShow is the windows camera driver, only works on windows
    input_format = av_find_input_format(SYSTEM_LIBRARY_NAME);
    if (!input_format) {
        LOGE("av_find_input_format failed");
        return -1;
    }

    ret = avdevice_list_input_sources(input_format, NULL, NULL, &device_info_list);
    if (ret < 0) {
        av_strerror(ret, error_msg, 256);
        LOGE("avdevice_list_input_sources failed: %s", error_msg);
        return -1;
    }

    // allocate the camera_list base on the number of video devices
    camera_list = g_array_new(false, true, sizeof(CameraProp));
    for (int i = 0; i < device_info_list->nb_devices; ++i) {
        AVDeviceInfo *device_info = device_info_list->devices[i];
        for(int j = 0; j < device_info->nb_media_types; ++j) {
            if (device_info->media_types[j] == AVMEDIA_TYPE_VIDEO) {
                // "video=" is the prefix for DirectShow, 
                // if you are using other drivers, you may need to change this
                CameraProp prop;
                strcpy_s(prop.name, sizeof(prop.name), device_info->device_description);
                prop.camera_id = camera_count;

                // todo: query the real capabilities of the camera
                set_camera_capabilties(&prop);

                LOGI("discovered camera id %d: %s", camera_count, prop.name);
                g_array_append_val(camera_list, prop);
                ++camera_count;

                break;
            }
        }
    }

    LOGI("list_cameras: found %d cameras.", camera_count);

    return camera_count;

#endif
}

static void camera_output_call_handle(struct Thread_Context *context, Teleport_Express_Call *call)
{

    Camera_Context *camera_context = &(((Camera_Thread_Context *)context)->ctx);
    Call_Para all_para[6];
    int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
    int camera_id = camera_context->camera_id;

    if(camera_context == NULL) {
        LOGE("error! camera context is null!");
        return;
    }

    if (camera_context->camera_id != ((Camera_Thread_Context *)context)->camera_id) {
        LOGE("error! inconsistent camera_id between camera context and thread context!");
    }

    unsigned int fun_id = GET_FUN_ID(call->id);

    // todo: param len checking

    LOGD("express_camera received call id %u", fun_id);

    switch (fun_id)
    {
    case CAMERA_FUN_GET_CAMERA_COUNT:
    {
        write_to_guest_mem(all_para[0].data, &camera_count, 0, sizeof(int));
    }
    break;
    case CAMERA_FUN_START_STREAM:
    {
        int need_free = 0;
        char *params = (char *)call_para_to_ptr(all_para[0], &need_free);

        if (camera_context->status == CAMERA_STATUS_IDLE)
        {
            LOGI("camera id %d start stream", camera_id);
            camera_context->frame_queue = g_async_queue_new_full(free_queue_entry);
            g_async_queue_ref(camera_context->frame_queue);
            qemu_thread_create(&camera_context->stream_thread, "camera_capturing_thread", camera_capturing_thread, camera_context, QEMU_THREAD_JOINABLE);
            camera_context->status = CAMERA_STATUS_STREAMING;
        }
        
        if (need_free) {
            g_free(params);
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
            g_async_queue_unref(camera_context->frame_queue);
            qemu_thread_join(&camera_context->stream_thread);
        }
        
        if (need_free) {
            g_free(params);
        }
    }
    break;
    case CAMERA_FUN_GET_PROP:
    {
        int need_free = 0;
        char *params = (char *)call_para_to_ptr(all_para[0], &need_free);
        CameraProp *prop = &g_array_index(camera_list, CameraProp, camera_id);

        write_to_guest_mem(all_para[1].data, prop, 0, sizeof(CameraProp));
        
        if (need_free) {
            g_free(params);
        }
    }
    break;
    case CAMERA_FUN_QUEUE_BUFFER:
    {
        int need_free = 0;
        BufferDesc *desc = g_malloc0(sizeof(BufferDesc));
        char *params = (char *)call_para_to_ptr(all_para[0], &need_free);

        desc->type = CAMERA_BUFFER_SW;
        desc->id = *(uint64_t *)params;
        desc->ptr = copy_guest_mem_from_call(call, 2);

        LOGD("queue buffer sw: camera_id %d gbuffer_id %" PRIx64 " guest_mem %p queue_len %d ", camera_id, desc->id, desc->ptr, g_async_queue_length(camera_context->frame_queue));

        g_async_queue_push(camera_context->frame_queue, (gpointer)desc);

        if (need_free) {
            g_free(params);
        }
    }
    break;
    case CAMERA_FUN_QUEUE_BUFFER_HW:
    {
        int need_free = 0;
        BufferDesc *desc = g_malloc0(sizeof(BufferDesc));
        char *params = (char *)call_para_to_ptr(all_para[0], &need_free);

        desc->type = CAMERA_BUFFER_HW;
        desc->id = *(uint64_t *)params;
        desc->ptr = NULL;

        LOGD("queue buffer hw: camera_id %d gbuffer_id %" PRIx64 " queue_len %d ", camera_id, desc->id, g_async_queue_length(camera_context->frame_queue));
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
    LOGI("in get_camera_thread_context");
    if (camera_thread_contexts_map == NULL)
    {
        // first time thread safety?
        camera_thread_contexts_map = g_hash_table_new(g_direct_hash, g_direct_equal);
        list_cameras();
    }

    Thread_Context *context = (Thread_Context *)g_hash_table_lookup(camera_thread_contexts_map, GUINT_TO_POINTER(unique_id));
    // 没有context就新建线程
    if (context == NULL)
    {
        LOGI("create new thread context");
        context = thread_context_create(thread_id, device_id, sizeof(Camera_Thread_Context), info);
        Camera_Thread_Context *c_context = (Camera_Thread_Context *)context;
        c_context->camera_id = (int)unique_id;

        c_context->ctx.camera_id = (int)unique_id;
        c_context->ctx.status = CAMERA_STATUS_IDLE;

        g_hash_table_insert(camera_thread_contexts_map, GUINT_TO_POINTER(unique_id), (gpointer)context);
    }
    return context;
}

static bool remove_camera_thread_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    g_hash_table_remove(camera_thread_contexts_map, GUINT_TO_POINTER(unique_id));
    return true;
}

static Device_Context *get_camera_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    Camera_Thread_Context * c_context = (Camera_Thread_Context *)g_hash_table_lookup(camera_thread_contexts_map, GUINT_TO_POINTER(unique_id));
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