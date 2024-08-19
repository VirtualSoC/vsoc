#include "hw/express-gpu/gl_helper.h"

#include "hw/teleport-express/teleport_express_call.h"

#ifndef _WIN32
#define max(a, b)                       \
  (((a) > (b)) ? (a) : (b))
#define min(a, b)                       \
  (((a) < (b)) ? (a) : (b))
#endif

/**
 * @brief 根据像素格式和类型计算一个像素所占的空间的字节大小
 *
 * @param format 像素格式
 * @param type 像素类型
 * @return int
 */
int pixel_size_calc(GLenum format, GLenum type)
{
    switch (type)
    {
    case GL_BYTE:
        switch (format)
        {
        case GL_R8:
        case GL_R8I:
        case GL_R8_SNORM:
        case GL_RED:
            return sizeof(char);
        case GL_RED_INTEGER:
            return sizeof(char);
        case GL_RG8:
        case GL_RG8I:
        case GL_RG8_SNORM:
        case GL_RG:
            return sizeof(char) * 2;
        case GL_RG_INTEGER:
            return sizeof(char) * 2;
        case GL_RGB8:
        case GL_RGB8I:
        case GL_RGB8_SNORM:
        case GL_RGB:
            return sizeof(char) * 3;
        case GL_RGB_INTEGER:
            return sizeof(char) * 3;
        case GL_RGBA8:
        case GL_RGBA8I:
        case GL_RGBA8_SNORM:
        case GL_RGBA:
            return sizeof(char) * 4;
        case GL_RGBA_INTEGER:
            return sizeof(char) * 4;
        default:
            LOGE("error! pixel_size_calc type %x format %x", type, format);
        }

        break;
    case GL_UNSIGNED_BYTE:
        switch (format)
        {
        case GL_R8:
        case GL_R8UI:
        case GL_RED:
            return sizeof(unsigned char);
        case GL_RED_INTEGER:
            return sizeof(unsigned char);
        case GL_ALPHA8_EXT:
        case GL_ALPHA:
            return sizeof(unsigned char);
        case GL_LUMINANCE8_EXT:
        case GL_LUMINANCE:
            return sizeof(unsigned char);
        case GL_LUMINANCE8_ALPHA8_EXT:
        case GL_LUMINANCE_ALPHA:
            return sizeof(unsigned char) * 2;
        case GL_RG8:
        case GL_RG8UI:
        case GL_RG:
            return sizeof(unsigned char) * 2;
        case GL_RG_INTEGER:
            return sizeof(unsigned char) * 2;
        case GL_RGB8:
        case GL_RGB8UI:
        case GL_SRGB8:
        case GL_RGB:
            return sizeof(unsigned char) * 3;
        case GL_RGB_INTEGER:
            return sizeof(unsigned char) * 3;
        case GL_RGBA8:
        case GL_RGBA8UI:
        case GL_SRGB8_ALPHA8:
        case GL_RGBA:
            return sizeof(unsigned char) * 4;
        case GL_RGBA_INTEGER:
            return sizeof(unsigned char) * 4;
        case GL_BGRA_EXT:
        case GL_BGRA8_EXT:
            return sizeof(unsigned char) * 4;
        default:
            LOGE("error! pixel_size_calc type %x format %x", type, format);
        }
        break;
    case GL_SHORT:
        switch (format)
        {
        case GL_R16I:
        case GL_RED_INTEGER:
            return sizeof(short);
        case GL_RG16I:
        case GL_RG_INTEGER:
            return sizeof(short) * 2;
        case GL_RGB16I:
        case GL_RGB_INTEGER:
            return sizeof(short) * 3;
        case GL_RGBA16I:
        case GL_RGBA_INTEGER:
            return sizeof(short) * 4;
        default:
            LOGE("error! pixel_size_calc type %x format %x", type, format);
        }
        break;
    case GL_UNSIGNED_SHORT:
        switch (format)
        {
        case GL_DEPTH_COMPONENT16:
        case GL_DEPTH_COMPONENT:
            return sizeof(unsigned short);
        case GL_R16UI:
        case GL_RED_INTEGER:
            return sizeof(unsigned short);
        case GL_RG:
        case GL_RG16UI:
        case GL_RG_INTEGER:
            return sizeof(unsigned short) * 2;
        case GL_RGB16UI:
        case GL_RGB_INTEGER:
            return sizeof(unsigned short) * 3;
        case GL_RGBA16UI:
        case GL_RGBA_INTEGER:
            return sizeof(unsigned short) * 4;
        default:
            LOGE("error! pixel_size_calc type %x format %x", type, format);
        }
        break;
    case GL_INT:
        switch (format)
        {
        case GL_R32I:
        case GL_RED_INTEGER:
            return sizeof(int);
        case GL_RG32I:
        case GL_RG_INTEGER:
            return sizeof(int) * 2;
        case GL_RGB32I:
        case GL_RGB_INTEGER:
            return sizeof(int) * 3;
        case GL_RGBA32I:
        case GL_RGBA_INTEGER:
            return sizeof(int) * 4;
        default:
            LOGE("error! pixel_size_calc type %x format %x", type, format);
        }
        break;
    case GL_UNSIGNED_INT:
        switch (format)
        {
        case GL_DEPTH_COMPONENT16:
        case GL_DEPTH_COMPONENT24:
        case GL_DEPTH_COMPONENT32_OES:
        case GL_DEPTH_COMPONENT:
            return sizeof(unsigned int);
        case GL_R32UI:
        case GL_RED_INTEGER:
            return sizeof(unsigned int);
        case GL_RG32UI:
        case GL_RG_INTEGER:
            return sizeof(unsigned int) * 2;
        case GL_RGB32UI:
        case GL_RGB_INTEGER:
            return sizeof(unsigned int) * 3;
        case GL_RGBA32UI:
        case GL_RGBA_INTEGER:
            return sizeof(unsigned int) * 4;
        default:
            LOGE("error! pixel_size_calc type %x format %x", type, format);
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
        switch (format)
        {
        case GL_DEPTH_COMPONENT32F:
        case GL_DEPTH_COMPONENT:
            return sizeof(float);
        case GL_ALPHA32F_EXT:
        case GL_ALPHA:
            return sizeof(float);
        case GL_LUMINANCE32F_EXT:
        case GL_LUMINANCE:
            return sizeof(float);
        case GL_LUMINANCE_ALPHA32F_EXT:
        case GL_LUMINANCE_ALPHA:
            return sizeof(float) * 2;
        case GL_RED:
            return sizeof(float);
        case GL_R32F:
            return sizeof(float);
        case GL_RG:
            return sizeof(float) * 2;
        case GL_RG32F:
            return sizeof(float) * 2;
        case GL_RGB:
            return sizeof(float) * 3;
        case GL_RGB32F:
            return sizeof(float) * 3;
        case GL_RGBA:
            return sizeof(float) * 4;
        case GL_RGBA32F:
            return sizeof(float) * 4;
        default:
            LOGE("error! pixel_size_calc type %x format %x", type, format);
        }
        break;
    case GL_HALF_FLOAT:
    case GL_HALF_FLOAT_OES:
        switch (format)
        {
        case GL_ALPHA16F_EXT:
        case GL_ALPHA:
            return sizeof(unsigned short);
        case GL_LUMINANCE16F_EXT:
        case GL_LUMINANCE:
            return sizeof(unsigned short);
        case GL_LUMINANCE_ALPHA16F_EXT:
        case GL_LUMINANCE_ALPHA:
            return sizeof(unsigned short) * 2;
        case GL_RED:
            return sizeof(unsigned short);
        case GL_R16F:
            return sizeof(unsigned short);
        case GL_RG:
            return sizeof(unsigned short) * 2;
        case GL_RG16F:
            return sizeof(unsigned short) * 2;
        case GL_RGB:
            return sizeof(unsigned short) * 3;
        case GL_RGB16F:
            return sizeof(unsigned short) * 3;
        case GL_RGBA:
            return sizeof(unsigned short) * 4;
        case GL_RGBA16F:
            return sizeof(unsigned short) * 4;
        default:
            LOGE("error! pixel_size_calc type %x format %x", type, format);
        }
        break;
    default:
        LOGE("error! pixel_size_calc type %x format %x", type, format);
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
    switch (type)
    {
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
        retval = 1;
        break;
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
    case GL_HALF_FLOAT:
        // case GL_HALF_FLOAT_OES:
        retval = 2;
        break;
    case GL_IMAGE_2D:
    case GL_IMAGE_3D:
    case GL_UNSIGNED_INT:
    case GL_INT:
    case GL_FLOAT:
    case GL_FIXED:
    case GL_BOOL:
        retval = 4;
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
    case GL_UNSIGNED_INT_IMAGE_BUFFER:
    case GL_IMAGE_BUFFER:
    case GL_INT_IMAGE_BUFFER:
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
        // case GL_UNSIGNED_INT_24_8_OES:
        retval = 4;
        break;
    case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
        retval = 4 + 4;
        break;
    default:
        LOGI("**** ERROR unknown type 0x%x (%s,%d)", type, __FUNCTION__, __LINE__);
        retval = 4;
    }
    return retval;
}

/**
 * 查询pname对应的元素个数。
 * 可以是估计值，保证估计值比实际值大（避免写越界）
 * 和host端必须保持一致
*/
size_t gl_pname_size(GLenum pname)
{
    size_t s = 0;

    switch (pname)
    {
    case GL_DEPTH_TEST:
    case GL_DEPTH_FUNC:
    case GL_DEPTH_BITS:
    case GL_MAX_CLIP_PLANES:
    case GL_MAX_COLOR_ATTACHMENTS:
    case GL_MAX_DRAW_BUFFERS:
    case GL_GREEN_BITS:
    case GL_MAX_MODELVIEW_STACK_DEPTH:
    case GL_MAX_PROJECTION_STACK_DEPTH:
    case GL_MAX_TEXTURE_STACK_DEPTH:
    case GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES:
    case GL_IMPLEMENTATION_COLOR_READ_TYPE_OES:
    case GL_NUM_COMPRESSED_TEXTURE_FORMATS:
    case GL_MAX_TEXTURE_SIZE:
    // case GL_TEXTURE_GEN_MODE_OES:
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
    case GL_CULL_FACE:
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
    case GL_DITHER:
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
    // case GL_POINT_SIZE_ARRAY_BUFFER_BINDING_OES:
    // case GL_POINT_SIZE_ARRAY_STRIDE_OES:
    // case GL_POINT_SIZE_ARRAY_TYPE_OES:
    case GL_POINT_SMOOTH:
    case GL_POINT_SMOOTH_HINT:
    // case GL_POINT_SPRITE_OES:
    // case GL_COORD_REPLACE_OES:
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
    case GL_MAX_SAMPLES_EXT:
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
    case GL_TEXTURE_BASE_LEVEL:
    case GL_TEXTURE_BINDING_2D:
    case GL_TEXTURE_BINDING_EXTERNAL_OES:
    case GL_TEXTURE_BINDING_CUBE_MAP:
    case GL_TEXTURE_COMPARE_FUNC:
    case GL_TEXTURE_COMPARE_MODE:
    case GL_TEXTURE_COORD_ARRAY:
    case GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING:
    case GL_TEXTURE_COORD_ARRAY_SIZE:
    case GL_TEXTURE_COORD_ARRAY_STRIDE:
    case GL_TEXTURE_COORD_ARRAY_TYPE:
    case GL_TEXTURE_IMMUTABLE_FORMAT:
    case GL_UNPACK_ALIGNMENT:
    case GL_VERTEX_ARRAY:
    case GL_VERTEX_ARRAY_BUFFER_BINDING:
    case GL_VERTEX_ARRAY_SIZE:
    case GL_VERTEX_ARRAY_STRIDE:
    case GL_VERTEX_ARRAY_TYPE:
    case GL_SPOT_CUTOFF:
    case GL_TEXTURE_MIN_FILTER:
    case GL_TEXTURE_MAG_FILTER:
    case GL_TEXTURE_MAX_LOD:
    case GL_TEXTURE_MIN_LOD:
    case GL_TEXTURE_WRAP_S:
    case GL_TEXTURE_WRAP_T:
    case GL_TEXTURE_WRAP_R:
    case GL_TEXTURE_SWIZZLE_R:
    case GL_TEXTURE_SWIZZLE_G:
    case GL_TEXTURE_SWIZZLE_B:
    case GL_TEXTURE_SWIZZLE_A:
    case GL_GENERATE_MIPMAP:
    case GL_GENERATE_MIPMAP_HINT:
    // case GL_RENDERBUFFER_WIDTH_OES:
    // case GL_RENDERBUFFER_HEIGHT_OES:
    // case GL_RENDERBUFFER_INTERNAL_FORMAT_OES:
    // case GL_RENDERBUFFER_RED_SIZE_OES:
    // case GL_RENDERBUFFER_GREEN_SIZE_OES:
    // case GL_RENDERBUFFER_BLUE_SIZE_OES:
    // case GL_RENDERBUFFER_ALPHA_SIZE_OES:
    // case GL_RENDERBUFFER_DEPTH_SIZE_OES:
    // case GL_RENDERBUFFER_STENCIL_SIZE_OES:
    case GL_RENDERBUFFER_BINDING:
    case GL_FRAMEBUFFER_BINDING:
    // case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_OES:
    // case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_OES:
    // case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_OES:
    // case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_OES:
    case GL_FENCE_STATUS_NV:
    case GL_FENCE_CONDITION_NV:
    // case GL_TEXTURE_WIDTH_QCOM:
    // case GL_TEXTURE_HEIGHT_QCOM:
    // case GL_TEXTURE_DEPTH_QCOM:
    // case GL_TEXTURE_INTERNAL_FORMAT_QCOM:
    // case GL_TEXTURE_FORMAT_QCOM:
    // case GL_TEXTURE_TYPE_QCOM:
    // case GL_TEXTURE_IMAGE_VALID_QCOM:
    // case GL_TEXTURE_NUM_LEVELS_QCOM:
    // case GL_TEXTURE_TARGET_QCOM:
    // case GL_TEXTURE_OBJECT_VALID_QCOM:
    // case GL_BLEND_EQUATION_RGB_OES:
    // case GL_BLEND_EQUATION_ALPHA_OES:
    // case GL_BLEND_DST_RGB_OES:
    // case GL_BLEND_SRC_RGB_OES:
    // case GL_BLEND_DST_ALPHA_OES:
    // case GL_BLEND_SRC_ALPHA_OES:
    case GL_MAX_LIGHTS:
    case GL_SHADER_TYPE:
    case GL_COMPILE_STATUS:
    case GL_SHADER_SOURCE_LENGTH:
    case GL_CURRENT_PROGRAM:
    case GL_SUBPIXEL_BITS:
    case GL_MAX_3D_TEXTURE_SIZE:
    case GL_MAX_ARRAY_TEXTURE_LAYERS:
    case GL_MAX_CUBE_MAP_TEXTURE_SIZE:
    case GL_SHADER_COMPILER:
    case GL_MAX_VERTEX_ATTRIBS:
    case GL_MAX_VERTEX_UNIFORM_VECTORS:
    case GL_MAX_VARYING_VECTORS:
    case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS:
    case GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS:
    case GL_MAX_FRAGMENT_UNIFORM_VECTORS:
    case GL_MAX_RENDERBUFFER_SIZE:
    case GL_MAX_TEXTURE_IMAGE_UNITS:
    // case GL_REQUIRED_TEXTURE_IMAGE_UNITS_OES:
    // case GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES:
    case GL_LINE_WIDTH:
    case GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS:
    case GL_MAX_UNIFORM_BUFFER_BINDINGS:
    case GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS:
    case GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS:
    case GL_UNIFORM_BLOCK_BINDING:
    case GL_UNIFORM_BLOCK_DATA_SIZE:
    case GL_UNIFORM_BLOCK_NAME_LENGTH:
    case GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS:
    case GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER:
    case GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER:
    case GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT:
    case GL_CURRENT_QUERY:
    case GL_QUERY_RESULT:
    case GL_QUERY_RESULT_AVAILABLE:
    case GL_READ_BUFFER:
    case GL_ACTIVE_ATOMIC_COUNTER_BUFFERS:
    case GL_ACTIVE_ATTRIBUTES:
    case GL_ACTIVE_ATTRIBUTE_MAX_LENGTH:
    case GL_ACTIVE_UNIFORM_BLOCKS:
    case GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH:
    case GL_ACTIVE_UNIFORMS:
    case GL_ACTIVE_UNIFORM_MAX_LENGTH:
    case GL_ATTACHED_SHADERS:
    case GL_DELETE_STATUS:
    case GL_INFO_LOG_LENGTH:
    case GL_LINK_STATUS:
    case GL_PROGRAM_BINARY_LENGTH:
    case GL_PROGRAM_BINARY_RETRIEVABLE_HINT:
    case GL_PROGRAM_SEPARABLE:
    case GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT:
    case GL_TRANSFORM_FEEDBACK_BUFFER_MODE:
    case GL_TRANSFORM_FEEDBACK_VARYINGS:
    case GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH:
    case GL_TRANSFORM_FEEDBACK_ACTIVE:
#define GL_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS 0x8C8A
#define GL_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS 0x8C80
    case GL_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS:
    case GL_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS:
    case GL_VALIDATE_STATUS:
    case GL_ACTIVE_RESOURCES:
    case GL_MAX_IMAGE_UNITS:
    case GL_FRAMEBUFFER_DEFAULT_WIDTH:
    case GL_FRAMEBUFFER_DEFAULT_HEIGHT:
    case GL_FRAMEBUFFER_DEFAULT_SAMPLES:
    case GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS:
    case GL_FRAMEBUFFER_DEFAULT_LAYERS:
    case GL_MAX_NAME_LENGTH:
    case GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT:
        s = 1;
        break;
    case GL_ALIASED_LINE_WIDTH_RANGE:
    case GL_ALIASED_POINT_SIZE_RANGE:
    case GL_DEPTH_RANGE:
    case GL_MAX_VIEWPORT_DIMS:
    case GL_SMOOTH_POINT_SIZE_RANGE:
    case GL_SMOOTH_LINE_WIDTH_RANGE:
    case GL_SAMPLE_POSITION:
        s = 2;
        break;
    case GL_SPOT_DIRECTION:
    case GL_POINT_DISTANCE_ATTENUATION:
    case GL_CURRENT_NORMAL:
    case GL_COMPUTE_WORK_GROUP_SIZE:
        s = 3;
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
    // case GL_TEXTURE_CROP_RECT_OES:
    case GL_COLOR_CLEAR_VALUE:
    case GL_COLOR_WRITEMASK:
    case GL_AMBIENT_AND_DIFFUSE:
    case GL_BLEND_COLOR:
    case GL_TEXTURE_BORDER_COLOR:
    case GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT:
        s = 4;
        break;
    case GL_MODELVIEW_MATRIX:
    case GL_PROJECTION_MATRIX:
    case GL_TEXTURE_MATRIX:
        s = 16;
        break;
    case GL_MAX_ELEMENTS_VERTICES:
    case GL_MAX_VERTEX_UNIFORM_COMPONENTS:
    case GL_MAX_VERTEX_UNIFORM_BLOCKS:
    case GL_MAX_VERTEX_OUTPUT_COMPONENTS:
    case GL_MAX_FRAGMENT_UNIFORM_COMPONENTS:
    case GL_MAX_FRAGMENT_UNIFORM_BLOCKS:
    case GL_MAX_FRAGMENT_INPUT_COMPONENTS:
    case GL_MIN_PROGRAM_TEXEL_OFFSET:
    case GL_MAX_PROGRAM_TEXEL_OFFSET:
    case GL_MAX_COMBINED_UNIFORM_BLOCKS:
    case GL_MAX_VARYING_COMPONENTS:
    case GL_NUM_EXTENSIONS:
    case GL_VERTEX_ARRAY_BINDING:
    case GL_BLEND_SRC_RGB:
    case GL_BLEND_SRC_ALPHA:
    case GL_BLEND_DST_RGB:
    case GL_BLEND_DST_ALPHA:
    case GL_BLEND_EQUATION:
    case GL_BLEND_EQUATION_ALPHA:
    case GL_DRAW_BUFFER0:
    case GL_DRAW_BUFFER1:
    case GL_DRAW_BUFFER2:
    case GL_DRAW_BUFFER3:
    case GL_DRAW_BUFFER4:
    case GL_DRAW_BUFFER5:
    case GL_DRAW_BUFFER6:
    case GL_DRAW_BUFFER7:
    case GL_DRAW_BUFFER8:
    case GL_DRAW_BUFFER9:
    case GL_DRAW_BUFFER10:
    case GL_DRAW_BUFFER11:
    case GL_DRAW_BUFFER12:
    case GL_DRAW_BUFFER13:
    case GL_DRAW_BUFFER14:
    case GL_DRAW_BUFFER15:
    case GL_FRAGMENT_SHADER_DERIVATIVE_HINT:
    case GL_TRANSFORM_FEEDBACK_BUFFER_START:
    case GL_TRANSFORM_FEEDBACK_BUFFER_SIZE:
    case GL_TRANSFORM_FEEDBACK_PAUSED:
    case GL_MAX_ELEMENT_INDEX:
        s = 2;
        break;
    case GL_MAX_TEXTURE_LOD_BIAS:
        s = 1;
        break;
    case GL_MAX_ELEMENTS_INDICES:
        s = 2;
        break;
    case GL_MAX_SERVER_WAIT_TIMEOUT:
        s = 2;
        break;
    case GL_MAX_UNIFORM_BLOCK_SIZE:
        s = 2;
        break;
    case GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS:
        s = 2;
        break;
    case GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS:
        s = 2;
        break;
    case GL_PROGRAM_BINARY_FORMATS:
        s = 8;
        break;
    case GL_NUM_PROGRAM_BINARY_FORMATS:
        s = 1;
        break;
    case GL_SHADER_BINARY_FORMATS:
        s = 8;
        break;
    case GL_NUM_SHADER_BINARY_FORMATS:
        s = 1;
        break;
    case GL_COMPRESSED_TEXTURE_FORMATS:
        s = 128;
        break;
    case GL_MAX_COMPUTE_UNIFORM_BLOCKS:
    case GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS:
    case GL_MAX_COMPUTE_IMAGE_UNIFORMS:
    case GL_MAX_COMPUTE_SHARED_MEMORY_SIZE:
    case GL_MAX_COMPUTE_UNIFORM_COMPONENTS:
    case GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS:
    case GL_MAX_COMPUTE_ATOMIC_COUNTERS:
    case GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS:
    case GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS:
    case GL_MAX_UNIFORM_LOCATIONS:
    case GL_MAX_FRAMEBUFFER_WIDTH:
    case GL_MAX_FRAMEBUFFER_HEIGHT:
    case GL_MAX_FRAMEBUFFER_SAMPLES:
    case GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS:
    case GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS:
    case GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS:
    case GL_MAX_VERTEX_ATOMIC_COUNTERS:
    case GL_MAX_FRAGMENT_ATOMIC_COUNTERS:
    case GL_MAX_COMBINED_ATOMIC_COUNTERS:
    case GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE:
    case GL_MAX_VERTEX_IMAGE_UNIFORMS:
    case GL_MAX_FRAGMENT_IMAGE_UNIFORMS:
    case GL_MAX_COMBINED_IMAGE_UNIFORMS:
    case GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS:
    case GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS:
    case GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS:
    case GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS:
    case GL_MAX_SHADER_STORAGE_BLOCK_SIZE:
    case GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES:
    case GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET:
    case GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET:
    case GL_MAX_SAMPLE_MASK_WORDS:
    case GL_MAX_COLOR_TEXTURE_SAMPLES:
    case GL_MAX_DEPTH_TEXTURE_SAMPLES:
    case GL_MAX_INTEGER_SAMPLES:
    case GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET:
    case GL_MAX_VERTEX_ATTRIB_BINDINGS:
    case GL_MAX_VERTEX_ATTRIB_STRIDE:
    case GL_MAJOR_VERSION:
    case GL_MINOR_VERSION:
        s = 1;
        break;
    case GL_MAX_COMPUTE_WORK_GROUP_COUNT:
    case GL_MAX_COMPUTE_WORK_GROUP_SIZE:
        s = 3;
        break;
    default:
        LOGW("warning! gl_pname_size unknown pname 0x%08x", pname);
        s = 4; // 估计一个比较大的值
    }
    return s;
}

void prepare_integer_value(Static_Context_Values *s_values)
{

    s_values->implementation_color_read_type = 5121;
    s_values->implementation_color_read_format = 6408;
    s_values->max_array_texture_layers = 2048;
    s_values->max_color_attachments = 8;
    s_values->max_combined_uniform_blocks = 84;
    s_values->max_draw_buffers = 16;
    s_values->max_fragment_input_components = 128;
    s_values->max_fragment_uniform_blocks = 14;
    s_values->max_program_texel_offset = 7;
    s_values->max_transform_feedback_interleaved_components = 128;
    s_values->max_transform_feedback_separate_attribs = 4;
    s_values->max_transform_feedback_separate_components = 4;
    s_values->max_uniform_buffer_bindings = 84;
    s_values->max_varying_components = 124;
    s_values->max_varying_vectors = 31;
    s_values->max_vertex_output_components = 128;
    s_values->max_vertex_uniform_blocks = 14;
    s_values->min_program_texel_offset = -8;
    s_values->max_uniform_block_size = 65536;
    s_values->aliased_point_size_range[0] = 1.0f;
    s_values->aliased_point_size_range[1] = 2047.0f;

    int temp_int_value[128];

    glGetIntegerv(GL_NUM_EXTENSIONS, (GLint *)temp_int_value);
    s_values->num_extensions = temp_int_value[0];

    glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, (GLint *)temp_int_value);
    s_values->num_shader_binary_formats = temp_int_value[0];
    glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, (GLint *)temp_int_value);
    s_values->num_compressed_texture_formats = temp_int_value[0];
    glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, (GLint *)temp_int_value);
    s_values->num_program_binary_formats = temp_int_value[0];

