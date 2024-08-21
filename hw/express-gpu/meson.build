express_gpu = ss.source_set()
targetos = host_machine.system()
if targetos == 'windows'

express_gpu.add(when: 'CONFIG_EXPRESS_GPU',
               if_true: files(
                    'express_gpu_render.c',
                    'express_gpu.c',
                    'glad.c',
                    'glv3_context.c',
                    'glv3_trans.c',
                    'glv3_mem.c',
                    'glv3_texture.c',
                    'glv3_vertex.c',
                    'glv3_resource.c',
                    'glv3_program.c',
                    'glv3_status.c',
                    'egl_trans.c',
                    'egl_surface.c',
                    'egl_config.c',
                    'egl_display.c',
                    'egl_display_wgl.c',
                    'egl_context.c',
                    'egl_draw.c',
                    'egl_sync.c',
                    'glv1.c',
                    'gl_helper.c',
                    'egl_window_wgl.c',
                    'test_trans.c',
                    'device_interface_window.c',
                    'express_display.c',
               ))

glfw = cc.find_library('glfw3')
opengl32 = cc.find_library('opengl32')
cimgui = cc.find_library('cimgui')
express_gpu.add(cimgui)
express_gpu.add(glfw)
express_gpu.add(opengl32)


softmmu_ss.add_all(express_gpu)

elif targetos == 'darwin'
    # macOS 系统下的配置
    message('Building for macOS')
    # 添加 macOS 相关的构建选项、链接库等
express_gpu.add(when: 'CONFIG_EXPRESS_GPU',
               if_true: files(
                    'express_gpu_render.c',
                    'express_gpu.c',
                    'glad.c',
                    'glv3_context.c',
                    'glv3_trans.c',
                    'egl_display_darwin.c',
                    'egl_window_nsgl.m',
                    'glv3_mem.c',
                    'glv3_texture.c',
                    'glv3_vertex.c',
                    'glv3_resource.c',
                    'glv3_program.c',
                    'glv3_status.c',
                    'egl_trans.c',
                    'egl_surface.c',
                    'egl_config.c',
                    'egl_display.c',
                    'egl_context.c',
                    'egl_draw.c',
                    'egl_sync.c',
                    'glv1.c',
                    'gl_helper.c',
                    'test_trans.c',
                    'device_interface_window.c',
                    'express_display.c',
               ))

glfw = cc.find_library('glfw')
glew = cc.find_library('GLEW')
#opengl32 = cc.find_library('opengl32')
opengl32 = dependency('appleframeworks', modules : ['OpenGL'])
Cocoa = dependency('appleframeworks', modules: ['Cocoa', 'CoreVideo'],
                   required: get_option('cocoa'))
IOKit = dependency('appleframeworks', modules: 'IOKit')
AppKit = dependency('appleframeworks', modules: 'AppKit')
CoreFoundation = dependency('appleframeworks', modules: 'CoreFoundation')
cimgui = cc.find_library('cimgui')
express_gpu.add(cimgui)
express_gpu.add(glfw)
express_gpu.add(glew)
express_gpu.add(opengl32)

express_gpu.add(Cocoa)
express_gpu.add(IOKit)
express_gpu.add(CoreFoundation)
express_gpu.add(AppKit)

softmmu_ss.add_all(express_gpu)

endif
