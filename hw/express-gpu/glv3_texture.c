#define STD_DEBUG_LOG




#include "express-gpu/glv3_texture.h"

#include "express-gpu/offscreen_render_thread.h"





#include "direct-express/express_log.h"

void prepare_unpack_texture(void *context, Guest_Mem *guest_mem, int start_loc, int end_loc);

void prepare_unpack_texture_to_egl_image(void *context, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, Guest_Mem *guest_mem);



void prepare_unpack_texture(void *context, Guest_Mem *guest_mem, int start_loc, int end_loc)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    GLint asyn_texture = bound_buffer->asyn_unpack_texture_buffer;
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, asyn_texture);
    // express_printf("gl get error %x\n",glGetError());

    //因为曾经bind过texture，所以这里bind相应的buffer，这里重新bufferdata是为了孤立缓冲区
    glBufferData(GL_PIXEL_UNPACK_BUFFER, end_loc, NULL, GL_STREAM_DRAW);
    // express_printf("gl get error %x\n",glGetError());

    //然后把数据复制到内存里，之后交给dma传输   到底是invalidata还是unsync？
    GLubyte *map_pointer = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, start_loc, end_loc - start_loc, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

    // express_printf("gl get error %x\n",glGetError());

    // GLubyte *temp_data=g_malloc(end_loc-start_loc);
    // guest_write(guest_mem,temp_data,start_loc,end_loc-start_loc);
    // express_printf("texture:");
    // for(int i=0;i<end_loc-start_loc;i++){
    //     express_printf("%u ",(unsigned int)temp_data[i]);
    // }
    // express_printf("\n");
    guest_write(guest_mem, map_pointer, start_loc, end_loc - start_loc);
    // host_guest_buffer_exchange(s_data,map_pointer,start_loc,end_loc-start_loc,1);

    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    express_printf("unpack texture start %d end %d\n", start_loc, end_loc);
}

// void gl_pixel_data_loc(void *store_status, GLsizei width, GLsizei height, GLenum format, GLenum type, int pack, int *start_loc, int *end_loc)
// {
//     gl_pixel_data_3d_loc(store_status, width, height, 1, format, type, pack, start_loc, end_loc);
// }

// void gl_pixel_data_3d_loc(void *store_status, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, int pack, int *start_loc, int *end_loc)
// {
//     Pixel_Store_Status *status = (Pixel_Store_Status *)store_status;
//     int align = 4;
//     int row_length = 0;
//     int skip_rows = 0;
//     int skip_pixels = 0;
//     int skip_images = 0;
//     int image_height = 0;

//     GLsizei real_width;
//     GLsizei real_height;

//     //获得此时的状态值
//     if (pack)
//     {
//         align = status->pack_alignment;
//         row_length = status->pack_row_length;
//         skip_rows = status->pack_skip_rows;
//         skip_pixels = status->pack_skip_pixels;
//     }
//     else
//     {
//         align = status->unpack_alignment;
//         row_length = status->unpack_row_length;
//         skip_rows = status->unpack_skip_rows;
//         skip_pixels = status->unpack_skip_pixels;

//         skip_images = status->unpack_skip_images;
//         image_height = status->unpack_image_height;
//     }

//     //获得一张图片的真实像素宽度和高度
//     real_width = row_length == 0 ? width : row_length;
//     real_height = image_height == 0 ? height : image_height;

//     //每个像素点所占的空间
//     GLsizei pixel_size = pixel_size_calc(format, type);
//     //获得图片每一行的字节数
//     GLsizei width_size = pixel_size * real_width;
//     //每一行的数据进行对其
//     width_size = (width_size + align - 1) & (~(align - 1));

//     //数据图像开始读取的地方，等于（跳过的图片数目*图片的高度+跳过的行数）*每行所占的字节数+跳过的像素数*像素大小
//     *start_loc = (skip_images * real_height + skip_rows) * width_size + skip_pixels * pixel_size;

//     //数据图像结束读取的地方，等于 开始读取的地方+要读取的图片的高度*图片每行所占的字节数*图片的张数，depth表示深度也就表示要读取多少张图片
//     *end_loc = *start_loc + real_height * width_size * depth;

//     return;
// }