#ifdef _WIN32
    int *temp_int_array = g_alloca(max(max(s_values->num_shader_binary_formats, s_values->num_compressed_texture_formats), s_values->num_program_binary_formats) * sizeof(int));

    glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, (GLint *)temp_int_array);
    memcpy(s_values->program_binary_formats, temp_int_array, max(s_values->num_program_binary_formats, 8) * sizeof(int));
    glGetIntegerv(GL_SHADER_BINARY_FORMATS, (GLint *)temp_int_array);
    memcpy(s_values->shader_binary_formats, temp_int_array, max(s_values->num_shader_binary_formats, 8) * sizeof(int));
#else
    int *temp_int_array = g_alloca(max(max(s_values->num_shader_binary_formats, s_values->num_compressed_texture_formats), s_values->num_program_binary_formats) * sizeof(int));
    glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, (GLint *)temp_int_array);
    memcpy(s_values->compressed_texture_formats, temp_int_array, max(s_values->num_compressed_texture_formats, 128) * sizeof(int));
    glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, (GLint *)temp_int_array);
    memcpy(s_values->program_binary_formats, temp_int_array, max(s_values->num_program_binary_formats, 8) * sizeof(int));
    glGetIntegerv(GL_SHADER_BINARY_FORMATS, (GLint *)temp_int_array);
    memcpy(s_values->shader_binary_formats, temp_int_array, max(s_values->num_shader_binary_formats, 8) * sizeof(int));
