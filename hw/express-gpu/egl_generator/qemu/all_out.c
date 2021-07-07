
#include "express-gpu/egl_define.h"

#include "express-gpu/egl_trans.h"

#include "express-gpu/egl_surface.h"


void egl_decode_invoke(Render_Thread_Context *context,Direct_Express_Call *call)
{
    Render_Thread_Context *render_context=(Render_Thread_Context *)context;


    //Double_Buffer *egl_context = &(render_context->render_double_buffer);

    Call_Para all_para[MAX_PARA_NUM];

    unsigned char ret_local_buf[1024*4];

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

int para_num=get_para_from_call(call,all_para,MAX_PARA_NUM);
if(para_num<PARA_NUM_MIN_eglSwapBuffers_special){
    break;
}



                size_t temp_len=0;
                unsigned char *temp=NULL;

                temp_len=all_para[0].data_len;
                if(temp_len< 16 * 1){
                    break;
                }

                int null_flag=0;
                temp=get_direct_ptr(all_para[0].data,&null_flag);
                if(temp==NULL){
                    if(temp_len!=0 && null_flag==0){
                        temp=no_ptr_buf;
                        guest_write(all_para[0].data,temp,0,all_para[0].data_len);
                    }else{
                        break;
                    }
                }

            unsigned int temp_loc=0;

                    dpy = *(EGLDisplay *)(temp+temp_loc);
                    temp_loc+=8;
                

                    surface = *(EGLSurface *)(temp+temp_loc);
                    temp_loc+=8;
                int out_buf_len=all_para[1].data_len;


                    unsigned char *ret_buf=NULL;

                    if(out_buf_len>MAX_OUT_BUF_LEN){
                        ret_buf=g_malloc(out_buf_len);
                    }else{
                        ret_buf=ret_local_buf;
                    }
                    int out_buf_loc=0;
                
                
                        EGLBoolean *ret_ptr=(EGLBoolean *)(ret_buf+out_buf_loc);
                        out_buf_loc+=sizeof(EGLBoolean);
                    
                    if(out_buf_loc > out_buf_len){
                        if(out_buf_len>MAX_OUT_BUF_LEN){
                            g_free(ret_buf);
                        }
                        break;
                    }
                

EGLBoolean ret = d_eglSwapBuffers_special(render_context,dpy, surface);
*ret_ptr=ret;

                    guest_read(all_para[1].data,ret_buf,0,out_buf_len);
                

if(out_buf_len>MAX_OUT_BUF_LEN){g_free(ret_buf);}}
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

int para_num=get_para_from_call(call,all_para,MAX_PARA_NUM);
if(para_num<PARA_NUM_MIN_eglMakeCurrent_special){
    break;
}



                size_t temp_len=0;
                unsigned char *temp=NULL;

                temp_len=all_para[0].data_len;
                if(temp_len< 32 * 1){
                    break;
                }

                int null_flag=0;
                temp=get_direct_ptr(all_para[0].data,&null_flag);
                if(temp==NULL){
                    if(temp_len!=0 && null_flag==0){
                        temp=no_ptr_buf;
                        guest_write(all_para[0].data,temp,0,all_para[0].data_len);
                    }else{
                        break;
                    }
                }

            unsigned int temp_loc=0;

                    dpy = *(EGLDisplay *)(temp+temp_loc);
                    temp_loc+=8;
                

                    draw = *(EGLSurface *)(temp+temp_loc);
                    temp_loc+=8;
                

                    read = *(EGLSurface *)(temp+temp_loc);
                    temp_loc+=8;
                

                    ctx = *(EGLContext *)(temp+temp_loc);
                    temp_loc+=8;
                int out_buf_len=all_para[1].data_len;


                    unsigned char *ret_buf=NULL;

                    if(out_buf_len>MAX_OUT_BUF_LEN){
                        ret_buf=g_malloc(out_buf_len);
                    }else{
                        ret_buf=ret_local_buf;
                    }
                    int out_buf_loc=0;
                
                
                        EGLBoolean *ret_ptr=(EGLBoolean *)(ret_buf+out_buf_loc);
                        out_buf_loc+=sizeof(EGLBoolean);
                    
                    if(out_buf_loc > out_buf_len){
                        if(out_buf_len>MAX_OUT_BUF_LEN){
                            g_free(ret_buf);
                        }
                        break;
                    }
                

