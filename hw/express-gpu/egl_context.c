#define STD_DEBUG_LOG

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

    int independ_mode = 0;

    for (int i = 0; attrib_list[i] != EGL_NONE; i += 2)
    {
        if (attrib_list[i] == 0xffffff && attrib_list[i + 1] == 0xffffff)
        {
            independ_mode = 1;
        }
    }

    if(express_gpu_independ_window_enable)
    {
        independ_mode = 1;
    }

    Opengl_Context *opengl_context = opengl_context_create(real_share_context, independ_mode);
    for (int i = 0; attrib_list[i] != EGL_NONE; i += 2)
    {
        express_printf("eglcontext %llx attrib_list %x %x\n", (uint64_t)opengl_context, attrib_list[i], attrib_list[i + 1]);
    }

    // todo:attrib有些什么设置？无论是关于窗口的啥设置的话，得留到makecurrent的时候，那时候才有窗口，才知道如何设置
    express_printf("#%llx context create share %llx\n", (uint64_t)opengl_context, (uint64_t)real_share_context);
    express_printf("context create guest %llx host %llx\n", (uint64_t)guest_context, (uint64_t)opengl_context);

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