#endif
    glGetIntegerv(GL_SUBPIXEL_BITS, (GLint *)temp_int_value);
    s_values->subpixel_bits = temp_int_value[0];
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, (GLint *)temp_int_value);
    s_values->max_3d_texture_size = temp_int_value[0];
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, (GLint *)temp_int_value);
    s_values->max_combined_texture_image_units = temp_int_value[0];
    glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, (GLint *)temp_int_value);
    s_values->max_cube_map_texture_size = temp_int_value[0];
    glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, (GLint *)temp_int_value);
    s_values->max_elements_vertices = temp_int_value[0];
    glGetIntegerv(GL_MAX_ELEMENTS_INDICES, (GLint *)temp_int_value);
    s_values->max_elements_indices = temp_int_value[0];
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, (GLint *)temp_int_value);
    s_values->max_fragment_uniform_components = temp_int_value[0];
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, (GLint *)temp_int_value);
    s_values->max_renderbuffer_size = temp_int_value[0];
    glGetIntegerv(GL_MAX_SAMPLES, (GLint *)temp_int_value);
    s_values->max_samples = temp_int_value[0];
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint *)temp_int_value);
    s_values->max_texture_size = temp_int_value[0];
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, (GLint *)temp_int_value);
    s_values->max_vertex_attribs = temp_int_value[0];
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, (GLint *)temp_int_value);
    s_values->max_vertex_uniform_components = temp_int_value[0];

    glGetIntegerv(GL_MAX_IMAGE_UNITS, (GLint *)temp_int_value);
    s_values->max_image_units = temp_int_value[0];
    glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, (GLint *)temp_int_value);
    s_values->max_vertex_attrib_bindings = temp_int_value[0];
    glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_BLOCKS, (GLint *)temp_int_value);
    s_values->max_compute_uniform_blocks = temp_int_value[0];
    glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, (GLint *)temp_int_value);
    s_values->max_compute_texture_image_units = temp_int_value[0];
    glGetIntegerv(GL_MAX_COMPUTE_IMAGE_UNIFORMS, (GLint *)temp_int_value);
    s_values->max_compute_image_uniforms = temp_int_value[0];
    glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, (GLint *)temp_int_value);
    s_values->max_compute_shared_memory_size = temp_int_value[0];
    glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_COMPONENTS, (GLint *)temp_int_value);
    s_values->max_compute_uniform_components = temp_int_value[0];
    glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS, (GLint *)temp_int_value);
    s_values->max_compute_atomic_counter_buffers = temp_int_value[0];
    glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTERS, (GLint *)temp_int_value);
    s_values->max_compute_atomic_counters = temp_int_value[0];
    glGetIntegerv(GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS, (GLint *)temp_int_value);
    s_values->max_combined_compute_uniform_components = temp_int_value[0];
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, (GLint *)temp_int_value);
    s_values->max_compute_work_group_invocations = temp_int_value[0];

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, (GLint *)(temp_int_value));
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, (GLint *)(temp_int_value + 1));
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, (GLint *)(temp_int_value + 2));
    memcpy(s_values->max_compute_work_group_count, temp_int_array, 3 * sizeof(int));

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, (GLint *)(temp_int_value));
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, (GLint *)(temp_int_value + 1));
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, (GLint *)(temp_int_value + 2));
    memcpy(s_values->max_compute_work_group_size, temp_int_array, 3 * sizeof(int));

    glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, (GLint *)temp_int_value);
    s_values->max_uniform_locations = temp_int_value[0];
    glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, (GLint *)temp_int_value);
    s_values->max_framebuffer_width = temp_int_value[0];
    glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, (GLint *)temp_int_value);
    s_values->max_framebuffer_height = temp_int_value[0];
    glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, (GLint *)temp_int_value);
    s_values->max_framebuffer_samples = temp_int_value[0];
    glGetIntegerv(GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS, (GLint *)temp_int_value);
    s_values->max_vertex_atomic_counter_buffers = temp_int_value[0];
    glGetIntegerv(GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS, (GLint *)temp_int_value);
    s_values->max_fragment_atomic_counter_buffers = temp_int_value[0];
    glGetIntegerv(GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS, (GLint *)temp_int_value);
    s_values->max_combined_atomic_counter_buffers = temp_int_value[0];
    glGetIntegerv(GL_MAX_FRAGMENT_ATOMIC_COUNTERS, (GLint *)temp_int_value);
    s_values->max_fragment_atomic_counters = temp_int_value[0];
    glGetIntegerv(GL_MAX_COMBINED_ATOMIC_COUNTERS, (GLint *)temp_int_value);
    s_values->max_combined_atomic_counters = temp_int_value[0];
    glGetIntegerv(GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE, (GLint *)temp_int_value);
    s_values->max_atomic_counter_buffer_size = temp_int_value[0];
    glGetIntegerv(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, (GLint *)temp_int_value);
    s_values->max_atomic_counter_buffer_bindings = temp_int_value[0];
    glGetIntegerv(GL_MAX_VERTEX_IMAGE_UNIFORMS, (GLint *)temp_int_value);
    s_values->max_vertex_image_uniforms = temp_int_value[0];
    glGetIntegerv(GL_MAX_FRAGMENT_IMAGE_UNIFORMS, (GLint *)temp_int_value);
    s_values->max_fragment_image_uniforms = temp_int_value[0];
    glGetIntegerv(GL_MAX_COMBINED_IMAGE_UNIFORMS, (GLint *)temp_int_value);
    s_values->max_combined_image_uniforms = temp_int_value[0];
    glGetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS, (GLint *)temp_int_value);
    s_values->max_vertex_shader_storage_blocks = temp_int_value[0];
    glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, (GLint *)temp_int_value);
    s_values->max_fragment_shader_storage_blocks = temp_int_value[0];
    glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, (GLint *)temp_int_value);
    s_values->max_compute_shader_storage_blocks = temp_int_value[0];
    glGetIntegerv(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, (GLint *)temp_int_value);
    s_values->max_combined_shader_storage_blocks = temp_int_value[0];
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, (GLint *)temp_int_value);
    s_values->max_shader_storage_buffer_bindings = temp_int_value[0];
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, (GLint *)temp_int_value);
    s_values->max_shader_storage_block_size = temp_int_value[0];
    glGetIntegerv(GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES, (GLint *)temp_int_value);
    s_values->max_combined_shader_output_resources = temp_int_value[0];
    glGetIntegerv(GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET, (GLint *)temp_int_value);
    s_values->min_program_texture_gather_offset = temp_int_value[0];
    glGetIntegerv(GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET, (GLint *)temp_int_value);
    s_values->max_program_texture_gather_offset = temp_int_value[0];
    glGetIntegerv(GL_MAX_SAMPLE_MASK_WORDS, (GLint *)temp_int_value);
    s_values->max_sample_mask_words = temp_int_value[0];
    glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, (GLint *)temp_int_value);
    s_values->max_color_texture_samples = temp_int_value[0];
    glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, (GLint *)temp_int_value);
    s_values->max_depth_texture_samples = temp_int_value[0];
    glGetIntegerv(GL_MAX_INTEGER_SAMPLES, (GLint *)temp_int_value);
    s_values->max_integer_samples = temp_int_value[0];
    glGetIntegerv(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, (GLint *)temp_int_value);
    s_values->max_vertex_attrib_relative_offset = temp_int_value[0];
    // glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, (GLint *)&(s_values->max_vertex_attrib_bindings));
    glGetIntegerv(GL_MAX_VERTEX_ATTRIB_STRIDE, (GLint *)temp_int_value);
    s_values->max_vertex_attrib_stride = temp_int_value[0];

    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, (GLint *)temp_int_value);
    s_values->max_vertex_texture_image_units = temp_int_value[0];
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, (GLint *)temp_int_value);
    s_values->max_vertex_uniform_vectors = temp_int_value[0];
    // s_values->max_vertex_uniform_vectors = 256;
    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, (GLint *)temp_int_value);
    s_values->max_viewport_dims[0] = temp_int_value[0];
    s_values->max_viewport_dims[1] = temp_int_value[1];

    // glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, (GLint *)&(s_values->uniform_buffer_offset_alignment));
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, (GLint *)temp_int_value);
    s_values->max_array_texture_layers = temp_int_value[0];
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, (GLint *)temp_int_value);
    s_values->max_color_attachments = temp_int_value[0];
    glGetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS, (GLint *)temp_int_value);
    s_values->max_combined_uniform_blocks = temp_int_value[0];
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, (GLint *)temp_int_value);
    s_values->max_draw_buffers = temp_int_value[0];
    glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, (GLint *)temp_int_value);
    s_values->max_fragment_input_components = temp_int_value[0];
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, (GLint *)temp_int_value);
    s_values->max_fragment_uniform_blocks = temp_int_value[0];
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, (GLint *)temp_int_value);
    s_values->max_fragment_uniform_vectors = temp_int_value[0];
    // s_values->max_fragment_uniform_vectors = 256;
    glGetIntegerv(GL_MAX_PROGRAM_TEXEL_OFFSET, (GLint *)temp_int_value);
    s_values->max_program_texel_offset = temp_int_value[0];
    glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS, (GLint *)temp_int_value);
    s_values->max_transform_feedback_separate_attribs = temp_int_value[0];
    glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS, (GLint *)temp_int_value);
    s_values->max_transform_feedback_separate_components = temp_int_value[0];
    glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS, (GLint *)temp_int_value);
    s_values->max_transform_feedback_interleaved_components = temp_int_value[0];
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, (GLint *)temp_int_value);
    s_values->max_uniform_buffer_bindings = temp_int_value[0];
    // glGetIntegerv(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, (GLint *)&(s_values->max_atomic_counter_buffer_bindings));
    // glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, (GLint *)&(s_values->max_shader_storage_buffer_bindings));
    glGetIntegerv(GL_MAX_VARYING_VECTORS, (GLint *)temp_int_value);
    s_values->max_varying_vectors = temp_int_value[0];
    glGetIntegerv(GL_MAX_VARYING_COMPONENTS, (GLint *)temp_int_value);
    s_values->max_varying_components = temp_int_value[0];
    glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, (GLint *)temp_int_value);
    s_values->max_vertex_output_components = temp_int_value[0];
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, (GLint *)temp_int_value);
    s_values->max_vertex_uniform_blocks = temp_int_value[0];
    glGetIntegerv(GL_MIN_PROGRAM_TEXEL_OFFSET, (GLint *)temp_int_value);
    s_values->min_program_texel_offset = temp_int_value[0];
    // glGetIntegerv(GL_SAMPLES, (GLint *)&(s_values->samples));
    // glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, (GLint *)&(s_values->shader_storage_buffer_offset_alignment));
    glGetIntegerv(GL_SUBPIXEL_BITS, (GLint *)temp_int_value);
    s_values->subpixel_bits = temp_int_value[0];

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, (GLint *)temp_int_value);
    s_values->texture_image_units = temp_int_value[0];
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, (GLint *)temp_int_value);
    s_values->uniform_buffer_offset_alignment = temp_int_value[0];
    glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY, (GLint *)temp_int_value);
    s_values->max_texture_anisotropy = temp_int_value[0];
    // s_values->uniform_buffer_offset_alignment = 1;

    glGetIntegerv(GL_LAYER_PROVOKING_VERTEX, (GLint *)temp_int_value);
    s_values->layer_provoking_vertex = temp_int_value[0];
    glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS, (GLint *)temp_int_value);
    s_values->max_geometry_uniform_components = temp_int_value[0];
    glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, (GLint *)temp_int_value);
    s_values->max_geometry_uniform_blocks = temp_int_value[0];
    glGetIntegerv(GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS, (GLint *)temp_int_value);
    s_values->max_combined_geometry_uniform_components = temp_int_value[0];
    glGetIntegerv(GL_MAX_GEOMETRY_INPUT_COMPONENTS, (GLint *)temp_int_value);
    s_values->max_geometry_input_components = temp_int_value[0];
    glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, (GLint *)temp_int_value);
    s_values->max_geometry_output_components = temp_int_value[0];
    glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, (GLint *)temp_int_value);
    s_values->max_geometry_output_vertices = temp_int_value[0];
    glGetIntegerv(GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS, (GLint *)temp_int_value);
    s_values->max_geometry_total_output_components = temp_int_value[0];
    glGetIntegerv(GL_MAX_GEOMETRY_SHADER_INVOCATIONS, (GLint *)temp_int_value);
    s_values->max_geometry_shader_invocations = temp_int_value[0];
    glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, (GLint *)temp_int_value);
    s_values->max_geometry_texture_image_units = temp_int_value[0];
    glGetIntegerv(GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS, (GLint *)temp_int_value);
    s_values->max_geometry_atomic_counter_buffers = temp_int_value[0];
    glGetIntegerv(GL_MAX_GEOMETRY_ATOMIC_COUNTERS, (GLint *)temp_int_value);
    s_values->max_geometry_atomic_counters = temp_int_value[0];
    glGetIntegerv(GL_MAX_GEOMETRY_IMAGE_UNIFORMS, (GLint *)temp_int_value);
    s_values->max_geometry_image_uniforms = temp_int_value[0];
    glGetIntegerv(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS, (GLint *)temp_int_value);
    s_values->max_geometry_shader_storage_blocks = temp_int_value[0];
    glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, (GLint *)temp_int_value);
    s_values->max_framebuffer_layers = temp_int_value[0];

    glGetIntegerv(GL_MAX_PATCH_VERTICES, (GLint *)temp_int_value);
    s_values->max_patch_vertices = temp_int_value[0];
    glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, (GLint *)temp_int_value);
    s_values->max_tess_gen_level = temp_int_value[0];
    glGetIntegerv(GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS, (GLint *)temp_int_value);
    s_values->max_tess_control_uniform_components = temp_int_value[0];
    glGetIntegerv(GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS, (GLint *)temp_int_value);
    s_values->max_tess_evaluation_uniform_components = temp_int_value[0];
    glGetIntegerv(GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS, (GLint *)temp_int_value);
    s_values->max_tess_control_texture_image_units = temp_int_value[0];
    glGetIntegerv(GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS, (GLint *)temp_int_value);
    s_values->max_tess_evaluation_texture_image_units = temp_int_value[0];
    glGetIntegerv(GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS, (GLint *)temp_int_value);
    s_values->max_tess_control_output_components = temp_int_value[0];
    glGetIntegerv(GL_MAX_TESS_PATCH_COMPONENTS, (GLint *)temp_int_value);
    s_values->max_tess_patch_components = temp_int_value[0];
    glGetIntegerv(GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS, (GLint *)temp_int_value);
    s_values->max_tess_control_total_output_components = temp_int_value[0];
    glGetIntegerv(GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS, (GLint *)temp_int_value);
    s_values->max_tess_evaluation_output_components = temp_int_value[0];
    glGetIntegerv(GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS, (GLint *)temp_int_value);
    s_values->max_tess_control_uniform_blocks = temp_int_value[0];
    glGetIntegerv(GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS, (GLint *)temp_int_value);
    s_values->max_tess_evaluation_uniform_blocks = temp_int_value[0];
    glGetIntegerv(GL_MAX_TESS_CONTROL_INPUT_COMPONENTS, (GLint *)temp_int_value);
    s_values->max_tess_control_input_components = temp_int_value[0];
    glGetIntegerv(GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS, (GLint *)temp_int_value);
    s_values->max_tess_evaluation_input_components = temp_int_value[0];
    glGetIntegerv(GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS, (GLint *)temp_int_value);
    s_values->max_combined_tess_control_uniform_components = temp_int_value[0];
    glGetIntegerv(GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS, (GLint *)temp_int_value);
    s_values->max_combined_tess_evaluation_uniform_components = temp_int_value[0];
    glGetIntegerv(GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS, (GLint *)temp_int_value);
    s_values->max_tess_control_atomic_counter_buffers = temp_int_value[0];
    glGetIntegerv(GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS, (GLint *)temp_int_value);
    s_values->max_tess_evaluation_atomic_counter_buffers = temp_int_value[0];
    glGetIntegerv(GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS, (GLint *)temp_int_value);
    s_values->max_tess_control_atomic_counters = temp_int_value[0];
    glGetIntegerv(GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS, (GLint *)temp_int_value);
    s_values->max_tess_evaluation_atomic_counters = temp_int_value[0];
    glGetIntegerv(GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS, (GLint *)temp_int_value);
    s_values->max_tess_control_image_uniforms = temp_int_value[0];
    glGetIntegerv(GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS, (GLint *)temp_int_value);
    s_values->max_tess_evaluation_image_uniforms = temp_int_value[0];
    glGetIntegerv(GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS, (GLint *)temp_int_value);
    s_values->max_tess_control_shader_storage_blocks = temp_int_value[0];
    glGetIntegerv(GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS, (GLint *)temp_int_value);
    s_values->max_tess_evaluation_shader_storage_blocks = temp_int_value[0];
    
    glGetIntegerv(GL_MAX_DEBUG_MESSAGE_LENGTH, (GLint *)temp_int_value);
    s_values->max_debug_message_length = temp_int_value[0];
    glGetIntegerv(GL_MAX_DEBUG_LOGGED_MESSAGES, (GLint *)temp_int_value);
    s_values->max_debug_logged_messages = temp_int_value[0];
    glGetIntegerv(GL_MAX_DEBUG_GROUP_STACK_DEPTH, (GLint *)temp_int_value);
    s_values->max_debug_group_stack_depth = temp_int_value[0];
    glGetIntegerv(GL_MAX_LABEL_LENGTH, (GLint *)temp_int_value);
    s_values->max_label_length = temp_int_value[0];

    GLboolean temp_boolean_value;
    glGetBooleanv(GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED,
                  &temp_boolean_value);
    s_values->primitive_restart_for_patches_supported = temp_boolean_value;


    GLfloat temp_float_value[2];
    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, temp_float_value);
    s_values->aliased_line_width_range[0] = temp_float_value[0];
    s_values->aliased_line_width_range[1] = temp_float_value[1];
    glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, temp_float_value);
    s_values->aliased_point_size_range[0] = temp_float_value[0];
    s_values->aliased_point_size_range[1] = temp_float_value[1];
    glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, temp_float_value);
    s_values->smooth_line_width_range[0] = temp_float_value[0];
    s_values->smooth_line_width_range[1] = temp_float_value[1];
    glGetFloatv(GL_SMOOTH_LINE_WIDTH_GRANULARITY, temp_float_value);
    s_values->smooth_line_width_granularity = temp_float_value[0];
    glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, temp_float_value);
    s_values->max_texture_log_bias = temp_float_value[0];
    glGetFloatv(GL_MIN_FRAGMENT_INTERPOLATION_OFFSET, temp_float_value);
    s_values->min_fragment_interpolation_offset = temp_float_value[0];
    glGetFloatv(GL_MAX_FRAGMENT_INTERPOLATION_OFFSET, temp_float_value);
    s_values->max_fragment_interpolation_offset = temp_float_value[0];

    GLint64 temp_int64_value;
    glGetInteger64v(GL_MAX_ELEMENT_INDEX, &temp_int64_value);
    s_values->max_element_index = temp_int64_value;
    glGetInteger64v(GL_MAX_SERVER_WAIT_TIMEOUT, &temp_int64_value);
    s_values->max_server_wait_timeout = temp_int64_value;
    glGetInteger64v(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS, &temp_int64_value);
    s_values->max_combined_vertex_uniform_components = temp_int64_value;
    glGetInteger64v(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS, &temp_int64_value);
    s_values->max_combined_fragment_uniform_components = temp_int64_value;
    glGetInteger64v(GL_MAX_UNIFORM_BLOCK_SIZE, &temp_int64_value);
    s_values->max_uniform_block_size = temp_int64_value;
    glGetInteger64v(GL_FRAGMENT_INTERPOLATION_OFFSET_BITS, &temp_int64_value);
    s_values->fragment_interpolation_offset_bits = temp_int64_value;

    return;
}

