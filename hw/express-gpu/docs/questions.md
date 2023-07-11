# Questions

* [x] glBlendEquationSeparatei为什么要做如下的buffer转换?
```c++
void d_glBlendEquationSeparatei(void *context, GLuint buf, GLenum modeRGB, GLenum modeAlpha)
{
    CHECK_VALID_ENUM(__LINE__, modeRGB, 5, GL_FUNC_ADD, GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT, GL_MIN, GL_MAX);
    CHECK_VALID_ENUM(__LINE__, modeAlpha, 5, GL_FUNC_ADD, GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT, GL_MIN, GL_MAX);
    //@todo status save
    if (buf == GL_BACK)
    {
        buf = GL_COLOR_ATTACHMENT0;
    }
    r_glBlendEquationSeparatei(context, buf, modeRGB, modeAlpha);
}
```  
A: 因为要做FB模拟，FBO没有GL_BACK的buffer。

* [x] 对GLSL shading language的修改仅限于版本？照理说，OpenGL兼容GLSL ES的shading language，所以涉及GLSL的扩展和扩充功能应该都不用改我们的图形映射。  
A: 要改。Intel显卡对GLSL ES兼容性比较强；NVIDIA显卡就比较弱了，需要改的。
* [ ] glDrawArraysIndirect/glVertexAttribBinding的流程不懂。
* [x] 对PBO使用glBufferData(NULL)的时候，之前旧缓冲区的数据是被GPU用完以后自动释放的？  
A: 是的。

* [x] Desktop端，VBO，texture等是否先在内存里，然后在glDrawArrays的时候copy到显存里。Default framebuffer/FBO attachment是否只在显存里有？  
A: 在Desktop端，VBO等所有需要glBufferData的实际缓冲区位置由GL驱动决定，当然，GL驱动会参考glBufferData指定的usage。最终绘画时，用到的这些VBO等一定会读到显存里，因为独立GPU只能操作显存。
* [x] Guest端因为是共享内存，所以原则上CPU也能拿到显存的数据。但他怎么知道显存的数据在哪里？  
A: GPU直接写进GLES提供的framebuffer内存里。
* [x] FBO默认渲染到COLOR_ATTACHMENT_0吗。  
A: 由glDrawBuffers指定。
* [x] What is Bound_Buffer? 它和Binding Status是什么关系。
```c++
typedef struct Bound_Buffer
{
    Buffer_Status *buffer_status;
    Attrib_Point *attrib_point;
    GLuint now_vao;
    std::unordered_map<GLint, GLenum> buffer_type;
    std::unordered_map<GLint, Attrib_Point *> vao_point_data;
    std::unordered_map<GLint, Element_Array_Buffer *> ebo_buffer;
    //打表表示索引下标最少是2的多少幂次
    GLuint *ebo_mm;
    GLuint ebo_mm_num;
} Bound_Buffer;
```
A: Bound_Buffer存的是当前绑定的gl object和Buffer_Status（即当前绑定的buffer object）；Binding Status其实应该叫Other Binding Status，指的是非gl object的binding status。
* [x] 一个texture里面只会有一张图片？不同level的mipmap和texture的图片之间是什么关系？level0 = image; level1 = resized_image1; ...? 它们之间地位相等？  
A: 名义上，一个texture里只有一张图片，mipmap的GL格式和图像格式和texture相等，i级mipmap长宽各是i-1级的一半。level>0的mipmap可以调glGenerateMipmap来生成，也可以自己生成以后，调glTexStorage2D传进去。所以，理论上也许可以把各级mipmap当独立图片用。
* [x] need help with `d_glFramebufferTexture2D_special`.  
A: glFramebufferTexture2D中的texture可能是从EGLImage生成的，并不是调glGenTextures生成的，所以需要特判一下。如果texture是从EGLImage生成的，就需要去拿它对应的gbuffer，然后和这个texture绑定起来。
* [x] 那么问题来了，为啥d_glFramebufferTextureLayer里面就没有特判EGLImage？  
A: 高迪也不清楚。可能EGLImage的texture不能调glFramebufferTextureLayer。
* [x] 为啥Guest和Host端的VAO需要同步`vao_binding_status_sync`？为啥要允许不同步的情况出现？  
A: DSA模式的Host没有binding status，所以需要同步。
* [x] 为啥只有一个gbuffer_2d_binding和一个gbuffer_external_binding？照理说，draw的时候可能会用到不止一个gbuffer？  
A: 理论上确实可能用到不止一个gbuffer，但目前没遇到过这样用的app。
* [x] glSamplerParameter怎么没有在guest端缓存，而是直接r_glSampleParameter传到host端了。  
A: 忘记做缓存了。
* [x] gbuffer类型介绍（1）：GBUFFER_TYPE_WINDOW。这类gbuffer一一对应安卓窗口的graphic buffer，全程由host端管理，不需要guest端上传。
* [x] gbuffer_type_bitmap和gbuffer_type_fbo在buffer upload/download上有什么区别吗。  
A: 有一点点区别。在一个gbuffer被第二次用来创建EGLImage，并且没有调用glTexSubImage2D就又被销毁时，gbuffer_type_bitmap类型的gbuffer不会下载数据。其实这个区别是因为高迪想复用之前的gbuffer状态。理论上，第二次创建EGLImage时也可以直接重制为gbuffer_type_native，不过这样会多几次调用。
* [x] 是不是EGLImage生成的texture也可以绑定到GL_TEXTURE_2D点上去。  
A: 是的。而且EGLImage生成的texture只能绑定到GL_TEXTURE_2D或者GL_TEXTURE_EXTERNAL_OES上。
* [x] guest->host传数据的时候可以直接写teleport_express，但host->guest传数据的时候是不是得先让guest开一段内存，然后把指针传给host端？  
A: 是的。并且如果要持久映射的话，需要pin_mem_page()，避免内存被安卓压缩或者swap掉。另外，实现持久映射的r_gl函数FUNID是`(EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x2) << 24u) + xxx`。
* [ ] 其他外设怎么读显存？能否在没有CPU的帮助下读显存？比如相机到GPU、GPU到屏幕。
* [ ] 没有GPU的时候，是不是graphicbuffer之上的逻辑都不变，只是gralloc分配出来的是内存罢了？