void d_glTexImage2D_without_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{
    // if(pixels==NULL){
    //     glTexImage2D(target,level,internalformat,width,height,border,format,type,NULL);
    // }
    //没有绑定时，正好可以使用异步纹理传输
    Guest_Mem *guest_mem = (Guest_Mem *)pixels;
    // Scatter_Data *s_data=guest_mem->scatter_data;
    GLuint t;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&t);
    // if(width==64 &&height ==64 && buf_len ==16384){
    //     printf("debug");
    // }

    // printf("%llx target %x teximage2d %u without size %d %d len %d %llx\n",context,target,t, width,height,buf_len,pixels);
    if (guest_mem->all_len == 0)
    {
        //pixels=NULL
        glTexImage2D(target, level, internalformat, width, height, border, format, type, NULL);
        return;
    }

    // Pixel_Store_Status *status=&(((Opengl_Context *)context)->pixel_store_status);

    int start_loc = 0, end_loc = buf_len;
    //gl_pixel_data_loc(status,width,height,format,type,0,&start_loc,&end_loc);

    prepare_unpack_texture(context, guest_mem, start_loc, end_loc);

    // GLubyte *temp_data=g_malloc(end_loc-start_loc);
    // guest_write(guest_mem,temp_data,start_loc,end_loc-start_loc);
    // express_printf("texture:");
    // for(int i=0;i<end_loc-start_loc;i++){
    //     express_printf("%u ",(unsigned int)temp_data[i]);
    // }
    // express_printf("\n");

    //这时候是立即返回的，后续会进行dma传输
    glTexImage2D(target, level, internalformat, width, height, border, format, type, NULL);
    // g_free(temp_data);

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, status->host_pixel_unpack_buffer);
}

void d_glTexImage2D_with_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLintptr pixels)
{
    // GLuint t;
    // glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&t);
    // printf("teximage %u size %d %d %lld\n", t, width, height, pixels);
    glTexImage2D(target, level, internalformat, width, height, border, format, type, (void *)pixels);
}

void d_glTexSubImage2D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{
    // if(pixels==NULL){
    //     glTexSubImage2D(target, level, xoffset, yoffset, width, height, format,  type,  NULL);
    // }
    Guest_Mem *guest_mem = (Guest_Mem *)pixels;
    // Scatter_Data *s_data=guest_mem->scatter_data;
    Opengl_Context *opengl_context = (Opengl_Context *)context;


    if (guest_mem->all_len == 0)
    {
        //pixels=NULL
        glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, NULL);
        return;
    }



    Texture_Binding_Status *texture_status = &(opengl_context->texture_binding_status);
    Buffer_Status *buffer_status = &(opengl_context->bound_buffer_status.buffer_status);


    // Pixel_Store_Status *status=&(((Opengl_Context *)context)->pixel_store_status);

    int start_loc = 0, end_loc = buf_len;
    // gl_pixel_data_loc(status,width,height,format,type,0,&start_loc,&end_loc);

    prepare_unpack_texture(context, guest_mem, start_loc, end_loc);
    if(target == GL_TEXTURE_EXTERNAL_OES)
    {
        if(texture_status->host_current_active_texture != 0)
        {
           glActiveTexture(GL_TEXTURE0);
        }
        glBindTexture(GL_TEXTURE_2D, texture_status->current_texture_external);
        // printf("%s external texture %u\n",__FUNCTION__,opengl_context->current_texture_external);        
        glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, type, 0);
        glBindTexture(GL_TEXTURE_2D, texture_status->host_current_texture_2D[0]);
        if(texture_status->host_current_active_texture != 0)
        {
            glActiveTexture(texture_status->host_current_active_texture + GL_TEXTURE0);
        }
    }
    else
    {
        glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, 0);
    }
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer_status->host_pixel_unpack_buffer);


    // if (opengl_context->current_texture_external == 0 || target != GL_TEXTURE_2D)
    // {
    //     prepare_unpack_texture(context, guest_mem, start_loc, end_loc);
    //     //这时候是立即返回的，后续会进行dma传输
    //     glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, 0);
    // }
    // else
    // {
    //     // printf("glTexSubImage2D to egl image gbuffer_id %llx \n",opengl_context->bind_image->gbuffer_id);
    //     // opengl_context->bind_image->host_has_data = 1;
        
    //     GLuint pre_texture = 0;
    //     if(target != GL_TEXTURE_2D)
    //     {
    //         glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&pre_texture);
    //         glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_external);
    //     }
    //     printf("%s external texture %u\n",__FUNCTION__,opengl_context->current_texture_external);

    //     prepare_unpack_texture_to_egl_image(context, width, height, format, type, buf_len, guest_mem);
    //     int real_height = height;
    //     int real_yoffset = yoffset;
    //     glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_HEIGHT, &real_height);
    //现在不知道下面这个判断是为了干啥
    //     if (real_height >= height)
    //     {
    //         real_yoffset = real_height - yoffset - height;
    //     }
    //     else if (real_height > height)
    //     {
    //         printf("error! get texture size real_height %d widht %d height %d", real_height, width, height);
    //     }
    //     glTexSubImage2D(target, level, xoffset, real_yoffset, width, height, format, type, 0);

    //     if(target != GL_TEXTURE_2D)
    //     {
    //         glBindTexture(GL_TEXTURE_2D, pre_texture);
    //     }

    // }
    // GLuint t;
    // glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&t);

    // printf("#%llx glTexSubImage2D_without %u target %x level %d xoffset %d yoffset %d width %d height %d format %x type %x start %d end %d\n",context,t,target,level,xoffset,yoffset,width,height,format,type,start_loc,end_loc);

    // glBindBuffer(GL_PIXEL_UNPACK_BUFFER, opengl_context->current_unpack_buffer);
}

