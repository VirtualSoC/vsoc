


express_gpu = ss.source_set()

express_gpu.add(when: 'CONFIG_EXPRESS_GPU',
               if_true: files(
                    'express_gpu_render.c',
                    'offscreen_render_thread.c',
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
                    'device_gtkinterface.c'
               ))

glfw = cc.find_library('glfw3')
opengl32 = cc.find_library('opengl32')
gtk3 = dependency('gtk+-3.0')
express_gpu.add(gtk3)
express_gpu.add(glfw)
express_gpu.add(opengl32)



softmmu_ss.add_all(express_gpu)


