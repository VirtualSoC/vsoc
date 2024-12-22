// #include "hw/express-gpu/express_gpu_snapshot.h"
// #include "migration/qemu-file.h"
// #include <stdint.h>
// #include "migration/qemu-file-types.h"
// #include "migration/qemu-file.h"
// #include "exec/cpu-common.h"
#include "hw/teleport-express/teleport_express.h"

#include "hw/teleport-express/teleport_express_call.h"
#include "hw/teleport-express/teleport_express_distribute.h"
#include "hw/teleport-express/teleport_express_register.h"

#include "hw/teleport-express/express_log.h"

#include "hw/virtio/virtio.h"

#include "hw/express-gpu/express_gpu.h"
#include "hw/express-gpu/egl_trans.h"
#include "hw/express-gpu/express_gpu_snapshot.h"
#include "hw/express-gpu/glv3_context.h"
#include "hw/teleport-express/express_handle_thread.h"

#include "migration/qemu-file.h"



//ztodo: 这些资源也得实现保存和加载
// GList *native_shaders = NULL;
// int native_shaders_num = 0;
// int native_shaders_locker = 0;
GHashTable *g_resource_list[NUM_RESOURCES];// = { NULL };
// int g_resource_count[NUM_RESOURCES] = { 0 };
int g_resource_locker[NUM_RESOURCES] = { 0 }; 


//hashtable肯定不是效率最高的，先这样吧.直接用数组肯定是最快的
static GHashTable *g_resource_ids_map[NUM_RESOURCES] = { NULL };


static __thread void *g_gl_context = NULL;
static bool is_init = false;

void init_saving_snapshot() {
    if (is_init) {
        return;
    }
    is_init = true;
    g_gl_context = get_native_opengl_context(0);
    egl_makeCurrent(g_gl_context);
    for (int i = 0; i < NUM_RESOURCES; i++) {
        g_resource_ids_map[i] = g_hash_table_new(g_direct_hash, g_direct_equal);
        // g_resource_list[i] = g_hash_table_new(g_direct_hash, g_direct_equal);
    }
}

void clear_resource_tables() {
    for (int i = 0; i < NUM_RESOURCES; i++) {
        if (g_resource_ids_map[i] != NULL) {
            g_hash_table_remove_all(g_resource_ids_map[i]);
        }
    }
}

void save_native_resources(QEMUFile *f){
    save_native_shaders(f);
    save_native_programs(f);
    save_native_textures(f);
}

void load_native_resources(QEMUFile *f){
    load_native_shaders(f);
    load_native_programs(f);
    load_native_textures(f);
}

// GLboolean compare_shader(GLint shader_id, GLenum shader_type, GLboolean deleted_status, GLboolean compile_status, GLint source_length, char* shader_source) {
//     //ztodo:如果已经删除了就不要直接重建了！以及好像有时候相同会被误判(或者就是因为已经被删除了)
//     GLint currentType = 0;
//     GLboolean currentDeleteStatus = GL_FALSE;
//     GLboolean currentCompileStatus = GL_FALSE;
//     GLint currentSourceLength = 0;
//     char* currentSource = NULL;
//     LOGD("going to compare shader of id %d", shader_id);
//     glGetShaderiv(shader_id, GL_SHADER_TYPE, &currentType);
//     glGetShaderiv(shader_id, GL_DELETE_STATUS, (GLint*)&currentDeleteStatus);
//     glGetShaderiv(shader_id, GL_COMPILE_STATUS, (GLint*)&currentCompileStatus);
//     glGetShaderiv(shader_id, GL_SHADER_SOURCE_LENGTH, &currentSourceLength);
//     LOGI("in compare shader of type %d status %d %d length %d", currentType, currentDeleteStatus, currentCompileStatus, currentSourceLength);
//     if (currentSourceLength > 0) {
//         currentSource = (char*)malloc(currentSourceLength + 1);
//         if (currentSource) {
//             glGetShaderSource(shader_id, currentSourceLength, NULL, currentSource);
//         }
//     }
//     LOGI("in compare shader of content %s %s", currentSource, shader_source);
//     GLboolean isSame = (currentDeleteStatus == deleted_status) &&
//                     //    (currentType == shader_type) &&
//                        (currentCompileStatus == compile_status) &&
//                        (currentSourceLength == source_length) &&
//                        (currentSource != NULL && strcmp(currentSource, shader_source) == 0);

//     free(currentSource);
//     LOGI("compare result %d", isSame);
//     return isSame;
// }

void change_host_id_map(int type, GLint old_id, GLint new_id){ //ztodo:记得每次load snapshot结束之后清空哈希表！
    g_hash_table_insert(g_resource_ids_map[type], GUINT_TO_POINTER(old_id), GUINT_TO_POINTER(new_id));
    return;
}

GLint get_host_id_map(int type, GLint old_id) {
    GLint new_id = (GLint)g_hash_table_lookup(g_resource_ids_map[type], GUINT_TO_POINTER(old_id));
    return new_id;
}

void save_native_shaders(QEMUFile *f) {
    ATOMIC_LOCK(g_resource_locker[RESOURCE_TYPE_SHADER]);
    GHashTable* resource_list = g_resource_list[RESOURCE_TYPE_SHADER];
    qemu_put_be32(f, g_hash_table_size(resource_list)); //first save how many shaders
    LOGD("saving shader num %d", g_hash_table_size(resource_list));

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, resource_list);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Express_Native_Shader* shader = (Express_Native_Shader *)value;
        LOGD("saving shader ID: %d, Type: %d, Delete Status: %d", shader->id, shader->type, shader->deleteStatus);
        qemu_put_be64(f, shader->id);
        qemu_put_be32(f, shader->type);
        qemu_put_byte(f, shader->deleteStatus);
        GLint compile_status;
        glGetShaderiv(shader->id, GL_COMPILE_STATUS, &compile_status);

        qemu_put_byte(f, compile_status); //ztodo:byte可以吗
        GLint sourceLength;
        glGetShaderiv(shader->id, GL_SHADER_SOURCE_LENGTH, &sourceLength);

        qemu_put_be32(f, sourceLength);

        char* shader_source;
        if (sourceLength > 0) {
            shader_source = (char*)g_malloc0(sourceLength);
            if (shader_source) {
                glGetShaderSource(shader->id, sourceLength, NULL, shader_source);
                LOGD("saving shader of content length %d %d %s", sourceLength, sizeof(shader_source), shader_source);
                qemu_put_buffer(f, shader_source, sourceLength);
            }
            g_free(shader_source); 
        } //ztodo:如果是0，load之前判断一下
    }

    ATOMIC_UNLOCK(g_resource_locker[RESOURCE_TYPE_SHADER]);
}

void load_native_programs(QEMUFile *f){  //ztodo:应该先重新创建program、更新id！

    GHashTable* programs = g_hash_table_new(g_direct_hash, g_direct_equal);

    int program_num = qemu_get_be32(f);
    for(int i = 0; i < program_num ; i++) {
        uint64_t program_id = qemu_get_be64(f);

        glDeleteProgram(program_id);//ztodo:重启场景应该可以去掉这个
        GLint new_program_id = glCreateProgram();
        change_host_id_map(RESOURCE_TYPE_PROGRAM, program_id, new_program_id); //重新映射guest-host的id


        GLint linked = (GLint)qemu_get_byte(f);
        g_hash_table_insert(programs, GUINT_TO_POINTER(new_program_id), GUINT_TO_POINTER(program_id));

        // if(linked) {
        //     glLinkProgram(program_id);
        // } 
        LOGI("loading program of %lld %lld", program_id, new_program_id); //ztodo:id重用了之前被删除的，感觉没问题，但不确定！作为一个可能bug点
    }

    int linked_program_num = qemu_get_be32(f);   
    LOGD("in load native programs! num %d", linked_program_num);

    for (int i = 0; i < linked_program_num; i++) {
        uint64_t linked_program = qemu_get_be64(f);
        uint64_t program_id = (linked_program >> 32) & ((1 << 32) - 1); //取高32位
        GLint new_program_id = get_host_id_map(RESOURCE_TYPE_PROGRAM, program_id);
        uint64_t shader_id = linked_program & ((1 << 32) - 1); //取低32位
        glAttachShader(new_program_id, shader_id);
        LOGI("attaching program of %lld", new_program_id);

    }

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, programs); //最后link
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        uint64_t program_id = (uint64_t)key;
        GLint status = (GLint)value;
        if(status) {
            glLinkProgram(program_id);
        } 

        LOGI("linking program of id %lld status %d", program_id, status);
    }

}

void save_native_programs(QEMUFile *f) {
    ATOMIC_LOCK(g_resource_locker[RESOURCE_TYPE_PROGRAM]);
    GHashTable* resource_list = g_resource_list[RESOURCE_TYPE_PROGRAM];
    // qemu_put_be32(f, g_hash_table_size(resource_list)); //first save how many shaders
    // LOGI("saving program num %d", g_hash_table_size(resource_list));

    GHashTableIter iter;
    gpointer key, value;
    
    GHashTable* programs = g_hash_table_new(g_direct_hash, g_direct_equal);
    g_hash_table_iter_init(&iter, resource_list);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        uint64_t linked_program = (uint64_t)key;
        uint64_t program_id = (linked_program >> 32) & ((1 << 32) - 1); //取高32位
        // LOGI("saving program ID:%lld %d, shader: %d",linked_program, (linked_program >> 32) & ((1 << 32) - 1), linked_program & ((1 << 32) - 1));
        // qemu_put_be64(f, linked_program);

        GLint linkStatus = 0;
        glGetProgramiv(program_id, GL_LINK_STATUS, &linkStatus);

        g_hash_table_insert(programs, GUINT_TO_POINTER(program_id), GUINT_TO_POINTER(linkStatus));

    }

    g_hash_table_iter_init(&iter, programs); //先单独存program，因为要重建
    qemu_put_be32(f, g_hash_table_size(programs));
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        uint64_t program_id = (uint64_t)key;
        GLint status = (GLint)value;
        qemu_put_be64(f, program_id);
        qemu_put_byte(f, status);
        LOGD("saving program of id %lld status %d", program_id, status);
    }

    qemu_put_be32(f, g_hash_table_size(resource_list)); //first save how many shaders
    LOGD("saving program num %d", g_hash_table_size(resource_list));
    g_hash_table_iter_init(&iter, resource_list);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        uint64_t linked_program = (uint64_t)key;
        qemu_put_be64(f, linked_program);

    }

    ATOMIC_UNLOCK(g_resource_locker[RESOURCE_TYPE_SHADER]);
}