void d_glTexSubImage2D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Texture_Binding_Status *texture_status = &(opengl_context->texture_binding_status);

    if(target == GL_TEXTURE_EXTERNAL_OES)
    {
        if(texture_status->host_current_active_texture != 0)
        {
            glActiveTexture(GL_TEXTURE0);
        }
        glBindTexture(GL_TEXTURE_2D, texture_status->current_texture_external);
        // printf("%s external texture %u\n",__FUNCTION__,opengl_context->current_texture_external);
        glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, type, (void *)pixels);
        glBindTexture(GL_TEXTURE_2D, texture_status->host_current_texture_2D[0]);
        if(texture_status->host_current_active_texture != 0)
        {
            glActiveTexture(texture_status->host_current_active_texture + GL_TEXTURE0);
        }

    }
    else
    {
        glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, (void *)pixels);
    }
}

void d_glTexImage3D_without_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{
    // if(pixels==NULL){
    //     glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, NULL);

    // }
    //没有绑定时，正好可以使用异步纹理传输
    Guest_Mem *guest_mem = (Guest_Mem *)pixels;
    // Scatter_Data *s_data=guest_mem->scatter_data;

    if (guest_mem->all_len == 0)
    {
        //pixels=NULL
        glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, NULL);

        return;
    }

    // Pixel_Store_Status *status=&(((Opengl_Context *)context)->pixel_store_status);

    int start_loc = 0, end_loc = buf_len;
    // gl_pixel_data_3d_loc(status,width,height,depth,format,type,0,&start_loc,&end_loc);

    express_printf("pixel start loc %d end loc %d\n", start_loc, end_loc);

    prepare_unpack_texture(context, guest_mem, start_loc, end_loc);

    //这时候是立即返回的，后续会进行dma传输
    glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, 0);

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, status->host_pixel_unpack_buffer);
}

void d_glTexImage3D_with_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLintptr pixels)
{
    glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, (void *)pixels);
}

void d_glTexSubImage3D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{

    // if(pixels==NULL){
    //     glTexSubImage3D( target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, NULL);
    // }
    //没有绑定时，正好可以使用异步纹理传输
    Guest_Mem *guest_mem = (Guest_Mem *)pixels;
    // Scatter_Data *s_data=guest_mem->scatter_data;

    if (guest_mem->all_len == 0)
    {
        //pixels=NULL
        glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, NULL);
        return;
    }

    // Pixel_Store_Status *status=&(((Opengl_Context *)context)->pixel_store_status);

    int start_loc = 0, end_loc = buf_len;
    // gl_pixel_data_3d_loc(status,width,height,depth,format,type,0,&start_loc,&end_loc);

    prepare_unpack_texture(context, guest_mem, start_loc, end_loc);

    //这时候是立即返回的，后续会进行dma传输
    glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, 0);

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, status->host_pixel_unpack_buffer);
}

void d_glTexSubImage3D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLintptr pixels)
{
    glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, (void *)pixels);
}

