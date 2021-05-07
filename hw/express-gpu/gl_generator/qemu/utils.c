#include "glad.h"

#include "glext.h"
#include "gl2ext.h"

#include "gl3.h"
#include "gl.h"

#include "utils.h"



/**
 * @brief 根据像素格式和类型计算一个像素所占的空间的字节大小
 * 
 * @param format 像素格式
 * @param type 像素类型
 * @return int 
 */
int pixel_size_calc(GLenum format, GLenum type) {
    switch(type) {
    case GL_BYTE:
        switch(format) {
        case GL_R8:
        case GL_R8I:
        case GL_R8_SNORM:
        case GL_RED:             return sizeof(char);
        case GL_RED_INTEGER:     return sizeof(char);
        case GL_RG8:
        case GL_RG8I:
        case GL_RG8_SNORM:
        case GL_RG:              return sizeof(char) * 2;
        case GL_RG_INTEGER:      return sizeof(char) * 2;
        case GL_RGB8:
        case GL_RGB8I:
        case GL_RGB8_SNORM:
        case GL_RGB:             return sizeof(char) * 3;
        case GL_RGB_INTEGER:     return sizeof(char) * 3;
        case GL_RGBA8:
        case GL_RGBA8I:
        case GL_RGBA8_SNORM:
        case GL_RGBA:            return sizeof(char) * 4;
        case GL_RGBA_INTEGER:    return sizeof(char) * 4;
        }
        break;
    case GL_UNSIGNED_BYTE:
        switch(format) {
        case GL_R8:
        case GL_R8UI:
        case GL_RED:             return sizeof(unsigned char);
        case GL_RED_INTEGER:     return sizeof(unsigned char);
        case GL_ALPHA8_EXT:
        case GL_ALPHA:           return sizeof(unsigned char);
        case GL_LUMINANCE8_EXT:
        case GL_LUMINANCE:       return sizeof(unsigned char);
        case GL_LUMINANCE8_ALPHA8_EXT:
        case GL_LUMINANCE_ALPHA: return sizeof(unsigned char) * 2;
        case GL_RG8:
        case GL_RG8UI:
        case GL_RG:              return sizeof(unsigned char) * 2;
        case GL_RG_INTEGER:      return sizeof(unsigned char) * 2;
        case GL_RGB8:
        case GL_RGB8UI:
        case GL_SRGB8:
        case GL_RGB:             return sizeof(unsigned char) * 3;
        case GL_RGB_INTEGER:     return sizeof(unsigned char) * 3;
        case GL_RGBA8:
        case GL_RGBA8UI:
        case GL_SRGB8_ALPHA8:
        case GL_RGBA:            return sizeof(unsigned char) * 4;
        case GL_RGBA_INTEGER:    return sizeof(unsigned char) * 4;
        case GL_BGRA_EXT:
        case GL_BGRA8_EXT:       return sizeof(unsigned char)* 4;
        }
        break;
    case GL_SHORT:
        switch(format) {
        case GL_R16I:
        case GL_RED_INTEGER:     return sizeof(short);
        case GL_RG16I:
        case GL_RG_INTEGER:      return sizeof(short) * 2;
        case GL_RGB16I:
        case GL_RGB_INTEGER:     return sizeof(short) * 3;
        case GL_RGBA16I:
        case GL_RGBA_INTEGER:    return sizeof(short) * 4;
        }
        break;
    case GL_UNSIGNED_SHORT:
        switch(format) {
        case GL_DEPTH_COMPONENT16:
        case GL_DEPTH_COMPONENT: return sizeof(unsigned short);
        case GL_R16UI:
        case GL_RED_INTEGER:     return sizeof(unsigned short);
        case GL_RG16UI:
        case GL_RG_INTEGER:      return sizeof(unsigned short) * 2;
        case GL_RGB16UI:
        case GL_RGB_INTEGER:     return sizeof(unsigned short) * 3;
        case GL_RGBA16UI:
        case GL_RGBA_INTEGER:    return sizeof(unsigned short) * 4;
        }
        break;
    case GL_INT:
        switch(format) {
        case GL_R32I:
        case GL_RED_INTEGER:     return sizeof(int);
        case GL_RG32I:
        case GL_RG_INTEGER:      return sizeof(int) * 2;
        case GL_RGB32I:
        case GL_RGB_INTEGER:     return sizeof(int) * 3;
        case GL_RGBA32I:
        case GL_RGBA_INTEGER:    return sizeof(int) * 4;
        }
        break;
    case GL_UNSIGNED_INT:
        switch(format) {
        case GL_DEPTH_COMPONENT16:
        case GL_DEPTH_COMPONENT24:
        case GL_DEPTH_COMPONENT32_OES:
        case GL_DEPTH_COMPONENT: return sizeof(unsigned int);
        case GL_R32UI:
        case GL_RED_INTEGER:     return sizeof(unsigned int);
        case GL_RG32UI:
        case GL_RG_INTEGER:      return sizeof(unsigned int) * 2;
        case GL_RGB32UI:
        case GL_RGB_INTEGER:     return sizeof(unsigned int) * 3;
        case GL_RGBA32UI:
        case GL_RGBA_INTEGER:    return sizeof(unsigned int) * 4;
        }
        break;
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_5_5_5_1:
    case GL_UNSIGNED_SHORT_5_6_5:
    case GL_UNSIGNED_SHORT_4_4_4_4_REV_EXT:
    case GL_UNSIGNED_SHORT_1_5_5_5_REV_EXT:
        return sizeof(unsigned short);
    case GL_UNSIGNED_INT_10F_11F_11F_REV:
    case GL_UNSIGNED_INT_5_9_9_9_REV:
    case GL_UNSIGNED_INT_2_10_10_10_REV:
    case GL_UNSIGNED_INT_24_8_OES:
        return sizeof(unsigned int);
    case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
        return sizeof(float) + sizeof(unsigned int);
    case GL_FLOAT:
        switch(format) {
        case GL_DEPTH_COMPONENT32F:
        case GL_DEPTH_COMPONENT: return sizeof(float);
        case GL_ALPHA32F_EXT:
        case GL_ALPHA:           return sizeof(float);
        case GL_LUMINANCE32F_EXT:
        case GL_LUMINANCE:       return sizeof(float);
        case GL_LUMINANCE_ALPHA32F_EXT:
        case GL_LUMINANCE_ALPHA: return sizeof(float) * 2;
        case GL_RED:             return sizeof(float);
        case GL_R32F:            return sizeof(float);
        case GL_RG:              return sizeof(float) * 2;
        case GL_RG32F:           return sizeof(float) * 2;
        case GL_RGB:             return sizeof(float) * 3;
        case GL_RGB32F:          return sizeof(float) * 3;
        case GL_RGBA:            return sizeof(float) * 4;
        case GL_RGBA32F:         return sizeof(float) * 4;
        }
        break;
    case GL_HALF_FLOAT:
    case GL_HALF_FLOAT_OES:
        switch(format) {
        case GL_ALPHA16F_EXT:
        case GL_ALPHA:           return sizeof(unsigned short);
        case GL_LUMINANCE16F_EXT:
        case GL_LUMINANCE:       return sizeof(unsigned short);
        case GL_LUMINANCE_ALPHA16F_EXT:
        case GL_LUMINANCE_ALPHA: return sizeof(unsigned short) * 2;
        case GL_RED:             return sizeof(unsigned short);
        case GL_R16F:            return sizeof(unsigned short);
        case GL_RG:              return sizeof(unsigned short) * 2;
        case GL_RG16F:           return sizeof(unsigned short) * 2;
        case GL_RGB:             return sizeof(unsigned short) * 3;
        case GL_RGB16F:          return sizeof(unsigned short) * 3;
        case GL_RGBA:            return sizeof(unsigned short) * 4;
        case GL_RGBA16F:         return sizeof(unsigned short) * 4;
        }
        break;
    }

    return 0;
}


