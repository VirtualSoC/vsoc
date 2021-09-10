// #define STD_DEBUG_LOG

#include "express-gpu/glv3_context.h"

// #include "gl.h"

// #include "express-gpu/gl.h"
#include "glad/glad.h"

// #include "express-gpu/glext.h"
// #include "express-gpu/gl2ext.h"

//下面这三个函数都是销毁函数，不提供外部调用，只用来给g_hash_table_new_full用
static void g_buffer_map_destroy(gpointer data);

// static void g_vao_status_destroy(gpointer data);

static void g_vao_point_data_destroy(gpointer data);

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
            //case GL_BGRA8_EXT:
            return sizeof(unsigned char) * 4;
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
        case GL_RG16UI:
        case GL_RG_INTEGER:
            return sizeof(unsigned short) * 2;
        case GL_RGB16UI:
        case GL_RGB_INTEGER:
            return sizeof(unsigned short) * 3;
        case GL_RGBA16UI:
        case GL_RGBA_INTEGER:
            return sizeof(unsigned short) * 4;
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
        }
        break;
    case GL_UNSIGNED_INT:
        switch (format)
        {
        case GL_DEPTH_COMPONENT16:
        case GL_DEPTH_COMPONENT24:
        // case GL_DEPTH_COMPONENT32_OES:
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
        }
        break;
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_5_5_5_1:
    case GL_UNSIGNED_SHORT_5_6_5:
        // case GL_UNSIGNED_SHORT_4_4_4_4_REV_EXT:
        // case GL_UNSIGNED_SHORT_1_5_5_5_REV_EXT:
        return sizeof(unsigned short);
    case GL_UNSIGNED_INT_10F_11F_11F_REV:
    case GL_UNSIGNED_INT_5_9_9_9_REV:
    case GL_UNSIGNED_INT_2_10_10_10_REV:
        // case GL_UNSIGNED_INT_24_8_OES:
        return sizeof(unsigned int);
    case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
        return sizeof(float) + sizeof(unsigned int);
    case GL_FLOAT:
        switch (format)
        {
        case GL_DEPTH_COMPONENT32F:
        case GL_DEPTH_COMPONENT:
            return sizeof(float);
        // case GL_ALPHA32F_EXT:
        case GL_ALPHA:
            return sizeof(float);
        // case GL_LUMINANCE32F_EXT:
        case GL_LUMINANCE:
            return sizeof(float);
        // case GL_LUMINANCE_ALPHA32F_EXT:
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
        }
        break;
    case GL_HALF_FLOAT:
        // case GL_HALF_FLOAT_OES:
        switch (format)
        {
        // case GL_ALPHA16F_EXT:
        case GL_ALPHA:
            return sizeof(unsigned short);
        // case GL_LUMINANCE16F_EXT:
        case GL_LUMINANCE:
            return sizeof(unsigned short);
        // case GL_LUMINANCE_ALPHA16F_EXT:
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
        retval = 4;
        break;
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_5_5_5_1:
    case GL_UNSIGNED_SHORT_5_6_5:
        // case GL_UNSIGNED_SHORT_4_4_4_4_REV_EXT:
        // case GL_UNSIGNED_SHORT_1_5_5_5_REV_EXT:
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
        printf("**** ERROR unknown type 0x%x (%s,%d)\n", type, __FUNCTION__, __LINE__);
        retval = 4;
    }
    return retval;
}

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
    // case GL_NUM_COMPRESSED_TEXTURE_FORMATS:
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
    case GL_TEXTURE_BINDING_CUBE_MAP:
    // case GL_TEXTURE_BINDING_EXTERNAL_OES:
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
    // case GL_NUM_SHADER_BINARY_FORMATS:
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
        // case GL_NUM_PROGRAM_BINARY_FORMATS:
        // case GL_PROGRAM_BINARY_FORMATS:

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
        s = 4;
        break;
    case GL_MODELVIEW_MATRIX:
    case GL_PROJECTION_MATRIX:
    case GL_TEXTURE_MATRIX:
        s = 16;
        break;
    // case GL_COMPRESSED_TEXTURE_FORMATS:
    //     s = 16;
    //     break;
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
        s = 1;
        break;
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
    case GL_NUM_COMPRESSED_TEXTURE_FORMATS:
        s = 1;
        break;
    default:
        printf("gl_pname_size: unknow pname 0x%08x\n", pname);
        s = 1; // assume 1
    }
    return s;
}