void d_glCompressedTexImage3D_without_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data)
{

    Guest_Mem *guest_mem = (Guest_Mem *)data;
    // Scatter_Data *s_data=guest_mem->scatter_data;
    // printf("compress texture format %x\n",internalformat);
    if (guest_mem->all_len == 0)
    {
        //pixels=NULL
        glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, NULL);
        return;
    }

    // Pixel_Store_Status *status=&(((Opengl_Context *)context)->pixel_store_status);
    prepare_unpack_texture(context, guest_mem, 0, imageSize);
    glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, 0);
        
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, status->host_pixel_unpack_buffer);
}

void d_glCompressedTexImage3D_with_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, GLintptr data)
{
    // printf("compress texture format %x\n",internalformat);

    glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, (void *)data);
}

void d_glCompressedTexSubImage3D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data)
{
    Guest_Mem *guest_mem = (Guest_Mem *)data;
    // Scatter_Data *s_data=guest_mem->scatter_data;

    if (guest_mem->all_len == 0)
    {
        //pixels=NULL
        glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, NULL);
        return;
    }

    // Pixel_Store_Status *status=&(((Opengl_Context *)context)->pixel_store_status);
    prepare_unpack_texture(context, guest_mem, 0, imageSize);
    glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, 0);
    

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, status->host_pixel_unpack_buffer);
}

void d_glCompressedTexSubImage3D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, GLintptr data)
{
    glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, (void *)data);
}

void d_glCompressedTexImage2D_without_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data)
{
    Guest_Mem *guest_mem = (Guest_Mem *)data;
    // Scatter_Data *s_data=guest_mem->scatter_data;
    // printf("compress texture format %x\n",internalformat);

    if (guest_mem->all_len == 0)
    {
        //pixels=NULL
        glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, NULL);
        return;
    }
    // Pixel_Store_Status *status=&(((Opengl_Context *)context)->pixel_store_status);
    prepare_unpack_texture(context, guest_mem, 0, imageSize);
    glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, 0);
    

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, status->host_pixel_unpack_buffer);

}

void d_glCompressedTexImage2D_with_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, GLintptr data)
{
    // printf("compress texture format %x\n",internalformat);

    glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, (void *)data);
}

void d_glCompressedTexSubImage2D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data)
{
    Guest_Mem *guest_mem = (Guest_Mem *)data;
    // Scatter_Data *s_data=guest_mem->scatter_data;

    if (guest_mem->all_len == 0)
    {
        //pixels=NULL
        glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, NULL);
        return;
    }

    // Pixel_Store_Status *status=&(((Opengl_Context *)context)->pixel_store_status);
    prepare_unpack_texture(context, guest_mem, 0, imageSize);

    // GLenum error = glGetError();

    // if(error!=GL_NO_ERROR)
    // {
    //     printf("glCompressedTexSubImage2D prepare error %x\n",error);
    // }

    glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, 0);

    // error = glGetError();

    // if(error!=GL_NO_ERROR)
    // {
    //     Opengl_Context *opengl_context = (Opengl_Context *)context;
    //     printf("glCompressedTexSubImage2D error %x target %d level %d xoffset %d yoffset %d width %d height %d format %x size %d\n",error,target, level, xoffset, yoffset, width, height, format, imageSize);
    //     GLint now_texture = 0;
    //     glGetIntegerv(GL_TEXTURE_BINDING_2D, &now_texture);
    //     printf("current texture %d current active %d real %d is_texture %d\n",opengl_context->current_texture_2D[opengl_context->current_active_texture],
    //         opengl_context->current_active_texture, now_texture, (int)glIsTexture(now_texture));

    //     // GLint row_len = 0;
    //     // glGetIntegerv(GL_UNPACK_ROW_LENGTH, &row_len);

    //     // GLint row_len = 0;
    //     // glGetIntegerv(GL_UNPACK_ROW_LENGTH, &row_len);



    // }

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, status->host_pixel_unpack_buffer);

}

void d_glCompressedTexSubImage2D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, GLintptr data)
{
    glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, (void *)data);
}