void update_native_shader(GLint shader_id, GLenum shader_type, GLboolean deleted_status, GLboolean compile_status, GLint source_length, const char* shader_source) {

    glDeleteShader(shader_id);


    GLint new_shader_id = glCreateShader(shader_type);
    LOGI("create new shader of new id %d old id %d", new_shader_id, shader_id);
    glShaderSource(new_shader_id, 1, &shader_source, NULL);
    LOGD("going to compile shader!");
    if(compile_status) {
        
        glCompileShader(new_shader_id);

        GLint compileStatus;
        glGetShaderiv(new_shader_id, GL_COMPILE_STATUS, &compileStatus);
        if (compileStatus != GL_TRUE) {
            GLint logLength = 0;
            glGetShaderiv(new_shader_id, GL_INFO_LOG_LENGTH, &logLength);
            char* log = (char*)malloc(logLength);
            glGetShaderInfoLog(new_shader_id, logLength, &logLength, log);
            LOGE("Shader compile failed in saving snapshot: %s %s", log, shader_source);
            free(log);
        }        
    }
    LOGD("change when load shader from %d to %d", shader_id, new_shader_id);
    change_host_id_map(RESOURCE_TYPE_SHADER, shader_id, new_shader_id); //ztodo: 重新映射guest-host的id
}

void load_native_shaders(QEMUFile *f) {
    // 没有办法在保留原来id的情况下改属性的值，这样的话就得新建然后重新映射host id
    int shader_num = qemu_get_be32(f);   
    LOGD("in load native shaders! num %d", shader_num);

    for (int i = 0; i < shader_num; i++) {
        GLint shader_id = qemu_get_be64(f);
        GLenum shader_type = qemu_get_be32(f);
        GLboolean deleted_status = qemu_get_byte(f);
        GLboolean compile_status = qemu_get_byte(f);
        GLint source_length = qemu_get_be32(f);
        const char* shader_source = (char*)malloc(source_length);
        qemu_get_buffer(f, shader_source, source_length);
        LOGD("loading shader of id %d type %d content %s", shader_id, shader_type, shader_source);
        // bool isSame = compare_shader(shader_id, shader_type, deleted_status, compile_status, source_length, shader_source);
        // if(isSame) { //如果没变就不操作了 update:不再复用资源，全部重建
        //     continue;
        // }
        update_native_shader(shader_id, shader_type, deleted_status, compile_status, source_length, shader_source);
    }
}

#include <stdio.h>
#include <stdlib.h>

void saveTextureAsPPM(const char *baseFilename, int textureId, int width, int height, unsigned char *pixels) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s_%d.ppm", baseFilename, textureId);

    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }

    fprintf(file, "P6\n%d %d\n255\n", width, height);

    for (int i = 0; i < width * height; ++i) {
        fwrite(&pixels[i * 4], 1, 3, file); //只写入 RGB 三个字节 跳过 A 通道
    }


    fclose(file);
    LOGI("Saved texture to %s\n", filename);

    LOGI("First pixels in RGBA format:");
    for (int i = 0; i < width * height; i++) {
        LOGI("Pixel %d: R=%d, G=%d, B=%d, A=%d\n",
               i, 
               pixels[i * 4 + 0], //R
               pixels[i * 4 + 1], //G
               pixels[i * 4 + 2], //B
               pixels[i * 4 + 3]  //A
        );
    }
}

GLenum get_format_for_internal_format(GLint internal_format) {
    GLenum format; //ztodo:不确定是否齐全
    switch (internal_format) {
        case GL_ALPHA:
            format = GL_ALPHA;
            break;
        case GL_RGBA8:
            format = GL_RGBA;
            break;
        case GL_SRGB8_ALPHA8_EXT:
            format = GL_RGBA;
            break;
        case GL_R8:
            format = GL_RED;
            break;
        case GL_RG8:
            format = GL_RG;
            break;
        case GL_RGB8:
            format = GL_RGB;
            break;
        case GL_RGBA32F:
            format = GL_RGBA;
            break;
        case GL_RGB32F:
            format = GL_RGB;
            break;
        case GL_R16F:
            format = GL_RED;
            break;
        case GL_RG16F:
            format = GL_RG;
            break;
        case GL_R8I:
            format = GL_RED;
            break;
        case GL_RG8I:
            format = GL_RG;
            break;
        case GL_RGBA8I:
            format = GL_RGBA;
            break;
        case GL_R16I:
            format = GL_RED;
            break;
        case GL_RG16I:
            format = GL_RG;
            break;
        case GL_RGB16F:
            format = GL_RGB;
            break;
        case GL_RGBA16F:
            format = GL_RGBA;
            break;
        case GL_SRGB:
            format = GL_RGB;
            break;
        case GL_SRGB_ALPHA:
            format = GL_RGBA;
            break;
        case GL_DEPTH_COMPONENT:
            format = GL_DEPTH_COMPONENT;
            break;
        case GL_DEPTH_STENCIL:
            format = GL_DEPTH_STENCIL;
            break;
        default:
            format = GL_RGBA;
            break;
    }
    return format;
}

void save_single_texture(QEMUFile *f, GLint texture_id, GLenum texture_type) {
    Express_Native_Texture* state = g_malloc0(sizeof(Express_Native_Texture));

    state->textureId = texture_id;
    state->target = texture_type;
    

    GLenum target = state->target;
    if(target == GL_TEXTURE_EXTERNAL_OES) {
        target = GL_TEXTURE_2D;
    }


    glBindTexture(target, state->textureId);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);


    glGetTexLevelParameteriv(target, 0, GL_TEXTURE_WIDTH, &state->width);
    glGetTexLevelParameteriv(target, 0, GL_TEXTURE_HEIGHT, &state->height);
    if (target == GL_TEXTURE_3D) {
        glGetTexLevelParameteriv(target, 0, GL_TEXTURE_DEPTH, &state->depth);
    } else {
        state->depth = 0;
    }
    glGetTexLevelParameteriv(target, 0, GL_TEXTURE_INTERNAL_FORMAT, &state->internalFormat);

    glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&state->binding2D);

    glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, (GLint *)&state->bindingCubeMap);

    glGetTexParameteriv(target, GL_TEXTURE_MIN_FILTER, &state->minFilter);
    glGetTexParameteriv(target, GL_TEXTURE_MAG_FILTER, &state->magFilter);
    glGetTexParameteriv(target, GL_TEXTURE_WRAP_S, &state->wrapS);
    glGetTexParameteriv(target, GL_TEXTURE_WRAP_T, &state->wrapT);

    qemu_put_be32(f, state->width);
    qemu_put_be32(f, state->height);
    qemu_put_be32(f, state->depth);
    qemu_put_be32(f, state->internalFormat);

    qemu_put_be32(f, state->minFilter);
    qemu_put_be32(f, state->magFilter);
    qemu_put_be32(f, state->wrapS);
    qemu_put_be32(f, state->wrapT);

    qemu_put_be32(f, state->binding2D);
    qemu_put_be32(f, state->bindingCubeMap);
    
    GLint size = state->width * state->height * 4;
    state->pixels = (GLubyte *)g_malloc(size); //ztodo:记得free
    memset(state->pixels, 0, size);

    //ztodo:使用pbo加速!!!


    GLenum format = get_format_for_internal_format(state->internalFormat);

    glGetTexImage(target, 0, format, GL_UNSIGNED_BYTE, state->pixels); //ztodo:第二个、倒数第二个参数
    LOGI("in saving texture of id %d target %d height %d width %d depth %d format %d pixels", texture_id, state->target, state->height, state->width, state->depth, state->internalFormat);
    qemu_put_buffer(f, state->pixels, size);    

    // unsigned char* zero_buffer = (unsigned char*)malloc(size);
    // memset(zero_buffer, 0, size);
    // if (memcmp(state->pixels, (const void*)zero_buffer, size) == 0) {
    // // 说明 state->pixels 的内容全为 0
    //     LOGI("Memory is all zeros");
    // } else {
    //     // 说明 state->pixels 的内容不是全为 0
    //     LOGI("Memory is not all zeros");
    //     for (int i = 0; i < min(30, size); i++) {
    //         LOGI("unpack texture %d: R=%d, G=%d, B=%d, A=%d\n",
    //             i, 
    //             state->pixels[i * 4 + 0], //R
    //             state->pixels[i * 4 + 1], //G
    //             state->pixels[i * 4 + 2], //B
    //             state->pixels[i * 4 + 3]  //A
    //         );
    //     }
    // }
    // free(zero_buffer);




//zodo:pbo加速相关代码
    // GLuint downloadPboId;
    // glGenBuffers(1, &downloadPboId);
    // glBindBuffer(GL_PIXEL_PACK_BUFFER, downloadPboId);
    // glBufferData(GL_PIXEL_PACK_BUFFER, dataSize, NULL, GL_STREAM_READ);
    // glGetTexImage(state->target, 0, state->internalFormat, GL_UNSIGNED_BYTE, 0);
    // GLubyte* downloadPtr = (GLubyte*)glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, dataSize, GL_MAP_READ_BIT);
    // if(downloadPtr) {
    //     memcpy(state->pixels, downloadPtr, dataSize);
    //     glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    // }
    // glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    // glDeleteBuffers(1, &downloadPboId);



}

void save_native_textures(QEMUFile *f){
    ATOMIC_LOCK(g_resource_locker[RESOURCE_TYPE_TEXTURE]);
    
    GHashTable* resource_list = g_resource_list[RESOURCE_TYPE_TEXTURE];

    qemu_put_be32(f, g_hash_table_size(resource_list));
    LOGI("saving texture num %d", g_hash_table_size(resource_list));

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, resource_list);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Express_Native_Texture_Simple* texture = (Express_Native_Texture_Simple *)value;
        qemu_put_be64(f, texture->textureId);
        qemu_put_be32(f, texture->target);
        save_single_texture(f, texture->textureId, texture->target);        
        g_free(texture);
        
    }
    ATOMIC_UNLOCK(g_resource_locker[RESOURCE_TYPE_TEXTURE]);
}


void update_native_texture(Express_Native_Texture* texture_data){
    GLint new_texture_id;
    glGenTextures(1, &new_texture_id);
    glBindTexture(texture_data->target, new_texture_id);

    glTexParameteri(texture_data->target, GL_TEXTURE_MIN_FILTER, texture_data->minFilter);
    glTexParameteri(texture_data->target, GL_TEXTURE_MAG_FILTER, texture_data->magFilter);
    glTexParameteri(texture_data->target, GL_TEXTURE_WRAP_S, texture_data->wrapS);
    glTexParameteri(texture_data->target, GL_TEXTURE_WRAP_T, texture_data->wrapT);
    
    GLenum format = get_format_for_internal_format(texture_data->internalFormat); //ztodo:这么转换吗？


    if (texture_data->target == GL_TEXTURE_3D || texture_data->target == GL_TEXTURE_CUBE_MAP) {
        glTexParameteri(texture_data->target, GL_TEXTURE_WRAP_R, texture_data->wrapS);
    }

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    if (texture_data->target == GL_TEXTURE_2D || texture_data->target == GL_TEXTURE_EXTERNAL_OES) {
        glTexImage2D(GL_TEXTURE_2D, 0, texture_data->internalFormat, texture_data->width, texture_data->height, 0, format, GL_UNSIGNED_BYTE, texture_data->pixels);
    } else if (texture_data->target == GL_TEXTURE_3D) {
        glTexImage3D(texture_data->target, 0, texture_data->internalFormat, texture_data->width, texture_data->height, texture_data->depth, 0, format, GL_UNSIGNED_BYTE, texture_data->pixels);
    } else if (texture_data->target == GL_TEXTURE_CUBE_MAP) {
        for (GLuint i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, texture_data->internalFormat, texture_data->width, texture_data->height, 0, format, GL_UNSIGNED_BYTE, texture_data->pixels);
        }
    }

    if (texture_data->target == GL_TEXTURE_2D) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(texture_data->target, 0); //ztodo:应该不用

    change_host_id_map(RESOURCE_TYPE_TEXTURE, texture_data->textureId, new_texture_id); //ztodo: 重新映射guest-host的id

}