void d_glBindFramebuffer_special(void *context, GLenum target, GLuint framebuffer)
{
    GLuint draw_fbo0 = ((Opengl_Context *)context)->draw_fbo0;
    GLuint read_fbo0 = ((Opengl_Context *)context)->read_fbo0;

    if (framebuffer == 0)
    {
        if (target == GL_DRAW_FRAMEBUFFER || target == GL_FRAMEBUFFER)
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw_fbo0);
        }
        if (target == GL_READ_FRAMEBUFFER || target == GL_FRAMEBUFFER)
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, read_fbo0);
        }
    }
    else
    {
        express_printf("bind framebuffer %u\n", framebuffer);
        glBindFramebuffer(target, framebuffer);
    }

    // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, real_opengl_context->draw_fbo0);
    // glBindFramebuffer(GL_READ_FRAMEBUFFER, real_opengl_context->read_fbo0);
}

void d_glBindBuffer_origin(void *context, GLenum target, GLuint buffer)
{

    // Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    // // Buffer_Status *status = bound_buffer->buffer_status;
    // GLint id = buffer;
    // //由于这里status是指针，所以对这个status的更新也会直接反映到vao相应的status上去
    // switch (target)
    // {
    // case GL_ARRAY_BUFFER:
    //     status->array_buffer = id;
    //     express_printf("bind GL_ARRAY_BUFFER %u\n",buffer);
    //     //bound_buffer->vao_vbo[bound_buffer->vertex_array_buffer] = id;
    //     break;
    // case GL_ELEMENT_ARRAY_BUFFER:
    //     express_printf("bind GL_ELEMENT_ARRAY_BUFFER %u\n",buffer);

    //     status->element_array_buffer = id;
    //     //bound_buffer->vao_ebo[bound_buffer->vertex_array_buffer] = id;
    //     break;
    // case GL_COPY_READ_BUFFER:
    //     status->copy_read_buffer = id;
    //     break;
    // case GL_COPY_WRITE_BUFFER:
    //     status->copy_write_buffer = id;
    //     break;
    // case GL_PIXEL_PACK_BUFFER:
    //     status->pixel_pack_buffer = id;
    //     break;
    // case GL_PIXEL_UNPACK_BUFFER:
    //     status->pixel_unpack_buffer = id;
    //     break;
    // case GL_TRANSFORM_FEEDBACK_BUFFER:
    //     status->transform_feedback_buffer = id;
    //     break;
    // case GL_UNIFORM_BUFFER:
    //     status->uniform_buffer = id;
    //     break;
    // case GL_ATOMIC_COUNTER_BUFFER:
    //     status->atomic_counter_buffer = id;
    //     break;
    // case GL_DISPATCH_INDIRECT_BUFFER:
    //     status->dispatch_indirect_buffer = id;
    //     break;
    // case GL_DRAW_INDIRECT_BUFFER:
    //     status->draw_indirect_buffer = id;
    //     break;
    // case GL_SHADER_STORAGE_BUFFER:
    //     status->shader_storage_buffer = id;
    // }
    // express_printf("bind buffer %u\n", buffer);

    glBindBuffer(target, buffer);
}

void d_glLinkProgram_origin(void *context, GLuint program)
{
    glLinkProgram(program);
}

void d_glShaderSource_origin(void *context, GLuint shader, GLsizei count, const GLint *length, const GLchar *const *string)
{
    express_printf("gl shader source:\n%s", string[0]);
    glShaderSource(shader, count, string, length);
}

void d_glGetString_special(void *context, GLenum name, GLubyte *buffer)
{
    const GLubyte *static_string = glGetString(name);
    int len = strlen((const char *)static_string);
    if (len >= 1024)
    {
        len = 1023;
        printf("error, glGetString string too long %x %s", name, static_string);
    }
    memcpy(buffer, static_string, len);
    //#1024
}

void d_glGetStringi_special(void *context, GLenum name, GLuint index, GLubyte *buffer)
{
    const GLubyte *static_string = glGetStringi(name, index);
    int len = strlen((const char *)static_string);
    if (len >= 1024)
    {
        len = 1023;
        printf("error, glGetStringi string too long %x %u %s", name, index, static_string);
    }
    memcpy(buffer, static_string, len);
}

void d_glViewport_special(void *context, GLint x, GLint y, GLsizei width, GLsizei height)
{
    Opengl_Context *real_opengl_context = (Opengl_Context *)context;
    real_opengl_context->view_x = 0;
    real_opengl_context->view_y = 0;
    real_opengl_context->view_w = width;
    real_opengl_context->view_h = height;
    glViewport(x, y, width, height);
    return;
}

