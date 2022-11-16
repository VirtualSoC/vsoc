#ifndef DEVICE_INTERFACE_H
#define DEVICE_INTERFACE_H
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#define CIMGUI_USE_SDL
#define CIMGUI_USE_OPENGL3
#include "hw/express-gpu/cimgui/cimgui.h"
#include "hw/express-gpu/cimgui/cimgui_impl.h"
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdbool.h>

#ifdef _MSC_VER
#include <windows.h>
#endif

#ifdef IMGUI_HAS_IMSTR
#define igBegin igBegin_Str
#define igSliderFloat igSliderFloat_Str
#define igCheckbox igCheckbox_Str
#define igColorEdit3 igColorEdit3_Str
#define igButton igButton_Str
#endif


#define MAX_INPUT_BUFFER 256
#define MAX_ARRAY_LEN 20



typedef struct accelerometer_data {
    float scale;
    int x;
    int y;
    int z;
} accelerometer_data;

typedef struct magnetic_data {
    float scale_x;
    float scale_y;
    float scale_z;
    int x;
    int y;
    int z;
} magnetic_data;

typedef struct light_data {
    float scale;
    int input;
} light_data;

typedef struct gyroscope_data {
    float scale;
    int x;
    int y;
    int z;
} gyroscope_data;

typedef struct device_interface_data{
    accelerometer_data *acc_data;
    magnetic_data *mag_data;
    light_data *lig_data;
    gyroscope_data *gyr_data;
    volatile int *run;
}device_interface_data;

void *create_interface(void *data);
void handle_battery_change(int current_battery);
void handle_accelerometer_change(float scale, int x, int y, int z);
void handle_magnetic_change(float scale_x, float scale_y, float scale_z, int x, int y, int z);
void handle_light_change(float scale,int input);
void handle_gyroscope_change(float scale, int x, int y, int z);

#endif