void load_native_textures(QEMUFile *f){
    int texture_num = qemu_get_be32(f);   
    LOGI("in load native textures! num %d", texture_num);

    for (int i = 0; i < texture_num; i++) {
        Express_Native_Texture* native_texture = g_malloc0(sizeof(struct Express_Native_Texture));
        native_texture->textureId = qemu_get_be64(f);
        native_texture->target = qemu_get_be32(f);
        native_texture->width = qemu_get_be32(f);
        native_texture->height = qemu_get_be32(f);
        native_texture->depth = qemu_get_be32(f);
        native_texture->internalFormat = qemu_get_be32(f);

        native_texture->minFilter = qemu_get_be32(f);
        native_texture->magFilter = qemu_get_be32(f);
        native_texture->wrapS = qemu_get_be32(f);
        native_texture->wrapT = qemu_get_be32(f);
        native_texture->binding2D = qemu_get_be32(f);
        native_texture->bindingCubeMap = qemu_get_be32(f);
        GLint source_length = native_texture->width * native_texture->height * 4;

        const char* shader_source = (char*)malloc(source_length);
        qemu_get_buffer(f, shader_source, source_length);
        LOGI("loading texture of id %d type %d", native_texture->textureId, native_texture->target);
        native_texture->pixels = (GLubyte*)shader_source;
        // bool isSame = compare_texture(native_texture);
        // if(isSame) { //如果没变就不操作了
        //     continue;
        // }
        update_native_texture(native_texture);
        free(shader_source);
        g_free(native_texture);
    }
}

int save_single_render_thread_context(QEMUFile *f, Render_Thread_Context *thread_context) {
    LOGI("in save single render thread context!");

    if (save_thread_context(f, &thread_context->context) < 0) {
        return -1;
    }

    if (thread_context->process_context) {
        Process_Context *process = get_process_context_form_id((thread_context->context).process_id);
        if(process != NULL) {
            qemu_put_byte(f, 1);//说明load的时候直接在map里查就行
            LOGI("get process %lld from map %d", (thread_context->context).process_id, g_hash_table_size(process->surface_map));
        } else {
            LOGE("error! can't find process from global map.");
        }

    }

    save_thread_unique_ids(f, thread_context->thread_unique_ids);  

    if (thread_context->render_double_buffer_read) {
        Window_Buffer* window_buffer = (Window_Buffer *)g_hash_table_lookup((thread_context->process_context)->surface_map, GUINT_TO_POINTER((thread_context->render_double_buffer_read)->guest_surface));
        if(window_buffer != NULL) {
            // LOGI("save render_double_buffer_read is not null!");
            LOGI("save render_double_buffer_read is not null! %lld %lld", (uint64_t)(thread_context->render_double_buffer_read)->guest_surface, GUINT_TO_POINTER((thread_context->render_double_buffer_read)->guest_surface));
            qemu_put_be64(f, (uint64_t)(thread_context->render_double_buffer_read)->guest_surface);
        } else {
            qemu_put_be64(f, 0);
            LOGE("error! can't find render_double_buffer_read from global map!");
        }
    } else {
        qemu_put_be64(f, 0);
        LOGI("render_double_buffer_read is null!");
    }
    if (thread_context->render_double_buffer_draw) {
        Window_Buffer* window_buffer = (Window_Buffer *)g_hash_table_lookup((thread_context->process_context)->surface_map, GUINT_TO_POINTER((thread_context->render_double_buffer_draw)->guest_surface));
        if(window_buffer != NULL) {
            LOGI("save render_double_buffer_draw is not null! %lld", (uint64_t)(thread_context->render_double_buffer_draw)->guest_surface);
            qemu_put_be64(f, (uint64_t)(thread_context->render_double_buffer_draw)->guest_surface);
        } else {
            qemu_put_be64(f, 0);
            LOGE("error! can't find render_double_buffer_draw from global map!");
        }
    } else {
        qemu_put_be64(f, 0);
        LOGI("render_double_buffer_draw is null!");
    }

    if (thread_context->opengl_context) {
        Opengl_Context* opengl_context = (Opengl_Context *)g_hash_table_lookup((thread_context->process_context)->context_map, GUINT_TO_POINTER((thread_context->opengl_context)->guest_context));
        if(opengl_context != NULL) {
            qemu_put_be64(f, (uint64_t)(thread_context->opengl_context)->guest_context);
        } else {
            LOGE("error! can't from opengl context from map!");
            qemu_put_be64(f, 0);
        }
        
    } else {
        qemu_put_be64(f, 0);
        LOGI("opengl context is null!");
    }

    if (thread_context->egl_display) {
        qemu_put_be32(f, 1);
        save_egl_display(f, thread_context->egl_display);
    } else {
        qemu_put_be32(f, 0);
    }

    return 0;
}

Render_Thread_Context* load_single_render_thread_context(QEMUFile *f) {
    LOGI("in load single render thread context!");


    Render_Thread_Context* thread_context = load_thread_context(f);

    // if (load_thread_context(f, &thread_context->context, unique_id) < 0) {
    //     return -1;
    // }

    // thread_context->process_context = g_malloc0(sizeof(Process_Context));
    // load_process_context(f, thread_context->process_context);
    int has_process = (int)qemu_get_byte(f);
    if(has_process) {
        thread_context->process_context = get_process_context_form_id((thread_context->context).process_id);
        LOGI("has process and get %lld %d %d", (uint64_t)(thread_context->context).process_id, thread_context->process_context->thread_cnt, g_hash_table_size(thread_context->process_context->surface_map));
    } else {
        LOGE("error! can't find process!");
    }

    load_thread_unique_ids(f, thread_context->thread_unique_ids);

    uint64_t has_read_window = qemu_get_be64(f);
    if (has_read_window) {
        Window_Buffer* window_buffer = (Window_Buffer *)g_hash_table_lookup((thread_context->process_context)->surface_map, GUINT_TO_POINTER(has_read_window));
        if(window_buffer != NULL) {
            LOGI("successfully load render buffer read of %lld", has_read_window);
            thread_context->render_double_buffer_read = window_buffer;
        } else {
            LOGE("error! can't get render buffer read when loading %lld", has_read_window);
        }
        // thread_context->render_double_buffer_read = g_malloc0(sizeof(Window_Buffer));
        // load_window_buffer(f, thread_context->render_double_buffer_read, (thread_context->process_context)->gbuffer_map);
    } else {
        thread_context->render_double_buffer_read = NULL;
    }
    uint64_t has_write_window = qemu_get_be64(f);
    if (has_write_window) {
        Window_Buffer* window_buffer = (Window_Buffer *)g_hash_table_lookup((thread_context->process_context)->surface_map, GUINT_TO_POINTER(has_write_window));
        if(window_buffer != NULL) {
            thread_context->render_double_buffer_draw = window_buffer;
            LOGI("successfully load render buffer write of %lld %lld", has_write_window, GUINT_TO_POINTER(has_write_window));

        } else {
            LOGE("error! can't get render buffer write when loading %lld %d", has_write_window, g_hash_table_size((thread_context->process_context)->surface_map));

        }
        // thread_context->render_double_buffer_draw = g_malloc0(sizeof(Window_Buffer));
        // load_window_buffer(f, thread_context->render_double_buffer_draw, (thread_context->process_context)->gbuffer_map);
    } else {
        thread_context->render_double_buffer_draw = NULL;
    }

    uint64_t has_opengl_context = qemu_get_be64(f);
    if (has_opengl_context) {
        Opengl_Context* opengl_context = (Opengl_Context *)g_hash_table_lookup((thread_context->process_context)->context_map, GUINT_TO_POINTER(has_opengl_context));
        if(opengl_context != NULL) {
            thread_context->opengl_context = opengl_context;
        } else {
            LOGE("error! opengl context null when loading");
        }
        // thread_context->opengl_context = g_malloc0(sizeof(Opengl_Context));
        // if (load_opengl_context(f, thread_context->opengl_context) < 0) {
        //     LOGE("failed to load opengl context of thread context");
        //     return NULL;
        // }        
    } else {
        LOGI("has opengl context is null");
        thread_context->opengl_context = NULL;
    }

    int has_egl_display = qemu_get_be32(f);
    if(has_egl_display) {
        load_egl_display(f, thread_context->egl_display);
    }

    Teleport_Express_Call* call = g_malloc0(sizeof(Teleport_Express_Call));
    call->id = FUNID_snapshotLoad;
    call->thread_id = (thread_context->context).thread_id;
    call->process_id = (thread_context->context).process_id;
    call->unique_id = (thread_context->context).unique_id;

    if(has_opengl_context) {
        call_push((Thread_Context*)thread_context, call);
    }
    
    return thread_context;
}


int save_thread_context(QEMUFile *f, Thread_Context *context) {
    qemu_put_be64(f, context->device_id);
    // qemu_put_be32(f, context->read_loc);
    // qemu_put_be32(f, context->write_loc);
    // qemu_put_be32(f, context->init);
    // qemu_put_be32(f, context->thread_run);
    qemu_put_be64(f, context->thread_id);
    qemu_put_be64(f, context->unique_id);
    qemu_put_be64(f, context->process_id);

    LOGI("in save thread context with device id %lld thread id %lld process_id %lld unique_id %lld", context->device_id, context->thread_id, context->process_id, context->unique_id);

//可能需要保存事件的触发状态，恢复时重新创建handle?
// #ifdef _WIN32
//     DWORD event_state = WaitForSingleObject(context->data_event, 0);
//     qemu_put_be32(f, event_state == WAIT_OBJECT_0 ? 1 : 0);
// #endif

    return 0;
}

Render_Thread_Context* load_thread_context(QEMUFile *f) {
    uint64_t device_id = qemu_get_be64(f);
    // uint32_t read_loc = qemu_get_be32(f);
    // uint32_t write_loc = qemu_get_be32(f);
    // uint32_t init = qemu_get_be32(f);
    // uint32_t thread_run = qemu_get_be32(f);
    uint64_t thread_id = qemu_get_be64(f);
    uint64_t unique_id = qemu_get_be64(f);
    uint64_t process_id = qemu_get_be64(f);

    Express_Device_Info *device_info = get_express_device_info(EXPRESS_GPU_DEVICE_ID);

    Render_Thread_Context *context = (Render_Thread_Context *)device_info->get_context(device_id, thread_id, process_id, unique_id, device_info);

    LOGI("in load_thread_context with device id %lld thread id %lld process %lld unique %lld", device_id, thread_id, process_id, unique_id);

    return context;
}



