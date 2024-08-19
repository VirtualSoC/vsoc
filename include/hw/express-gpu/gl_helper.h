#ifndef GL_HELPER_H
#define GL_HELPER_H

#include "hw/teleport-express/express_device_common.h"
#include "hw/teleport-express/express_log.h"
#include "glad/glad.h"


typedef int (*Dying_Function)(void *data);

typedef struct Dying_List_Node{
    void *data;
    struct Dying_List_Node *next;
    struct Dying_List_Node *prev;
} Dying_List_Node;


typedef struct Dying_List{
    Dying_List_Node *header;
    Dying_List_Node *tail;
    int num;

} Dying_List;


//注意顺序，保证不影响结构体对齐
typedef struct Static_Context_Values
{
    GLuint composer_HZ;
    GLuint composer_pid;
    GLint num_extensions;
    //____________ FIXED VALUE ____________
    GLint major_version;
    GLint minor_version;
    GLint implementation_color_read_format;
    GLint implementation_color_read_type;
    GLint max_array_texture_layers;
    GLint max_color_attachments;
    GLint max_combined_uniform_blocks;
    GLint max_draw_buffers;
    GLint max_fragment_input_components;
    GLint max_fragment_uniform_blocks;
    GLint max_program_texel_offset;
    GLint max_transform_feedback_interleaved_components;
    GLint max_transform_feedback_separate_attribs;
    GLint max_transform_feedback_separate_components;
    GLint max_uniform_buffer_bindings;
    GLint max_varying_components;
    GLint max_varying_vectors;
    GLint max_vertex_output_components;
    GLint max_vertex_uniform_blocks;
    GLint min_program_texel_offset;
    GLint num_program_binary_formats;
    //____________ QUERY HOST ___________
    GLint subpixel_bits;
    GLint num_compressed_texture_formats;
    GLint compressed_texture_formats[128];
    GLint max_3d_texture_size;
    GLint max_texture_size;
    GLint max_combined_texture_image_units;
    GLint max_cube_map_texture_size;
    GLint max_elements_vertices;
    GLint max_elements_indices;
    GLint max_fragment_uniform_components;
    GLint max_fragment_uniform_vectors;
    GLint max_renderbuffer_size;
    GLint max_vertex_attribs;
    GLint max_image_units;
    GLint max_vertex_attrib_bindings;
    GLint max_compute_uniform_blocks;
    GLint max_compute_texture_image_units;
    GLint max_compute_image_uniforms;
    GLint max_compute_shared_memory_size;
    GLint max_compute_uniform_components;
    GLint max_compute_atomic_counter_buffers;
    GLint max_compute_atomic_counters;
    GLint max_combined_compute_uniform_components;
    GLint max_compute_work_group_invocations;
    GLint max_compute_work_group_count[3];
    GLint max_compute_work_group_size[3];
    GLint max_uniform_locations;
    GLint max_framebuffer_width;
    GLint max_framebuffer_height;
    GLint max_framebuffer_samples;
    GLint max_vertex_atomic_counter_buffers;
    GLint max_fragment_atomic_counter_buffers;
    GLint max_combined_atomic_counter_buffers;
    GLint max_vertex_atomic_counters;
    GLint max_fragment_atomic_counters;
    GLint max_combined_atomic_counters;
    GLint max_atomic_counter_buffer_size;
    GLint max_atomic_counter_buffer_bindings;
    GLint max_vertex_image_uniforms;
    GLint max_fragment_image_uniforms;
    GLint max_combined_image_uniforms;
    GLint max_vertex_shader_storage_blocks;
    GLint max_fragment_shader_storage_blocks;
    GLint max_compute_shader_storage_blocks;
    GLint max_combined_shader_storage_blocks;
    GLint max_shader_storage_buffer_bindings;
    GLint max_shader_storage_block_size;
    GLint max_combined_shader_output_resources;
    GLint min_program_texture_gather_offset;
    GLint max_program_texture_gather_offset;
    GLint max_sample_mask_words;
    GLint max_color_texture_samples;
    GLint max_depth_texture_samples;
    GLint max_integer_samples;
    GLint max_vertex_attrib_relative_offset;
    GLint max_vertex_attrib_stride;
    GLint max_vertex_texture_image_units;
    GLint max_vertex_uniform_components;
    GLint max_vertex_uniform_vectors;
    GLint max_viewport_dims[2];
    GLint max_samples;
    GLint texture_image_units;
    GLint uniform_buffer_offset_alignment;
    GLint max_texture_anisotropy;

    GLint num_shader_binary_formats;
    GLint program_binary_formats[8];
    GLint shader_binary_formats[8];

    // geometry shaders
    GLenum layer_provoking_vertex;
    GLint max_geometry_uniform_components;
    GLint max_geometry_uniform_blocks;
    GLint max_combined_geometry_uniform_components;
    GLint max_geometry_input_components;
    GLint max_geometry_output_components;
    GLint max_geometry_output_vertices;
    GLint max_geometry_total_output_components;
    GLint max_geometry_shader_invocations;
    GLint max_geometry_texture_image_units;
    GLint max_geometry_atomic_counter_buffers;
    GLint max_geometry_atomic_counters;
    GLint max_geometry_image_uniforms;
    GLint max_geometry_shader_storage_blocks;
    GLint max_framebuffer_layers;

    // tessellation shaders
    GLint max_patch_vertices;
    GLint max_tess_gen_level;
    GLint max_tess_control_uniform_components;
    GLint max_tess_evaluation_uniform_components;
    GLint max_tess_control_texture_image_units;
    GLint max_tess_evaluation_texture_image_units;
    GLint max_tess_control_output_components;
    GLint max_tess_patch_components;
    GLint max_tess_control_total_output_components;
    GLint max_tess_evaluation_output_components;
    GLint max_tess_control_uniform_blocks;
    GLint max_tess_evaluation_uniform_blocks;
    GLint max_tess_control_input_components;
    GLint max_tess_evaluation_input_components;
    GLint max_combined_tess_control_uniform_components;
    GLint max_combined_tess_evaluation_uniform_components;
    GLint max_tess_control_atomic_counter_buffers;
    GLint max_tess_evaluation_atomic_counter_buffers;
    GLint max_tess_control_atomic_counters;
    GLint max_tess_evaluation_atomic_counters;
    GLint max_tess_control_image_uniforms;
    GLint max_tess_evaluation_image_uniforms;
    GLint max_tess_control_shader_storage_blocks;
    GLint max_tess_evaluation_shader_storage_blocks;
    GLboolean primitive_restart_for_patches_supported;

    // GL_KHR_debug
    GLint max_debug_message_length;
    GLint max_debug_logged_messages;
    GLint max_debug_group_stack_depth;
    GLint max_label_length;

    GLfloat aliased_line_width_range[2];
    GLfloat aliased_point_size_range[2];
    GLfloat smooth_line_width_range[2];
    GLfloat smooth_line_width_granularity;
    GLfloat max_texture_log_bias;
    GLint64 max_element_index;
    GLint64 max_server_wait_timeout;
    GLint64 max_combined_fragment_uniform_components;
    GLint64 max_combined_vertex_uniform_components;
    GLint64 max_uniform_block_size;
    GLfloat min_fragment_interpolation_offset;
    GLfloat max_fragment_interpolation_offset;
    GLint64 fragment_interpolation_offset_bits;

    //下面实际要作为指针使用，保证与32位应用的兼容性，所以要这样弄
    GLuint64 vendor;
    GLuint64 version;
    GLuint64 renderer;
    GLuint64 shading_language_version;
    GLuint64 extensions_gles2;
    GLuint64 extensions[512];
    //大概需要512*100+400左右的空间存放字符串，这么大的空间应该是够了

} __attribute__((packed, aligned(1))) Static_Context_Values;