void d_glEGLImageTargetTexture2DOES(void *context, GLenum target, GLeglImageOES image)
{
    //不会调用到host端来
}

void d_glBindEGLImage(void *context, GLenum target, GLeglImageOES image)
{
    uint64_t gbuffer_id = (uint64_t)image;
    Window_Buffer *real_surface = get_surface_from_gbuffer_id(gbuffer_id);
    if (real_surface != NULL)
    {
        // gbuffer_id能映射到surface的情况，说明这个image用于输出，所以直接绑定texture
        if (target == GL_IMAGE_BINDING_ACCESS)
        {
            acquire_texture_from_surface(real_surface);
        }
        else if (target == GL_READ_ONLY)
        {
            //这里读取之所以进行绑定texture，是因为image在读取的时候就是连接到texture来读取的
            glBindTexture(GL_TEXTURE_2D, real_surface->fbo_texture[real_surface->now_read]);
        }
        else if (target == GL_WRITE_ONLY)
        {
            //不可能出现，因为是surface的情况下，不会被用来进行写入操作
            glBindFramebuffer(GL_FRAMEBUFFER, real_surface->display_fbo[real_surface->now_draw]);
            printf("error! Surface is write by image!");
        }
        else if (target == GL_NONE)
        {
            //GL_NONE的情况需要解除锁定
            release_texture_from_surface(real_surface);
        }
        return;
    }
    EGL_Image *egl_image = get_image_from_gbuffer_id(gbuffer_id);
    if (egl_image != NULL)
    {
        //gbuffer_id能映射到image的情况，说明这个image用于输出，需要在这个image上写点啥
        //guest端可能会调用glFramebufferTexture2D，在调用了这个函数后，还需要绑定fbo
        if (target == GL_IMAGE_BINDING_ACCESS)
        {
            acquire_texture_from_image(egl_image);
        }
        else if (target == GL_READ_ONLY)
        {
            glBindTexture(GL_TEXTURE_2D, egl_image->fbo_texture);
        }
        else if (target == GL_WRITE_ONLY)
        {
            //这个write_only一定出现在read_only之后，所以不需要加锁
            glBindFramebuffer(GL_FRAMEBUFFER, egl_image->display_fbo);
        }
        else if (target == GL_NONE)
        {
            //GL_NONE的情况需要解除锁定
            release_texture_from_image(egl_image);
        }
    }
}

void d_glEGLImageTargetRenderbufferStorageOES(void *context, GLenum target, GLeglImageOES image)
{
    //当前google没实现，所以暂时先不管
}

void resource_context_init(Resource_Context *resources, Share_Resources *share_resources)
{
    if (share_resources != NULL)
    {
        resources->share_resources = share_resources;
        resources->share_resources->counter += 1;
    }
    else
    {
        resources->share_resources = g_malloc(sizeof(Share_Resources));
        memset(resources->share_resources, 0, sizeof(Share_Resources));
        resources->share_resources->counter = 1;
    }

    resources->exclusive_resources = g_malloc(sizeof(Exclusive_Resources));
    memset(resources->exclusive_resources, 0, sizeof(Exclusive_Resources));

    resources->texture_resource = &(resources->share_resources->texture_resource);
    resources->buffer_resource = &(resources->share_resources->buffer_resource);
    resources->render_buffer_resource = &(resources->share_resources->render_buffer_resource);
    resources->sampler_resource = &(resources->share_resources->sample_resource);

    resources->shader_resource = &(resources->share_resources->shader_resource);
    resources->program_resource = &(resources->share_resources->program_resource);

    resources->sync_resource = &(resources->share_resources->sync_resource);

    resources->frame_buffer_resource = &(resources->exclusive_resources->frame_buffer_resource);
    resources->program_pipeline_resource = &(resources->exclusive_resources->program_pipeline_resource);
    resources->transform_feedback_resource = &(resources->exclusive_resources->transform_feedback_resource);
    resources->vertex_array_resource = &(resources->exclusive_resources->vertex_array_resource);

    resources->query_resource = &(resources->exclusive_resources->query_resource);
}