void save_process_context(QEMUFile *f, Process_Context *process_context) {
    LOGI("in save_process_context");
    GHashTableIter iter;
    gpointer key, value;
    guint gbuffer_count = g_hash_table_size(process_context->gbuffer_map);
    LOGI("in save_process_context with gbuffer count %d", gbuffer_count);
    qemu_put_be32(f, gbuffer_count);
    g_hash_table_iter_init(&iter, process_context->gbuffer_map);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        uint64_t gbuffer_id = (uint64_t)key;
        Hardware_Buffer *gbuffer = (Hardware_Buffer *)value;
        
        qemu_put_be64(f, gbuffer_id); // guest那边的id肯定是不会变的
        LOGI("save process context gbuffer id %lld", gbuffer_id);
        save_hardware_buffer(f, gbuffer); // host这边或许需要怎么重新处理一下
    }
    guint surface_count = g_hash_table_size(process_context->surface_map);
    LOGI("in save_process_context with surface_count count %d", surface_count);

    qemu_put_be32(f, surface_count);
    g_hash_table_iter_init(&iter, process_context->surface_map);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        uint64_t guest_surface_id = (uint64_t)key;
        Window_Buffer *window_buffer = (Window_Buffer *)value;
        LOGI("saving window buffer %lld %lld", (uint64_t)window_buffer->gbuffer->gbuffer_id, guest_surface_id);
        qemu_put_be64(f, guest_surface_id);
        save_window_buffer(f, window_buffer, process_context->gbuffer_map);
    }
    guint context_count = g_hash_table_size(process_context->context_map);
    LOGI("in save_process_context with context_count count %d", context_count);

    qemu_put_be32(f, context_count);
    g_hash_table_iter_init(&iter, process_context->context_map);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        uint64_t context_id = (uint64_t)key;
        Opengl_Context *opengl_context = (Opengl_Context *)value;
        
        qemu_put_be64(f, context_id);
        save_opengl_context(f, opengl_context);
    }
    LOGI("in save_process_context with thread count %d", process_context->thread_cnt);
    qemu_put_be32(f, process_context->thread_cnt);
}

void load_process_context(QEMUFile *f, Process_Context *process_context) {
    process_context->gbuffer_map = g_hash_table_new(g_direct_hash, g_direct_equal);
    process_context->surface_map = g_hash_table_new(g_direct_hash, g_direct_equal);
    process_context->context_map = g_hash_table_new(g_direct_hash, g_direct_equal);

    guint gbuffer_count = qemu_get_be32(f);
    LOGI("in load_process_context with gbuffer count %d", gbuffer_count);

    for (guint i = 0; i < gbuffer_count; i++) {
        uint64_t gbuffer_id = qemu_get_be64(f);
        LOGI("load process context gbuffer id %lld", gbuffer_id);
        Hardware_Buffer *gbuffer = load_hardware_buffer(f);
        g_hash_table_insert(process_context->gbuffer_map, GUINT_TO_POINTER(gbuffer_id), gbuffer);
    }

    guint surface_count = qemu_get_be32(f);
    LOGI("in load_process_context with surface_count count %d", surface_count);

    for (guint i = 0; i < surface_count; i++) {
        uint64_t guest_surface_id = qemu_get_be64(f);
        LOGI("in load process context with id %lld %lld", guest_surface_id, GUINT_TO_POINTER(guest_surface_id));
        Window_Buffer *window_buffer = g_malloc0(sizeof(Window_Buffer));
        load_window_buffer(f, window_buffer, process_context->gbuffer_map);
        g_hash_table_insert(process_context->surface_map, GUINT_TO_POINTER(guest_surface_id), window_buffer);
    }
    LOGI("process has surface map size %d", g_hash_table_size(process_context->surface_map));

    guint context_count = qemu_get_be32(f);
    LOGI("in load_process_context with context_count count %d", context_count);

    for (guint i = 0; i < context_count; i++) {
        uint64_t context_id = qemu_get_be64(f);
        Opengl_Context *opengl_context = g_malloc0(sizeof(Opengl_Context));
        load_opengl_context(f, opengl_context);
        g_hash_table_insert(process_context->context_map, GUINT_TO_POINTER(context_id), opengl_context);
    }

    process_context->thread_cnt = qemu_get_be32(f);
    LOGI("in load_process_context with thread count %d", process_context->thread_cnt);

}

void save_window_buffer(QEMUFile *f, Window_Buffer *buffer, GHashTable* gbuffer_map) {
    // if(type == 0) { //保存非map中的window buffer,就先看看map里有没有

    // }
    // LOGI("in save_window_buffer! %d", sizeof(buffer->window_hints.hints));
    qemu_put_be32(f, buffer->type);
    qemu_put_buffer(f, (uint8_t *)buffer->window_hints.hints, sizeof(buffer->window_hints.hints));

    save_egl_config(f, buffer->config);

//不确定这个guest_surface是这么存的啊，先这样吧 
    qemu_put_be64(f, (uint64_t)buffer->guest_surface);

    qemu_put_be64(f, buffer->gbuffer_id);
    qemu_put_be32(f, buffer->width);
    qemu_put_be32(f, buffer->height);
    qemu_put_be32(f, buffer->is_current);
    qemu_put_be32(f, buffer->need_destroy);
    qemu_put_be32(f, buffer->swap_interval);
    LOGI("in save_window_buffer! %lld %lld %d %d %d %d %d", buffer->guest_surface, buffer->gbuffer_id, buffer->width, buffer->height, buffer->is_current, buffer->need_destroy, buffer->swap_interval);

    for (int i = 0; i < 20; i++) {
        qemu_put_be64(f, buffer->swap_time[i]);
    }
    qemu_put_be64(f, buffer->swap_time_all);
    qemu_put_be32(f, buffer->swap_loc);
    qemu_put_be32(f, buffer->swap_time_cnt);
    qemu_put_be64(f, buffer->frame_start_time);
    qemu_put_be64(f, buffer->last_calc_time);
    qemu_put_be32(f, buffer->now_screen_hz);
    qemu_put_be32(f, buffer->sampler_num);
    qemu_put_be32(f, buffer->format);
    qemu_put_be32(f, buffer->pixel_type);
    qemu_put_be32(f, buffer->internal_format);
    qemu_put_be32(f, buffer->depth_internal_format);
    qemu_put_be32(f, buffer->stencil_internal_format);
    
    qemu_put_be32(f, buffer->now_fbo_loc);
    for (int i = 0; i < 3; i++) {
        qemu_put_be32(f, buffer->data_fbo[i]);
        qemu_put_be32(f, buffer->sampler_fbo[i]);
        qemu_put_be32(f, buffer->connect_texture[i]);
    }
    uint64_t gbuffer_id = (buffer->gbuffer)->gbuffer_id;
    int target = 0;
    if(gbuffer_id == 0){
        //处理来自create_gbuffer_from_surface的gbuffer
        LOGI("save gbuffer from surface!");
        qemu_put_be32(f, target);
        save_hardware_buffer(f, buffer->gbuffer); 
    } else {
        target = 1; //来自global map
        Hardware_Buffer *gbuffer = get_gbuffer_from_global_map(gbuffer_id);
        if(gbuffer == NULL) {
            target = 2; // 来自gbuffer_map
            gbuffer = (Hardware_Buffer *)g_hash_table_lookup(gbuffer_map, GUINT_TO_POINTER(gbuffer_id));
        } else {
            LOGI("save gbuffer from global map of id %lld", gbuffer_id);
        } 

        if(gbuffer == NULL) {
            LOGE("error! can't find gbuffer of %lld", gbuffer_id);
        } else {
            qemu_put_be32(f, target); //保存gbuffer来自哪的gbuffer
            qemu_put_be64(f, gbuffer_id);
        }
    }

    // save_hardware_buffer(f, buffer->gbuffer); //要是gbuffer id是global map的，那就可以查到。否则gbuffer_id是0，是create_from_surface的
}

int load_window_buffer(QEMUFile *f, Window_Buffer *buffer, GHashTable* gbuffer_map) {
    
    // Window_Buffer *buffer = g_malloc0(sizeof(Window_Buffer));
    LOGI("in load_window_buffer! %d", sizeof(buffer->window_hints.hints));

    buffer->type = qemu_get_be32(f);
    qemu_get_buffer(f, (uint8_t *)buffer->window_hints.hints, sizeof(buffer->window_hints.hints));

    buffer->config = load_egl_config(f);

    buffer->guest_surface = (EGLSurface)qemu_get_be64(f);

    buffer->gbuffer_id = qemu_get_be64(f);
    buffer->width = qemu_get_be32(f);
    buffer->height = qemu_get_be32(f);
    buffer->is_current = qemu_get_be32(f);
    buffer->need_destroy = qemu_get_be32(f);
    buffer->swap_interval = qemu_get_be32(f);
    LOGI("in load_window_buffer! %lld %lld %d %d %d %d %d", buffer->guest_surface, buffer->gbuffer_id, buffer->width, buffer->height, buffer->is_current, buffer->need_destroy, buffer->swap_interval);
    for (int i = 0; i < 20; i++) {
        buffer->swap_time[i] = qemu_get_be64(f);
    }
    buffer->swap_time_all = qemu_get_be64(f);
    buffer->swap_loc = qemu_get_be32(f);
    buffer->swap_time_cnt = qemu_get_be32(f);
    buffer->frame_start_time = qemu_get_be64(f);
    buffer->last_calc_time = qemu_get_be64(f);
    buffer->now_screen_hz = qemu_get_be32(f);
    buffer->sampler_num = qemu_get_be32(f);
    buffer->format = qemu_get_be32(f);
    buffer->pixel_type = qemu_get_be32(f);
    buffer->internal_format = qemu_get_be32(f);
    buffer->depth_internal_format = qemu_get_be32(f);
    buffer->stencil_internal_format = qemu_get_be32(f);

    buffer->now_fbo_loc = qemu_get_be32(f);
    for (int i = 0; i < 3; i++) {
        buffer->data_fbo[i] = qemu_get_be32(f);
        buffer->sampler_fbo[i] = qemu_get_be32(f);
        buffer->connect_texture[i] = qemu_get_be32(f);
    }

    // buffer->gbuffer = load_hardware_buffer(f);

    int target = qemu_get_be32(f);
    if(target == 0) {
        buffer->gbuffer = load_hardware_buffer(f);
    }
    else if(target == 1) {
        uint64_t gbuffer_id = qemu_get_be64(f);
        buffer->gbuffer = get_gbuffer_from_global_map(gbuffer_id);
        LOGI("load gbuffer from global map of id %lld", gbuffer_id);
    } else {
        uint64_t gbuffer_id = qemu_get_be64(f);
        buffer->gbuffer = (Hardware_Buffer *)g_hash_table_lookup(gbuffer_map, GUINT_TO_POINTER(gbuffer_id));
    }
    return 0;
}


//开始巨大的save opengl context！

void save_scatter_data(QEMUFile *f, Scatter_Data *scatter_data, int count) {
    qemu_put_be32(f, count);

    for (int i = 0; i < count; i++) {
        qemu_put_be32(f, scatter_data[i].len);
        qemu_put_buffer(f, scatter_data[i].data, scatter_data[i].len);
    }
}

