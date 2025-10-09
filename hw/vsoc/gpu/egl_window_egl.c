#include "hw/vsoc/express_log.h"
#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/gpu/egl_window.h"

#include <glib.h>
#include <dlfcn.h>
#include <stdio.h>

#include "hw/vsoc/gpu/egl.h"
#include "glad/glad.h"

typedef void (*EGLproc)(void);

typedef EGLproc (*PFN_eglGetProcAddress)(const char *procname);
typedef EGLint (*PFN_eglGetError)(void);
typedef EGLBoolean (*PFN_eglInitialize)(EGLDisplay, EGLint *, EGLint *);
typedef EGLBoolean (*PFN_eglTerminate)(EGLDisplay);
typedef EGLBoolean (*PFN_eglBindAPI)(EGLenum);
typedef char const * (*PFN_eglQueryString)(EGLDisplay display, EGLint name);
typedef EGLDisplay (*PFN_eglGetDisplay)(EGLNativeDisplayType);
typedef EGLContext (*PFN_eglCreateContext)(EGLDisplay, EGLConfig, EGLContext, const EGLint *);
typedef EGLBoolean (*PFN_eglChooseConfig)(EGLDisplay, const EGLint *, EGLConfig *, EGLint, EGLint *);
typedef EGLSurface (*PFN_eglCreatePbufferSurface)(EGLDisplay, EGLConfig, const EGLint *);
typedef EGLBoolean (*PFN_eglDestroySurface)(EGLDisplay, EGLSurface);
typedef EGLBoolean (*PFN_eglDestroyContext)(EGLDisplay, EGLContext);
typedef EGLBoolean (*PFN_eglMakeCurrent)(EGLDisplay, EGLSurface, EGLSurface, EGLContext);

typedef struct EglFunctions {
    PFN_eglGetProcAddress eglGetProcAddress;
    PFN_eglInitialize eglInitialize;
    PFN_eglTerminate eglTerminate;
    PFN_eglBindAPI eglBindAPI;
    PFN_eglQueryString eglQueryString;
    PFN_eglGetDisplay eglGetDisplay;
    PFN_eglMakeCurrent eglMakeCurrent;
    PFN_eglCreateContext eglCreateContext;
    PFN_eglCreatePbufferSurface eglCreatePbufferSurface;
    PFN_eglDestroySurface eglDestroySurface;
    PFN_eglDestroyContext eglDestroyContext; 
    PFN_eglChooseConfig eglChooseConfig;
    PFN_eglGetError eglGetError;
} EglFunctions;

static EglFunctions platform;

static EGLDisplay main_window_display;
static EGLContext main_window_context;


static bool supports_surfaceless_context;
static GHashTable *context_pbuffer_map;

static EGLConfig static_config;
static GMutex main_window_mutex;

static EGLproc load_egl_fun(const char *name)
{
    static void *egl_module = NULL;
    if (egl_module == NULL)
    {
        egl_module = dlopen("libEGL.so", RTLD_LAZY);
        if (egl_module == NULL)
        {
            LOGE("error! libEGL not found");
        }
    }

    EGLproc ret = dlsym(egl_module, name);
    if (ret == NULL)
    {
        LOGD("load egl fun %s failed", name);
    }
    return ret;
}