typedef enum {
    BLEND_NONE = 0,         /**< No blending */
    BLEND_CLEAR,            /**< CLEAR blending */
    BLEND_SRC,              /**< SRC blending */
    BLEND_SRCOVER,          /**< SRC_OVER blending */
    BLEND_DSTOVER,          /**< DST_OVER blending */
    BLEND_SRCIN,            /**< SRC_IN blending */
    BLEND_DSTIN,            /**< DST_IN blending */
    BLEND_SRCOUT,           /**< SRC_OUT blending */
    BLEND_DSTOUT,           /**< DST_OUT blending */
    BLEND_SRCATOP,          /**< SRC_ATOP blending */
    BLEND_DSTATOP,          /**< DST_ATOP blending */
    BLEND_ADD,              /**< ADD blending */
    BLEND_XOR,              /**< XOR blending */
    BLEND_DST,              /**< DST blending */
    BLEND_AKS,              /**< AKS blending */
    BLEND_AKD,              /**< AKD blending */
    BLEND_BUTT              /**< Null operation */
} BlendType;

typedef enum {
    ROTATE_NONE = 0,        /**< No rotation */
    ROTATE_90,              /**< Rotation by 90 degrees */
    ROTATE_180,             /**< Rotation by 180 degrees */
    ROTATE_270,             /**< Rotation by 270 degrees */
    FLIP_H,             
    FLIP_V,            
    FLIP_H_ROT,             
    FLIP_V_ROT,             


    ROTATE_BUTT             /**< Invalid operation */
} TransformType;

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

void prepare_integer_value(Static_Context_Values *s_values);

GLuint load_shader(GLenum type, const char *shaderSrc);

int main_window_opengl_prepare(GLuint *program, GLuint *VAO);

void adjust_blend_type(int blend_type);

Dying_List *dying_list_append(Dying_List *list, void *data);

Dying_List *dying_list_foreach(Dying_List *list, Dying_Function fun);

Dying_List *dying_list_remove(Dying_List *list, void *data);


void APIENTRY gl_debug_output(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);


void glTestIntAsyn(GLint a, GLuint b, GLfloat c, GLdouble d);

void glPrintfAsyn(GLint a, GLuint size, GLdouble c, const GLchar *out_string);

GLint glTestInt1(GLint a, GLuint b);
GLuint glTestInt2(GLint a, GLuint b);

GLint64 glTestInt3(GLint64 a, GLuint64 b);
GLuint64 glTestInt4(GLint64 a, GLuint64 b);

GLfloat glTestInt5(GLint a, GLuint b);
GLdouble glTestInt6(GLint a, GLuint b);

void glTestPointer1(GLint a, const GLint *b);

void glTestPointer2(GLint a, const GLint *b, GLint *c);

GLint d_glTestPointer3(void *context, GLint a, const GLint *b, GLint *c);

GLint glTestPointer4(GLint a, const GLint *b, GLint *c);

void glTestString(GLint a, GLint count, const GLchar *const *strings, GLint buf_len, GLchar *char_buf);

void d_glPrintf(void *context, GLint buf_len, const GLchar *out_string);

void d_glInOutTest(void *context, GLint a, GLint b, const GLchar *e, GLint *c, GLdouble *d, GLsizei buf_len, GLchar *f);

#endif