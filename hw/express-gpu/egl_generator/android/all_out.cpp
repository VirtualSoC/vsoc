
#include "define_egl.h"

#include "../context_common.h"

/******* file '1-1' *******/

/******* end of file '1-1', 14/13 functions *******/

/******* file '1-2' *******/

/* readline: "EGLBoolean eglMakeCurrent EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx, uint64_t gbuffer_id" */
/* func name: "eglMakeCurrent" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'draw', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'read', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'EGLContext', 'name': 'ctx', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "EGLBoolean" */
/* type: "120" */
EGLBoolean r_eglMakeCurrent(void *context, EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx, uint64_t gbuffer_id)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 32;
    unsigned char *save_buf;

    unsigned char local_save_buf[40];
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

    *(uint64_t *)ptr = gbuffer_id;
    ptr += 8;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_eglMakeCurrent;
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

    send_to_host(context, send_buf, send_buf_len, 1);

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

/* readline: "EGLBoolean eglSwapBuffers_sync EGLDisplay dpy, EGLSurface surface" */
/* func name: "eglSwapBuffers_sync" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'surface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "EGLBoolean" */
/* type: "120" */
EGLBoolean r_eglSwapBuffers_sync(void *context, EGLDisplay dpy, EGLSurface surface)
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
    *(uint64_t *)ptr = FUNID_eglSwapBuffers_sync;
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

    send_to_host(context, send_buf, send_buf_len, 1);

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