Scatter_Data* load_scatter_data(QEMUFile *f, int *count) {
    *count = qemu_get_be32(f);

    Scatter_Data *scatter_data = g_malloc0(sizeof(Scatter_Data) * (*count));
    for (int i = 0; i < *count; i++) {
        scatter_data[i].len = qemu_get_be32(f);
        scatter_data[i].data = g_malloc0(scatter_data[i].len);
        qemu_get_buffer(f, scatter_data[i].data, scatter_data[i].len);
    }

    return scatter_data;
}

void save_guest_mem(QEMUFile *f, Guest_Mem *guest_mem) {
    qemu_put_be32(f, guest_mem->num);
    qemu_put_be32(f, guest_mem->all_len);
    save_scatter_data(f, guest_mem->scatter_data, guest_mem->num);
}

Guest_Mem* load_guest_mem(QEMUFile *f) {
    Guest_Mem *guest_mem = g_malloc0(sizeof(Guest_Mem));
    guest_mem->num = qemu_get_be32(f);
    guest_mem->all_len = qemu_get_be32(f);
    guest_mem->scatter_data = load_scatter_data(f, &guest_mem->num);

    return guest_mem;
}

void save_hardware_buffer(QEMUFile *f, Hardware_Buffer *buffer) {
    qemu_put_be32(f, buffer->is_writing);
    qemu_put_be32(f, buffer->is_lock);
    qemu_put_be32(f, buffer->sampler_num);
    qemu_put_be32(f, buffer->data_texture);
    qemu_put_be32(f, buffer->reverse_rbo);
    qemu_put_be32(f, buffer->sampler_rbo);
    qemu_put_be32(f, buffer->rbo_depth);
    qemu_put_be32(f, buffer->rbo_stencil);
    qemu_put_be32(f, buffer->data_fbo);
    qemu_put_be32(f, buffer->sampler_fbo);
    qemu_put_be32(f, buffer->has_connected_fbo);
    qemu_put_be64(f, buffer->gbuffer_id);
    qemu_put_be32(f, buffer->remain_life_time);
    qemu_put_be32(f, buffer->is_dying);
    qemu_put_be32(f, buffer->is_using);
    qemu_put_be32(f, buffer->need_reverse);
    qemu_put_be32(f, buffer->format);
    qemu_put_be32(f, buffer->pixel_type);
    qemu_put_be32(f, buffer->internal_format);
    qemu_put_be32(f, buffer->row_byte_len);
    qemu_put_be32(f, buffer->depth_internal_format);
    qemu_put_be32(f, buffer->stencil_internal_format);
    qemu_put_be32(f, buffer->hal_format);
    qemu_put_be32(f, buffer->stride);
    qemu_put_be32(f, buffer->width);
    qemu_put_be32(f, buffer->height);
    qemu_put_be32(f, buffer->usage_type);
    qemu_put_be32(f, buffer->pixel_size);
    qemu_put_be32(f, buffer->usage);
    qemu_put_be32(f, buffer->size);
    qemu_put_be32(f, buffer->last_phy_dev);
    qemu_put_be32(f, buffer->last_phy_usage);
    qemu_put_be32(f, buffer->pref_phy_dev);
    qemu_put_be32(f, buffer->last_virt_dev);
    qemu_put_be32(f, buffer->last_virt_usage);
    qemu_put_be32(f, buffer->last_virt_time);

    // todo 咋存？
    qemu_put_be64(f, (uint64_t)buffer->data_sync);
    qemu_put_be64(f, (uint64_t)buffer->delete_sync);

    if (buffer->guest_data) {
        qemu_put_be32(f, 1);
        save_guest_mem(f, buffer->guest_data);
    } else {
        qemu_put_be32(f, 0);
    }

    qemu_put_be64(f, (uint64_t)buffer->host_data);
}

Hardware_Buffer* load_hardware_buffer(QEMUFile *f) {
    Hardware_Buffer *buffer = g_malloc0(sizeof(Hardware_Buffer));

    buffer->is_writing = qemu_get_be32(f);
    buffer->is_lock = qemu_get_be32(f);
    buffer->sampler_num = qemu_get_be32(f);
    buffer->data_texture = qemu_get_be32(f);
    buffer->reverse_rbo = qemu_get_be32(f);
    buffer->sampler_rbo = qemu_get_be32(f);
    buffer->rbo_depth = qemu_get_be32(f);
    buffer->rbo_stencil = qemu_get_be32(f);
    buffer->data_fbo = qemu_get_be32(f);
    buffer->sampler_fbo = qemu_get_be32(f);
    buffer->has_connected_fbo = qemu_get_be32(f);
    buffer->gbuffer_id = qemu_get_be64(f);
    buffer->remain_life_time = qemu_get_be32(f);
    buffer->is_dying = qemu_get_be32(f);
    buffer->is_using = qemu_get_be32(f);
    buffer->need_reverse = qemu_get_be32(f);
    buffer->format = qemu_get_be32(f);
    buffer->pixel_type = qemu_get_be32(f);
    buffer->internal_format = qemu_get_be32(f);
    buffer->row_byte_len = qemu_get_be32(f);
    buffer->depth_internal_format = qemu_get_be32(f);
    buffer->stencil_internal_format = qemu_get_be32(f);
    buffer->hal_format = qemu_get_be32(f);
    buffer->stride = qemu_get_be32(f);
    buffer->width = qemu_get_be32(f);
    buffer->height = qemu_get_be32(f);
    buffer->usage_type = qemu_get_be32(f);
    buffer->pixel_size = qemu_get_be32(f);
    buffer->usage = qemu_get_be32(f);
    buffer->size = qemu_get_be32(f);
    buffer->last_phy_dev = qemu_get_be32(f);
    buffer->last_phy_usage = qemu_get_be32(f);
    buffer->pref_phy_dev = qemu_get_be32(f);
    buffer->last_virt_dev = qemu_get_be32(f);
    buffer->last_virt_usage = qemu_get_be32(f);
    buffer->last_virt_time = qemu_get_be32(f);

    //todo 不知道这个是啥！该咋存！
    buffer->data_sync = (GLsync)qemu_get_be64(f);
    buffer->delete_sync = (GLsync)qemu_get_be64(f);

    int has_guest_data = qemu_get_be32(f);
    if (has_guest_data) {
        buffer->guest_data = load_guest_mem(f);
    } else {
        buffer->guest_data = NULL;
    }

    buffer->host_data = (void *)qemu_get_be64(f);

    return buffer;
}


void save_attrib_point(QEMUFile *f, Attrib_Point *point) {
    for (int i = 0; i < MAX_VERTEX_ATTRIBS_NUM; i++) {
        qemu_put_be32(f, point->buffer_object[i]);
        qemu_put_be32(f, point->buffer_loc[i]);
        qemu_put_be32(f, point->remain_buffer_len[i]);
        qemu_put_be32(f, point->buffer_len[i]);
    }
    qemu_put_be32(f, point->indices_buffer_object);
    qemu_put_be32(f, point->indices_buffer_len);
    qemu_put_be32(f, point->remain_indices_buffer_len);
    qemu_put_be32(f, point->element_array_buffer);
}

Attrib_Point* load_attrib_point(QEMUFile *f) {
    Attrib_Point *point = g_malloc0(sizeof(Attrib_Point));
    for (int i = 0; i < MAX_VERTEX_ATTRIBS_NUM; i++) {
        point->buffer_object[i] = qemu_get_be32(f);
        point->buffer_loc[i] = qemu_get_be32(f);
        point->remain_buffer_len[i] = qemu_get_be32(f);
        point->buffer_len[i] = qemu_get_be32(f);
    }
    point->indices_buffer_object = qemu_get_be32(f);
    point->indices_buffer_len = qemu_get_be32(f);
    point->remain_indices_buffer_len = qemu_get_be32(f);
    point->element_array_buffer = qemu_get_be32(f);
    return point;
}

void save_buffer_status(QEMUFile *f, Buffer_Status *status) {
    qemu_put_be32(f, status->guest_array_buffer);
    qemu_put_be32(f, status->host_array_buffer);

    qemu_put_be32(f, status->guest_element_array_buffer);
    qemu_put_be32(f, status->host_element_array_buffer);

    qemu_put_be32(f, status->guest_copy_read_buffer);
    qemu_put_be32(f, status->host_copy_read_buffer);

    qemu_put_be32(f, status->guest_copy_write_buffer);
    qemu_put_be32(f, status->host_copy_write_buffer);

    qemu_put_be32(f, status->guest_pixel_pack_buffer);
    qemu_put_be32(f, status->host_pixel_pack_buffer);

    qemu_put_be32(f, status->guest_pixel_unpack_buffer);
    qemu_put_be32(f, status->host_pixel_unpack_buffer);

    qemu_put_be32(f, status->guest_transform_feedback_buffer);
    qemu_put_be32(f, status->host_transform_feedback_buffer);

    qemu_put_be32(f, status->guest_uniform_buffer);
    qemu_put_be32(f, status->host_uniform_buffer);

    qemu_put_be32(f, status->guest_atomic_counter_buffer);
    qemu_put_be32(f, status->host_atomic_counter_buffer);

    qemu_put_be32(f, status->guest_dispatch_indirect_buffer);
    qemu_put_be32(f, status->host_dispatch_indirect_buffer);

    qemu_put_be32(f, status->guest_draw_indirect_buffer);
    qemu_put_be32(f, status->host_draw_indirect_buffer);

    qemu_put_be32(f, status->guest_shader_storage_buffer);
    qemu_put_be32(f, status->host_shader_storage_buffer);

    qemu_put_be32(f, status->guest_texture_buffer);
    qemu_put_be32(f, status->host_texture_buffer);

    qemu_put_be32(f, status->guest_vertex_array_buffer);
    qemu_put_be32(f, status->host_vertex_array_buffer);

    qemu_put_be32(f, status->guest_vao_ebo);
    qemu_put_be32(f, status->host_vao_ebo);

    qemu_put_be32(f, status->guest_vao);
    qemu_put_be32(f, status->host_vao);
}

Buffer_Status* load_buffer_status(QEMUFile *f) {
    Buffer_Status *status = g_malloc0(sizeof(Buffer_Status));

    status->guest_array_buffer = qemu_get_be32(f);
    status->host_array_buffer = qemu_get_be32(f);

    status->guest_element_array_buffer = qemu_get_be32(f);
    status->host_element_array_buffer = qemu_get_be32(f);

    status->guest_copy_read_buffer = qemu_get_be32(f);
    status->host_copy_read_buffer = qemu_get_be32(f);

    status->guest_copy_write_buffer = qemu_get_be32(f);
    status->host_copy_write_buffer = qemu_get_be32(f);

    status->guest_pixel_pack_buffer = qemu_get_be32(f);
    status->host_pixel_pack_buffer = qemu_get_be32(f);

    status->guest_pixel_unpack_buffer = qemu_get_be32(f);
    status->host_pixel_unpack_buffer = qemu_get_be32(f);

    status->guest_transform_feedback_buffer = qemu_get_be32(f);
    status->host_transform_feedback_buffer = qemu_get_be32(f);

    status->guest_uniform_buffer = qemu_get_be32(f);
    status->host_uniform_buffer = qemu_get_be32(f);

    status->guest_atomic_counter_buffer = qemu_get_be32(f);
    status->host_atomic_counter_buffer = qemu_get_be32(f);

    status->guest_dispatch_indirect_buffer = qemu_get_be32(f);
    status->host_dispatch_indirect_buffer = qemu_get_be32(f);

    status->guest_draw_indirect_buffer = qemu_get_be32(f);
    status->host_draw_indirect_buffer = qemu_get_be32(f);

    status->guest_shader_storage_buffer = qemu_get_be32(f);
    status->host_shader_storage_buffer = qemu_get_be32(f);

    status->guest_texture_buffer = qemu_get_be32(f);
    status->host_texture_buffer = qemu_get_be32(f);

    status->guest_vertex_array_buffer = qemu_get_be32(f);
    status->host_vertex_array_buffer = qemu_get_be32(f);

    status->guest_vao_ebo = qemu_get_be32(f);
    status->host_vao_ebo = qemu_get_be32(f);

    status->guest_vao = qemu_get_be32(f);
    status->host_vao = qemu_get_be32(f);

    return status;
}

