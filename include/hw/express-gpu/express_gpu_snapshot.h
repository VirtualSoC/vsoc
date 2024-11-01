#ifndef EXPRESS_GPU_SNAPSHOT_H
#define EXPRESS_GPU_SNAPSHOT_H

// #include "migration/qemu-file.h"
// #include "hw/teleport-express/express_device_common.h"
// #include "hw/teleport-express/teleport_express_call.h"

// #include "hw/express-gpu/glv3_context.h"
// #include "hw/express-gpu/egl_display.h"
// #include "hw/express-gpu/egl_surface.h"
#include "hw/teleport-express/teleport_express_call.h"
#include "hw/teleport-express/teleport_express_distribute.h"
#include "hw/teleport-express/teleport_express_register.h"

#include "hw/teleport-express/express_log.h"

#include "hw/virtio/virtio.h"

#include "hw/express-gpu/express_gpu.h"

int save_single_render_thread_context(QEMUFile *f, Render_Thread_Context *thread_context);
int load_single_render_thread_context(QEMUFile *f, Render_Thread_Context *thread_context);

int save_thread_context(QEMUFile *f, Thread_Context *context);
int load_thread_context(QEMUFile *f, Thread_Context *context);

void save_process_context(QEMUFile *f, Process_Context *process_context);
void load_process_context(QEMUFile *f, Process_Context *process_context);

void save_window_buffer(QEMUFile *f, Window_Buffer *buffer);
Window_Buffer* load_window_buffer(QEMUFile *f);

void save_scatter_data(QEMUFile *f, Scatter_Data *scatter_data, int count);
Scatter_Data* load_scatter_data(QEMUFile *f, int *count);

void save_guest_mem(QEMUFile *f, Guest_Mem *guest_mem);
Guest_Mem* load_guest_mem(QEMUFile *f);

void save_opengl_context(QEMUFile *f, Opengl_Context *context);
Opengl_Context* load_opengl_context(QEMUFile *f);

void save_bound_buffer(QEMUFile *f, Bound_Buffer *buffer);
Bound_Buffer* load_bound_buffer(QEMUFile *f);

void save_buffer_status(QEMUFile *f, Buffer_Status *status);
Buffer_Status* load_buffer_status(QEMUFile *f);

void save_texture_binding_status(QEMUFile *f, Texture_Binding_Status *status);
Texture_Binding_Status* load_texture_binding_status(QEMUFile *f);

void save_resource_context(QEMUFile *f, Resource_Context *context);
Resource_Context* load_resource_context(QEMUFile *f);

void save_resource_map_status(QEMUFile *f, Resource_Map_Status *status);
Resource_Map_Status* load_resource_map_status(QEMUFile *f);

void save_egl_display(QEMUFile *f, Egl_Display *display);
Egl_Display* load_egl_display(QEMUFile *f);

void save_egl_config(QEMUFile *f, eglConfig *config);
eglConfig* load_egl_config(QEMUFile *f);

void save_hardware_buffer(QEMUFile *f, Hardware_Buffer *buffer);
Hardware_Buffer* load_hardware_buffer(QEMUFile *f);

void save_thread_unique_ids(QEMUFile *f, GHashTable *thread_unique_ids);
GHashTable* load_thread_unique_ids(QEMUFile *f);

void save_resource_context(QEMUFile *f, Resource_Context *context);
Resource_Context* load_resource_context(QEMUFile *f);

void save_attrib_point(QEMUFile *f, Attrib_Point *point);
Attrib_Point* load_attrib_point(QEMUFile *f);

void save_guest_host_map(QEMUFile *f, Guest_Host_Map *map);
Guest_Host_Map* load_guest_host_map(QEMUFile *f);

#endif