/* readline: "EGLBoolean eglWaitGL void" */
/* func name: "eglWaitGL" */
/* args: [] */
/* ret: "EGLBoolean" */
/* type: "120" */
EGLBoolean r_eglWaitGL(void *context)
{

    unsigned char send_buf[16 + 1 * 16];
    size_t send_buf_len = 16 + 1 * 16;
    size_t para_num = 1;
    unsigned char *ptr = NULL;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_eglWaitGL;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
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

    send_to_host(context, send_buf, send_buf_len, 1);

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

/* readline: "EGLBoolean eglWaitNative EGLint engine" */
/* func name: "eglWaitNative" */
/* args: [{'type': 'EGLint', 'name': 'engine', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "EGLBoolean" */
/* type: "120" */
EGLBoolean r_eglWaitNative(void *context, EGLint engine)
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

    *(EGLint *)ptr = engine;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_eglWaitNative;
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

    send_to_host(context, send_buf, send_buf_len, 1);

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

/* readline: "EGLBoolean eglWaitClient void" */
/* func name: "eglWaitClient" */
/* args: [] */
/* ret: "EGLBoolean" */
/* type: "120" */
EGLBoolean r_eglWaitClient(void *context)
{

    unsigned char send_buf[16 + 1 * 16];
    size_t send_buf_len = 16 + 1 * 16;
    size_t para_num = 1;
    unsigned char *ptr = NULL;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_eglWaitClient;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
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

    send_to_host(context, send_buf, send_buf_len, 1);

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

/* readline: "EGLint eglClientWaitSync EGLDisplay dpy, EGLSync sync, EGLint flags, EGLTime timeout" */
/* func name: "eglClientWaitSync" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSync', 'name': 'sync', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLint', 'name': 'flags', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'EGLTime', 'name': 'timeout', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "EGLint" */
/* type: "120" */
EGLint r_eglClientWaitSync(void *context, EGLDisplay dpy, EGLSync sync, EGLint flags, EGLTime timeout)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 28;
    unsigned char *save_buf;

    unsigned char local_save_buf[28];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(EGLDisplay *)ptr = dpy;
    ptr += 8;

    *(EGLSync *)ptr = sync;
    ptr += 8;

    *(EGLint *)ptr = flags;
    ptr += 4;

    *(EGLTime *)ptr = timeout;
    ptr += 8;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_eglClientWaitSync;
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

    out_buf_len += sizeof(EGLint);

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

    send_to_host(context, send_buf, send_buf_len, 1);

    out_buf_ptr = out_buf;
    EGLint ret = *(EGLint *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "EGLBoolean eglGetSyncAttrib EGLDisplay dpy, EGLSync sync, EGLint attribute, EGLAttrib *value#sizeof(EGLAttrib)" */
/* func name: "eglGetSyncAttrib" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSync', 'name': 'sync', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLint', 'name': 'attribute', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'EGLAttrib*', 'name': 'value', 'ptr': 'out', 'ptr_len': 'sizeof(EGLAttrib)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "EGLBoolean" */
/* type: "120" */
EGLBoolean r_eglGetSyncAttrib(void *context, EGLDisplay dpy, EGLSync sync, EGLint attribute, EGLAttrib *value)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 20;
    unsigned char *save_buf;

    unsigned char local_save_buf[20];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(EGLDisplay *)ptr = dpy;
    ptr += 8;

    *(EGLSync *)ptr = sync;
    ptr += 8;

    *(EGLint *)ptr = attribute;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_eglGetSyncAttrib;
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

    uint64_t value_len = (uint64_t)sizeof(EGLAttrib);
    out_buf_len += value_len;

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

    send_to_host(context, send_buf, send_buf_len, 1);

    out_buf_ptr = out_buf;
    if (value != NULL)
    {

        memcpy(value, (EGLAttrib *)out_buf_ptr, value_len);
    }
    out_buf_ptr += value_len;

    EGLBoolean ret = *(EGLBoolean *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "EGLint eglGetEGLConfigParam EGLint *num_configs#sizeof(EGLint)" */
/* func name: "eglGetEGLConfigParam" */
/* args: [{'type': 'EGLint*', 'name': 'num_configs', 'ptr': 'out', 'ptr_len': 'sizeof(EGLint)', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "EGLint" */
/* type: "120" */
EGLint r_eglGetEGLConfigParam(void *context, EGLint *num_configs)
{

    unsigned char send_buf[16 + 1 * 16];
    size_t send_buf_len = 16 + 1 * 16;
    size_t para_num = 1;
    unsigned char *ptr = NULL;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_eglGetEGLConfigParam;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t num_configs_len = (uint64_t)sizeof(EGLint);
    out_buf_len += num_configs_len;

    out_buf_len += sizeof(EGLint);

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

    send_to_host(context, send_buf, send_buf_len, 1);

    out_buf_ptr = out_buf;
    if (num_configs != NULL)
    {

        memcpy(num_configs, (EGLint *)out_buf_ptr, num_configs_len);
    }
    out_buf_ptr += num_configs_len;

    EGLint ret = *(EGLint *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "EGLint eglGetEGLConfigs EGLint num_attrs, EGLint list_len, EGLint *attr_list#list_len*sizeof(EGLint)" */
/* func name: "eglGetEGLConfigs" */
/* args: [{'type': 'EGLint', 'name': 'num_attrs', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLint', 'name': 'list_len', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLint*', 'name': 'attr_list', 'ptr': 'out', 'ptr_len': 'list_len*sizeof(EGLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "EGLint" */
/* type: "120" */
EGLint r_eglGetEGLConfigs(void *context, EGLint num_attrs, EGLint list_len, EGLint *attr_list)
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

    *(EGLint *)ptr = num_attrs;
    ptr += 4;

    *(EGLint *)ptr = list_len;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_eglGetEGLConfigs;
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

    uint64_t attr_list_len = (uint64_t)list_len * sizeof(EGLint);
    out_buf_len += attr_list_len;

    out_buf_len += sizeof(EGLint);

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

    send_to_host(context, send_buf, send_buf_len, 1);

    out_buf_ptr = out_buf;
    if (attr_list != NULL)
    {

        memcpy(attr_list, (EGLint *)out_buf_ptr, attr_list_len);
    }
    out_buf_ptr += attr_list_len;

    EGLint ret = *(EGLint *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/******* end of file '1-2', 10/22 functions *******/

/******* file '2-1' *******/

/* readline: "EGLBoolean eglChooseConfig EGLDisplay dpy, const EGLint *attrib_list#get_attrib_list_len(attrib_list), EGLConfig *configs#config_size*sizeof(EGLConfig), EGLint config_size, EGLint *num_config#sizeof(EGLint)" */
/* func name: "eglChooseConfig" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const EGLint*', 'name': 'attrib_list', 'ptr': 'in', 'ptr_len': 'get_attrib_list_len(attrib_list)', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLConfig*', 'name': 'configs', 'ptr': 'out', 'ptr_len': 'config_size*sizeof(EGLConfig)', 'loc': 2, 'ptr_ptr': False}, {'type': 'EGLint', 'name': 'config_size', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'EGLint*', 'name': 'num_config', 'ptr': 'out', 'ptr_len': 'sizeof(EGLint)', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "EGLBoolean" */
/* type: "210" */
EGLBoolean r_eglChooseConfig(void *context, EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t attrib_list_len = get_attrib_list_len(attrib_list);
    save_buf_len += attrib_list_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(EGLDisplay *)ptr = dpy;
    ptr += 8;

    *(EGLint *)ptr = config_size;
    ptr += 4;

    memcpy(ptr, (unsigned char *)attrib_list, attrib_list_len);
    ptr += attrib_list_len;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_eglChooseConfig;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len, 0);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "EGLSurface eglCreatePbufferSurface EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list#get_attrib_list_len(attrib_list), EGLSurface surface" */
/* func name: "eglCreatePbufferSurface" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLConfig', 'name': 'config', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const EGLint*', 'name': 'attrib_list', 'ptr': 'in', 'ptr_len': 'get_attrib_list_len(attrib_list)', 'loc': 2, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'surface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "EGLSurface" */
/* type: "210" */
EGLSurface r_eglCreatePbufferSurface(void *context, EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list, EGLSurface surface)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 24;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t attrib_list_len = get_attrib_list_len(attrib_list);
    save_buf_len += attrib_list_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(EGLDisplay *)ptr = dpy;
    ptr += 8;

    *(EGLConfig *)ptr = config;
    ptr += 8;

    *(EGLSurface *)ptr = surface;
    ptr += 8;

    memcpy(ptr, (unsigned char *)attrib_list, attrib_list_len);
    ptr += attrib_list_len;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_eglCreatePbufferSurface;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len, 0);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "EGLSurface eglCreateWindowSurface EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list#get_attrib_list_len(attrib_list), EGLSurface surface" */
/* func name: "eglCreateWindowSurface" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLConfig', 'name': 'config', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLNativeWindowType', 'name': 'win', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const EGLint*', 'name': 'attrib_list', 'ptr': 'in', 'ptr_len': 'get_attrib_list_len(attrib_list)', 'loc': 3, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'surface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "EGLSurface" */
/* type: "210" */
EGLSurface r_eglCreateWindowSurface(void *context, EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list, EGLSurface surface)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 32;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t attrib_list_len = get_attrib_list_len(attrib_list);
    save_buf_len += attrib_list_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(EGLDisplay *)ptr = dpy;
    ptr += 8;

    *(EGLConfig *)ptr = config;
    ptr += 8;

    *(EGLNativeWindowType *)ptr = win;
    ptr += 8;

    *(EGLSurface *)ptr = surface;
    ptr += 8;

    memcpy(ptr, (unsigned char *)attrib_list, attrib_list_len);
    ptr += attrib_list_len;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_eglCreateWindowSurface;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len, 0);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "EGLContext eglCreateContext EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list#get_attrib_list_len(attrib_list), EGLContext egl_context" */
/* func name: "eglCreateContext" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLConfig', 'name': 'config', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLContext', 'name': 'share_context', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const EGLint*', 'name': 'attrib_list', 'ptr': 'in', 'ptr_len': 'get_attrib_list_len(attrib_list)', 'loc': 3, 'ptr_ptr': False}, {'type': 'EGLContext', 'name': 'egl_context', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "EGLContext" */
/* type: "210" */
EGLContext r_eglCreateContext(void *context, EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list, EGLContext egl_context)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 32;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t attrib_list_len = get_attrib_list_len(attrib_list);
    save_buf_len += attrib_list_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(EGLDisplay *)ptr = dpy;
    ptr += 8;

    *(EGLConfig *)ptr = config;
    ptr += 8;

    *(EGLContext *)ptr = share_context;
    ptr += 8;

    *(EGLContext *)ptr = egl_context;
    ptr += 8;

    memcpy(ptr, (unsigned char *)attrib_list, attrib_list_len);
    ptr += attrib_list_len;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_eglCreateContext;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len, 0);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "EGLBoolean eglDestroyContext EGLDisplay dpy, EGLContext ctx" */
/* func name: "eglDestroyContext" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLContext', 'name': 'ctx', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "EGLBoolean" */
/* type: "210" */
EGLBoolean r_eglDestroyContext(void *context, EGLDisplay dpy, EGLContext ctx)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(EGLDisplay *)ptr = dpy;
    ptr += 8;

    *(EGLContext *)ptr = ctx;
    ptr += 8;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_eglDestroyContext;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len, 0);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "EGLBoolean eglDestroySurface EGLDisplay dpy, EGLSurface surface" */
/* func name: "eglDestroySurface" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'surface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "EGLBoolean" */
/* type: "210" */
EGLBoolean r_eglDestroySurface(void *context, EGLDisplay dpy, EGLSurface surface)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(EGLDisplay *)ptr = dpy;
    ptr += 8;

    *(EGLSurface *)ptr = surface;
    ptr += 8;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_eglDestroySurface;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len, 0);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "EGLBoolean eglTerminate EGLDisplay dpy" */
/* func name: "eglTerminate" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "EGLBoolean" */
/* type: "210" */
EGLBoolean r_eglTerminate(void *context, EGLDisplay dpy)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(EGLDisplay *)ptr = dpy;
    ptr += 8;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_eglTerminate;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len, 0);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "eglIamComposer EGLSurface surface" */
/* func name: "eglIamComposer" */
/* args: [{'type': 'EGLSurface', 'name': 'surface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "210" */
void r_eglIamComposer(void *context, EGLSurface surface)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(EGLSurface *)ptr = surface;
    ptr += 8;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_eglIamComposer;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len, 0);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "EGLDisplay eglGetDisplay EGLNativeDisplayType display_id, EGLDisplay guest_display" */