EGLBoolean ret = d_eglMakeCurrent_special(render_context,dpy, draw, read, ctx);
*ret_ptr=ret;

                    guest_read(all_para[1].data,ret_buf,0,out_buf_len);
                

if(out_buf_len>MAX_OUT_BUF_LEN){g_free(ret_buf);}}
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

int para_num=get_para_from_call(call,all_para,MAX_PARA_NUM);
if(para_num<PARA_NUM_MIN_eglTerminate_special){
    break;
}



                size_t temp_len=0;
                unsigned char *temp=NULL;

                temp_len=all_para[0].data_len;
                if(temp_len< 8 * 1){
                    break;
                }

                int null_flag=0;
                temp=get_direct_ptr(all_para[0].data,&null_flag);
                if(temp==NULL){
                    if(temp_len!=0 && null_flag==0){
                        temp=no_ptr_buf;
                        guest_write(all_para[0].data,temp,0,all_para[0].data_len);
                    }else{
                        break;
                    }
                }

            unsigned int temp_loc=0;

                    dpy = *(EGLDisplay *)(temp+temp_loc);
                    temp_loc+=8;
                int out_buf_len=all_para[1].data_len;


                    unsigned char *ret_buf=NULL;

                    if(out_buf_len>MAX_OUT_BUF_LEN){
                        ret_buf=g_malloc(out_buf_len);
                    }else{
                        ret_buf=ret_local_buf;
                    }
                    int out_buf_loc=0;
                
                
                        EGLBoolean *ret_ptr=(EGLBoolean *)(ret_buf+out_buf_loc);
                        out_buf_loc+=sizeof(EGLBoolean);
                    
                    if(out_buf_loc > out_buf_len){
                        if(out_buf_len>MAX_OUT_BUF_LEN){
                            g_free(ret_buf);
                        }
                        break;
                    }
                

EGLBoolean ret = d_eglTerminate_special(render_context,dpy);
*ret_ptr=ret;

                    guest_read(all_para[1].data,ret_buf,0,out_buf_len);
                

if(out_buf_len>MAX_OUT_BUF_LEN){g_free(ret_buf);}}
break;


case FUNID_getEGLConfigParam_special:

{

/* readline: "int getEGLConfigParam_special int *num_configs#sizeof(int)" */
/* func name: "getEGLConfigParam_special" */
/* args: [{'type': 'int*', 'name': 'num_configs', 'ptr': 'out', 'ptr_len': 'sizeof(int)', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "int" */
/* type: "0" */

/* Define variables */

int para_num=get_para_from_call(call,all_para,MAX_PARA_NUM);
if(para_num<PARA_NUM_MIN_getEGLConfigParam_special){
    break;
}


int out_buf_len=all_para[0].data_len;


                    unsigned char *ret_buf=NULL;

                    if(out_buf_len>MAX_OUT_BUF_LEN){
                        ret_buf=g_malloc(out_buf_len);
                    }else{
                        ret_buf=ret_local_buf;
                    }
                    int out_buf_loc=0;
                
                

                            int* num_configs=(int*)(ret_buf+out_buf_loc);
                            out_buf_loc+=sizeof(int);
                        
                        int *ret_ptr=(int *)(ret_buf+out_buf_loc);
                        out_buf_loc+=sizeof(int);
                    
                    if(out_buf_loc > out_buf_len){
                        if(out_buf_len>MAX_OUT_BUF_LEN){
                            g_free(ret_buf);
                        }
                        break;
                    }
                

int ret = d_getEGLConfigParam_special(render_context,num_configs);
*ret_ptr=ret;

                    guest_read(all_para[0].data,ret_buf,0,out_buf_len);
                

if(out_buf_len>MAX_OUT_BUF_LEN){g_free(ret_buf);}}
break;


case FUNID_getEGLConfigs_special:

{

/* readline: "EGLint getEGLConfigs_special int num_attrs, EGLint *attr_list#num_attrs*sizeof(EGLint)" */
/* func name: "getEGLConfigs_special" */
/* args: [{'type': 'int', 'name': 'num_attrs', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLint*', 'name': 'attr_list', 'ptr': 'out', 'ptr_len': 'num_attrs*sizeof(EGLint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "EGLint" */
/* type: "0" */

/* Define variables */
int num_attrs;

int para_num=get_para_from_call(call,all_para,MAX_PARA_NUM);
if(para_num<PARA_NUM_MIN_getEGLConfigs_special){
    break;
}