/**
 * @brief 主窗口绘制时使用的着色器加载的代码
 *
 * @param type 着色器类型
 * @param shaderSrc 着色器源码
 * @return GLuint 返回着色器编号，若为0则生成失败
 */
GLuint load_shader(GLenum type, const char *shaderSrc)
{
    GLuint shader;
    GLint compiled;

    shader = glCreateShader(type);

    if (shader == 0)
    {
        //    express_printf("Shader==0\n");
        return 0;
    }

    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, NULL);

    // Compile the shader
    glCompileShader(shader);

    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled)
    {
        GLint infoLen = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1)
        {
            char *infoLog = malloc(sizeof(char) * infoLen);

            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);

            free(infoLog);
        }

        glDeleteShader(shader);
        //    express_printf("compiled==0\n");
        return 0;
    }

    return shader;
}

/**
 * @brief 窗口界面使用OpenGL渲染的前置操作，例如加载着色器，生成顶点等
 *
 * @param program 返回值，填入生成的着色器ID
 * @param VAO 返回值，填入生成的顶点数组ID
 * @return int 返回1表示准备成功，为0则说明准备失败
 */
int main_window_opengl_prepare(GLuint *program, GLuint *VAO)
{

    // @todo 暂时未支持旋转和翻转操作
    char vShaderStr[] =
    #ifdef _WIN32
        "#version 300 es\n"
    #else
        "#version 330\n"
    #endif
        "layout (location = 0) in vec2 position;\n"
        "layout (location = 1) in vec2 texCoords;\n"
        "uniform int transform_loc;\n"
        "out vec2 TexCoords;\n"
        "void main()\n"
        "{\n"
        "    if(transform_loc == 1)\n"
        "    {\n"
        "       gl_Position = vec4(position.x, -position.y, 0.0f, 1.0f);\n"
        "    }\n"
        "    else if(transform_loc == 2)\n"
        "    {\n"
        "       gl_Position = vec4(position.x, -position.y, 0.0f, 1.0f);\n"
        "    }\n"
        "    else if(transform_loc == 3)\n"
        "    {\n"
        "       gl_Position = vec4(position.x, -position.y, 0.0f, 1.0f);\n"
        "    }\n"
        "    else if(transform_loc == 4)\n"
        "    {\n"
        "       gl_Position = vec4(-position.x, position.y, 0.0f, 1.0f);\n"
        "    }\n"
        "    else if(transform_loc == 5)\n"
        "    {\n"
        "       gl_Position = vec4(position.x, -position.y, 0.0f, 1.0f);\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "       gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);\n"
        "    }\n"
        "    TexCoords = texCoords;\n"
        "}\n";

    char fShaderStr[] =
    #ifdef _WIN32
        "#version 300 es\n"
    #else
        "#version 330\n"
    #endif
        "precision mediump float;                     \n"
        "in vec2 TexCoords;\n"
        "out vec4 color;\n"
        "uniform sampler2D screenTexture;\n"
        "void main(){\n"
        "color = texture(screenTexture, TexCoords);\n"
        "}\n";

    GLuint programObject = glCreateProgram();
    if (programObject == 0)
    {
        // express_printf("shit glCreateProgram2 %ld\n", GetLastError());
        return 0;
    }

    GLuint vertexShader = load_shader(GL_VERTEX_SHADER, vShaderStr);
    GLuint fragmentShader = load_shader(GL_FRAGMENT_SHADER, fShaderStr);
    // express_printf("shader %d %d\n", vertexShader, fragmentShader);

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    glLinkProgram(programObject);

    GLint linked;
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        // express_printf("shit glGetProgramiv2 %ld\n", GetLastError());
        return 0;
    }

    GLfloat quadVertices[] = {// Vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
                              // Positions   // TexCoords
                              -1.0f, 1.0f, 0.0f, 1.0f,
                              -1.0f, -1.0f, 0.0f, 0.0f,
                              1.0f, -1.0f, 1.0f, 0.0f,

                              -1.0f, 1.0f, 0.0f, 1.0f,
                              1.0f, -1.0f, 1.0f, 0.0f,
                              1.0f, 1.0f, 1.0f, 1.0f};

    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    *program = programObject;
    *VAO = quadVAO;

    // 开启透明度混合后，默认不开透明度的线程的绘制结果对应的texture的透明度默认为0，叠加上去后会导致透明，看不到东西
    //  glEnable(GL_BLEND);
    //  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(programObject);

    glClearColor(0, 0, 0, 1);

    return 1;
}

