// #define STD_DEBUG_LOG

#include "express-gpu/glv3_texture.h"

void prepare_unpack_texture(void *context, Guest_Mem *guest_mem, int start_loc, int end_loc);

void prepare_unpack_texture_to_egl_image(void *context, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, Guest_Mem *guest_mem);

void d_glPixelStorei_origin(void *context, GLenum pname, GLint param)
{

    // Pixel_Store_Status *pixel_store = &(((Opengl_Context *)context)->pixel_store_status);
    // switch (pname)
    // {
    // case GL_UNPACK_ALIGNMENT:
    //     pixel_store->unpack_alignment = param;
    //     break;
    // case GL_PACK_ALIGNMENT:
    //     pixel_store->pack_alignment = param;
    //     break;
    // case GL_UNPACK_ROW_LENGTH:
    //     pixel_store->unpack_row_length = param;
    //     break;
    // case GL_UNPACK_IMAGE_HEIGHT:
    //     pixel_store->unpack_image_height = param;
    //     break;
    // case GL_UNPACK_SKIP_PIXELS:
    //     pixel_store->unpack_skip_pixels = param;
    //     break;
    // case GL_UNPACK_SKIP_ROWS:
    //     pixel_store->unpack_skip_rows = param;
    //     break;
    // case GL_UNPACK_SKIP_IMAGES:
    //     pixel_store->unpack_skip_images = param;
    //     break;
    // case GL_PACK_ROW_LENGTH:
    //     pixel_store->pack_row_length = param;
    //     break;
    // case GL_PACK_SKIP_PIXELS:
    //     pixel_store->pack_skip_pixels = param;
    //     break;
    // case GL_PACK_SKIP_ROWS:
    //     pixel_store->pack_skip_rows = param;
    //     break;
    // default:
    //     return;
    // }
    glPixelStorei(pname, param);
    //express_printf("glPixelStorei %x %d\n",pname,param);

    return;
}

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

void gl_pixel_data_loc(void *store_status, GLsizei width, GLsizei height, GLenum format, GLenum type, int pack, int *start_loc, int *end_loc)
{
    gl_pixel_data_3d_loc(store_status, width, height, 1, format, type, pack, start_loc, end_loc);
}

void gl_pixel_data_3d_loc(void *store_status, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, int pack, int *start_loc, int *end_loc)
{
    Pixel_Store_Status *status = (Pixel_Store_Status *)store_status;
    int align = 4;
    int row_length = 0;
    int skip_rows = 0;
    int skip_pixels = 0;
    int skip_images = 0;
    int image_height = 0;

    GLsizei real_width;
    GLsizei real_height;

    //获得此时的状态值
    if (pack)
    {
        align = status->pack_alignment;
        row_length = status->pack_row_length;
        skip_rows = status->pack_skip_rows;
        skip_pixels = status->pack_skip_pixels;
    }
    else
    {
        align = status->unpack_alignment;
        row_length = status->unpack_row_length;
        skip_rows = status->unpack_skip_rows;
        skip_pixels = status->unpack_skip_pixels;

        skip_images = status->unpack_skip_images;
        image_height = status->unpack_image_height;
    }

    //获得一张图片的真实像素宽度和高度
    real_width = row_length == 0 ? width : row_length;
    real_height = image_height == 0 ? height : image_height;

    //每个像素点所占的空间
    GLsizei pixel_size = pixel_size_calc(format, type);
    //获得图片每一行的字节数
    GLsizei width_size = pixel_size * real_width;
    //每一行的数据进行对其
    width_size = (width_size + align - 1) & (~(align - 1));

    //数据图像开始读取的地方，等于（跳过的图片数目*图片的高度+跳过的行数）*每行所占的字节数+跳过的像素数*像素大小
    *start_loc = (skip_images * real_height + skip_rows) * width_size + skip_pixels * pixel_size;

    //数据图像结束读取的地方，等于 开始读取的地方+要读取的图片的高度*图片每行所占的字节数*图片的张数，depth表示深度也就表示要读取多少张图片
    *end_loc = *start_loc + real_height * width_size * depth;

    return;
}

