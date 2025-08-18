/**
 * vSoC video codec support for NVIDIA GPUs
 * 
 * Copyright (c) 2024 Jiaxing Qiu <jx.qiu@outlook.com>
 */

// #define STD_DEBUG_LOG
#include "hw/teleport-express/express_log.h"
#include "hw/express-codec/device_cuda.h"

static CudaFunctions *g_cu;
static CUdevice g_cuda_device;
static CUcontext g_cuda_ctx;

typedef CUresult CUDAAPI cuda_check_GetErrorName(CUresult error, const char** pstr);
typedef CUresult CUDAAPI cuda_check_GetErrorString(CUresult error, const char** pstr);

/**
 * Wrap a CUDA function call and print error information if it fails.
 */
static inline int ff_cuda_check(void *avctx,
                                void *cuGetErrorName_fn, void *cuGetErrorString_fn,
                                CUresult err, const char *func)
{
    const char *err_name;
    const char *err_string;

    if (err == CUDA_SUCCESS)
        return 0;

    ((cuda_check_GetErrorName *)cuGetErrorName_fn)(err, &err_name);
    ((cuda_check_GetErrorString *)cuGetErrorString_fn)(err, &err_string);

    LOGE("%s failed", func);
    if (err_name && err_string)
        LOGE(" -> %s: %s", err_name, err_string);

    return AVERROR_EXTERNAL;
}

/**
 * Convenience wrapper for ff_cuda_check when dynamically loading cuda symbols.
 */
#define FF_CUDA_CHECK_DL(avclass, cudl, x) ff_cuda_check(avclass, cudl->cuGetErrorName, cudl->cuGetErrorString, (x), #x)
#define CHECK_CU(x) FF_CUDA_CHECK_DL(NULL, g_cu, x)

typedef struct CsConverterCudaInternal {
    CUgraphicsResource mTextureY;
    CUgraphicsResource mTextureU;
    CUgraphicsResource mTextureV;
} CsConverterCudaInternal;

static void init_global_cu(void) {
    if (g_cu) {
        return;
    }
    // load cuda context
    int ret;
    ret = cuda_load_functions(&g_cu, NULL);
    if (ret < 0) {
        LOGE("could not dynamically load CUDA");
        return;
    }
    CHECK_CU(g_cu->cuDeviceGet(&g_cuda_device, 0));
    CHECK_CU(g_cu->cuDevicePrimaryCtxRetain(&g_cuda_ctx, g_cuda_device));
}

static void init_cuda_priv(CsConverter *conv) {
    CsConverterCudaInternal *private = g_malloc0(sizeof(CsConverterCudaInternal));
    CUcontext dummy;
    conv->private = private;
    CHECK_CU(g_cu->cuCtxPushCurrent(g_cuda_ctx));
    CHECK_CU(g_cu->cuGraphicsGLRegisterImage(&private->mTextureY, conv->mTextureY, GL_TEXTURE_2D, CU_GRAPHICS_REGISTER_FLAGS_WRITE_DISCARD));
    CHECK_CU(g_cu->cuGraphicsGLRegisterImage(&private->mTextureU, conv->mTextureU, GL_TEXTURE_2D, CU_GRAPHICS_REGISTER_FLAGS_WRITE_DISCARD));
    CHECK_CU(g_cu->cuGraphicsGLRegisterImage(&private->mTextureV, conv->mTextureV, GL_TEXTURE_2D, CU_GRAPHICS_REGISTER_FLAGS_WRITE_DISCARD));
    CHECK_CU(g_cu->cuCtxPopCurrent(&dummy));
}

void cs_deinit_cuda(CsConverter *conv) {
    CsConverterCudaInternal *private = conv->private;
    CUcontext dummy;

    if (private == NULL) {
        return;
    }

    CHECK_CU(g_cu->cuCtxPushCurrent(g_cuda_ctx));
    CHECK_CU(g_cu->cuGraphicsUnregisterResource(private->mTextureY));
    CHECK_CU(g_cu->cuGraphicsUnregisterResource(private->mTextureU));
    CHECK_CU(g_cu->cuGraphicsUnregisterResource(private->mTextureV));
    CHECK_CU(g_cu->cuCtxPopCurrent(&dummy));

    g_free(private);
    conv->private = NULL;
}

