/**
 * @file egl_trans.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief 
 * @version 0.1
 * @date 2020-11-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */



// #define EGL_EGLEXT_PROTOTYPES

#include "EGL/egl.h"
#include "GLES3/gl3.h"
#include "EGL/eglplatform.h"
#include "EGL/eglext.h"

#include "express-gpu/egl_trans.h"

#include "glib/ghash.h"
#include "express-gpu/express_log.h"









// EGL_ENTRY(EGLBoolean, eglHibernateProcessIMG, void)
// EGL_ENTRY(EGLBoolean, eglAwakenProcessIMG, void)

// EGL_ENTRY(EGLBoolean, eglSetSwapRectangleANDROID, EGLDisplay, EGLSurface, EGLint, EGLint, EGLint, EGLint)
// EGL_ENTRY(EGLClientBuffer, eglGetRenderBufferANDROID, EGLDisplay, EGLSurface)
// EGL_ENTRY(EGLint, eglDupNativeFenceFDANDROID, EGLDisplay, EGLSyncKHR)
// EGL_ENTRY(EGLClientBuffer, eglCreateNativeClientBufferANDROID, const EGLint *)
// EGL_ENTRY(EGLClientBuffer, eglGetNativeClientBufferANDROID, const AHardwareBuffer *)


// EGL_ENTRY(EGLBoolean, eglQueryStringImplementationANDROID, EGLDisplay, EGLint)
// EGL_ENTRY(EGLBoolean, eglGetNextFrameIdANDROID, EGLDisplay, EGLSurface, EGLuint64KHR *)
// EGL_ENTRY(EGLBoolean, eglGetFrameTimestampsANDROID, EGLDisplay, EGLSurface, EGLuint64KHR, EGLint, const EGLint *, EGLnsecsANDROID *)
// EGL_ENTRY(EGLBoolean, eglGetFrameTimestampSupportedANDROID, EGLDisplay, EGLSurface, EGLint)


// EGL_ENTRY(EGLBoolean, eglGetCompositorTimingANDROID, EGLDisplay, EGLSurface, EGLint, const EGLint *, EGLnsecsANDROID *)
// EGL_ENTRY(EGLBoolean, eglGetCompositorTimingSupportedANDROID, EGLDisplay, EGLSurface, EGLint)








// common-obj-$(CONFIG_MY_GPU_PCI) += my_gpu_pci.o
// common-obj-$(CONFIG_MY_GPU) += my_gpu.o
// common-obj-$(CONFIG_MY_GPU) += my_gpu_render.o
// common-obj-$(CONFIG_MY_GPU) += egl_trans.o
// common-obj-$(CONFIG_MY_GPU) += glv1_trans.o
// common-obj-$(CONFIG_MY_GPU) += glv3_trans.o








typedef struct{
    EGLDisplay eglDisplay;
    EGLContext eglContext;
    EGLSurface eglSurface;

    int now_thread_id;



} Native_Window;

GHashTable *thread_info;

Native_Window *egl_window;


static void destroy_context(gpointer hash_data) {

    gfree(hash_data);
}


void egl_trans_init(){
    thread_info=g_hash_table_new(g_int_hash,g_int_equal);
}



void egl_make_current_thread(int thread_id){
    Egl_Thread_Context *now_thread_context;
    if(egl_window->now_thread_id==thread_id){
        return;
    }else if(g_hash_table_contains(thread_info,thread_id)){
        now_thread_context=g_hash_table_lookup(thread_info,thread_id);
        if(now_thread_context->init){
            eglMakeCurrent(egl_window->eglDisplay,egl_window->eglSurface,egl_window->eglSurface,now_thread_context->now_egl_context);
            egl_window->now_thread_id==thread_id;
            return;
        }
    }else{
            now_thread_context=g_malloc(sizeof(Egl_Thread_Context));
            
            now_thread_context->thread_id=thread_id;
            now_thread_context->init=0;
            
            g_hash_table_insert(thread_info,thread_id,now_thread_context);
    }
    eglMakeCurrent(egl_window->eglDisplay,egl_window->eglSurface,egl_window->eglSurface,egl_window->eglContext);
    egl_window->now_thread_id==thread_id;
    return;

}



void egl_decode_invoke(MYGPU_Opengl_Call *call){
    egl_make_current_thread(call->thread_id);

    switch(call->opengl_id){
        default:
            break;
    }

}



EGLBoolean t_eglMakeCurrent(int thread_id,EGLContext ctx){

}




