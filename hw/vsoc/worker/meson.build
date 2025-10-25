# The vsoc worker module needs to be included after the dependencies have be defined 

worker_ss = ss.source_set()

worker_ss.add(files(
    'main.c', 
))

if targetos == 'linux'
  # Provide a standalone static library so external programs can link it.
  vsoc_static = static_library('vsoc_worker',
      sources: vsoc_ss.all_sources() + [
          files(
            'platform.c',
            'device.c',
            'guestmem.c',
            '../../../ui/input-keymap.c',
            ),
      ],
      include_directories: include_directories('..', '.'),
      pic: true,
  )

  # Use link_whole so all objects from vsoc_static are included in the final
  # executable. This ensures constructor-based device registration (EXPRESS_DEVICE_INIT)
  # objects like express_gpu.c are not dropped by the linker.
  vsoc_dep = declare_dependency(link_whole: vsoc_static,
          dependencies: [libm, glib, migration, io, chardev, blockdev, cimgui, backtrace, glfw, vulkan, avcodec, avdevice, avfilter, avutil, swresample, swscale])

  # worker executable
  executable('vsoc-worker',
      sources: worker_ss.all_sources(),
      dependencies: [vsoc_dep],
      include_directories: include_directories('..', '.'),
      install: true,
  )
endif