void d_glReadPixels_without_bound(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, void *pixels)
{
    //由于没有绑定的情况还是需要读取完数据后再进行复制，所以这里实际上无法做到异步
    Guest_Mem *guest_mem = (Guest_Mem *)pixels;
    // Scatter_Data *s_data=guest_mem->scatter_data;

    if (guest_mem->all_len == 0)
    {
        //pixels=NULL
        glReadPixels(x, y, width, height, format, type, NULL);
        return;
    }

    // Pixel_Store_Status *status=&(((Opengl_Context *)context)->pixel_store_status);

    int start_loc = 0, end_loc = buf_len;
    // gl_pixel_data_loc(status,width,height,format,type,0,&start_loc,&end_loc);

    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);

    GLint asyn_texture = bound_buffer->asyn_pack_texture_buffer;
    glBindBuffer(GL_PIXEL_PACK_BUFFER, asyn_texture);
    //因为曾经bind过texture，所以这里直接bind相应的buffer，这里重新bufferdata是为了孤立缓冲区
    glBufferData(GL_PIXEL_PACK_BUFFER, end_loc, NULL, GL_STREAM_READ);
    glReadPixels(x, y, width, height, format, type, 0);

    //注意，此句会阻塞，直到pixels全部下载下来
    GLubyte *map_pointer = glMapBufferRange(GL_PIXEL_PACK_BUFFER, start_loc, end_loc - start_loc, GL_MAP_READ_BIT);
    // host_guest_buffer_exchange(s_data,map_pointer,start_loc,end_loc-start_loc,0);

    // char print_chars[1000];
    // int print_loc = 0;
    // for(int i=0;i<end_loc-start_loc && i<100;i++)
    // {
    //     print_loc += sprintf(print_chars+print_loc, "%.2x",(int)map_pointer[i]);
    // }
    // printf("glreadpixels %s\n",print_chars);

    guest_read(guest_mem, map_pointer, 0, end_loc - start_loc);

    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);


    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, status->host_pixel_pack_buffer);
}

void d_glReadPixels_with_bound(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels)
{
    glReadPixels(x, y, width, height, format, type, (void *)pixels);
}

void d_glReadBuffer_special(void *context, GLenum src)
{
    glReadBuffer(src);
}

void prepare_unpack_texture_to_egl_image(void *context, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, Guest_Mem *guest_mem)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    GLint asyn_texture = bound_buffer->asyn_unpack_texture_buffer;
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, asyn_texture);

    //因为曾经bind过texture，所以这里bind相应的buffer，这里重新bufferdata是为了孤立缓冲区
    glBufferData(GL_PIXEL_UNPACK_BUFFER, buf_len, NULL, GL_STREAM_DRAW);
    // express_printf("gl get error %x\n",glGetError());

    //然后把数据复制到内存里，之后交给dma传输   到底是invalidata还是unsync？
    GLubyte *map_pointer = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, buf_len, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

    int row_byte_len = buf_len / height;
    if (buf_len % height != 0)
    {
        printf("error! prepare_unpack_texture_to_egl_image buf_len %d %% height %d (width %d, format %x type %x) = %d!", buf_len, height, width, format, type, buf_len % height);
    }
    for (int i = 0; i < height; i++)
    {
        // guest_write(guest_mem, map_pointer + (height - i - 1) * row_byte_len, i * row_byte_len, row_byte_len);
        guest_write(guest_mem, map_pointer + i * row_byte_len , i * row_byte_len, row_byte_len);

    }

    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
}

