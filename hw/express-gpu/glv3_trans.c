/**
 * @file glv3_trans.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief 
 * @version 0.1
 * @date 2020-11-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "GLES3/gl3.h"
#include "GLES2/gl2ext.h"
#include "GLES3/gl3platform.h"

#include "mygpu/glv3_trans.h"



// 1. 可以异步的函数
//   1.1 没有返回值，并且传入参数全是int型的标号
//   1.2 没有返回值，传入参数中有指针，但是指针是其他opengl函数产生，正常绘图不会直接操作该指针，而是通过opengl函数操作该指针，因而指针可以直接视为长整型数
//   1.3 没有返回值，传入参数中有指针，但是指针是不是由opengl函数产生，是用于向gpu传递数据的指针，例如图片数据指针，不会用于返回数据
// 2. 只能同步的函数
//   2.1 有返回值，但是传入参数全是int型的标号
//   2.2 有返回值，但是传入参数有指针，指针与1.2类型，没有直接操作，产生自opengl函数
//   2.3 有返回值，但是传入参数有指针，指针用于接收从opengl返回的数据
//   2.4 无返回值，但是传入参数有指针，指针用于接收从opengl返回的数据
//   2.5 用于cpu和gpu进行同步操作的函数
//   2.6 有返回值，返回值为指针，但是后续会直接操作该指针，对指针指向的内存进行赋值操作，例如各种内存映射map函数
//   2.7 传入参数有指针数据，尽管指针是其他opengl函数产生，但是会操作到该指针指向的数据，例如解除映射的unmap函数

//2.5 需要放到其他地方，2.6,2.7后续单独编写，1.2和2.2,2.3不存在，现在的2.2也需要放到其他地方

// 1. 异步的函数
//   1.1 没有返回值，并且传入参数全是int型（或者long long型）的标号
//   1.3.1 没有返回值，传入参数中有指针，是用于向gpu传递数据的指针，且指针指向的数据是int型（或者long long型）
// 2. 同步的函数
//   1.3.2 没有返回值，传入参数中有指针，是用于向gpu传递数据的指针，且指针指向的数据是非int型
//   2.1 有返回值，但是传入参数全是int型的标号
//   2.4 无返回值，但是传入参数有指针，指针用于接收从opengl返回的数据



//对于传入参数，int型(包括long long)都会被放到一个缓冲区中，存储到header后的第一个elem的para中，需要依次取出
//对于传入参数，假如包括int型的指针，且该指针不用于传出数据，则该指针指向的数据也会被存储到缓冲区上，一般这种函数都有一个参数来说明数组长度
//对于传入参数，其他类型的指针，都等于相应位置elem的para，假如为同步则可以直接使用，异步则需要把数据复制出来，假如这个指针作为传出，则需要标记now_elem->type|=RET_PARA
//对于返回值，假如返回值为int型（包括long long），则将其写入到header的para的ret位置处（参照下面示范例子），假如返回值为指针，若只会在opengl里使用则视为unsigned long型，例如GLsync，若为其他的（好像只有名字数组），则将其复制到最后一个elem的para中



//异步的id都是奇数,并且只有一个传入参数
#define FUNID_glActiveTexture 2001
#define FUNID_glBindAttribLocation 2002
#define FUNID_glDeleteBuffers 2003
#define FUNCID_glBufferData 2004
#define FUNCID_glCheckFramebufferStatus 2006
#define FUNCID_glGetString 2008
#define FUNCID_glGenBuffers 2010

void gl3_decode_invoke(MYGPU_Opengl_Call *call){
    
    unsigned int buf[1024];
    GLchar name_buf[1024];

    switch(call->opengl_id){
        case FUNID_glActiveTexture:
        //1.1
            GLenum texture;
            GLboolean a;
            MYGPU_Queue_Elem *header=call->elem_header;
            MYGPU_Queue_Elem *now_elem=header->next;

            if(now_elem==NULL){
                call->callback(call);
                break;
            }
            GLint *temp=now_elem->para;
            if(temp==NULL){
                call->callback(call);
                break;
            }
            size_t temp_len=now_elem->len;
            if(temp_len!=1*sizeof(GLint)){
                call->callback(call);
                break;
            }
            texture=(GLenum)temp[0];
            call->callback(call);
            glActiveTexture(texture);
            break;
        case FUNID_glDeleteBuffers:
        //1.3 指针指向int数组，使用异步，为小数据使用数组，大数据使用malloc
            GLsizei n;
            MYGPU_Queue_Elem *header=call->elem_header;
            MYGPU_Queue_Elem *now_elem=header->next;

            if(now_elem==NULL){
                call->callback(call);
                break;
            }
            GLint *temp=now_elem->para;
            if(temp==NULL){
                call->callback(call);
                break;
            }
            size_t temp_len=now_elem->len;
            n=(GLsizei)temp[0];
            if(temp_len!=(n+1)*sizeof(GLint)){
                call->callback(call);
                break;
            }
            if(n<1024){
                memcpy(buf,temp+1,n*sizeof(GLint));
                call->callback(call);
                glDeleteBuffers(n,buf);
            }else{
                GLint *large_buf=g_malloc(n*sizeof(GLint));
                memcpy(large_buf,temp+1,n*sizeof(GLint));
                call->callback(call);
                glDeleteBuffers(n,large_buf);
            }
            break;
        case FUNCID_glBufferData:
         //1.3 指针指向其他数据，使用同步
            GLuint program;
            GLuint index;
            MYGPU_Queue_Elem *header=call->elem_header;
            MYGPU_Queue_Elem *now_elem=header->next;

            if(now_elem==NULL){
                call->callback(call);
                break;
            }
            GLint *temp=now_elem->para;
            if(temp==NULL){
                call->callback(call);
                break;
            }
            if(temp_len!=2*sizeof(GLint)){
                call->callback(call);
                break;
            }
            program=(GLint)temp[0];
            index=(GLint)temp[1];

            now_elem=now_elem->next;

            if(now_elem==NULL){
                call->callback(call);
                break;
            }

            const GLchar *name=now_elem->para;

            glBindAttribLocation(program,index,name);
            call->callback(call);
            
            break;
        case FUNCID_glCheckFramebufferStatus:
        //2.1传入全是int，返回一个int
            GLenum target;
            MYGPU_Queue_Elem *header=call->elem_header;
            MYGPU_Queue_Elem *now_elem=header->next;

            if(now_elem==NULL){
                call->callback(call);
                break;
            }
            GLint *temp=now_elem->para;
            if(temp==NULL){
                call->callback(call);
                break;
            }
            size_t temp_len=now_elem->len;
            if(temp_len!=1*sizeof(GLint)){
                call->callback(call);
                break;
            }
            target=(GLenum)temp[0];

            GLenum ret=glCheckFramebufferStatus(target);
            
            MYGPU_Flag_Buf *ret_buf=(MYGPU_Flag_Buf *)header->para;
            //这一串的取地址强转然后赋值的过程是为了防止基本类型强转时，某些数据位被丢失
            *(GLenum *)(&(ret_buf->ret))=ret;


            call->callback(call);
            break;

        case FUNCID_glGetString:
        //2.1传入全是int，返回一个数组，似乎都是字符数组
            GLenum name;
            MYGPU_Queue_Elem *header=call->elem_header;
            MYGPU_Queue_Elem *now_elem=header->next;

            if(now_elem==NULL){
                call->callback(call);
                break;
            }
            GLint *temp=now_elem->para;
            if(temp==NULL){
                call->callback(call);
                break;
            }
            size_t temp_len=now_elem->len;
            if(temp_len!=1*sizeof(GLint)){
                call->callback(call);
                break;
            }
            name=(GLenum)temp[0];
            
            now_elem=now_elem->next;

            if(now_elem==NULL){
                call->callback(call);
                break;
            }

            GLubyte *ret_buf=(GLubyte *)now_elem->para;
            if(ret_buf==NULL){
                call->callback(call);
                break;
            }

            GLubyte *recv_buf=glGetString(name);
            
            strcpy(ret_buf,recv_buf);

            call->callback(call);
            break;
        case FUNCID_glGenBuffers:
        //2.4，没有返回值，返回值通过函数参数指针返回
            GLsizei n;
            MYGPU_Queue_Elem *header=call->elem_header;
            MYGPU_Queue_Elem *now_elem=header->next;

            if(now_elem==NULL){
                call->callback(call);
                break;
            }
            GLint *temp=now_elem->para;
            if(temp==NULL){
                call->callback(call);
                break;
            }
            size_t temp_len=now_elem->len;
           
            if(temp_len!=1*sizeof(GLint)){
                call->callback(call);
                break;
            }
            n=(GLsizei)temp[0];

            now_elem=now_elem->next;

            if(now_elem==NULL){
                call->callback(call);
                break;
            }

            now_elem->type|=RET_PARA;
            GLuint *ret_buf=(GLuint *)now_elem->para;
            if(ret_buf==NULL){
                call->callback(call);
                break;
            }

            glGenBuffers(n,ret_buf);
            call->callback(call);

            break;
        default:
            break;
    }
}
