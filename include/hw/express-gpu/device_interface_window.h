#ifndef DEVICE_INTERFACE_H
#define DEVICE_INTERFACE_H
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#define CIMGUI_USE_GLFW
#define CIMGUI_USE_OPENGL3
#include "hw/express-gpu/cimgui/cimgui.h"
#include "hw/express-gpu/cimgui/cimgui_impl.h"
#include <stdbool.h>
#include <GLFW/glfw3.h>

typedef struct Accelerometer_Data {
    float scale;
    int x;
    int y;
    int z;
} Accelerometer_Data;

typedef struct Magnetic_Data {
    float scale_x;
    float scale_y;
    float scale_z;
    int x;
    int y;
    int z;
} Magnetic_Data;

typedef struct Light_Data {
    float scale;
    int input;
} Light_Data;

typedef struct Gyroscope_Data {
    float scale;
    int x;
    int y;
    int z;
} Gyroscope_Data;

typedef struct Device_Interface_Data{
    Accelerometer_Data *acc_data;
    Magnetic_Data *mag_data;
    Light_Data *lig_data;
    Gyroscope_Data *gyr_data;
    volatile int *run;
}Device_Interface_Data;

void *interface_window_thread(void *data);
void handle_battery_change(int current_battery);
void handle_accelerometer_change(float scale, int x, int y, int z);
void handle_magnetic_change(float scale_x, float scale_y, float scale_z, int x, int y, int z);
void handle_light_change(float scale,int input);
void handle_gyroscope_change(float scale, int x, int y, int z);

#endif