void d_glGraphicBufferData(void *t_context, EGLContext ctx, uint64_t gbuffer_id,int width, int height, int buf_len, int row_byte_len, int stride, const void *real_buffer)
{
    //没有绑定时，正好可以使用异步纹理传输

    Render_Thread_Context *thread_context = (Render_Thread_Context *)t_context;

    Process_Context *process_context = thread_context->process_context;
    Guest_Mem *guest_mem = (Guest_Mem *)real_buffer;


    Graphic_Buffer *gbuffer = (Graphic_Buffer *)g_hash_table_lookup(process_context->gbuffer_map, GUINT_TO_POINTER(gbuffer_id));

    if(gbuffer == NULL)
    {
        gbuffer = get_gbuffer_from_global_map(gbuffer_id);
    }

    if(gbuffer == NULL || width != gbuffer->width || height != gbuffer->height)
    {
        return;
    }

    Opengl_Context *opengl_context = (Opengl_Context *)thread_context->opengl_context;
    if(opengl_context == NULL)
    {
        opengl_context = (Opengl_Context *)g_hash_table_lookup(process_context->context_map, GUINT_TO_POINTER(ctx));
        if(opengl_context == NULL)
        {
            printf("error! GraphicBufferData get null context!\n");
        }
        else
        {
            if(opengl_context->independ_mode==1)
            {
                glfwMakeContextCurrent((GLFWwindow *)opengl_context->window);
            }
            else
            {
                egl_makeCurrent(opengl_context->window);
            }
        }
        // #ifdef USE_GLFW_AS_WGL
        // // printf("make current context %llx windows %llx\n",real_opengl_context,real_opengl_context->window);
        //     glfwMakeContextCurrent((GLFWwindow *)opengl_context->window);
        // #else
        //     egl_makeCurrent(opengl_context->window);
        // #endif
    }

    int real_width = width;
    if (real_width % (stride) != 0)
    {
        real_width = (real_width / stride + 1) * stride;
    }

    int guest_row_byte_len = row_byte_len / width * real_width;

    // printf("GraphicBuffer data width %d height %d row_byte_len %d guest_row_byte_len %d\n", egl_image->width, egl_image->height, row_byte_len, guest_row_byte_len);

    if (row_byte_len * height > buf_len)
    {
        printf("error! GraphicBuffer Data len error! row %d height %d get len %d\n", row_byte_len, height, buf_len);
        return;
    }

    // GLuint pre_texture;
    // GLuint pre_unpack;
    // glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&pre_texture);
    // glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, (GLint *)&pre_unpack);

    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);
    GLint asyn_texture = bound_buffer->asyn_unpack_texture_buffer;
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, asyn_texture);

    //因为曾经bind过texture，所以这里bind相应的buffer，这里重新bufferdata是为了孤立缓冲区
    glBufferData(GL_PIXEL_UNPACK_BUFFER, row_byte_len * height, NULL, GL_STREAM_DRAW);

    GLubyte *map_pointer = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, row_byte_len * height, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

    // GraphicBuffer里的图片是正的，放到纹理里要倒个个
    // -- 不用倒个了，因为系统内整体进行了倒个
    // for (int i = 0; i < egl_image->height; i++)
    // {
    //     guest_write(guest_mem, map_pointer + (egl_image->height - i - 1) * row_byte_len, i * guest_row_byte_len, row_byte_len);
    // }
    guest_write(guest_mem, map_pointer, 0, buf_len);
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

    glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);
    //这时候是立即返回的，后续会进行dma传输

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, gbuffer->width, gbuffer->height, gbuffer->format, gbuffer->pixel_type, NULL);

    //注意，graphicdata调用前会调用bindEGLImage来绑定EGLImage对应的那个特殊纹理，所以这里不能使用缓存值
    // glBindTexture(GL_TEXTURE_2D, pre_texture);
    // glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pre_unpack);
    Buffer_Status *buffer_status = &(opengl_context->bound_buffer_status.buffer_status);
    Texture_Binding_Status *texture_status = &(opengl_context->texture_binding_status);

    glBindTexture(GL_TEXTURE_2D, texture_status->host_current_texture_2D[texture_status->host_current_active_texture]);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer_status->host_pixel_unpack_buffer);
    

    // glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_2D[opengl_context->current_active_texture]);
    // glBindBuffer(GL_PIXEL_UNPACK_BUFFER, opengl_context->current_unpack_buffer);



    if(thread_context->opengl_context == NULL)
    {
        if(opengl_context->independ_mode==1)
        {
            glfwMakeContextCurrent((GLFWwindow *)NULL);
        }
        else
        {
            egl_makeCurrent(NULL);
        }
        // #ifdef USE_GLFW_AS_WGL
        // // printf("make current context %llx windows %llx\n",real_opengl_context,real_opengl_context->window);
        //     glfwMakeContextCurrent((GLFWwindow *)NULL);
        // #else
        //     egl_makeCurrent(NULL);
        // #endif
    }

    // printf("get graphic buffer from image %llx guest width %d height %d format %x len %d texture %u\n", gbuffer_id, width, height, gbuffer->format, buf_len, gbuffer->data_texture);
}

