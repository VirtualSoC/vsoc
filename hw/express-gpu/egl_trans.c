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
#include "express-gpu/egl_define.h"

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

    //Double_Buffer *egl_context = &(render_context->render_double_buffer);

    Call_Para all_para[MAX_PARA_NUM];

    unsigned char ret_local_buf[1024 * 4];

    unsigned char no_ptr_buf[512];

    switch (call->id)
    {

        /******* file '1-1-1' *******/

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
                temp = no_ptr_buf;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
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
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        EGLBoolean *ret_ptr = (EGLBoolean *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(EGLBoolean);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        EGLBoolean ret = d_eglSwapBuffers_special(render_context, dpy, surface);
        *ret_ptr = ret;

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
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
                temp = no_ptr_buf;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
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
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        EGLBoolean *ret_ptr = (EGLBoolean *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(EGLBoolean);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        EGLBoolean ret = d_eglMakeCurrent_special(render_context, dpy, draw, read, ctx);
        *ret_ptr = ret;

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
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
                temp = no_ptr_buf;
                guest_write(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        unsigned int temp_loc = 0;

        dpy = *(EGLDisplay *)(temp + temp_loc);
        temp_loc += 8;
        int out_buf_len = all_para[1].data_len;

        unsigned char *ret_buf = NULL;

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            ret_buf = g_malloc(out_buf_len);
        }
        else
        {
            ret_buf = ret_local_buf;
        }
        int out_buf_loc = 0;

        EGLBoolean *ret_ptr = (EGLBoolean *)(ret_buf + out_buf_loc);
        out_buf_loc += sizeof(EGLBoolean);

        if (out_buf_loc > out_buf_len)
        {
            if (out_buf_len > MAX_OUT_BUF_LEN)
            {
                g_free(ret_buf);
            }
            break;
        }

        EGLBoolean ret = d_eglTerminate_special(render_context, dpy);
        *ret_ptr = ret;

        guest_read(all_para[1].data, ret_buf, 0, out_buf_len);

        if (out_buf_len > MAX_OUT_BUF_LEN)
        {
            g_free(ret_buf);
        }
    }
    break;

    case FUNID_getEGLConfigParam_special:

    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (para_num < PARA_NUM_MIN_getEGLConfigParam_special)
        {
            break;
        }

        int *num_configs = NULL, *num_config_attrs = NULL;
        int null_flag = 0;
        num_configs = (int *)get_direct_ptr(all_para[0].data, &null_flag);
        num_config_attrs = (int *)get_direct_ptr(all_para[1].data, &null_flag);

        d_getEGLConfigParam_special(render_context, num_configs, num_config_attrs);
    }
    break;
        /******* end of file '1-1', 2/2 functions*******/

        /******* file '1-2' *******/

        /******* end of file '1-2', 1/3 functions*******/

        /******* file '2-1-1' *******/

        /******* end of file '2-1-1', 1/3 functions*******/

        /******* file '2-1-2' *******/

        /******* end of file '2-1-2', 1/3 functions*******/

        /******* file '2-2' *******/

        /******* end of file '2-2', 1/3 functions*******/

    default:
        break;
    }

    //if(need_speed){
    call->callback(call, 1);
    //}else{
    //    call->callback(call, 0);
    //}
    return;
}