void adjust_blend_type(int blend_type)
{
    switch (blend_type)
    {
    case BLEND_NONE:
    {
        glDisable(GL_BLEND);
    }
    break;
    case BLEND_CLEAR:
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ZERO, GL_ZERO);
    }
    break;
    case BLEND_SRC:
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ZERO);
    }
    break;
    case BLEND_SRCOVER:
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    }
    break;
    case BLEND_DSTOVER:
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
    }
    break;
    case BLEND_SRCIN:
    {
        // 只显示src和dst重叠的地方，dst只有alpha参与运算
        glEnable(GL_BLEND);
        glBlendFunc(GL_DST_ALPHA, GL_ZERO);
    }
    break;
    case BLEND_DSTIN:
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ZERO, GL_SRC_ALPHA);
    }
    break;
    case BLEND_SRCOUT:
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ZERO);
    }
    break;
    case BLEND_DSTOUT:
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
    }
    break;
    case BLEND_SRCATOP:
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    break;
    case BLEND_DSTATOP:
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
    }
    break;
    case BLEND_ADD:
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
    }
    break;
    case BLEND_XOR:
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    break;
    case BLEND_DST:
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ZERO, GL_ONE);
    }
    break;
    case BLEND_AKS:
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE_MINUS_SRC_ALPHA);
    }
    break;
    case BLEND_AKD:
    {
    }
    break;
    default:
    {
        glDisable(GL_BLEND);
        LOGW("unknown blend type %d", blend_type);
    }
    break;
    }
}

