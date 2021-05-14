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

#include "express-gpu/egl_trans.h"

#include "express-gpu/egl_surface.h"

// typedef struct{
//     EGLDisplay eglDisplay;
//     EGLContext eglContext;
//     EGLSurface eglSurface;

//     int now_thread_id;

// } Native_Window;

void egl_decode_invoke(Render_Thread_Context *context, Direct_Express_Call *call)
{
    Render_Thread_Context *render_context = (Render_Thread_Context *)context;
    // Double_Buffer *egl_context = &(render_context->render_double_buffer);
    //uint64_t fun_id=GET_FUN_ID(call->id);
    //uint64_t is_async=FUN_IS_ASYNC(call->id);
    uint64_t need_speed = FUN_NEED_SPEED(call->id);
    Call_Para all_para[MAX_PARA_NUM];

    unsigned char ret_local_buf[1024 * 4];

    switch (call->id)
    {

        /******* file '1-1' *******/

    case FUNID_eglSwapBuffers_special:

    {

        /* readline: "EGLBoolean eglSwapBuffers_special EGLDisplay dpy, EGLSurface surface" */
        /* func name: "eglSwapBuffers_special" */
        /* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'surface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
        /* ret: "EGLBoolean" */
        /* type: "0" */

        /* Define variables */
        EGLDisplay dpy;
        EGLSurface surface;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglSwapBuffers_special)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        int need_delete = 0;
        temp_len = all_para[0].data_len;
        if (temp_len < 16 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(all_para[0].data_len);
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);

                need_delete = 1;
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        surface = *(EGLSurface *)(temp + temp_loc);
        temp_loc += 8;

        EGLBoolean ret = d_eglSwapBuffers_special(render_context, dpy, surface);

        set_call_return_val(call, (unsigned char *)&ret, 4);

        if (need_delete)
        {
            g_free(temp);
        }
    }
    break;

    case FUNID_eglMakeCurrent_special:

    {

        /* readline: "EGLBoolean eglMakeCurrent_special EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx" */
        /* func name: "eglMakeCurrent_special" */
        /* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'draw', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'read', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'EGLContext', 'name': 'ctx', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
        /* ret: "EGLBoolean" */
        /* type: "0" */

        /* Define variables */
        EGLDisplay dpy;
        EGLSurface draw;
        EGLSurface read;
        EGLContext ctx;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglMakeCurrent_special)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        int need_delete = 0;
        temp_len = all_para[0].data_len;
        if (temp_len < 32 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(all_para[0].data_len);
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);

                need_delete = 1;
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        draw = *(EGLSurface *)(temp + temp_loc);
        temp_loc += 8;

        read = *(EGLSurface *)(temp + temp_loc);
        temp_loc += 8;

        ctx = *(EGLContext *)(temp + temp_loc);
        temp_loc += 8;

        EGLBoolean ret = d_eglMakeCurrent_special(render_context, dpy, draw, read, ctx);

        set_call_return_val(call, (unsigned char *)&ret, 4);

        if (need_delete)
        {
            g_free(temp);
        }
    }
    break;

    case FUNID_eglTerminate_special:

    {

        /* readline: "EGLBoolean eglTerminate_special EGLDisplay dpy" */
        /* func name: "eglTerminate_special" */
        /* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
        /* ret: "EGLBoolean" */
        /* type: "0" */

        /* Define variables */
        EGLDisplay dpy;

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_eglTerminate_special)
        {
            break;
        }

        size_t temp_len = 0;
        unsigned char *temp = NULL;

        int need_delete = 0;
        temp_len = all_para[0].data_len;
        if (temp_len < 8 * 1)
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (temp == NULL)
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(all_para[0].data_len);
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);

                need_delete = 1;
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;

        EGLBoolean ret = d_eglTerminate_special(render_context, dpy);

        set_call_return_val(call, (unsigned char *)&ret, 4);

        if (need_delete)
        {
            g_free(temp);
        }
    }
    break;
        /******* end of file '1-1', 2/2 functions*******/

        /******* file '1-2' *******/

        /******* end of file '1-2', 0/2 functions*******/

        /******* file '2-1' *******/

        /******* end of file '2-1', 0/2 functions*******/

        /******* file '2-2' *******/

        /******* end of file '2-2', 0/2 functions*******/

    default:
        break;
    }

    if (need_speed)
    {
        call->callback(call, 1);
    }
    else
    {
        call->callback(call, 0);
    }
    return;
}