void save_bound_buffer(QEMUFile *f, Bound_Buffer *buffer) {
    save_attrib_point(f, buffer->attrib_point);
    save_buffer_status(f, &buffer->buffer_status);
    qemu_put_be32(f, buffer->asyn_unpack_texture_buffer);
    qemu_put_be32(f, buffer->asyn_pack_texture_buffer);
    qemu_put_be32(f, buffer->has_init);

    guint num_entries = g_hash_table_size(buffer->vao_point_data);
    qemu_put_be32(f, num_entries);

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, buffer->vao_point_data);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        GLuint vao_id = GPOINTER_TO_UINT(key);
        Attrib_Point *point_data = (Attrib_Point *)value;

        qemu_put_be64(f, vao_id);

        save_attrib_point(f, point_data);
    }
}

Bound_Buffer* load_bound_buffer(QEMUFile *f) {
    Bound_Buffer *buffer = g_malloc0(sizeof(Bound_Buffer));
    buffer->attrib_point = load_attrib_point(f);
    buffer->buffer_status = *load_buffer_status(f);
    buffer->asyn_unpack_texture_buffer = qemu_get_be32(f);
    buffer->asyn_pack_texture_buffer = qemu_get_be32(f);
    buffer->has_init = qemu_get_be32(f);

    guint num_entries = qemu_get_be32(f);
    buffer->vao_point_data = g_hash_table_new(g_direct_hash, g_direct_equal);

    for (guint i = 0; i < num_entries; i++) {
        guint64 vao_id = qemu_get_be64(f);

        Attrib_Point *point_data = load_attrib_point(f);

        g_hash_table_insert(buffer->vao_point_data, GUINT_TO_POINTER(vao_id), point_data);
    }

    return buffer;
}

void save_resource_map_status(QEMUFile *f, Resource_Map_Status *status) {
    LOGI("save resource map status with map_size %d max size %d", status->map_size, status->gbuffer_map_max_size);
    qemu_put_be32(f, status->max_id);
    qemu_put_be32(f, status->map_size);
    
    for (unsigned int i = 0; i < status->map_size; i++) {
        LOGD("saving %d %d %d", i, status->resource_id_map[i], status->resource_is_init[i]);
        qemu_put_be64(f, status->resource_id_map[i]);
        qemu_put_byte(f, status->resource_is_init[i]);
    }

    qemu_put_be32(f, status->gbuffer_map_max_size);

    for (unsigned int i = 0; i < status->gbuffer_map_max_size; i++) {
        if (status->gbuffer_ptr_map[i] != NULL) {
            qemu_put_byte(f, 1); //有有效数据
            LOGD("save resource map status with gbuffer id %lld id %d", status->gbuffer_ptr_map[i]->gbuffer_id, i);
            save_hardware_buffer(f, status->gbuffer_ptr_map[i]);
        } else {
            LOGD("buffer of %d is null!", i);
            qemu_put_byte(f, 0);
        }
    }
    
}

Resource_Map_Status* load_resource_map_status(QEMUFile *f, int resource_type) {
    Resource_Map_Status *status = g_malloc0(sizeof(Resource_Map_Status));
    status->max_id = qemu_get_be32(f);
    status->map_size = qemu_get_be32(f);
    status->resource_id_map = g_malloc0(sizeof(long long) * status->map_size);
    status->resource_is_init = g_malloc0(sizeof(char) * status->map_size);
    for (unsigned int i = 0; i < status->map_size; i++) {
        status->resource_id_map[i] = qemu_get_be64(f);
        status->resource_is_init[i] = qemu_get_byte(f);
        long long new_id = (long long)g_hash_table_lookup(g_resource_ids_map[resource_type], GUINT_TO_POINTER(status->resource_id_map[i])); 
        if(new_id != NULL) {
            LOGI("in load resource of type %d change id from %lld to %lld", resource_type, status->resource_id_map[i], new_id);
            status->resource_id_map[i] = new_id;//ztodo：这里的语法？
        }

        // if(status->resource_id_map[i] != 0) 

        LOGI("loading %d %d %d %d", resource_type, status->resource_id_map[i], status->resource_is_init[i], g_hash_table_size(g_resource_ids_map[resource_type]));
    }

    status->gbuffer_map_max_size = qemu_get_be32(f);
    status->gbuffer_ptr_map = g_malloc0(sizeof(void*) * status->gbuffer_map_max_size);

    for (unsigned int i = 0; i < status->gbuffer_map_max_size; i++) {
        if (qemu_get_byte(f)) {
            status->gbuffer_ptr_map[i] = load_hardware_buffer(f);
            LOGI("load resource map status with gbuffer id %lld id %d", status->gbuffer_ptr_map[i]->gbuffer_id, i);

        } else {
            status->gbuffer_ptr_map[i] = NULL;
        }
    }
    LOGI("load resource map status with map_size %d max size %d", status->map_size, status->gbuffer_map_max_size);
    return status;
}

void save_resource_context(QEMUFile *f, Resource_Context *context) {

    save_resource_map_status(f, context->texture_resource);
    LOGI("finish saving texture resource!");
    save_resource_map_status(f, context->buffer_resource);
    save_resource_map_status(f, context->render_buffer_resource);
    save_resource_map_status(f, context->sampler_resource);

    save_resource_map_status(f, context->shader_resource);
    save_resource_map_status(f, context->program_resource);

    save_resource_map_status(f, context->sync_resource);

    save_resource_map_status(f, context->frame_buffer_resource);
    save_resource_map_status(f, context->program_pipeline_resource);
    save_resource_map_status(f, context->transform_feedback_resource);
    save_resource_map_status(f, context->vertex_array_resource);

    save_resource_map_status(f, context->query_resource);

    qemu_put_be32(f, context->share_resources->counter);
    save_resource_map_status(f, &context->share_resources->texture_resource);
    save_resource_map_status(f, &context->share_resources->buffer_resource);
    save_resource_map_status(f, &context->share_resources->render_buffer_resource);
    save_resource_map_status(f, &context->share_resources->sample_resource);
    save_resource_map_status(f, &context->share_resources->program_resource);
    save_resource_map_status(f, &context->share_resources->shader_resource);
    save_resource_map_status(f, &context->share_resources->sync_resource);

    save_resource_map_status(f, &context->exclusive_resources->frame_buffer_resource);
    save_resource_map_status(f, &context->exclusive_resources->program_pipeline_resource);
    save_resource_map_status(f, &context->exclusive_resources->transform_feedback_resource);
    save_resource_map_status(f, &context->exclusive_resources->vertex_array_resource);
    save_resource_map_status(f, &context->exclusive_resources->query_resource);
}

Resource_Context* load_resource_context(QEMUFile *f) {
    Resource_Context *context = g_malloc0(sizeof(Resource_Context));

    context->texture_resource = load_resource_map_status(f, RESOURCE_TYPE_TEXTURE);
    LOGI("finish loading texture resource!");

    context->buffer_resource = load_resource_map_status(f, RESOURCE_TYPE_BUFFER);
    context->render_buffer_resource = load_resource_map_status(f, RESOURCE_TYPE_RENDERBUFFER);
    context->sampler_resource = load_resource_map_status(f, RESOURCE_TYPE_SAMPLER);

    context->shader_resource = load_resource_map_status(f, RESOURCE_TYPE_SHADER);
    context->program_resource = load_resource_map_status(f, RESOURCE_TYPE_PROGRAM);

    context->sync_resource = load_resource_map_status(f, RESOURCE_TYPE_SYNC);

    context->frame_buffer_resource = load_resource_map_status(f, RESOURCE_TYPE_FRAMEBUFFER);
    context->program_pipeline_resource = load_resource_map_status(f, RESOURCE_TYPE_PROGRAM_PIPELINE);
    context->transform_feedback_resource = load_resource_map_status(f, RESOURCE_TYPE_TRANSFORM_FEEDBACK);
    context->vertex_array_resource = load_resource_map_status(f, RESOURCE_TYPE_VERTEX_ARRAY);

    context->query_resource = load_resource_map_status(f, RESOURCE_TYPE_QUERY);

    context->share_resources = g_malloc0(sizeof(Share_Resources));
    context->share_resources->counter = qemu_get_be32(f);
    context->share_resources->texture_resource = *load_resource_map_status(f, RESOURCE_TYPE_TEXTURE);
    context->share_resources->buffer_resource = *load_resource_map_status(f, RESOURCE_TYPE_BUFFER);
    context->share_resources->render_buffer_resource = *load_resource_map_status(f, RESOURCE_TYPE_RENDERBUFFER);
    context->share_resources->sample_resource = *load_resource_map_status(f, RESOURCE_TYPE_SAMPLER);
    context->share_resources->program_resource = *load_resource_map_status(f, RESOURCE_TYPE_PROGRAM);
    context->share_resources->shader_resource = *load_resource_map_status(f, RESOURCE_TYPE_SHADER);
    context->share_resources->sync_resource = *load_resource_map_status(f, RESOURCE_TYPE_SYNC);

    context->exclusive_resources = g_malloc0(sizeof(Exclusive_Resources));
    context->exclusive_resources->frame_buffer_resource = *load_resource_map_status(f, RESOURCE_TYPE_FRAMEBUFFER);
    context->exclusive_resources->program_pipeline_resource = *load_resource_map_status(f, RESOURCE_TYPE_PROGRAM_PIPELINE);
    context->exclusive_resources->transform_feedback_resource = *load_resource_map_status(f, RESOURCE_TYPE_TRANSFORM_FEEDBACK);
    context->exclusive_resources->vertex_array_resource = *load_resource_map_status(f, RESOURCE_TYPE_VERTEX_ARRAY);
    context->exclusive_resources->query_resource = *load_resource_map_status(f, RESOURCE_TYPE_QUERY);

    return context;
}