/* func name: "eglGetDisplay" */
/* args: [{'type': 'EGLNativeDisplayType', 'name': 'display_id', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLDisplay', 'name': 'guest_display', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "EGLDisplay" */
/* type: "210" */
EGLDisplay r_eglGetDisplay(void *context, EGLNativeDisplayType display_id, EGLDisplay guest_display)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(EGLNativeDisplayType *)ptr = display_id;
    ptr += 8;

    *(EGLDisplay *)ptr = guest_display;
    ptr += 8;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_eglGetDisplay;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len, 0);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "EGLBoolean eglBindTexImage EGLDisplay dpy, EGLSurface surface, EGLint buffer" */
/* func name: "eglBindTexImage" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'surface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLint', 'name': 'buffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "EGLBoolean" */
/* type: "210" */
EGLBoolean r_eglBindTexImage(void *context, EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 20;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(EGLDisplay *)ptr = dpy;
    ptr += 8;

    *(EGLSurface *)ptr = surface;
    ptr += 8;

    *(EGLint *)ptr = buffer;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_eglBindTexImage;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len, 0);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "EGLBoolean eglReleaseTexImage EGLDisplay dpy, EGLSurface surface, EGLint buffer" */
/* func name: "eglReleaseTexImage" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'surface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLint', 'name': 'buffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "EGLBoolean" */
/* type: "210" */
EGLBoolean r_eglReleaseTexImage(void *context, EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 20;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(EGLDisplay *)ptr = dpy;
    ptr += 8;

    *(EGLSurface *)ptr = surface;
    ptr += 8;

    *(EGLint *)ptr = buffer;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_eglReleaseTexImage;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len, 0);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "EGLBoolean eglSurfaceAttrib EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value" */