/**
 * @brief opengl各种类型数据的sizeof函数
 * 
 * @param type 
 * @return size_t 
 */
size_t gl_sizeof(GLenum type)
{
    size_t retval = 0;
    switch(type) {
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
        retval = 1;
        break;
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
    case GL_HALF_FLOAT:
    case GL_HALF_FLOAT_OES:
        retval = 2;
        break;
    case GL_IMAGE_2D:
    case GL_IMAGE_3D:
    case GL_UNSIGNED_INT:
    case GL_INT:
    case GL_FLOAT:
    case GL_FIXED:
    case GL_BOOL:
        retval =  4;
        break;
#ifdef GL_DOUBLE
    case GL_DOUBLE:
        retval = 8;
        break;
    case GL_DOUBLE_VEC2:
        retval = 16;
        break;
    case GL_DOUBLE_VEC3:
        retval = 24;
        break;
    case GL_DOUBLE_VEC4:
        retval = 32;
        break;
    case GL_DOUBLE_MAT2:
        retval = 8 * 4;
        break;
    case GL_DOUBLE_MAT3:
        retval = 8 * 9;
        break;
    case GL_DOUBLE_MAT4:
        retval = 8 * 16;
        break;
    case GL_DOUBLE_MAT2x3:
    case GL_DOUBLE_MAT3x2:
        retval = 8 * 6;
        break;
    case GL_DOUBLE_MAT2x4:
    case GL_DOUBLE_MAT4x2:
        retval = 8 * 8;
        break;
    case GL_DOUBLE_MAT3x4:
    case GL_DOUBLE_MAT4x3:
        retval = 8 * 12;
        break;
#endif
    case GL_FLOAT_VEC2:
    case GL_INT_VEC2:
    case GL_UNSIGNED_INT_VEC2:
    case GL_BOOL_VEC2:
        retval = 8;
        break;
    case GL_INT_VEC3:
    case GL_UNSIGNED_INT_VEC3:
    case GL_BOOL_VEC3:
    case GL_FLOAT_VEC3:
        retval = 12;
        break;
    case GL_FLOAT_VEC4:
    case GL_BOOL_VEC4:
    case GL_INT_VEC4:
    case GL_UNSIGNED_INT_VEC4:
    case GL_FLOAT_MAT2:
        retval = 16;
        break;
    case GL_FLOAT_MAT3:
        retval = 36;
        break;
    case GL_FLOAT_MAT4:
        retval = 64;
        break;
    case GL_FLOAT_MAT2x3:
    case GL_FLOAT_MAT3x2:
        retval = 4 * 6;
        break;
    case GL_FLOAT_MAT2x4:
    case GL_FLOAT_MAT4x2:
        retval = 4 * 8;
        break;
    case GL_FLOAT_MAT3x4:
    case GL_FLOAT_MAT4x3:
        retval = 4 * 12;
        break;
    case GL_SAMPLER_2D:
    case GL_SAMPLER_3D:
    case GL_SAMPLER_CUBE:
    case GL_SAMPLER_2D_SHADOW:
    case GL_SAMPLER_2D_ARRAY:
    case GL_SAMPLER_2D_ARRAY_SHADOW:
    case GL_SAMPLER_2D_MULTISAMPLE:
    case GL_SAMPLER_CUBE_SHADOW:
    case GL_INT_SAMPLER_2D:
    case GL_INT_SAMPLER_3D:
    case GL_INT_SAMPLER_CUBE:
    case GL_INT_SAMPLER_2D_ARRAY:
    case GL_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_UNSIGNED_INT_SAMPLER_2D:
    case GL_UNSIGNED_INT_SAMPLER_3D:
    case GL_UNSIGNED_INT_SAMPLER_CUBE:
    case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_IMAGE_CUBE:
    case GL_IMAGE_2D_ARRAY:
    case GL_INT_IMAGE_2D:
    case GL_INT_IMAGE_3D:
    case GL_INT_IMAGE_CUBE:
    case GL_INT_IMAGE_2D_ARRAY:
    case GL_UNSIGNED_INT_IMAGE_2D:
    case GL_UNSIGNED_INT_IMAGE_3D:
    case GL_UNSIGNED_INT_IMAGE_CUBE:
    case GL_UNSIGNED_INT_IMAGE_2D_ARRAY:
    case GL_UNSIGNED_INT_ATOMIC_COUNTER:
        retval = 4;
        break;
    case GL_UNSIGNED_SHORT_4_4_4_4:
	case GL_UNSIGNED_SHORT_5_5_5_1:
	case GL_UNSIGNED_SHORT_5_6_5:
	case GL_UNSIGNED_SHORT_4_4_4_4_REV_EXT:
	case GL_UNSIGNED_SHORT_1_5_5_5_REV_EXT:
        retval = 2;
        break;
	case GL_INT_2_10_10_10_REV:
	case GL_UNSIGNED_INT_10F_11F_11F_REV:
	case GL_UNSIGNED_INT_5_9_9_9_REV:
	case GL_UNSIGNED_INT_2_10_10_10_REV:
	case GL_UNSIGNED_INT_24_8_OES:;
        retval = 4;
        break;
	case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
		retval = 4 + 4;
        break;
    default:
        printf("**** ERROR unknown type 0x%x (%s,%d)\n", type, __FUNCTION__,__LINE__);
        retval = 4;
    } 
    return retval;

}