void save_texture_binding_status(QEMUFile *f, Texture_Binding_Status *status) {
    qemu_put_be32(f, status->guest_current_active_texture);
    qemu_put_be32(f, status->host_current_active_texture);
    qemu_put_be32(f, status->now_max_texture_unit);
    LOGI("in save texture binding status with max unit %d %d %d", status->now_max_texture_unit, sizeof(status->guest_current_texture_2D)/sizeof(int), status->texture_unit_num);

    int tot_num = sizeof(status->guest_current_texture_2D)/sizeof(int);
    qemu_put_be32(f, tot_num);
    for (GLuint i = 0; i < tot_num; i++) {
        qemu_put_be32(f, status->guest_current_texture_2D[i]);
        qemu_put_be32(f, status->host_current_texture_2D[i]);
        qemu_put_be32(f, status->guest_current_texture_cube_map[i]);
        qemu_put_be32(f, status->host_current_texture_cube_map[i]);
        qemu_put_be32(f, status->guest_current_texture_3D[i]);
        qemu_put_be32(f, status->host_current_texture_3D[i]);
        qemu_put_be32(f, status->guest_current_texture_2D_array[i]);
        qemu_put_be32(f, status->host_current_texture_2D_array[i]);
        qemu_put_be32(f, status->guest_current_texture_2D_multisample[i]);
        qemu_put_be32(f, status->host_current_texture_2D_multisample[i]);
        qemu_put_be32(f, status->guest_current_texture_2D_multisample_array[i]);
        qemu_put_be32(f, status->host_current_texture_2D_multisample_array[i]);
        qemu_put_be32(f, status->guest_current_texture_cube_map_array[i]);
        qemu_put_be32(f, status->host_current_texture_cube_map_array[i]);
        qemu_put_be32(f, status->guest_current_texture_buffer[i]);
        qemu_put_be32(f, status->host_current_texture_buffer[i]);
    }

    qemu_put_be32(f, status->texture_unit_num);
    
    qemu_put_be32(f, status->current_texture_external);

    qemu_put_be64(f, (uint64_t)status->current_2D_gbuffer);
    qemu_put_be64(f, (uint64_t)status->current_external_gbuffer);
}

Texture_Binding_Status* load_texture_binding_status(QEMUFile *f) { //ztodo:这些都得改
    Texture_Binding_Status *status = g_malloc0(sizeof(Texture_Binding_Status));

    status->guest_current_active_texture = qemu_get_be32(f);
    status->host_current_active_texture = qemu_get_be32(f);
    status->now_max_texture_unit = qemu_get_be32(f);

    int tot_num = qemu_get_be32(f);
    LOGI("in load texture binding status with max unit %d %d", tot_num, status->now_max_texture_unit);

    status->guest_current_texture_2D = g_malloc0(sizeof(GLuint) * tot_num);
    status->host_current_texture_2D = g_malloc0(sizeof(GLuint) * tot_num);
    status->guest_current_texture_cube_map = g_malloc0(sizeof(GLuint) * tot_num);
    status->host_current_texture_cube_map = g_malloc0(sizeof(GLuint) * tot_num);
    status->guest_current_texture_3D = g_malloc0(sizeof(GLuint) * tot_num);
    status->host_current_texture_3D = g_malloc0(sizeof(GLuint) * tot_num);
    status->guest_current_texture_2D_array = g_malloc0(sizeof(GLuint) * tot_num);
    status->host_current_texture_2D_array = g_malloc0(sizeof(GLuint) * tot_num);
    status->guest_current_texture_2D_multisample = g_malloc0(sizeof(GLuint) * tot_num);
    status->host_current_texture_2D_multisample = g_malloc0(sizeof(GLuint) * tot_num);
    status->guest_current_texture_2D_multisample_array = g_malloc0(sizeof(GLuint) * tot_num);
    status->host_current_texture_2D_multisample_array = g_malloc0(sizeof(GLuint) * tot_num);
    status->guest_current_texture_cube_map_array = g_malloc0(sizeof(GLuint) * tot_num);
    status->host_current_texture_cube_map_array = g_malloc0(sizeof(GLuint) * tot_num);
    status->guest_current_texture_buffer = g_malloc0(sizeof(GLuint) * tot_num);
    status->host_current_texture_buffer = g_malloc0(sizeof(GLuint) * tot_num);

    for (GLuint i = 0; i < tot_num; i++) {
        status->guest_current_texture_2D[i] = qemu_get_be32(f);
        status->host_current_texture_2D[i] = qemu_get_be32(f);
        status->guest_current_texture_cube_map[i] = qemu_get_be32(f);
        status->host_current_texture_cube_map[i] = qemu_get_be32(f);
        status->guest_current_texture_3D[i] = qemu_get_be32(f);
        status->host_current_texture_3D[i] = qemu_get_be32(f);
        status->guest_current_texture_2D_array[i] = qemu_get_be32(f);
        status->host_current_texture_2D_array[i] = qemu_get_be32(f);
        status->guest_current_texture_2D_multisample[i] = qemu_get_be32(f);
        status->host_current_texture_2D_multisample[i] = qemu_get_be32(f);
        status->guest_current_texture_2D_multisample_array[i] = qemu_get_be32(f);
        status->host_current_texture_2D_multisample_array[i] = qemu_get_be32(f);
        status->guest_current_texture_cube_map_array[i] = qemu_get_be32(f);
        status->host_current_texture_cube_map_array[i] = qemu_get_be32(f);
        status->guest_current_texture_buffer[i] = qemu_get_be32(f);
        status->host_current_texture_buffer[i] = qemu_get_be32(f);
    }

    status->texture_unit_num = qemu_get_be32(f);
    // status->now_max_texture_unit = qemu_get_be32(f);
    status->current_texture_external = qemu_get_be32(f);

    status->current_2D_gbuffer = (Hardware_Buffer *)(uint64_t)qemu_get_be64(f);
    status->current_external_gbuffer = (Hardware_Buffer *)(uint64_t)qemu_get_be64(f);

    return status;
}


void save_opengl_context(QEMUFile *f, Opengl_Context *context) {

    LOGI("in save opengl context of %llx %llx", context, context->window);

    //不确定是否需要深拷贝，先浅拷贝了 update:需要额外处理，先注释了
    // qemu_put_be64(f, context->window);
    // LOGI("the native context saving now is %llx", (int64_t)context->window);
    // save_native_context(f, context->window);

    save_bound_buffer(f, &context->bound_buffer_status);
    save_resource_context(f, &context->resource_status);
    save_texture_binding_status(f, &context->texture_binding_status);    

    //不确定是否需要深拷贝，先浅拷贝了
    qemu_put_be64(f, (uint64_t)context->share_context);





    qemu_put_be32(f, context->draw_fbo0);
    qemu_put_be32(f, context->read_fbo0);
    qemu_put_be32(f, context->vao0);

    qemu_put_be32(f, context->view_x);
    qemu_put_be32(f, context->view_y);
    qemu_put_be32(f, context->view_w);
    qemu_put_be32(f, context->view_h);

    qemu_put_be32(f, context->is_current);
    qemu_put_be32(f, context->need_destroy);

    qemu_put_be64(f, (uint64_t)context->guest_context);
    
    // save_texture_binding_status(f, &context->texture_binding_status);

    qemu_put_be32(f, context->is_using_external_program);
    qemu_put_be32(f, context->enable_scissor);
    qemu_put_be32(f, context->context_flags);

    qemu_put_be32(f, context->draw_texi_vao);
    qemu_put_be32(f, context->draw_texi_vbo);
    qemu_put_be32(f, context->draw_texi_ebo);

    guint num_entries = g_hash_table_size(context->buffer_map);
    LOGI("in save opengl context with num_entries %d", num_entries);

    qemu_put_be32(f, num_entries);

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, context->buffer_map);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        guint64 buffer_key = (guint64)key;
        Guest_Host_Map *map = (Guest_Host_Map *)value;

        qemu_put_be64(f, buffer_key);
        save_guest_host_map(f, map);
    }
}

void restore_opengl_context_textures(Opengl_Context *context) {
    Texture_Binding_Status *status = &(context->texture_binding_status);
    // egl_makeCurrent(context->window);


    texture_binding_status_sync(context, GL_TEXTURE_2D);
    texture_binding_status_sync(context, GL_TEXTURE_2D_MULTISAMPLE);
    texture_binding_status_sync(context, GL_TEXTURE_2D_MULTISAMPLE_ARRAY);
    texture_binding_status_sync(context, GL_TEXTURE_3D);
    texture_binding_status_sync(context, GL_TEXTURE_2D_ARRAY);
    texture_binding_status_sync(context, GL_TEXTURE_CUBE_MAP);
    texture_binding_status_sync(context, GL_TEXTURE_CUBE_MAP_ARRAY);
    texture_binding_status_sync(context, GL_TEXTURE_BUFFER);
    texture_binding_status_sync(context, GL_TEXTURE_EXTERNAL_OES);

    GLuint current_active_texture = status->guest_current_active_texture;
    glActiveTexture(current_active_texture + GL_TEXTURE0);

    GLint textureId = 0;

    glBindTexture(GL_TEXTURE_2D, status->guest_current_texture_2D[current_active_texture]);   
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &textureId);
    LOGI("The currently bound GL_TEXTURE_2D ID is: %d", textureId);


    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, status->guest_current_texture_2D_multisample[current_active_texture]);
    glGetIntegerv(GL_TEXTURE_2D_MULTISAMPLE, &textureId);
    LOGI("The currently bound GL_TEXTURE_2D_MULTISAMPLE ID is: %d", textureId);


    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, status->guest_current_texture_2D_multisample_array[current_active_texture]);    
    glGetIntegerv(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, &textureId);
    LOGI("The currently bound GL_TEXTURE_2D_MULTISAMPLE_ARRAY ID is: %d", textureId);

    glBindTexture(GL_TEXTURE_3D, status->guest_current_texture_3D[current_active_texture]);
    glGetIntegerv(GL_TEXTURE_3D, &textureId);
    LOGI("The currently bound GL_TEXTURE_3D ID is: %d", textureId);

    glBindTexture(GL_TEXTURE_2D_ARRAY, status->guest_current_texture_2D_array[current_active_texture]);
    glBindTexture(GL_TEXTURE_CUBE_MAP, status->guest_current_texture_cube_map[current_active_texture]);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, status->guest_current_texture_cube_map_array[current_active_texture]);
    glBindTexture(GL_TEXTURE_BUFFER, status->guest_current_texture_buffer[current_active_texture]);
    LOGI("restoring textures %d %d %d %d", status->guest_current_texture_2D[current_active_texture], status->guest_current_texture_2D_multisample[current_active_texture], status->guest_current_texture_2D_multisample_array[current_active_texture], status->guest_current_texture_3D[current_active_texture])

}