#define DESTROY_RESOURCES(resource_name, resource_delete)                                              \
    if (resources->resource_name->resource_id_map != NULL)                                             \
    {                                                                                                  \
        for (int i = 1; i <= resources->resource_name->max_id; i++)                                    \
        {                                                                                              \
            if (resources->resource_name->resource_id_map[i] == 0)                                     \
                continue;                                                                              \
            if (now_delete_len < 1000)                                                                 \
            {                                                                                          \
                delete_buffers[now_delete_len] = (GLuint)resources->resource_name->resource_id_map[i]; \
                now_delete_len += 1;                                                                   \
            }                                                                                          \
            else                                                                                       \
            {                                                                                          \
                resource_delete(now_delete_len, delete_buffers);                                       \
                now_delete_len = 0;                                                                    \
            }                                                                                          \
        }                                                                                              \
        if (now_delete_len != 0)                                                                       \
        {                                                                                              \
            resource_delete(now_delete_len, delete_buffers);                                           \
            now_delete_len = 0;                                                                        \
        }                                                                                              \
        g_free(resources->resource_name->resource_id_map);                                             \
    }

void resource_context_destroy(Resource_Context *resources)
{
    resources->share_resources->counter -= 1;
    if (resources->share_resources->counter == 0)
    {
        GLuint delete_buffers[1000];
        GLuint now_delete_len = 0;
        DESTROY_RESOURCES(texture_resource, glDeleteTextures);
        DESTROY_RESOURCES(buffer_resource, glDeleteBuffers);
        DESTROY_RESOURCES(render_buffer_resource, glDeleteRenderbuffers);
        DESTROY_RESOURCES(sampler_resource, glDeleteSamplers);

        if (resources->shader_resource->resource_id_map != NULL)
        {
            for (int i = 1; i <= resources->shader_resource->max_id; i++)
            {
                if (resources->shader_resource->resource_id_map[i] != 0)
                {
                    glDeleteShader((GLuint)resources->shader_resource->resource_id_map[i]);
                }
            }
            g_free(resources->shader_resource->resource_id_map);
        }

        if (resources->program_resource->resource_id_map != NULL)
        {
            for (int i = 1; i <= resources->program_resource->max_id; i++)
            {
                if (resources->program_resource->resource_id_map[i] != 0)
                {
                    glDeleteProgram((GLuint)resources->program_resource->resource_id_map[i]);
                }
            }
            g_free(resources->program_resource->resource_id_map);
        }

        if (resources->sync_resource->resource_id_map != NULL)
        {
            for (int i = 1; i <= resources->sync_resource->max_id; i++)
            {
                if (resources->sync_resource->resource_id_map[i] != 0)
                {
                    glDeleteSync((GLsync)resources->sync_resource->resource_id_map[i]);
                }
            }
            g_free(resources->sync_resource->resource_id_map);
        }

        g_free(resources->share_resources);
    }

    //下面这些资源不是共享资源，在windows删除后就会释放，所以不去管它
    // DESTROY_RESOURCES(frame_buffer_resource, glDeleteFramebuffers);
    // DESTROY_RESOURCES(program_pipeline_resource, glDeleteProgramPipelines);
    // DESTROY_RESOURCES(transform_feedback_resource, glDeleteTransformFeedbacks);
    // DESTROY_RESOURCES(vertex_array_resource, glDeleteVertexArrays);

    // DESTROY_RESOURCES(query_resource, glDeleteQueries);

    g_free(resources->frame_buffer_resource->resource_id_map);
    g_free(resources->program_pipeline_resource->resource_id_map);
    g_free(resources->transform_feedback_resource->resource_id_map);
    g_free(resources->vertex_array_resource->resource_id_map);
    g_free(resources->query_resource->resource_id_map);

    g_free(resources->exclusive_resources);
}

Opengl_Context *opengl_context_create(Opengl_Context *share_context)
{
    Opengl_Context *opengl_context = g_malloc(sizeof(Opengl_Context));
    opengl_context->is_current = 0;
    opengl_context->need_destroy = 0;

    //要在opengl_context里创建window，因为opengl环境保存在window里
    //send是同步的，发送完消息需要等待消息处理完
    SendMessage(draw_native_window, WM_USER_WINDOW_CREATE, 0, (LPARAM)(&(opengl_context->window)));

    Share_Resources *share_resources = NULL;
    if (share_context != NULL)
    {
        share_resources = share_context->resource_status.share_resources;
    }

    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);

    // opengl_context->pixel_store_status.pack_alignment=4;
    // opengl_context->pixel_store_status.unpack_alignment=4;

    opengl_context->buffer_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_buffer_map_destroy);

    // bound_buffer->vao_status=g_hash_table_new_full(g_direct_hash, g_direct_equal,NULL,g_vao_status_destroy);
    bound_buffer->vao_point_data = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_vao_point_data_destroy);

    // bound_buffer->buffer_type=g_hash_table_new(g_direct_hash, g_direct_equal);

    // Buffer_Status *status=g_malloc(sizeof(Buffer_Status));
    // memset(status,0,sizeof(Buffer_Status));
    // g_hash_table_insert(bound_buffer->vao_status, GINT_TO_POINTER(0), (gpointer)status);

    Attrib_Point *temp_point = g_malloc(sizeof(Attrib_Point));
    memset(temp_point, 0, sizeof(Attrib_Point));

    g_hash_table_insert(bound_buffer->vao_point_data, GINT_TO_POINTER(0), (gpointer)temp_point);

    // bound_buffer->buffer_status=status;
    bound_buffer->attrib_point = temp_point;

    resource_context_init(&(opengl_context->resource_status), share_resources);

    bound_buffer->asyn_unpack_texture_buffer = 0;
    bound_buffer->asyn_pack_texture_buffer = 0;

    bound_buffer->has_init = 0;

    opengl_context->draw_fbo0 = 0;
    opengl_context->read_fbo0 = 0;

    return opengl_context;
}

