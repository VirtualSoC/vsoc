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

typedef struct device_interface_data{
    char device[16];       // check FOREACH_DEVICE
    char datatype[16];     // check DATA_TYPE_STRING
    char *data[MAX_ARRAY_LEN];     // retrun data array
    int data_len;                  // the len of data array
    void (* senddata_callback)(char *device, char *datatype, int data_len, char *data[data_len]);
    bool run;
}device_interface_data;

void *create_interface(void *data);

#endif