#define LOAD_EGL_FUN(name) \
    platform.name = (PFN_##name)platform.eglGetProcAddress(#name); \
    if (platform.name == NULL) \
        platform.name = (PFN_##name)platform.eglGetProcAddress(#name "ARB"); \
    if (platform.name == NULL) \
        platform.name = (PFN_##name)platform.eglGetProcAddress(#name "KHR"); \
    if (platform.name == NULL) \
        platform.name = (PFN_##name)platform.eglGetProcAddress(#name "EXT"); \
    if (platform.name == NULL) \
        printf("eglGetProcAddress failed to load %s\n", #name);

void egl_init(void *dpy, void *father_context)
{
    platform.eglGetProcAddress = (PFN_eglGetProcAddress)load_egl_fun("eglGetProcAddress");
    if (platform.eglGetProcAddress == NULL) {
        LOGE("error! eglGetProcAddress not found, cannot load EGL functions");
        return;
    }

    LOAD_EGL_FUN(eglInitialize);
    LOAD_EGL_FUN(eglTerminate);
    LOAD_EGL_FUN(eglBindAPI);
    LOAD_EGL_FUN(eglQueryString);
    LOAD_EGL_FUN(eglGetDisplay);
    LOAD_EGL_FUN(eglMakeCurrent);
    LOAD_EGL_FUN(eglCreateContext);
    LOAD_EGL_FUN(eglCreatePbufferSurface);
    LOAD_EGL_FUN(eglDestroySurface);
    LOAD_EGL_FUN(eglDestroyContext);
    LOAD_EGL_FUN(eglChooseConfig);
    LOAD_EGL_FUN(eglGetError);

    context_pbuffer_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);

    main_window_display = (EGLDisplay)dpy;
    main_window_context = (EGLContext)father_context;

    bool offscreen_init = (main_window_display == NULL && main_window_context == NULL);
    if (offscreen_init) {
        main_window_display = platform.eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (main_window_display == EGL_NO_DISPLAY)
        {
            LOGE("eglGetDisplay error %x display %p context %p", platform.eglGetError(), main_window_display, main_window_context);
            return;
        }
    }

    if (!platform.eglInitialize(main_window_display, NULL, NULL))
    {
        LOGE("eglInitialize error %x display %p context %p", platform.eglGetError(), main_window_display, main_window_context);
    }

    const char *version = platform.eglQueryString(main_window_display, EGL_VERSION);
    const char *vendor = platform.eglQueryString(main_window_display, EGL_VENDOR);
    const char *extensions = platform.eglQueryString(main_window_display, EGL_EXTENSIONS);
    LOGI("EGL version: %s, vendor: %s", version ? version : "unknown", vendor ? vendor : "unknown");
    if (g_ops.express_gpu_gl_debug_enable)
    {
        LOGI("EGL extensions: %s", extensions ? extensions : "none");
    }

    if (strstr(extensions, "EGL_KHR_surfaceless_context") != NULL)
    {
        supports_surfaceless_context = true;
        LOGD("EGL_KHR_surfaceless_context is supported, will create surfaceless contexts");
    }

    EGLint attrib_list[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };

    EGLint num_configs = 0;
    if (!platform.eglChooseConfig(main_window_display, attrib_list, &static_config, 1, &num_configs) || num_configs == 0)
    {
        LOGE("choose config error %x display %p context %p", platform.eglGetError(), main_window_display, main_window_context);
    }

    g_mutex_init(&main_window_mutex);

    if (offscreen_init) {
        platform.eglBindAPI(EGL_OPENGL_API);

        int context_attribs[] = {
            EGL_CONTEXT_MAJOR_VERSION, 4,
#ifdef __APPLE__
            EGL_CONTEXT_MINOR_VERSION, 1,
#else
            EGL_CONTEXT_MINOR_VERSION, 6,
#endif
            EGL_NONE
        };

        main_window_context = egl_createContext(0);
        if (main_window_context == EGL_NO_CONTEXT)
        {
            LOGE("error! eglCreateContext failed with error 0x%x display %p context %p", platform.eglGetError(), main_window_display, main_window_context);
            return;
        }
        egl_makeCurrent(main_window_context);

        gladLoadGLLoader((GLADloadproc)platform.eglGetProcAddress);
    }
}

void *egl_createContext(int context_flags)
{
    g_mutex_lock(&main_window_mutex);

    int enable_debug = EGL_FALSE;
    if ((context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) || g_ops.express_gpu_gl_debug_enable) {
        enable_debug = EGL_TRUE;
    }

    int context_attribs[] = {
        EGL_CONTEXT_MAJOR_VERSION, 4,
#ifdef __APPLE__
        EGL_CONTEXT_MINOR_VERSION, 1,
#else
        EGL_CONTEXT_MINOR_VERSION, 6,
#endif
        EGL_CONTEXT_OPENGL_DEBUG, enable_debug,
        EGL_NONE
    };

    EGLContext context = platform.eglCreateContext(main_window_display, static_config, main_window_context, context_attribs);
    if (context == EGL_NO_CONTEXT)
    {
        LOGE("error! eglCreateContext failed with error 0x%x main_window_display %p main_window_context %p", platform.eglGetError(), main_window_display, main_window_context);
        g_mutex_unlock(&main_window_mutex);
        return EGL_NO_CONTEXT;
    }

    if (!supports_surfaceless_context) {
        
        static int static_pbuffer_attribs[] = {
            EGL_WIDTH, 1,
            EGL_HEIGHT, 1,
            EGL_NONE
        };

        EGLSurface pbuffer = platform.eglCreatePbufferSurface(main_window_display, static_config, static_pbuffer_attribs);

        if (pbuffer == EGL_NO_SURFACE)
        {
            LOGE("error! eglCreatePbufferSurface failed with error 0x%x main_window_display %p main_window_context %p", platform.eglGetError(), main_window_display, main_window_context);
            platform.eglDestroyContext(main_window_display, context);
            g_mutex_unlock(&main_window_mutex);
            return EGL_NO_CONTEXT;
        }

        g_hash_table_insert(context_pbuffer_map, (gpointer)context, pbuffer);
    }

    g_mutex_unlock(&main_window_mutex);
    return context;
}

int egl_makeCurrent(void *context)
{
    int result;
    EGLSurface pbuffer = EGL_NO_SURFACE;

    g_mutex_lock(&main_window_mutex);

    if (context != NULL)
    {
        if (!supports_surfaceless_context) {
            pbuffer = g_hash_table_lookup(context_pbuffer_map, (gpointer)context);
        }
        result = platform.eglMakeCurrent(main_window_display, pbuffer, pbuffer, context);
    }
    else
    {
        result = platform.eglMakeCurrent(main_window_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }

    g_mutex_unlock(&main_window_mutex);

    if (result != EGL_TRUE) {
        LOGE("error! eglMakeCurrent failed with error 0x%x main_window_display %p context %p pbuffer %p", platform.eglGetError(), main_window_display, context, pbuffer);
        return EGL_FALSE;
    }

    return EGL_TRUE;
}

void egl_destroyContext(void *context)
{
    if (context != NULL)
    {
        g_mutex_lock(&main_window_mutex);

        platform.eglDestroyContext(main_window_display, context);

        if (!supports_surfaceless_context)
        {
            EGLSurface pbuffer = g_hash_table_lookup(context_pbuffer_map, (gpointer)context);
            platform.eglDestroySurface(main_window_display, pbuffer);
            g_hash_table_remove(context_pbuffer_map, (gpointer)context);
        }
        
        g_mutex_unlock(&main_window_mutex);
    }
}