void opengl_context_init(Opengl_Context *context)
{
    //初始化opengl_context的一些资源，因为这个时候已经makecurrent了
    Bound_Buffer *bound_buffer = &(context->bound_buffer_status);
    if (bound_buffer->has_init == 0)
    {
        //这个has_init也指opengl_context是否已经初始化
        bound_buffer->has_init = 1;
        glGenBuffers(1, &(bound_buffer->asyn_unpack_texture_buffer));
        glGenBuffers(1, &(bound_buffer->asyn_pack_texture_buffer));

        glGenBuffers(1, &(bound_buffer->attrib_point->indices_buffer_object));
        glGenBuffers(MAX_VERTEX_ATTRIBS_NUM, bound_buffer->attrib_point->buffer_object);

        //这两个选项在gles中是默认开启，这样能够在着色器中获取到一些内建变量，所以在gl中要手动开启
        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_POINT_SPRITE);

        //原窗口大小是1*1，所以默认的viewport也是1*1，所以在初始化的时候要手动设置下viewport
        glViewport(context->view_x, context->view_y, context->view_w, context->view_h);

    }
}

/**
 * @brief 销毁opengl_context函数，只能由主窗口线程调用，通过发送WM_USER_CONTEXT_DESTROY消息实现调用
 * 因为销毁的时候肯定没有makecurrent了，就不能调用opengl函数了
 * 
 * @param context 
 */
void opengl_context_destroy(Opengl_Context *context)
{
    express_printf("opengl context destroy %lx\n", context);
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);

    //这三个remove后都有默认的销毁函数
    // g_hash_table_remove_all(opengl_context->buffer_map);
    g_hash_table_destroy(opengl_context->buffer_map);
    // g_hash_table_destroy(bound_buffer->vao_status);
    g_hash_table_destroy(bound_buffer->vao_point_data);

    glDeleteBuffers(1, &(bound_buffer->asyn_unpack_texture_buffer));
    glDeleteBuffers(1, &(bound_buffer->asyn_pack_texture_buffer));

    resource_context_destroy(&(opengl_context->resource_status));
}

//下面这三个函数都是销毁函数，不提供外部调用，只用来给g_hash_table_new_full用
static void g_buffer_map_destroy(gpointer data)
{
    express_printf("buffer_map destroy\n");
    Guest_Host_Map *map_res = (Guest_Host_Map *)data;
    g_free(map_res);
}

// static void g_vao_status_destroy(gpointer data)
// {
//     express_printf("vao_status destroy\n");

//     Buffer_Status *vao_status = (Buffer_Status *)data;
//     g_free(vao_status);
// }

static void g_vao_point_data_destroy(gpointer data)
{
    Attrib_Point *vao_point = (Attrib_Point *)data;

    glDeleteBuffers(1, &(vao_point->indices_buffer_object));
    glDeleteBuffers(MAX_VERTEX_ATTRIBS_NUM, vao_point->buffer_object);

    express_printf("vao_point destroy\n");

    // GLuint buffer_index[2];
    // int t=0;
    // if(vao_point->indices_buffer_object!=0){
    //     buffer_index[t]=vao_point->indices_buffer_object;
    //     t++;
    // }
    // if(vao_point->buffer_object){
    //     buffer_index[t]=vao_point->buffer_object;
    //     t++;
    // }
    // if(t!=0){
    //     glDeleteBuffers(t,buffer_index);
    // }

    // for(int i=0;i<32;i++){
    //     if(vao_point->data[i]!=NULL){
    //         g_free(vao_point->data[i]);
    //     }
    // }

    g_free(vao_point);
}