static void copy_cuda_to_tex(CUgraphicsResource dst, CUdeviceptr src, int pitch, int height) {
    CUarray cuda_array;
    CUcontext dummy;

    CHECK_CU(g_cu->cuCtxPushCurrent(g_cuda_ctx));
    CHECK_CU(g_cu->cuGraphicsMapResources(1, &dst, NULL));
    CHECK_CU(g_cu->cuGraphicsSubResourceGetMappedArray(&cuda_array, dst, 0, 0));

    CUDA_ARRAY_DESCRIPTOR desc;
    CHECK_CU(g_cu->cuArrayGetDescriptor(&desc, cuda_array));
    LOGD("cuarray width %zu height %zu format %d channels %u", desc.Width, desc.Height, desc.Format, desc.NumChannels);

    CUDA_MEMCPY2D cpy = {
        .srcMemoryType = CU_MEMORYTYPE_DEVICE,
        .dstMemoryType = CU_MEMORYTYPE_ARRAY,
        .srcDevice     = src,
        .dstArray      = cuda_array,
        .srcPitch      = pitch,
        .srcY          = 0,
        .srcXInBytes   = 0,
        .dstXInBytes   = 0,
        .WidthInBytes  = desc.Width * desc.NumChannels,
        .Height        = height,
    };

    LOGD("copy_cuda_to_tex dst array %p src %llu pitch %d height %d", cuda_array, src, pitch, height);
    CHECK_CU(g_cu->cuMemcpy2DAsync(&cpy, NULL));

    CHECK_CU(g_cu->cuGraphicsUnmapResources(1, &dst, NULL));
    CHECK_CU(g_cu->cuCtxPopCurrent(&dummy));
}

/**
 * maps data specified in the cuda device ptr to the tex lines.
*/
int cs_map_cuda(CsConverter *conv, CUdeviceptr *data, int *linesize) {
    init_global_cu();

    if (!conv->private) {
        init_cuda_priv(conv);
    }
    if (!conv->private) {
        return ERR_COLORSPACE_FAILED;
    }
    CsConverterCudaInternal *private = conv->private;

    const int height = conv->mHeight;

    // nvenc only supports NV12 output
    CHECK_EQ(conv->mFormat, AV_PIX_FMT_NV12);
    copy_cuda_to_tex(private->mTextureY, data[0], linesize[0], height);
    copy_cuda_to_tex(private->mTextureU, data[1], linesize[1], height / 2);
    return ERR_OK;
}

/**
 * copies an opengl texture to CUdeviceptr.
*/
void copy_tex_to_cuda(CUdeviceptr dst, GLuint src_tex, int *linesize) {
    init_global_cu();
    CUgraphicsResource cuda_res;
    CUarray cuda_array;
    CUcontext dummy;

    CHECK_CU(g_cu->cuCtxPushCurrent(g_cuda_ctx));
    CHECK_CU(g_cu->cuGraphicsGLRegisterImage(&cuda_res, src_tex, GL_TEXTURE_2D, CU_GRAPHICS_REGISTER_FLAGS_READ_ONLY));
    CHECK_CU(g_cu->cuGraphicsMapResources(1, &cuda_res, NULL));
    CHECK_CU(g_cu->cuGraphicsSubResourceGetMappedArray(&cuda_array, cuda_res, 0, 0));

    CUDA_ARRAY_DESCRIPTOR desc;
    CHECK_CU(g_cu->cuArrayGetDescriptor(&desc, cuda_array));
    LOGD("cuarray width %zu height %zu format %d channels %u", desc.Width, desc.Height, desc.Format, desc.NumChannels);

    CUDA_MEMCPY2D cpy = {
        .srcMemoryType = CU_MEMORYTYPE_ARRAY,
        .dstMemoryType = CU_MEMORYTYPE_DEVICE,
        .srcArray      = cuda_array,
        .dstDevice     = dst,
        .dstPitch      = linesize[0], /* todo */
        .srcY          = 0,
        .srcXInBytes   = 0,
        .dstXInBytes   = 0,
        .WidthInBytes  = desc.Width * desc.NumChannels,
        .Height        = desc.Height,
    };

    LOGD("copy_tex_to_cuda tex %u -> devptr %p pitch %d height %d", src_tex, dst, linesize[0], desc.Height);
    CHECK_CU(g_cu->cuMemcpy2DAsync(&cpy, NULL));

    CHECK_CU(g_cu->cuGraphicsUnmapResources(1, &cuda_res, NULL));
    CHECK_CU(g_cu->cuGraphicsUnregisterResource(cuda_res));
    CHECK_CU(g_cu->cuCtxPopCurrent(&dummy));
}
