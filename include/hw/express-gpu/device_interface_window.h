#ifndef DEVICE_INTERFACE_H
#define DEVICE_INTERFACE_H
#include "hw/express-network/express_modem.h"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#define CIMGUI_USE_GLFW
#define CIMGUI_USE_OPENGL3
#include "hw/express-gpu/cimgui/cimgui.h"
#include "hw/express-gpu/cimgui/cimgui_impl.h"
#include <stdbool.h>
#include "hw/express-gpu/GLFW/glfw3.h"

typedef struct Accelerometer_Data {
    float scale;
    float sample_hz;
    float temperature;
    float voltage;
    int x;
    int y;
    int z;
} Accelerometer_Data;

typedef struct Gyroscope_Data {
    float scale;
    float sample_hz;
    float temperature;
    float voltage;
    int x;
    int y;
    int z;
} Gyroscope_Data;
typedef struct Battery_Data {
    bool charge;
    float current_now;
    float charge_full;
    float voltage;
    float temperature; // temperature of the power supply.
    int status;
    int health;
    int present;
    int capacity; // capacity in percents
    int technology;
    int cycle_count;
    int charge_counter;
} Battery_Data;

typedef struct GPS_Data {
    double lat;
    double lon;
    float ground_speed;
    float altitude;
    float speed_dir;
    float hdop;
    float pdop;
    float vdop;
    float sec;
    int year;
    int month;
    int day;
    int hr;
    int min;
    int num_sv;
    int sv_prns[12];
    int num_sv_inview;
    int sv_inview[16][4];
} GPS_Data;

typedef struct Mic_Data {
    bool using_mic;
    bool start_capture;
    char file_path[100];
} Mic_Data;
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

#include "include/hw/express-network/em_input.h"

typedef struct Device_Interface_Data{
    Accelerometer_Data *acc_data;
    Magnetic_Data *mag_data;
    Light_Data *lig_data;
    Gyroscope_Data *gyr_data;
    volatile int *run;
}Device_Interface_Data;

void *interface_window_thread(void *data);
void handle_battery_change(int property,int current_battery);
void handle_accelerometer_change(int property, int value);
void handle_magnetic_change(float scale_x, float scale_y, float scale_z, int x, int y, int z);
void handle_light_change(float scale,int input);
void handle_gyroscope_change(int property, int value);
void handle_gps_change(int property, int value);
void handle_modem_change(int slot, int property);

#endif