// #define STD_DEBUG_LOG

#include "hw/express-gpu/egl_context.h"
#include "hw/express-gpu/glv3_context.h"


EGLBoolean d_eglTerminate(void *context, EGLDisplay dpy)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;
    // If display_id is EGL_DEFAULT_DISPLAY, a default display is returned. Multiple calls made to eglGetDisplay with the same display_id will return the same EGLDisplay handle.
    //根据eglGetDisplay的描述来看，这个进程get的所有display都是相同的，所以应该不同线程的dpy是一样的，所以这里应该直接释放整个线程的资源

    //假如还在使用的话，下面两个都只是标记清理
    g_hash_table_remove_all(process_context->context_map);
    g_hash_table_remove_all(process_context->surface_map);
    //不需要remove native windows的的东西，因为surface对应的gbuffer可能还存在，还能被合成
    // g_hash_table_remove_all(process_context->native_window_surface_map);
    // g_hash_table_remove_all(process_context->gbuffer_image_map);

    // 此时不能设为空，不然makecurrent NULL的时候就无法销毁相关资源了
    // thread_context->opengl_context = NULL;
    // thread_context->render_double_buffer_read = NULL;
    // thread_context->render_double_buffer_draw = NULL;

    express_printf("eglTerminate context %llx\n", (uint64_t)thread_context->opengl_context);
    return GL_TRUE;
}

EGLBoolean d_eglReleaseThread(void *context)
{
    return EGL_TRUE;
}

void d_eglCreateContext(void *context, EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list, EGLContext guest_context)
{

    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;
    Opengl_Context *real_share_context = NULL;
    if (share_context != NULL && share_context != EGL_NO_CONTEXT)
    {
        real_share_context = (Opengl_Context *)g_hash_table_lookup(process_context->context_map, GUINT_TO_POINTER(share_context));
    }

    int context_flags = 0;

    for (int i = 0; attrib_list[i] != EGL_NONE; i += 2)
    {
        if (attrib_list[i] == DGL_CONTEXT_INDEPENDENT_MODE && attrib_list[i + 1] == EGL_TRUE)
        {
            context_flags |= DGL_CONTEXT_FLAG_INDEPENDENT_MODE_BIT;
        }
        if (attrib_list[i] == EGL_CONTEXT_OPENGL_DEBUG && attrib_list[i + 1] == EGL_TRUE)
        {
            context_flags |= GL_CONTEXT_FLAG_DEBUG_BIT;
        }
        // host端context创建时默认使用EGL_GL_NO_RESET_NOTIFICATION，这里只需要检查EGL_LOSE_CONTEXT_ON_RESET即可。
        // if (attrib_list[i] == EGL_CONTEXT_OPENGL_ROBUST_ACCESS && attrib_list[i+1] == EGL_TRUE)
        // {
        //     context_flags |= GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT;
        //     // robust_access_strategy 默认是EGL_NO_RESET_NOTIFICATION
        //     robust_access_strategy = EGL_NO_RESET_NOTIFICATION;
        // }
        if (attrib_list[i] == EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY && attrib_list[i + 1] == EGL_LOSE_CONTEXT_ON_RESET)
        {
            context_flags |= GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT;
        }
    }

    if(express_gpu_independ_window_enable)
    {
        context_flags |= DGL_CONTEXT_FLAG_INDEPENDENT_MODE_BIT;
    }

    Opengl_Context *opengl_context = opengl_context_create(real_share_context, context_flags);
    for (int i = 0; attrib_list[i] != EGL_NONE; i += 2)
    {
        express_printf("eglcontext %llx attrib_list %x %x\n", (uint64_t)opengl_context, attrib_list[i], attrib_list[i + 1]);
    }

    LOGI("create context guest %llx host %llx", (uint64_t)guest_context, (uint64_t)opengl_context);
    LOGD("#%llx create context share %llx", (uint64_t)opengl_context, (uint64_t)real_share_context);

    opengl_context->guest_context = guest_context;

    g_hash_table_insert(process_context->context_map, GUINT_TO_POINTER(guest_context), (gpointer)opengl_context);
}

EGLBoolean d_eglDestroyContext(void *context, EGLDisplay dpy, EGLContext ctx)
{
    //此时不知道当前的surface的context还在不在，不确定是否能调用opengl，因此要发送给主窗口来清除各种资源

    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    //这个context_map的销毁函数g_context_map_destroy里已经包含对context的处理了
    express_printf("context remove guest %llx\n", (uint64_t)ctx);
    g_hash_table_remove(process_context->context_map, GUINT_TO_POINTER(ctx));
    return EGL_TRUE;
}

/**
 * 在host端记录guest端映射过来的buffer
*/
void d_eglCreateDebugMessageBuffer(void *context, EGLContext guest_gl_context, void *guest_mem) 
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;
    Opengl_Context *real_opengl_context = (Opengl_Context *)g_hash_table_lookup(process_context->context_map, GUINT_TO_POINTER(guest_gl_context));

    if (real_opengl_context == NULL || (real_opengl_context->context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) == 0)
    {
        LOGE("attempt to create debug message buffer on non-debug context %p guest context %p context flag %x, ignoring.", real_opengl_context, guest_gl_context, real_opengl_context->context_flags);
        return;
    }
    real_opengl_context->debug_message_buffer = guest_mem;
    express_printf("create debug message buffer context %p guest context %p context flag %x buffer %p\n", real_opengl_context, guest_gl_context, real_opengl_context->context_flags, guest_mem);
}

/**
 * 解除占用并删除guest端映射过来的buffer
*/
void d_eglDestroyDebugMessageBuffer(void *context, EGLContext guest_gl_context, void *guest_mem) 
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;
    Opengl_Context *real_opengl_context = (Opengl_Context *)g_hash_table_lookup(process_context->context_map, GUINT_TO_POINTER(guest_gl_context));

    if (real_opengl_context == NULL)
    {
        // context might have already been destroyed
        return;
    }
    if (!(real_opengl_context->context_flags & GL_CONTEXT_FLAG_DEBUG_BIT))
    {
        LOGE("error! attempt to destroy debug message buffer on non-debug context %p guest context %p context flag %x, ignoring.", real_opengl_context, guest_gl_context, real_opengl_context->context_flags);
        return;
    }
    if (real_opengl_context->is_current)
    {
        #ifndef __APPLE__
        glDebugMessageCallback(NULL, NULL);
        #endif
    }
    
    if ((real_opengl_context->context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) && real_opengl_context->debug_message_buffer)
    {
        free_copied_guest_mem((Guest_Mem *)real_opengl_context->debug_message_buffer);
        real_opengl_context->debug_message_buffer = NULL;
    }
    express_printf("destroy debug message buffer context %p guest context %p context flag %x\n", real_opengl_context, guest_gl_context, real_opengl_context->context_flags);
}