Dying_List *dying_list_append(Dying_List *list, void *data)
{
    if (list == NULL)
    {
        list = (Dying_List *)g_malloc(sizeof(Dying_List));
        list->header = NULL;
        list->tail = NULL;
        list->num = 0;
    }
    Dying_List_Node *node = g_malloc(sizeof(Dying_List_Node));
    node->data = data;
    node->next = NULL;
    if (list->tail == NULL)
    {
        list->tail = node;
        list->header = node;
        node->prev = NULL;
    }
    else
    {
        list->tail->next = node;
        node->prev = list->tail;
        list->tail = node;
    }
    list->num++;
    return list;
}

Dying_List *dying_list_remove(Dying_List *list, void *data)
{
    if (list == NULL)
    {
        return list;
    }
    for (Dying_List_Node *node = list->header; node != NULL;)
    {
        if (node->data == data)
        {
            if (node->prev == NULL)
            {
                list->header = node->next;
            }
            else
            {
                node->prev->next = node->next;
            }
            if (node->next == NULL)
            {
                list->tail = node->prev;
            }
            else
            {
                node->next->prev = node->prev;
            }
            Dying_List_Node *node_next = node->next;
            g_free(node);
            node = node_next;
            list->num--;
            return list;
        }
        else
        {
            node = node->next;
        }
    }
    return list;
}