/* func name: "eglSurfaceAttrib" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'surface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLint', 'name': 'attribute', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'EGLint', 'name': 'value', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "EGLBoolean" */
/* type: "210" */
EGLBoolean r_eglSurfaceAttrib(void *context, EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 24;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(EGLDisplay *)ptr = dpy;
    ptr += 8;

    *(EGLSurface *)ptr = surface;
    ptr += 8;

    *(EGLint *)ptr = attribute;
    ptr += 4;

    *(EGLint *)ptr = value;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_eglSurfaceAttrib;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len, 0);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "EGLBoolean eglSwapInterval EGLDisplay dpy, EGLint interval" */
/* func name: "eglSwapInterval" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLint', 'name': 'interval', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "EGLBoolean" */
/* type: "210" */
EGLBoolean r_eglSwapInterval(void *context, EGLDisplay dpy, EGLint interval)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(EGLDisplay *)ptr = dpy;
    ptr += 8;

    *(EGLint *)ptr = interval;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_eglSwapInterval;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len, 1);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "EGLBoolean eglReleaseThread void" */
/* func name: "eglReleaseThread" */
/* args: [] */
/* ret: "EGLBoolean" */
/* type: "210" */
EGLBoolean r_eglReleaseThread(void *context)
{

    unsigned char send_buf[16 + 16 * 0];
    size_t send_buf_len = 16 + 16 * 0;
    uint64_t save_buf_len = 0;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_eglReleaseThread;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 0;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len, 0);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "EGLSync eglCreateSync EGLDisplay dpy, EGLenum type, const EGLAttrib *attrib_list#sizeof(EGLAttrib), EGLSync sync" */
/* func name: "eglCreateSync" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const EGLAttrib*', 'name': 'attrib_list', 'ptr': 'in', 'ptr_len': 'sizeof(EGLAttrib)', 'loc': 2, 'ptr_ptr': False}, {'type': 'EGLSync', 'name': 'sync', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "EGLSync" */
/* type: "210" */
EGLSync r_eglCreateSync(void *context, EGLDisplay dpy, EGLenum type, const EGLAttrib *attrib_list, EGLSync sync)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 20;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t attrib_list_len = sizeof(EGLAttrib);
    save_buf_len += attrib_list_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(EGLDisplay *)ptr = dpy;
    ptr += 8;

    *(EGLenum *)ptr = type;
    ptr += 4;

    *(EGLSync *)ptr = sync;
    ptr += 8;

    memcpy(ptr, (unsigned char *)attrib_list, attrib_list_len);
    ptr += attrib_list_len;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_eglCreateSync;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len, 0);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "EGLBoolean eglDestroySync EGLDisplay dpy, EGLSync sync" */
