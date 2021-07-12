
#include "define_egl.h"

#include "../context_common.h"

/******* file '1-1-1' *******/

/* readline: "EGLBoolean eglSwapBuffers_special EGLDisplay dpy, EGLSurface surface" */
/* func name: "eglSwapBuffers_special" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'surface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "EGLBoolean" */
/* type: "0" */

EGLBoolean d_eglSwapBuffers_special(void *context, EGLDisplay dpy, EGLSurface surface)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 16;
    unsigned char *save_buf;

    unsigned char local_save_buf[16];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(EGLDisplay *)ptr = dpy;
    ptr += 8;

    *(EGLSurface *)ptr = surface;
    ptr += 8;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_eglSwapBuffers_special;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(EGLBoolean);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    EGLBoolean ret = *(EGLBoolean *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "EGLBoolean eglMakeCurrent_special EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx" */
/* func name: "eglMakeCurrent_special" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'draw', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'read', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'EGLContext', 'name': 'ctx', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "EGLBoolean" */
/* type: "0" */

EGLBoolean d_eglMakeCurrent_special(void *context, EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 32;
    unsigned char *save_buf;

    unsigned char local_save_buf[32];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(EGLDisplay *)ptr = dpy;
    ptr += 8;

    *(EGLSurface *)ptr = draw;
    ptr += 8;

    *(EGLSurface *)ptr = read;
    ptr += 8;

    *(EGLContext *)ptr = ctx;
    ptr += 8;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_eglMakeCurrent_special;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(EGLBoolean);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    EGLBoolean ret = *(EGLBoolean *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "EGLBoolean eglTerminate_special EGLDisplay dpy" */
/* func name: "eglTerminate_special" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "EGLBoolean" */
/* type: "0" */

EGLBoolean d_eglTerminate_special(void *context, EGLDisplay dpy)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(EGLDisplay *)ptr = dpy;
    ptr += 8;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_eglTerminate_special;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(EGLBoolean);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    EGLBoolean ret = *(EGLBoolean *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "int getEGLConfigParam_special int *num_configs#sizeof(int)" */
/* func name: "getEGLConfigParam_special" */
/* args: [{'type': 'int*', 'name': 'num_configs', 'ptr': 'out', 'ptr_len': 'sizeof(int)', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "int" */
/* type: "0" */

int d_getEGLConfigParam_special(void *context, int *num_configs)
{

    unsigned char send_buf[16 + 1 * 16];
    size_t send_buf_len = 16 + 1 * 16;
    size_t para_num = 1;
    unsigned char *ptr = NULL;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_getEGLConfigParam_special;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t num_configs_len = (uint64_t)sizeof(int);
    out_buf_len += num_configs_len;

    out_buf_len += sizeof(int);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (num_configs != NULL)
    {

        memcpy(num_configs, (int *)out_buf_ptr, num_configs_len);
    }
    out_buf_ptr += num_configs_len;

    int ret = *(int *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "EGLint getEGLConfigs_special EGLint num_attrs, EGLint *attr_list#num_attrs*sizeof(EGLint" */
/* func name: "getEGLConfigs_special" */
/* args: [{'type': 'EGLint', 'name': 'num_attrs', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLint*', 'name': 'attr_list', 'ptr': 'out', 'ptr_len': 'num_attrs*sizeof(EGLint', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "EGLint" */
/* type: "0" */

EGLint d_getEGLConfigs_special(void *context, EGLint num_attrs, EGLint *attr_list)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(EGLint *)ptr = num_attrs;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_getEGLConfigs_special;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

                    uint64_t attr_list_len=(uint64_t)num_attrs*sizeof(EGLint;
                    out_buf_len+=attr_list_len;
                    
                        out_buf_len+=sizeof(EGLint);
                    
                    if(out_buf_len>MAX_OUT_BUF_LEN){
        out_buf = (unsigned char *)malloc(out_buf_len); 
                    }else{
        out_buf = out_buf_local;
                    }
                

                    *(uint64_t*)ptr=(uint64_t)out_buf_len;
                    ptr+=sizeof(uint64_t);
                    *(uint64_t*)ptr=(uint64_t)out_buf;
                    ptr+=sizeof(uint64_t);
                

            send_to_host(context,send_buf,send_buf_len);


        out_buf_ptr=out_buf;
                    if(attr_list!=NULL){
        memcpy(attr_list, (EGLint *)out_buf_ptr, attr_list_len);
                    }
                    out_buf_ptr+=attr_list_len;
                
                    EGLint ret=*(EGLint *)out_buf_ptr;
                
                if(out_buf_len>MAX_OUT_BUF_LEN){
        {
            free(out_buf);
        }}
            
                return ret;
}

/* readline: "EGLBoolean getEGLVersion_special EGLint *ver_major#sizeof(EGLint), EGLint *ver_minor#sizeof(EGLint)" */
/* func name: "getEGLVersion_special" */
/* args: [{'type': 'EGLint*', 'name': 'ver_major', 'ptr': 'out', 'ptr_len': 'sizeof(EGLint)', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLint*', 'name': 'ver_minor', 'ptr': 'out', 'ptr_len': 'sizeof(EGLint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "EGLBoolean" */
/* type: "0" */

EGLBoolean d_getEGLVersion_special(void *context, EGLint *ver_major, EGLint *ver_minor)
{

    unsigned char send_buf[16 + 1 * 16];
    size_t send_buf_len = 16 + 1 * 16;
    size_t para_num = 1;
    unsigned char *ptr = NULL;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_getEGLVersion_special;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t ver_major_len = (uint64_t)sizeof(EGLint);
    out_buf_len += ver_major_len;

    uint64_t ver_minor_len = (uint64_t)sizeof(EGLint);
    out_buf_len += ver_minor_len;

    out_buf_len += sizeof(EGLBoolean);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (ver_major != NULL)
    {

        memcpy(ver_major, (EGLint *)out_buf_ptr, ver_major_len);
    }
    out_buf_ptr += ver_major_len;

    if (ver_minor != NULL)
    {

        memcpy(ver_minor, (EGLint *)out_buf_ptr, ver_minor_len);
    }
    out_buf_ptr += ver_minor_len;

    EGLBoolean ret = *(EGLBoolean *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/******* end of file '1-1-1', 7/6 functions *******/

/******* file '1-1-2' *******/

/******* end of file '1-1-2', 1/6 functions *******/

/******* file '1-2' *******/

/******* end of file '1-2', 1/6 functions *******/

/******* file '2-1-1' *******/

/******* end of file '2-1-1', 1/6 functions *******/

/******* file '2-1-2' *******/

/******* end of file '2-1-2', 1/6 functions *******/

/******* file '2-2' *******/

/******* end of file '2-2', 1/6 functions *******/