void glTestIntAsyn(GLint a, GLuint b, GLfloat c, GLdouble d)
{
    printf("glTestInt asyn %d,%u,%f,%lf\n", a, b, c, d);
    fflush(stdout);
}

void glPrintfAsyn(GLint a, GLuint size, GLdouble c, const GLchar *out_string)
{

    printf("glPrintfAsyn asyn string %d,%u,%lf,%s\n", a, size, c, out_string);
    return;
}

GLint glTestInt1(GLint a, GLuint b)
{
    express_printf("glTestInt1 %d,%u\n", a, b);
    //fflush(stdout);
    return 576634565;
}
GLuint glTestInt2(GLint a, GLuint b)
{
    printf("glTestInt2 %d,%u\n", a, b);
    fflush(stdout);
    return 4000001200u;
}

GLint64 glTestInt3(GLint64 a, GLuint64 b)
{
    printf("glTestInt3 %lld,%llu\n", a, b);
    fflush(stdout);
    return 453489431344456;
}
GLuint64 glTestInt4(GLint64 a, GLuint64 b)
{
    printf("glTestInt4 %lld,%llu\n", a, b);
    fflush(stdout);
    return 436004354364364345;
}

GLfloat glTestInt5(GLint a, GLuint b)
{
    printf("glTestInt5 %d,%u\n", a, b);
    fflush(stdout);
    return 3.1415926;
}
GLdouble glTestInt6(GLint a, GLuint b)
{
    printf("glTestInt6 %d,%u\n", a, b);
    fflush(stdout);
    return 3.1415926535;
}

void glTestPointer1(GLint a, const GLint *b)
{
    printf("glTestPointer1 %d ", a);
    for (int i = 0; i < 10; i++)
    {
        printf("%d ", b[i]);
    }
    printf("\n");
    fflush(stdout);
    return;
}

void glTestPointer2(GLint a, const GLint *b, GLint *c)
{
    printf("glTestPointer2 %d %d\n", a, *b);
    for (int i = 0; i < 10; i++)
    {
        c[i] = b[i];
    }
    fflush(stdout);
    return;
}

GLint glTestPointer4(GLint a, const GLint *b, GLint *c)
{
    printf("glTestPointer4 %d,%d\n", a, *b);
    for (int i = 0; i < 1000; i++)
    {
        c[i] = b[i];
    }
    fflush(stdout);
    return 12456687;
}

GLint d_glTestPointer3(void *context, GLint a, const GLint *b, GLint *c)
{

    int len;
    char *temp = g_malloc(a * sizeof(int));
    memset(temp, 0, a * sizeof(int));
    printf("glTestPointer3 %d\n", a);
    guest_write((Guest_Mem *)b, temp, 0, a * sizeof(int));

    char *temp_s[100];
    int loc = 0;
    for (int i = a / 2; i < a / 2 + 10; i++)
    {
        loc += sprintf(temp_s + loc, "%d ", temp[i]);
    }
    printf("glTestPointer3 %s\n", temp_s);

    guest_read((Guest_Mem *)c, temp, 0, a * sizeof(int));

    fflush(stdout);
    return 12456687;
}

void glTestString(GLint a, GLint count, const GLchar *const *strings, GLint buf_len, GLchar *char_buf)
{
    printf("glTestString %d %d %d\nString:\n", a, count, buf_len);
    for (int i = 0; i < count; i++)
    {
        printf("%s\n", strings[i]);
    }
    char *t = "printf ok!";
    memcpy(char_buf, t, strlen(t));
    fflush(stdout);
}

void d_glPrintf(void *context, GLint buf_len, const GLchar *out_string)
{
    // char *t="temp test abcd";
    // memcpy(out_string,t,strlen(t));
    char *temp = g_malloc(buf_len);
    guest_write((Guest_Mem *)out_string, temp, 0, buf_len);

    if (buf_len < 100)
    {
        printf("glPrintf %d %s\n", buf_len, temp);
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
            printf("glPrintf check error!\n");
        }
        else
        {
            printf("glPrintf check ok!\n");
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
    // printf("glInOutTest %d,%d   buf_len%llu\n",a,b,buf_len);
    // *c=78646313;
    // *d=3.141592653543;

    // char *temp;

    // char *t="glInOutTest printf ok! test ok!";
    // memcpy(f,t,strlen(t));
    // fflush(stdout);
}