Dying_List *dying_list_foreach(Dying_List *list, Dying_Function fun)
{
    if (list == NULL)
    {
        return list;
    }
    for (Dying_List_Node *node = list->header; node != NULL;)
    {
        if (fun(node->data) == 1)
        {
            if (node->prev == NULL)
            {
                list->header = node->next;
            }
            else
            {
                node->prev->next = node->next;
            }
            if (node->next == NULL)
            {
                list->tail = node->prev;
            }
            else
            {
                node->next->prev = node->prev;
            }
            Dying_List_Node *node_next = node->next;
            g_free(node);
            node = node_next;
            list->num--;
        }
        else
        {
            node = node->next;
        }
    }
    return list;
}

void APIENTRY gl_debug_output(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    // 忽略一些不是错误的id
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;
    if (severity == GL_DEBUG_SEVERITY_LOW || severity == GL_DEBUG_SEVERITY_NOTIFICATION)
    {
        return;
    }

#ifdef ENABLE_OPENGL_PERFORMANCE_WARNING

#else
    if (type == GL_DEBUG_TYPE_PERFORMANCE)
    {
        return;
    }
#endif

    LOGI("\ndebug message(%u):%s", id, message);
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        LOGI("Source: API ");
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        LOGI("Source: Window System ");
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        LOGI("Source: Shader Compiler ");
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        LOGI("Source: Third Party ");
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        LOGI("Source: APPLICATION ");
        break;
    case GL_DEBUG_SOURCE_OTHER:
        break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        LOGI("Type: Error ");
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        LOGI("Type: Deprecated Behaviour ");
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        LOGI("Type: Undefined Behaviour ");
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        LOGI("Type: Portability ");
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        LOGI("Type: Performance ");
        break;
    case GL_DEBUG_TYPE_MARKER:
        LOGI("Type: Marker ");
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        LOGI("Type: Push Group ");
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        LOGI("Type: Pop Group ");
        break;
    case GL_DEBUG_TYPE_OTHER:
        LOGI("Type: Other ");
        break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        LOGI("Severity: high");
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        LOGI("Severity: medium");
        break;
    case GL_DEBUG_SEVERITY_LOW:
        LOGI("Severity: low");
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        LOGI("Severity: notification");
        break;
    }
    LOGI("");
}

