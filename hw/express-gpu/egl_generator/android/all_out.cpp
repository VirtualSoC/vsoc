
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

            unsigned char send_buf[16+2*16];
            size_t send_buf_len=16+2*16;
            size_t para_num=2;
            unsigned char *ptr=NULL;

            
            uint64_t save_buf_len=16;
            unsigned char *save_buf;
            
            unsigned char local_save_buf[16];
            save_buf=local_save_buf;
            ptr=save_buf;
            

            *(EGLDisplay *)ptr = dpy;
            ptr += 8;
            

            *(EGLSurface *)ptr = surface;
            ptr += 8;
            

        ptr=send_buf;
        *(uint64_t*)ptr=FUNID_eglSwapBuffers_special;
        ptr+=sizeof(uint64_t);
        
            *(uint64_t*)ptr=(uint64_t)para_num;
            ptr+=sizeof(uint64_t);

            *(uint64_t*)ptr=(uint64_t)save_buf_len;
            ptr+=sizeof(uint64_t);
            *(uint64_t*)ptr=(uint64_t)save_buf;
            ptr+=sizeof(uint64_t);

            
                    unsigned char * out_buf;
                    unsigned char * out_buf_ptr;
                    uint64_t out_buf_len=0;
                    unsigned char out_buf_local[MAX_OUT_BUF_LEN];
                
                        out_buf_len+=sizeof(EGLBoolean);
                    
                    if(out_buf_len>MAX_OUT_BUF_LEN){
                        out_buf=(unsigned char *)malloc(out_buf_len); 
                    }else{
                        out_buf=out_buf_local;
                    }
                

                    *(uint64_t*)ptr=(uint64_t)out_buf_len;
                    ptr+=sizeof(uint64_t);
                    *(uint64_t*)ptr=(uint64_t)out_buf;
                    ptr+=sizeof(uint64_t);
                

            send_to_host(context,send_buf,send_buf_len);


        out_buf_ptr=out_buf;
                    EGLBoolean ret=*(EGLBoolean *)out_buf_ptr;
                
                if(out_buf_len>MAX_OUT_BUF_LEN){{
                    free(out_buf); 
                }}
            
                return ret;

            }


/* readline: "EGLBoolean eglMakeCurrent_special EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx" */
/* func name: "eglMakeCurrent_special" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'draw', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'EGLSurface', 'name': 'read', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'EGLContext', 'name': 'ctx', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "EGLBoolean" */
/* type: "0" */

EGLBoolean d_eglMakeCurrent_special(void *context, EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{

            unsigned char send_buf[16+2*16];
            size_t send_buf_len=16+2*16;
            size_t para_num=2;
            unsigned char *ptr=NULL;

            
            uint64_t save_buf_len=32;
            unsigned char *save_buf;
            
            unsigned char local_save_buf[32];
            save_buf=local_save_buf;
            ptr=save_buf;
            

            *(EGLDisplay *)ptr = dpy;
            ptr += 8;
            

            *(EGLSurface *)ptr = draw;
            ptr += 8;
            

            *(EGLSurface *)ptr = read;
            ptr += 8;
            

            *(EGLContext *)ptr = ctx;
            ptr += 8;
            

        ptr=send_buf;
        *(uint64_t*)ptr=FUNID_eglMakeCurrent_special;
        ptr+=sizeof(uint64_t);
        
            *(uint64_t*)ptr=(uint64_t)para_num;
            ptr+=sizeof(uint64_t);

            *(uint64_t*)ptr=(uint64_t)save_buf_len;
            ptr+=sizeof(uint64_t);
            *(uint64_t*)ptr=(uint64_t)save_buf;
            ptr+=sizeof(uint64_t);

            
                    unsigned char * out_buf;
                    unsigned char * out_buf_ptr;
                    uint64_t out_buf_len=0;
                    unsigned char out_buf_local[MAX_OUT_BUF_LEN];
                
                        out_buf_len+=sizeof(EGLBoolean);
                    
                    if(out_buf_len>MAX_OUT_BUF_LEN){
                        out_buf=(unsigned char *)malloc(out_buf_len); 
                    }else{
                        out_buf=out_buf_local;
                    }
                

                    *(uint64_t*)ptr=(uint64_t)out_buf_len;
                    ptr+=sizeof(uint64_t);
                    *(uint64_t*)ptr=(uint64_t)out_buf;
                    ptr+=sizeof(uint64_t);
                

            send_to_host(context,send_buf,send_buf_len);


        out_buf_ptr=out_buf;
                    EGLBoolean ret=*(EGLBoolean *)out_buf_ptr;
                
                if(out_buf_len>MAX_OUT_BUF_LEN){{
                    free(out_buf); 
                }}
            
                return ret;

            }


/* readline: "EGLBoolean eglTerminate_special EGLDisplay dpy" */
/* func name: "eglTerminate_special" */
/* args: [{'type': 'EGLDisplay', 'name': 'dpy', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "EGLBoolean" */
/* type: "0" */

EGLBoolean d_eglTerminate_special(void *context, EGLDisplay dpy)
{

            unsigned char send_buf[16+2*16];
            size_t send_buf_len=16+2*16;
            size_t para_num=2;
            unsigned char *ptr=NULL;

            
            uint64_t save_buf_len=8;
            unsigned char *save_buf;
            
            unsigned char local_save_buf[8];
            save_buf=local_save_buf;
            ptr=save_buf;
            

            *(EGLDisplay *)ptr = dpy;
            ptr += 8;
            

        ptr=send_buf;
        *(uint64_t*)ptr=FUNID_eglTerminate_special;
        ptr+=sizeof(uint64_t);
        
            *(uint64_t*)ptr=(uint64_t)para_num;
            ptr+=sizeof(uint64_t);

            *(uint64_t*)ptr=(uint64_t)save_buf_len;
            ptr+=sizeof(uint64_t);
            *(uint64_t*)ptr=(uint64_t)save_buf;
            ptr+=sizeof(uint64_t);

            
                    unsigned char * out_buf;
                    unsigned char * out_buf_ptr;
                    uint64_t out_buf_len=0;
                    unsigned char out_buf_local[MAX_OUT_BUF_LEN];
                
                        out_buf_len+=sizeof(EGLBoolean);
                    
                    if(out_buf_len>MAX_OUT_BUF_LEN){
                        out_buf=(unsigned char *)malloc(out_buf_len); 
                    }else{
                        out_buf=out_buf_local;
                    }
                

                    *(uint64_t*)ptr=(uint64_t)out_buf_len;
                    ptr+=sizeof(uint64_t);
                    *(uint64_t*)ptr=(uint64_t)out_buf;
                    ptr+=sizeof(uint64_t);
                

            send_to_host(context,send_buf,send_buf_len);


        out_buf_ptr=out_buf;
                    EGLBoolean ret=*(EGLBoolean *)out_buf_ptr;
                
                if(out_buf_len>MAX_OUT_BUF_LEN){{
                    free(out_buf); 
                }}
            
                return ret;

            }



/******* end of file '1-1-1', 4/3 functions *******/




/******* file '1-1-2' *******/




/******* end of file '1-1-2', 1/3 functions *******/




/******* file '1-2' *******/




/******* end of file '1-2', 1/3 functions *******/




/******* file '2-1-1' *******/




/******* end of file '2-1-1', 1/3 functions *******/




/******* file '2-1-2' *******/




/******* end of file '2-1-2', 1/3 functions *******/




/******* file '2-2' *******/




/******* end of file '2-2', 1/3 functions *******/