int load_opengl_context(QEMUFile *f, Opengl_Context *context) {

    // Opengl_Context *context = g_malloc0(sizeof(Opengl_Context));
    
    // context->window = (void*)qemu_get_be64(f);

    

    LOGI("in load opengl context of %llx %llx", context, context->window);
    // load_native_context(f, context->window);

    context->bound_buffer_status = *load_bound_buffer(f);
    context->resource_status = *load_resource_context(f);
    context->texture_binding_status = *load_texture_binding_status(f);


    context->share_context = (void*)qemu_get_be64(f);


    context->draw_fbo0 = qemu_get_be32(f);
    context->read_fbo0 = qemu_get_be32(f);
    context->vao0 = qemu_get_be32(f);

    context->view_x = qemu_get_be32(f);
    context->view_y = qemu_get_be32(f);
    context->view_w = qemu_get_be32(f);
    context->view_h = qemu_get_be32(f);

    context->is_current = qemu_get_be32(f);
    context->need_destroy = qemu_get_be32(f);

    context->guest_context = (EGLContext)qemu_get_be64(f);

    // context->texture_binding_status = *load_texture_binding_status(f);

    context->is_using_external_program = qemu_get_be32(f);
    context->enable_scissor = qemu_get_be32(f);
    context->context_flags = qemu_get_be32(f);

    context->window = get_native_opengl_context(context->context_flags);

    
    // restore_opengl_context_textures(context);

    context->draw_texi_vao = qemu_get_be32(f);
    context->draw_texi_vbo = qemu_get_be32(f);
    context->draw_texi_ebo = qemu_get_be32(f);

    guint num_entries = qemu_get_be32(f);
    context->buffer_map = g_hash_table_new(g_direct_hash, g_direct_equal);

    LOGI("in load opengl context with num_entries %d", num_entries);
    for (guint i = 0; i < num_entries; i++) {
        guint64 buffer_key = qemu_get_be64(f);
        Guest_Host_Map *map = load_guest_host_map(f);

        g_hash_table_insert(context->buffer_map, GUINT_TO_POINTER(buffer_key), map);
    }


    return 0;
}

void save_guest_host_map(QEMUFile *f, Guest_Host_Map *map) {
    qemu_put_be64(f, (uint64_t)map->host_data);
    qemu_put_be64(f, map->map_len);
    qemu_put_be32(f, map->target);
    qemu_put_be32(f, map->access);
}

Guest_Host_Map* load_guest_host_map(QEMUFile *f) {
    Guest_Host_Map *map = g_malloc0(sizeof(Guest_Host_Map));
    map->host_data = (GLubyte *)qemu_get_be64(f);
    map->map_len = qemu_get_be64(f);
    map->target = qemu_get_be32(f);
    map->access = qemu_get_be32(f);
    return map;
}

void save_thread_unique_ids(QEMUFile *f, GHashTable *thread_unique_ids) {
    GHashTableIter iter;
    gpointer key;
    
    guint unique_id_count = g_hash_table_size(thread_unique_ids);
    qemu_put_be32(f, unique_id_count);

    g_hash_table_iter_init(&iter, thread_unique_ids);
    while (g_hash_table_iter_next(&iter, &key, NULL)) {
        uint64_t unique_id = (uint64_t)key;
        qemu_put_be64(f, unique_id);
    }
}

int load_thread_unique_ids(QEMUFile *f, GHashTable *thread_unique_ids) {
    guint unique_id_count = qemu_get_be32(f);

    for (guint i = 0; i < unique_id_count; i++) {
        uint64_t unique_id = qemu_get_be64(f);
        g_hash_table_insert(thread_unique_ids, GUINT_TO_POINTER(unique_id), GUINT_TO_POINTER(1));
    }

    return 0;
}


void save_egl_display(QEMUFile *f, Egl_Display *display) {
    guint num_configs = g_hash_table_size(display->egl_config_set);
    qemu_put_be32(f, num_configs);

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, display->egl_config_set);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        eglConfig *config = (eglConfig *)value;
        save_egl_config(f, config);
    }

    qemu_put_be32(f, display->guest_ver_major);
    qemu_put_be32(f, display->guest_ver_minor);
    qemu_put_be32(f, display->is_init);
}

int load_egl_display(QEMUFile *f, Egl_Display *display) {
    display->egl_config_set = g_hash_table_new(g_direct_hash, g_direct_equal);

    guint num_configs = qemu_get_be32(f);

    for (guint i = 0; i < num_configs; i++) {
        eglConfig *config = load_egl_config(f);
        g_hash_table_insert(display->egl_config_set, GUINT_TO_POINTER(config->config_id), config);
    }

    display->guest_ver_major = qemu_get_be32(f);
    display->guest_ver_minor = qemu_get_be32(f);
    display->is_init = qemu_get_be32(f);

    return 0;
}

void save_egl_config(QEMUFile *f, eglConfig *config) {
    qemu_put_be32(f, config->red_size);
    qemu_put_be32(f, config->green_size);
    qemu_put_be32(f, config->blue_size);
    qemu_put_be32(f, config->alpha_size);
    qemu_put_be32(f, config->bind_to_tex_rgb);
    qemu_put_be32(f, config->bind_to_tex_rgba);
    qemu_put_be32(f, config->caveat);
    qemu_put_be32(f, config->config_id);
    qemu_put_be32(f, config->frame_buffer_level);
    qemu_put_be32(f, config->depth_size);
    qemu_put_be32(f, config->max_pbuffer_width);
    qemu_put_be32(f, config->max_pbuffer_height);
    qemu_put_be32(f, config->max_pbuffer_size);
    qemu_put_be32(f, config->max_swap_interval);
    qemu_put_be32(f, config->min_swap_interval);
    qemu_put_be32(f, config->native_renderable);
    qemu_put_be32(f, config->renderable_type);
    qemu_put_be32(f, config->native_visual_id);
    qemu_put_be32(f, config->native_visual_type);
    qemu_put_be32(f, config->sample_buffers_num);
    qemu_put_be32(f, config->samples_per_pixel);
    qemu_put_be32(f, config->stencil_size);
    qemu_put_be32(f, config->luminance_size);
    qemu_put_be32(f, config->buffer_size);
    qemu_put_be32(f, config->surface_type);
    qemu_put_be32(f, config->transparent_type);
    qemu_put_be32(f, config->trans_red_val);
    qemu_put_be32(f, config->trans_green_val);
    qemu_put_be32(f, config->trans_blue_val);
    qemu_put_be32(f, config->conformant);
    qemu_put_be32(f, config->color_buffer_type);
    qemu_put_be32(f, config->alpha_mask_size);
    qemu_put_be32(f, config->recordable_android);
    qemu_put_be32(f, config->framebuffer_target_android);
}

eglConfig* load_egl_config(QEMUFile *f) {
    eglConfig *config = g_malloc0(sizeof(eglConfig));
    config->red_size = qemu_get_be32(f);
    config->green_size = qemu_get_be32(f);
    config->blue_size = qemu_get_be32(f);
    config->alpha_size = qemu_get_be32(f);
    config->bind_to_tex_rgb = qemu_get_be32(f);
    config->bind_to_tex_rgba = qemu_get_be32(f);
    config->caveat = qemu_get_be32(f);
    config->config_id = qemu_get_be32(f);
    config->frame_buffer_level = qemu_get_be32(f);
    config->depth_size = qemu_get_be32(f);
    config->max_pbuffer_width = qemu_get_be32(f);
    config->max_pbuffer_height = qemu_get_be32(f);
    config->max_pbuffer_size = qemu_get_be32(f);
    config->max_swap_interval = qemu_get_be32(f);
    config->min_swap_interval = qemu_get_be32(f);
    config->native_renderable = qemu_get_be32(f);
    config->renderable_type = qemu_get_be32(f);
    config->native_visual_id = qemu_get_be32(f);
    config->native_visual_type = qemu_get_be32(f);
    config->sample_buffers_num = qemu_get_be32(f);
    config->samples_per_pixel = qemu_get_be32(f);
    config->stencil_size = qemu_get_be32(f);
    config->luminance_size = qemu_get_be32(f);
    config->buffer_size = qemu_get_be32(f);
    config->surface_type = qemu_get_be32(f);
    config->transparent_type = qemu_get_be32(f);
    config->trans_red_val = qemu_get_be32(f);
    config->trans_green_val = qemu_get_be32(f);
    config->trans_blue_val = qemu_get_be32(f);
    config->conformant = qemu_get_be32(f);
    config->color_buffer_type = qemu_get_be32(f);
    config->alpha_mask_size = qemu_get_be32(f);
    config->recordable_android = qemu_get_be32(f);
    config->framebuffer_target_android = qemu_get_be32(f);

    return config;
}



// bool compare_two_textures(const struct Express_Native_Texture* texture1, const struct Express_Native_Texture* texture2) {
//     if (texture1 == NULL || texture2 == NULL) {
//         return false;
//     }

//     if (texture1->textureId != texture2->textureId) return false;
//     if (texture1->target != texture2->target) return false;
//     if (texture1->width != texture2->width) return false;
//     if (texture1->height != texture2->height) return false;
//     if (texture1->depth != texture2->depth) return false;
//     if (texture1->internalFormat != texture2->internalFormat) return false;

//     if (texture1->pixels && texture2->pixels) {
//         size_t size1 = texture1->width * texture1->height * 4;
//         size_t size2 = texture2->width * texture2->height * 4;
//         if (size1 != size2) {
//             return false;
//         }
//         if (memcmp(texture1->pixels, texture2->pixels, size1) != 0) {
//             return false;
//         }
//     } else if (texture1->pixels != texture2->pixels) {
//         return false;
//     }
//     return true;
// }


// bool compare_texture(Express_Native_Texture* native_texture) {
//     Express_Native_Texture* current_texture = g_malloc0(sizeof(struct Express_Native_Texture));
//     current_texture->textureId = native_texture->textureId;
//     current_texture->target = native_texture->target; //ztodo:type会change吗 

//     glBindTexture(current_texture->target, current_texture->textureId);



//     glGetTexLevelParameteriv(current_texture->target, 0, GL_TEXTURE_WIDTH, &current_texture->width);
//     glGetTexLevelParameteriv(current_texture->target, 0, GL_TEXTURE_HEIGHT, &current_texture->height);
//     if (current_texture->target == GL_TEXTURE_3D) {
//         glGetTexLevelParameteriv(current_texture->target, 0, GL_TEXTURE_DEPTH, &current_texture->depth);
//     } else {
//         current_texture->depth = 0;
//     }
//     glGetTexLevelParameteriv(current_texture->target, 0, GL_TEXTURE_INTERNAL_FORMAT, &current_texture->internalFormat);

//     // glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&current_texture->binding2D);

//     // glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, (GLint *)&current_texture->bindingCubeMap);

//     // glGetTexParameteriv(current_texture->target, GL_TEXTURE_MIN_FILTER, &current_texture->minFilter);
//     // glGetTexParameteriv(current_texture->target, GL_TEXTURE_MAG_FILTER, &current_texture->magFilter);
//     // glGetTexParameteriv(current_texture->target, GL_TEXTURE_WRAP_S, &current_texture->wrapS);
//     // glGetTexParameteriv(current_texture->target, GL_TEXTURE_WRAP_T, &current_texture->wrapT); //ztodo:这些还没存
    
//     GLint size = current_texture->width * current_texture->height * 4;
//     current_texture->pixels = (GLubyte *)malloc(size); //ztodo:记得free
//     glGetTexImage(current_texture->target, 0, current_texture->internalFormat, GL_UNSIGNED_BYTE, current_texture->pixels); //ztodo:第二个、倒数第二个参数
//     return compare_two_textures(native_texture, current_texture);

// }