void d_glTexImage2D_without_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{
    // if(pixels==NULL){
    //     glTexImage2D(target,level,internalformat,width,height,border,format,type,NULL);
    // }
    //没有绑定时，正好可以使用异步纹理传输
    Guest_Mem *guest_mem = (Guest_Mem *)pixels;
    // Scatter_Data *s_data=guest_mem->scatter_data;
    // GLuint t;
    // glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&t);
    // if(width==64 &&height ==64 && buf_len ==16384){
    //     printf("debug");
    // }

    // printf(#"%llx teximage2d %u without size %d %d len %d %lld\n",context,t,width,height,buf_len,pixels);
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

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
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

    if (guest_mem->all_len == 0)
    {
        //pixels=NULL
        glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, NULL);
        return;
    }

    // Pixel_Store_Status *status=&(((Opengl_Context *)context)->pixel_store_status);

    int start_loc = 0, end_loc = buf_len;
    // gl_pixel_data_loc(status,width,height,format,type,0,&start_loc,&end_loc);

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    if (opengl_context->bind_image == NULL || target != GL_TEXTURE_2D)
    {
        prepare_unpack_texture(context, guest_mem, start_loc, end_loc);
        //这时候是立即返回的，后续会进行dma传输
        glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, 0);
    }
    else
    {
        // printf("glTexSubImage2D to egl image gbuffer_id %llx \n",opengl_context->bind_image->gbuffer_id);
        opengl_context->bind_image->host_has_data = 1;

        prepare_unpack_texture_to_egl_image(context, width, height, format, type, buf_len, guest_mem);
        int real_height = height;
        int real_yoffset = yoffset;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_HEIGHT, &real_height);

        if (real_height >= height)
        {
            real_yoffset = real_height - yoffset - height;
        }
        else if (real_height > height)
        {
            printf("error! get texture size real_height %d widht %d height %d", real_height, width, height);
        }
        glTexSubImage2D(target, level, xoffset, real_yoffset, width, height, format, type, 0);
    }
    // GLuint t;
    // glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&t);

    // printf("#%llx glTexSubImage2D_without %u target %x level %d xoffset %d yoffset %d width %d height %d format %x type %x start %d end %d\n",context,t,target,level,xoffset,yoffset,width,height,format,type,start_loc,end_loc);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void d_glTexSubImage2D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels)
{
    glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, (void *)pixels);
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

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
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

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void d_glTexSubImage3D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLintptr pixels)
{
    glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, (void *)pixels);
}

void d_glCompressedTexImage3D_without_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data)
{

    Guest_Mem *guest_mem = (Guest_Mem *)data;
    // Scatter_Data *s_data=guest_mem->scatter_data;

    if (guest_mem->all_len == 0)
    {
        //pixels=NULL
        glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, NULL);
        return;
    }

    // Pixel_Store_Status *status=&(((Opengl_Context *)context)->pixel_store_status);
    prepare_unpack_texture(context, guest_mem, 0, imageSize);
    glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void d_glCompressedTexImage3D_with_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, GLintptr data)
{

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
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void d_glCompressedTexSubImage3D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, GLintptr data)
{
    glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, (void *)data);
}

void d_glCompressedTexImage2D_without_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data)
{
    Guest_Mem *guest_mem = (Guest_Mem *)data;
    // Scatter_Data *s_data=guest_mem->scatter_data;

    if (guest_mem->all_len == 0)
    {
        //pixels=NULL
        glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, NULL);
        return;
    }
    // Pixel_Store_Status *status=&(((Opengl_Context *)context)->pixel_store_status);
    prepare_unpack_texture(context, guest_mem, 0, imageSize);
    glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void d_glCompressedTexImage2D_with_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, GLintptr data)
{
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
    glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
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
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
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
        printf("error！ prepare_unpack_texture_to_egl_image buf_len %d %% height %d (width %d, format %x type %x) = %d!", buf_len, height, width, format, type, buf_len % height);
    }
    for (int i = 0; i < height; i++)
    {
        // guest_write(guest_mem, map_pointer + (height - i - 1) * row_byte_len, i * row_byte_len, row_byte_len);
        guest_write(guest_mem, map_pointer + i * row_byte_len , i * row_byte_len, row_byte_len);

    }

    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
}