/* func name: "eglDestroySync" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSync', 'name': 'sync', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "EGLBoolean" */
/* type: "210" */
EGLBoolean r_eglDestroySync(void *context, EGLDisplay dpy, EGLSync sync)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(EGLDisplay *)ptr = dpy;
    ptr += 8;

    *(EGLSync *)ptr = sync;
    ptr += 8;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_eglDestroySync;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len, 0);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "EGLImage eglCreateImage EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLAttrib *attrib_list#sizeof(EGLAttrib), EGLImage image" */
/* func name: "eglCreateImage" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLContext', 'name': 'ctx', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'EGLClientBuffer', 'name': 'buffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'const EGLAttrib*', 'name': 'attrib_list', 'ptr': 'in', 'ptr_len': 'sizeof(EGLAttrib)', 'loc': 4, 'ptr_ptr': False}, {'type': 'EGLImage', 'name': 'image', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "EGLImage" */
/* type: "210" */
EGLImage r_eglCreateImage(void *context, EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLAttrib *attrib_list, EGLImage image)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 36;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t attrib_list_len = sizeof(EGLAttrib);
    save_buf_len += attrib_list_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(EGLDisplay *)ptr = dpy;
    ptr += 8;

    *(EGLContext *)ptr = ctx;
    ptr += 8;

    *(EGLenum *)ptr = target;
    ptr += 4;

    *(EGLClientBuffer *)ptr = buffer;
    ptr += 8;

    *(EGLImage *)ptr = image;
    ptr += 8;

    memcpy(ptr, (unsigned char *)attrib_list, attrib_list_len);
    ptr += attrib_list_len;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_eglCreateImage;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len, 0);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "EGLBoolean eglDestroyImage EGLDisplay dpy, EGLImage image" */
/* func name: "eglDestroyImage" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLImage', 'name': 'image', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "EGLBoolean" */
/* type: "210" */
EGLBoolean r_eglDestroyImage(void *context, EGLDisplay dpy, EGLImage image)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(EGLDisplay *)ptr = dpy;
    ptr += 8;

    *(EGLImage *)ptr = image;
    ptr += 8;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_eglDestroyImage;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len, 0);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "EGLBoolean eglWaitSync EGLDisplay dpy, EGLSync sync, EGLint flags" */
/* func name: "eglWaitSync" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSync', 'name': 'sync', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLint', 'name': 'flags', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "EGLBoolean" */
/* type: "210" */
EGLBoolean r_eglWaitSync(void *context, EGLDisplay dpy, EGLSync sync, EGLint flags)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 20;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(EGLDisplay *)ptr = dpy;
    ptr += 8;

    *(EGLSync *)ptr = sync;
    ptr += 8;

    *(EGLint *)ptr = flags;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_eglWaitSync;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len, 0);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "EGLClientBuffer eglGetNativeClientBufferANDROID AHardwareBuffer buffer" */
/* func name: "eglGetNativeClientBufferANDROID" */
/* args: [{'type': 'AHardwareBuffer', 'name': 'buffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "EGLClientBuffer" */
/* type: "210" */
EGLClientBuffer r_eglGetNativeClientBufferANDROID(void *context, AHardwareBuffer buffer)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(AHardwareBuffer *)ptr = buffer;
    ptr += 8;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_eglGetNativeClientBufferANDROID;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len, 0);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/******* end of file '2-1', 21/42 functions *******/

/******* file '2-2' *******/

/* readline: "eglSwapBuffers EGLDisplay dpy, EGLSurface surface, EGLint *ret_flag#sizeof(GLint)" */
/* func name: "eglSwapBuffers" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'surface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLint*', 'name': 'ret_flag', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "220" */
void r_eglSwapBuffers(void *context, EGLDisplay dpy, EGLSurface surface, EGLint *ret_flag)
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
    *(uint64_t *)ptr = FUNID_eglSwapBuffers;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)sizeof(GLint);
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)ret_flag;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len, 1);
}

/******* end of file '2-2', 2/43 functions *******/

/******* file '3' *******/

/******* end of file '3', 7/49 functions *******/