void d_glReadGraphicBuffer(void *r_context, EGLContext ctx, uint64_t gbuffer_id, int width, int height, int buf_len, int row_byte_len, int stride, void *real_buffer)
{
    //没有绑定时，正好可以使用异步纹理传输
    Render_Thread_Context *thread_context = (Render_Thread_Context *)r_context;

    Process_Context *process_context = thread_context->process_context;
    Guest_Mem *guest_mem = (Guest_Mem *)real_buffer;


    Graphic_Buffer *gbuffer = (Graphic_Buffer *)g_hash_table_lookup(process_context->gbuffer_map, GUINT_TO_POINTER(gbuffer_id));

    if(gbuffer == NULL)
    {
        gbuffer = get_gbuffer_from_global_map(gbuffer_id);
    }

    if(gbuffer == NULL || width != gbuffer->width || height != gbuffer->height)
    {
        return;
    }

    Opengl_Context *opengl_context = (Opengl_Context *)thread_context->opengl_context;
    if(opengl_context == NULL)
    {
        opengl_context = (Opengl_Context *)g_hash_table_lookup(process_context->context_map, GUINT_TO_POINTER(ctx));
        if(opengl_context->independ_mode==1)
        {
            glfwMakeContextCurrent((GLFWwindow *)opengl_context->window);
        }
        else
        {
            egl_makeCurrent(opengl_context->window);
        }
        // #ifdef USE_GLFW_AS_WGL
        // // printf("make current context %llx windows %llx\n",real_opengl_context,real_opengl_context->window);
        //     glfwMakeContextCurrent((GLFWwindow *)opengl_context->window);
        // #else
        //     egl_makeCurrent(opengl_context->window);
        // #endif
    }


    int real_width = width;
    if (real_width % (stride) != 0)
    {
        real_width = (real_width / stride + 1) * stride;
    }

    int guest_row_byte_len = row_byte_len / width * real_width;

    // printf("GraphicBuffer data width %d height %d row_byte_len %d guest_row_byte_len %d\n", egl_image->width, egl_image->height, row_byte_len, guest_row_byte_len);

    if (row_byte_len * height > buf_len)
    {
        printf("error! GraphicBuffer read Data len error! row %d height %d get len %d\n", row_byte_len, height, buf_len);
        return;
    }

    GLuint pre_fbo;
    // GLuint pre_pack;
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, (GLint *)&pre_fbo);
    // glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, (GLint *)&pre_pack);

    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);
    GLint asyn_texture = bound_buffer->asyn_pack_texture_buffer;
    glBindBuffer(GL_PIXEL_PACK_BUFFER, asyn_texture);

    //因为曾经bind过texture，所以这里bind相应的buffer，这里重新bufferdata是为了孤立缓冲区
    glBufferData(GL_PIXEL_PACK_BUFFER, buf_len, NULL, GL_STREAM_DRAW);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, gbuffer->data_fbo);
    
    glReadPixels(0, 0, gbuffer->width, gbuffer->height, gbuffer->format, gbuffer->pixel_type, 0);

    GLubyte *map_pointer = glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, buf_len, GL_MAP_READ_BIT);

    //read不确定是否需要颠倒，遇到再说@todo
    for (int i = 0; i < gbuffer->height; i++)
    {
        guest_read(guest_mem, map_pointer + (gbuffer->height - i - 1) * row_byte_len, i * row_byte_len, row_byte_len);
    }

    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);


    glBindFramebuffer(GL_READ_FRAMEBUFFER, pre_fbo);
    // glBindBuffer(GL_PIXEL_PACK_BUFFER, pre_pack);

    // glBindFramebuffer(GL_READ_FRAMEBUFFER, opengl_context->rea);

    Buffer_Status *buffer_status = &(opengl_context->bound_buffer_status.buffer_status);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer_status->host_pixel_pack_buffer);
    

    // glBindBuffer(GL_PIXEL_PACK_BUFFER, opengl_context->current_pack_buffer);

    if(thread_context->opengl_context == NULL)
    {
        #ifdef USE_GLFW_AS_WGL
        // printf("make current context %llx windows %llx\n",real_opengl_context,real_opengl_context->window);
            glfwMakeContextCurrent((GLFWwindow *)NULL);
        #else
            egl_makeCurrent(NULL);
        #endif
    }

    // printf("send graphic buffer from image %llx guest width %d height %d format %x len %d\n", g_buffer_id, egl_image->width, egl_image->height, egl_image->format, buf_len);
}