void d_glGraphicBufferData(void *context, uint64_t g_buffer_id, int buf_len, const void *real_buffer)
{
    //没有绑定时，正好可以使用异步纹理传输
    Guest_Mem *guest_mem = (Guest_Mem *)real_buffer;
    Opengl_Context *opengl_context = (Opengl_Context *)context;


    EGL_Image *egl_image = get_image_from_gbuffer_id(g_buffer_id);

    int row_byte_len = egl_image->row_byte_len;

    int real_width = egl_image->width;
    if (real_width % (egl_image->stride) != 0)
    {
        real_width = (real_width / egl_image->stride + 1) * egl_image->stride;
    }

    int guest_row_byte_len = row_byte_len / egl_image->width * real_width;

    // printf("GraphicBuffer data width %d height %d row_byte_len %d guest_row_byte_len %d\n", egl_image->width, egl_image->height, row_byte_len, guest_row_byte_len);

    if (row_byte_len * egl_image->height > buf_len)
    {
        printf("error! GraphicBuffer Data len error! row %d height %d get len %d\n", row_byte_len, egl_image->height, buf_len);
        return;
    }

    GLuint pre_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&pre_texture);

    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);
    GLint asyn_texture = bound_buffer->asyn_unpack_texture_buffer;
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, asyn_texture);

    //因为曾经bind过texture，所以这里bind相应的buffer，这里重新bufferdata是为了孤立缓冲区
    glBufferData(GL_PIXEL_UNPACK_BUFFER, row_byte_len * egl_image->height, NULL, GL_STREAM_DRAW);

    GLubyte *map_pointer = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, row_byte_len * egl_image->height, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

    // GraphicBuffer里的图片是正的，放到纹理里要倒个个
    for (int i = 0; i < egl_image->height; i++)
    {
        guest_write(guest_mem, map_pointer + (egl_image->height - i - 1) * row_byte_len, i * guest_row_byte_len, row_byte_len);
    }
    // guest_write(guest_mem, map_pointer, 0, buf_len);
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

    glBindTexture(GL_TEXTURE_2D, egl_image->fbo_texture);
    //这时候是立即返回的，后续会进行dma传输

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, egl_image->width, egl_image->height, egl_image->format, egl_image->pixel_type, NULL);

    //注意，graphicdata调用前会调用bindEGLImage来绑定EGLImage对应的那个特殊纹理，所以这里不能使用缓存值
    glBindTexture(GL_TEXTURE_2D, pre_texture);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    express_printf("get graphic buffer from image %llx guest width %d height %d format %x len %d\n", g_buffer_id, egl_image->width, egl_image->height, egl_image->format, buf_len);
}

void d_glReadGraphicBuffer(void *context, uint64_t g_buffer_id, int buf_len, void *real_buffer)
{
    //没有绑定时，正好可以使用异步纹理传输
    Guest_Mem *guest_mem = (Guest_Mem *)real_buffer;

    EGL_Image *egl_image = get_image_from_gbuffer_id(g_buffer_id);

    int row_byte_len = egl_image->row_byte_len;

    if (row_byte_len * egl_image->height > buf_len)
    {
        printf("error! GraphicBuffer Data len error! row %d height %d get len %d", row_byte_len, egl_image->height, buf_len);
        return;
    }

    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    GLint asyn_texture = bound_buffer->asyn_unpack_texture_buffer;
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, asyn_texture);

    //因为曾经bind过texture，所以这里bind相应的buffer，这里重新bufferdata是为了孤立缓冲区
    glBufferData(GL_PIXEL_UNPACK_BUFFER, buf_len, NULL, GL_STREAM_DRAW);

    glReadPixels(0, 0, egl_image->width, egl_image->height, egl_image->format, egl_image->pixel_type, 0);

    GLubyte *map_pointer = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, buf_len, GL_MAP_READ_BIT);

    for (int i = 0; i < egl_image->height; i++)
    {
        guest_read(guest_mem, map_pointer + (egl_image->height - i - 1) * row_byte_len, i * row_byte_len, row_byte_len);
    }

    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    // printf("send graphic buffer from image %llx guest width %d height %d format %x len %d\n", g_buffer_id, egl_image->width, egl_image->height, egl_image->format, buf_len);
}

void d_glBindTexture_special(void *context, GLenum target, GLuint texture)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    // printf("context %llx target %x texture %u current %d\n", opengl_context,target, texture,opengl_context->current_active_texture);
    if (target == GL_TEXTURE_EXTERNAL_OES)
    {
        target = GL_TEXTURE_2D;
        opengl_context->current_texture_external = texture;
    }
    else if (target == GL_TEXTURE_2D)
    {
        opengl_context->current_texture_2D[opengl_context->current_active_texture] = texture;
    }
    

    opengl_context->bind_image = NULL;
    glBindTexture(target, texture);
}

void d_glActiveTexture_special(void *context, GLenum texture)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    if (texture - GL_TEXTURE0 >= 0 && texture - GL_TEXTURE0 < preload_static_context_value->max_combined_texture_image_units)
    {
        opengl_context->current_active_texture = texture - GL_TEXTURE0;
        
    }
    // printf("active texuture %d\n",opengl_context->current_active_texture);

    glActiveTexture(texture);
}
