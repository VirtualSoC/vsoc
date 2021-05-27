#include "glad.h"

#include <GLES/glplatform.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2platform.h>
#include <GLES/glext.h>

#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl31.h>




/**
 * @brief 根据像素格式和类型计算一个像素所占的空间的字节大小
 * 
 * @param format 像素格式
 * @param type 像素类型
 * @return int 
 */
int pixel_size_calc(GLenum format, GLenum type);


/**
 * @brief opengl各种类型数据的sizeof函数
 * 
 * @param type 
 * @return size_t 
 */
size_t gl_sizeof(GLenum type);


size_t gl_pname_size(GLenum pname);