size_t gl_pname_size(GLenum pname){
    size_t s = 0;

    switch(pname)
    {
    case GL_DEPTH_TEST:
    case GL_DEPTH_FUNC:
    case GL_DEPTH_BITS:
    case GL_MAX_CLIP_PLANES:
    case GL_GREEN_BITS:
    case GL_MAX_MODELVIEW_STACK_DEPTH:
    case GL_MAX_PROJECTION_STACK_DEPTH:
    case GL_MAX_TEXTURE_STACK_DEPTH:
    case GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES:
    case GL_IMPLEMENTATION_COLOR_READ_TYPE_OES:
    case GL_NUM_COMPRESSED_TEXTURE_FORMATS:
    case GL_MAX_TEXTURE_SIZE:
    case GL_TEXTURE_GEN_MODE_OES:
    case GL_TEXTURE_ENV_MODE:
    case GL_FOG_MODE:
    case GL_FOG_DENSITY:
    case GL_FOG_START:
    case GL_FOG_END:
    case GL_SPOT_EXPONENT:
    case GL_CONSTANT_ATTENUATION:
    case GL_LINEAR_ATTENUATION:
    case GL_QUADRATIC_ATTENUATION:
    case GL_SHININESS:
    case GL_LIGHT_MODEL_TWO_SIDE:
    case GL_POINT_SIZE:
    case GL_POINT_SIZE_MIN:
    case GL_POINT_SIZE_MAX:
    case GL_POINT_FADE_THRESHOLD_SIZE:
    case GL_CULL_FACE_MODE:
    case GL_FRONT_FACE:
    case GL_SHADE_MODEL:
    case GL_DEPTH_WRITEMASK:
    case GL_DEPTH_CLEAR_VALUE:
    case GL_STENCIL_FAIL:
    case GL_STENCIL_PASS_DEPTH_FAIL:
    case GL_STENCIL_PASS_DEPTH_PASS:
    case GL_STENCIL_REF:
    case GL_STENCIL_WRITEMASK:
    case GL_MATRIX_MODE:
    case GL_MODELVIEW_STACK_DEPTH:
    case GL_PROJECTION_STACK_DEPTH:
    case GL_TEXTURE_STACK_DEPTH:
    case GL_ALPHA_TEST_FUNC:
    case GL_ALPHA_TEST_REF:
    case GL_ALPHA_TEST:
    case GL_BLEND_DST:
    case GL_BLEND_SRC:
    case GL_BLEND:
    case GL_LOGIC_OP_MODE:
    case GL_SCISSOR_TEST:
    case GL_MAX_TEXTURE_UNITS:
    case GL_ACTIVE_TEXTURE:
    case GL_ALPHA_BITS:
    case GL_ARRAY_BUFFER_BINDING:
    case GL_BLUE_BITS:
    case GL_CLIENT_ACTIVE_TEXTURE:
    case GL_CLIP_PLANE0:
    case GL_CLIP_PLANE1:
    case GL_CLIP_PLANE2:
    case GL_CLIP_PLANE3:
    case GL_CLIP_PLANE4:
    case GL_CLIP_PLANE5:
    case GL_COLOR_ARRAY:
    case GL_COLOR_ARRAY_BUFFER_BINDING:
    case GL_COLOR_ARRAY_SIZE:
    case GL_COLOR_ARRAY_STRIDE:
    case GL_COLOR_ARRAY_TYPE:
    case GL_COLOR_LOGIC_OP:
    case GL_COLOR_MATERIAL:
    case GL_PACK_ALIGNMENT:
    case GL_PERSPECTIVE_CORRECTION_HINT:
    case GL_POINT_SIZE_ARRAY_BUFFER_BINDING_OES:
    case GL_POINT_SIZE_ARRAY_STRIDE_OES:
    case GL_POINT_SIZE_ARRAY_TYPE_OES:
    case GL_POINT_SMOOTH:
    case GL_POINT_SMOOTH_HINT:
    case GL_POINT_SPRITE_OES:
    case GL_COORD_REPLACE_OES:
    case GL_COMBINE_ALPHA:
    case GL_SRC0_RGB:
    case GL_SRC1_RGB:
    case GL_SRC2_RGB:
    case GL_OPERAND0_RGB:
    case GL_OPERAND1_RGB:
    case GL_OPERAND2_RGB:
    case GL_SRC0_ALPHA:
    case GL_SRC1_ALPHA:
    case GL_SRC2_ALPHA:
    case GL_OPERAND0_ALPHA:
    case GL_OPERAND1_ALPHA:
    case GL_OPERAND2_ALPHA:
    case GL_RGB_SCALE:
    case GL_ALPHA_SCALE:
    case GL_COMBINE_RGB:
    case GL_POLYGON_OFFSET_FACTOR:
    case GL_POLYGON_OFFSET_FILL:
    case GL_POLYGON_OFFSET_UNITS:
    case GL_RED_BITS:
    case GL_RESCALE_NORMAL:
    case GL_SAMPLE_ALPHA_TO_COVERAGE:
    case GL_SAMPLE_ALPHA_TO_ONE:
    case GL_SAMPLE_BUFFERS:
    case GL_SAMPLE_COVERAGE:
    case GL_SAMPLE_COVERAGE_INVERT:
    case GL_SAMPLE_COVERAGE_VALUE:
    case GL_SAMPLES:
    case GL_STENCIL_BITS:
    case GL_STENCIL_CLEAR_VALUE:
    case GL_STENCIL_FUNC:
    case GL_STENCIL_TEST:
    case GL_STENCIL_VALUE_MASK:
    case GL_STENCIL_BACK_FUNC:
    case GL_STENCIL_BACK_VALUE_MASK:
    case GL_STENCIL_BACK_REF:
    case GL_STENCIL_BACK_FAIL:
    case GL_STENCIL_BACK_PASS_DEPTH_FAIL:
    case GL_STENCIL_BACK_PASS_DEPTH_PASS:
    case GL_STENCIL_BACK_WRITEMASK:
    case GL_TEXTURE_2D:
    case GL_TEXTURE_BINDING_2D:
    case GL_TEXTURE_BINDING_CUBE_MAP:
    case GL_TEXTURE_BINDING_EXTERNAL_OES:
    case GL_TEXTURE_COORD_ARRAY:
    case GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING:
    case GL_TEXTURE_COORD_ARRAY_SIZE:
    case GL_TEXTURE_COORD_ARRAY_STRIDE:
    case GL_TEXTURE_COORD_ARRAY_TYPE:
    case GL_UNPACK_ALIGNMENT:
    case GL_VERTEX_ARRAY:
    case GL_VERTEX_ARRAY_BUFFER_BINDING:
    case GL_VERTEX_ARRAY_SIZE:
    case GL_VERTEX_ARRAY_STRIDE:
    case GL_VERTEX_ARRAY_TYPE:
    case GL_SPOT_CUTOFF:
    case GL_TEXTURE_MIN_FILTER:
    case GL_TEXTURE_MAG_FILTER:
    case GL_TEXTURE_WRAP_S:
    case GL_TEXTURE_WRAP_T:
    case GL_GENERATE_MIPMAP:
    case GL_GENERATE_MIPMAP_HINT:
    case GL_RENDERBUFFER_WIDTH_OES:
    case GL_RENDERBUFFER_HEIGHT_OES:
    case GL_RENDERBUFFER_INTERNAL_FORMAT_OES:
    case GL_RENDERBUFFER_RED_SIZE_OES:
    case GL_RENDERBUFFER_GREEN_SIZE_OES:
    case GL_RENDERBUFFER_BLUE_SIZE_OES:
    case GL_RENDERBUFFER_ALPHA_SIZE_OES:
    case GL_RENDERBUFFER_DEPTH_SIZE_OES:
    case GL_RENDERBUFFER_STENCIL_SIZE_OES:
    case GL_RENDERBUFFER_BINDING:
    case GL_FRAMEBUFFER_BINDING:
    case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_OES:
    case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_OES:
    case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_OES:
    case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_OES:
    case GL_FENCE_STATUS_NV:
    case GL_FENCE_CONDITION_NV:
    case GL_TEXTURE_WIDTH_QCOM:
    case GL_TEXTURE_HEIGHT_QCOM:
    case GL_TEXTURE_DEPTH_QCOM:
    case GL_TEXTURE_INTERNAL_FORMAT_QCOM:
    case GL_TEXTURE_FORMAT_QCOM:
    case GL_TEXTURE_TYPE_QCOM:
    case GL_TEXTURE_IMAGE_VALID_QCOM:
    case GL_TEXTURE_NUM_LEVELS_QCOM:
    case GL_TEXTURE_TARGET_QCOM:
    case GL_TEXTURE_OBJECT_VALID_QCOM:
    case GL_BLEND_EQUATION_RGB_OES:
    case GL_BLEND_EQUATION_ALPHA_OES:
    case GL_BLEND_DST_RGB_OES:
    case GL_BLEND_SRC_RGB_OES:
    case GL_BLEND_DST_ALPHA_OES:
    case GL_BLEND_SRC_ALPHA_OES:
    case GL_MAX_LIGHTS:
    case GL_SHADER_TYPE:
    case GL_DELETE_STATUS:
    case GL_COMPILE_STATUS:
    case GL_INFO_LOG_LENGTH:
    case GL_SHADER_SOURCE_LENGTH:
    case GL_CURRENT_PROGRAM:
    case GL_LINK_STATUS:
    case GL_VALIDATE_STATUS:
    case GL_ATTACHED_SHADERS:
    case GL_ACTIVE_UNIFORMS:
    case GL_ACTIVE_ATTRIBUTES:
    case GL_SUBPIXEL_BITS:
    case GL_MAX_CUBE_MAP_TEXTURE_SIZE:
    case GL_NUM_SHADER_BINARY_FORMATS:
    case GL_SHADER_COMPILER:
    case GL_MAX_VERTEX_ATTRIBS:
    case GL_MAX_VERTEX_UNIFORM_VECTORS:
    case GL_MAX_VARYING_VECTORS:
    case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS:
    case GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS:
    case GL_MAX_FRAGMENT_UNIFORM_VECTORS:
    case GL_MAX_RENDERBUFFER_SIZE:
    case GL_MAX_TEXTURE_IMAGE_UNITS:
    case GL_REQUIRED_TEXTURE_IMAGE_UNITS_OES:
    case GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES:
    case GL_LINE_WIDTH:
        s = 1;
        break;
    case GL_ALIASED_LINE_WIDTH_RANGE:
    case GL_ALIASED_POINT_SIZE_RANGE:
    case GL_DEPTH_RANGE:
    case GL_MAX_VIEWPORT_DIMS:
    case GL_SMOOTH_POINT_SIZE_RANGE:
    case GL_SMOOTH_LINE_WIDTH_RANGE:
        s= 2;
        break;
    case GL_SPOT_DIRECTION:
    case GL_POINT_DISTANCE_ATTENUATION:
    case GL_CURRENT_NORMAL:
        s =  3;
        break;
    case GL_CURRENT_VERTEX_ATTRIB:
    case GL_CURRENT_TEXTURE_COORDS:
    case GL_CURRENT_COLOR:
    case GL_FOG_COLOR:
    case GL_AMBIENT:
    case GL_DIFFUSE:
    case GL_SPECULAR:
    case GL_EMISSION:
    case GL_POSITION:
    case GL_LIGHT_MODEL_AMBIENT:
    case GL_TEXTURE_ENV_COLOR:
    case GL_SCISSOR_BOX:
    case GL_VIEWPORT:
    case GL_TEXTURE_CROP_RECT_OES:
    case GL_COLOR_CLEAR_VALUE:
    case GL_COLOR_WRITEMASK:
    case GL_AMBIENT_AND_DIFFUSE:
    case GL_BLEND_COLOR:
        s =  4;
        break;
    case GL_MODELVIEW_MATRIX:
    case GL_PROJECTION_MATRIX:
    case GL_TEXTURE_MATRIX:
        s = 16;
    break;
    default:
        printf("gl_pname_size: unknow pname 0x%08x\n", pname);
        s = 1; // assume 1
    }
    return s;
}