void glTestIntAsyn(GLint a, GLuint b, GLfloat c, GLdouble d)
{
    LOGI("glTestInt asyn %d,%u,%f,%lf", a, b, c, d);
    fflush(stdout);
}

void glPrintfAsyn(GLint a, GLuint size, GLdouble c, const GLchar *out_string)
{

    LOGI("glPrintfAsyn asyn string %d,%u,%lf,%s", a, size, c, out_string);
    return;
}

GLint glTestInt1(GLint a, GLuint b)
{
    express_printf("glTestInt1 %d,%u\n", a, b);
    // fflush(stdout);
    return 576634565;
}
GLuint glTestInt2(GLint a, GLuint b)
{
    LOGI("glTestInt2 %d,%u", a, b);
    fflush(stdout);
    return 4000001200u;
}

GLint64 glTestInt3(GLint64 a, GLuint64 b)
{
    LOGI("glTestInt3 %lld,%llu", a, b);
    fflush(stdout);
    return 453489431344456;
}
GLuint64 glTestInt4(GLint64 a, GLuint64 b)
{
    LOGI("glTestInt4 %lld,%llu", a, b);
    fflush(stdout);
    return 436004354364364345;
}

GLfloat glTestInt5(GLint a, GLuint b)
{
    LOGI("glTestInt5 %d,%u", a, b);
    fflush(stdout);
    return 3.1415926;
}
GLdouble glTestInt6(GLint a, GLuint b)
{
    LOGI("glTestInt6 %d,%u", a, b);
    fflush(stdout);
    return 3.1415926535;
}

// void glTestPointer1(GLint a, const GLint *b)
// {
//     LOGI("glTestPointer1 %d ", a);
//     for (int i = 0; i < 10; i++)
//     {
//         LOGI("%d ", b[i]);
//     }
//     LOGI("");
//     fflush(stdout);
//     return;
// }

// void glTestPointer2(GLint a, const GLint *b, GLint *c)
// {
//     LOGI("glTestPointer2 %d %d", a, *b);
//     for (int i = 0; i < 10; i++)
//     {
//         c[i] = b[i];
//     }
//     fflush(stdout);
//     return;
// }

// GLint glTestPointer4(GLint a, const GLint *b, GLint *c)
// {
//     LOGI("glTestPointer4 %d,%d", a, *b);
//     for (int i = 0; i < 1000; i++)
//     {
//         c[i] = b[i];
//     }
//     fflush(stdout);
//     return 12456687;
// }

// GLint d_glTestPointer3(void *context, GLint a, const GLint *b, GLint *c)
// {

//     int len;
//     char *temp = g_malloc(a * sizeof(int));
//     memset(temp, 0, a * sizeof(int));
//     LOGI("glTestPointer3 %d", a);
//     read_from_guest_mem((Guest_Mem *)b, temp, 0, a * sizeof(int));

//     char *temp_s[100];
//     int loc = 0;
//     for (int i = a / 2; i < a / 2 + 10; i++)
//     {
//         loc += sprintf(temp_s + loc, "%d ", temp[i]);
//     }
//     LOGI("glTestPointer3 %s", temp_s);

//     write_to_guest_mem((Guest_Mem *)c, temp, 0, a * sizeof(int));

//     fflush(stdout);
//     return 12456687;
// }

void glTestString(GLint a, GLint count, const GLchar *const *strings, GLint buf_len, GLchar *char_buf)
{
    LOGI("glTestString %d %d %d\nString:", a, count, buf_len);
    for (int i = 0; i < count; i++)
    {
        LOGI("%s", strings[i]);
    }
    const char *t = "printf ok!";
    memcpy(char_buf, t, strlen(t));
    fflush(stdout);
}

void d_glPrintf(void *context, GLint buf_len, const GLchar *out_string)
{
    // char *t="temp test abcd";
    // memcpy(out_string,t,strlen(t));
    char *temp = g_malloc(buf_len);
    read_from_guest_mem((Guest_Mem *)out_string, temp, 0, buf_len);

    if (buf_len < 100)
    {
        LOGI("glPrintf %d %s", buf_len, temp);
    }
    else
    {
        int flag = 1;
        for (int i = 0; i < buf_len; i++)
        {
            if (temp[i] != 'c')
            {
                flag = 0;
            }
        }
        if (flag == 0)
        {
            LOGI("glPrintf check error!");
        }
        else
        {
            LOGI("glPrintf check ok!");
        }
    }
    g_free(temp);
    // fflush(stdout);
    // int flag=0;

    return;
}

// glInOutTest GLint a, GLint b, const GLchar *e#strlen(e), GLint *c#sizeof(GLint), GLdouble *d#sizeof(GLdouble), GLsizei buf_len, GLchar *f#buf_len

// glSaveLongTime GLuint a, GLdouble b, const void *pointer#a

void d_glInOutTest(void *context, GLint a, GLint b, const GLchar *e, GLint *c, GLdouble *d, GLsizei buf_len, GLchar *f)
{
    // LOGI("glInOutTest %d,%d   buf_len%llu",a,b,buf_len);
    // *c=78646313;
    // *d=3.141592653543;

    // char *temp;

    // char *t="glInOutTest printf ok! test ok!";
    // memcpy(f,t,strlen(t));
    // fflush(stdout);
}
