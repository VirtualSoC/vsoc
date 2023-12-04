
#include "hw/express-gpu/device_interface_window.h"
#include "hw/express-gpu/cimgui/cimgui.h"

#define TIMER_LOG
//#define STD_DEBUG_LOG
#include "hw/teleport-express/express_log.h"

#include "hw/express-sensor/express_battery.h"
#include "hw/express-sensor/express_accel.h"
#include "hw/express-sensor/express_gyro.h"
#include "hw/express-sensor/express_gps.h"
#include "hw/express-sensor/express_mic.h"
#include "hw/express-network/express_modem.h"


#define IM_COL32_R_SHIFT 0
#define IM_COL32_G_SHIFT 8
#define IM_COL32_B_SHIFT 16
#define IM_COL32_A_SHIFT 24
#define IM_COL32_A_MASK 0xFF000000
#define IM_COL32(R, G, B, A) (((ImU32)(A) << IM_COL32_A_SHIFT) | ((ImU32)(B) << IM_COL32_B_SHIFT) | ((ImU32)(G) << IM_COL32_G_SHIFT) | ((ImU32)(R) << IM_COL32_R_SHIFT))
#define IM_COL32_WHITE IM_COL32(255, 255, 255, 255) // Opaque white = 0xFFFFFFFF
#define IM_COL32_BLACK IM_COL32(0, 0, 0, 255)       // Opaque black
#define IM_COL32_BLACK_TRANS IM_COL32(0, 0, 0, 0)   // Transparent black = 0x00000000

#define LISTEN_INPUT_CHANGE(func, ...)  \
    if (igIsItemDeactivatedAfterEdit()) \
    {                                   \
        func(__VA_ARGS__);              \
    }

#define LISTEN_COMBO_CHANGE(func, ...)  \
    if (igIsItemDeactivatedAfterEdit()) \
    {                                   \
        func(__VA_ARGS__);              \
    }

static Device_Interface_Data all_interface_data;
static Accelerometer_Data cur_acc = {
    .scale = 0.78125,
    .sample_hz = 800,
    .temperature = 30.0,
    .voltage = 0.23,
    .x = 0,
    .y = 0,
    .z = 0};

static Gyroscope_Data cur_gyro = {
    .scale = 0.23145,
    .sample_hz = 200,
    .temperature = 30.0,
    .voltage = 0.23,
    .x = 0,
    .y = 0,
    .z = 0};

static GPS_Data cur_gps = {
    .lat = 116.326759,
    .lon = 40.003304,
    .ground_speed = 0,
    .altitude = 105.0,
    .hdop = 1.0,
    .vdop = 1.3,
    .pdop = 1.7,
    .speed_dir = 0,
    .year = 2022,
    .month = 12,
    .day = 30,
    .hr = 12,
    .min = 0,
    .sec = 0,
    .num_sv = 9,
    .sv_prns = {22, 11, 27, 01, 03, 02, 10, 21, 19, 0, 0, 0},
    .num_sv_inview = 15,
    .sv_inview =
        {// satellites in view
         {26, 25, 138, 53},
         {16, 25, 91, 67},
         {01, 51, 238, 77},
         {02, 45, 85, 41},
         {03, 38, 312, 01},
         {30, 68, 187, 37},
         {11, 22, 49, 44},
         {9, 67, 76, 71},
         {10, 14, 177, 12},
         {19, 86, 235, 37},
         {21, 84, 343, 95},
         {22, 77, 040, 66},
         {8, 50, 177, 60},
         {6, 81, 336, 46},
         {27, 63, 209, 83},
         {0, 0, 0, 0}}};

static Battery_Data cur_battery = {
    .charge = false,
    .status = POWER_SUPPLY_STATUS_NOT_CHARGING,
    .health = POWER_SUPPLY_HEALTH_GOOD,
    .capacity = 100, // capacity in percents
    .voltage = 4.0,
    .temperature = 25.0, // temperature of the power supply.
    .technology = POWER_SUPPLY_TECHNOLOGY_LION,
    .cycle_count = 10,
    .current_now = 900,
    .charge_full = 5000,
    .charge_counter = 10000};

static Mic_Data cur_mic = {
    .using_mic = true,
    .start_capture = false,
    .file_path = ""};

const char *battery_tech_name[] = {
    "UNKNOWN",
    "NiMH",
    "LION",
    "LIPO",
    "LiFe",
    "NiCd",
    "LiMn",
};

// POWER_SUPPLY_PROP_HEALTH
const char *battery_health_name[] = {
    "UNKNOWN",
    "GOOD",
    "OVERHEAT",
    "DEAD",
    "OVERVOLTAGE",
    "UNSPEC_FAILURE",
};

// POWER_SUPPLY_PROP_STATUS
const char *battery_status_name[] = {
    "UNKNOWN",
    "CHARGING",
    "DISCHARGING",
    "NOT CHARGING",
    "FULL",
};

// EXPRESS_MODEM_SIGNAL_QUALITY
const char *modem_signal_quality[] = {
    "NONE",
    "POOR",
    "MODERATE",
    "GOOD",
    "GREAT"
};

const char *modem_data_network_type[] = {
    "UNKNOWN",
    "GPRS",
    "EDGE",
    "UMTS",
    "LTE",
    "NR"
};

const char *modem_registration_type[] = {
    "UNREGISTERED",
    "HOME",
    "SEARCHING",
    "DENIED",
    "UNKNOWN",
    "ROAMING"
};

static Magnetic_Data cur_mag = {.scale_x = 0, .scale_y = 0, .scale_z = 0, .x = 0, .y = 0, .z = 0};
static Light_Data cur_light = {.scale = 0, .input = 0};
static ImVec2 window_size;
static float window_dpi_scale = 1.0;
GLFWwindow *window = NULL;

void handle_battery_change(int property, int value)
{
    LOGI("Device_interface::current_battery: %d\n", value);

    express_battery_status_changed(property, value);
    sync_express_battery_status();
}

static void handle_plugin_change(bool value)
{
    express_ac_plug_status_changed(value);
    sync_express_battery_status();
}

static void handle_mic_change(bool value)
{
    cur_mic.using_mic = value;
}

void handle_accelerometer_change(int property, int value)
{
    LOGI("Device_interface::accelerometer scale: %.2f, x: %d, y: %d, z: %d\n", cur_acc.scale, cur_acc.x, cur_acc.y, cur_acc.z);

    express_accel_status_changed(property, value);
    sync_express_accel_status();
}

void handle_magnetic_change(float scale_x, float scale_y, float scale_z, int x, int y, int z)
{
    LOGI("Device_interface::magnetic scale x: %.2f, scale y: %.2f, scale z: %.2f, x: %d, y: %d, z:%d\n", scale_x, scale_y, scale_z, x, y, z);
}

void handle_light_change(float scale, int input)
{
    LOGI("Device_interface::light scale: %.2f, input: %d\n", scale, input);
}

void handle_gyroscope_change(int property, int value)
{
    LOGI("Device_interface::gyroscope scale: %.2f, x: %d, y: %d, z: %d", cur_gyro.scale, cur_gyro.x, cur_gyro.y, cur_gyro.z);
    express_gyro_status_changed(property, value);
    sync_express_gyro_status();
}

void handle_gps_change(int property, int value)
{
    LOGI("Device_interface::gps latitude: %.6f, longitude: %.6f", cur_gps.lat, cur_gps.lon);
    express_gps_status_changed(property, value);
    sync_express_gps_status();
}

void handle_modem_change(int slot, int property)
{
    express_modem_status_changed(slot, property);
    sync_express_modem_status();
}

static void glfw_error_callback(int error, const char *description)
{
    LOGE("Device_interface::Glfw Error %d: %s", error, description);
    return;
}

static void update_window_scale(void)
{
    float new_dpi_scale = igGetWindowDpiScale();
    if (new_dpi_scale == 0.0f)
    {
        new_dpi_scale = 1.0f;
    }
    if (window_dpi_scale != new_dpi_scale)
    {
        window_size.x *= new_dpi_scale / (window_dpi_scale);
        window_size.y *= new_dpi_scale / (window_dpi_scale);
        window_dpi_scale = new_dpi_scale;
        igSetWindowFontScale(new_dpi_scale);
    }
}

static void igToggleButton(const char *str_id, bool *v, void (*func)(bool value))
{
    ImVec2 p;
    igGetCursorScreenPos(&p);
    ImDrawList *draw_list = igGetWindowDrawList();

    float height = igGetFrameHeight();
    float width = height * 1.55f;
    float radius = height * 0.50f;
    ImVec2 bwidth = {width, height};

    if (igInvisibleButton(str_id, bwidth, 0))
    {
        *v = !*v;
        func(*v);
    }

    float t = *v ? 1.0f : 0.0f;

    ImGuiContext *g = igGetCurrentContext();
    float ANIM_SPEED = 0.08f;
    if (g->LastActiveId == ImGuiWindow_GetID_Str(igGetCurrentWindow(), str_id, NULL)) // && g.LastActiveIdTimer < ANIM_SPEED)
    {
        float t_anim = igImSaturate(g->LastActiveIdTimer / ANIM_SPEED);
        t = *v ? (t_anim) : (1.0f - t_anim);
    }

    ImU32 col_bg;

    if (igIsItemHovered(0))
    {
        ImVec4 out, color1 = {0.78f, 0.78f, 0.78f, 1.0f}, color2 = {0.64f, 0.83f, 0.34f, 1.0f};
        igImLerp_Vec4(&out, color1, color2, t);
        col_bg = igGetColorU32_Vec4(out);
    }
    else
    {
        ImVec4 out, color1 = {0.85f, 0.85f, 0.85f, 1.0f}, color2 = {0.56f, 0.83f, 0.26f, 1.0f};
        igImLerp_Vec4(&out, color1, color2, t);
         col_bg = igGetColorU32_Vec4(out);
    }

    ImVec2 filled = {p.x + width, p.y + height};
    ImDrawList_AddRectFilled(draw_list, p, filled, col_bg, height * 0.5f, 0);
    filled.x = p.x + radius + t * (width - radius * 2.0f);
    filled.y = p.y + radius;
    ImDrawList_AddCircleFilled(draw_list, filled, radius - 1.5f, IM_COL32_WHITE, 0);
}

static void draw_window(bool *show_imgui)
{
    // start imgui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();
    igSetNextWindowSize(window_size, 0);
    // show a simple window that we created ourselves
    {
        igBegin("Device Input", show_imgui, 0);
        // fixed window ratio at 16:9, height = width * 9.0/16.0;
        window_size.x = igGetWindowWidth();
        window_size.y = window_size.x * 0.5625;
        update_window_scale();
        // Battery
        if (igCollapsingHeader_TreeNodeFlags("Battery", 0))
        {
            igSliderInt("\%", &cur_battery.capacity, 0, 100, "%d", 0);
            LISTEN_INPUT_CHANGE(handle_battery_change, POWER_SUPPLY_PROP_CAPACITY, cur_battery.capacity)
            igSameLine(0.0f, -1.0f);
            igText("Charged");
            igSameLine(0.0f, -1.0f);
            igToggleButton("##batterycharge", &cur_battery.charge, handle_plugin_change);

            igText("Technology:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.18f);
            if (igCombo_Str_arr("##batterytech", &cur_battery.technology, battery_tech_name, 7, 7))
                handle_battery_change(POWER_SUPPLY_PROP_TECHNOLOGY, cur_battery.technology);
            igSameLine(0.0f, -1.0f);
            igText("Health:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.18f);
            if (igCombo_Str_arr("##batteryhealth", &cur_battery.health, battery_health_name, 6, 6))
                handle_battery_change(POWER_SUPPLY_PROP_HEALTH, cur_battery.health);
            igSameLine(0.0f, -1.0f);
            igText("Status:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.18f);
            if (igCombo_Str_arr("##batterystatus", &cur_battery.status, battery_status_name, 5, 5))
                handle_battery_change(POWER_SUPPLY_PROP_STATUS, cur_battery.status);

            igText("Voltage:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.17f);
            igInputFloat("v", &cur_battery.voltage, 0.000001, 0.1, "%.6f", 0);
            LISTEN_INPUT_CHANGE(handle_battery_change, POWER_SUPPLY_PROP_VOLTAGE_NOW, (int)(cur_battery.voltage * 1000000))
            igSameLine(0.0f, -1.0f);
            igText("Cycle Count:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.15f);
            igInputInt("##batterycycle", &cur_battery.cycle_count, 1, 5, 0);
            LISTEN_INPUT_CHANGE(handle_battery_change, POWER_SUPPLY_PROP_CYCLE_COUNT, cur_battery.cycle_count)
            igSameLine(0.0f, -1.0f);
            igText("Temperature:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.15f);
            igInputFloat("##batterytemp", &cur_battery.temperature, 0.1, 5, "%.1f", 0);
            LISTEN_INPUT_CHANGE(handle_battery_change, POWER_SUPPLY_PROP_TEMP, (int)(cur_battery.temperature * 10))

            igText("Current:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.16f);
            igInputFloat("mA", &cur_battery.current_now, 0.001, 5, "%.3f", 0);
            LISTEN_INPUT_CHANGE(handle_battery_change, POWER_SUPPLY_PROP_CURRENT_NOW, (int)(cur_battery.current_now * 1000))
            igSameLine(0.0f, -1.0f);
            igText("Capacity:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.18f);
            igInputFloat("mAH", &cur_battery.charge_full, 0.001, 5, "%.4f", 0);
            LISTEN_INPUT_CHANGE(handle_battery_change, POWER_SUPPLY_PROP_CHARGE_FULL, (int)(cur_battery.charge_full * 1000))
            igSameLine(0.0f, -1.0f);
            igText("Charge Count:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.14f);
            igInputInt("##batterycount", &cur_battery.charge_counter, 1, 5, 0);
            LISTEN_INPUT_CHANGE(handle_battery_change, POWER_SUPPLY_PROP_CHARGE_COUNTER, cur_battery.charge_counter)
        }

        // Accelerometer
        if (igCollapsingHeader_TreeNodeFlags("Accelerometer", 0))
        {
            igText("Scale:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputFloat("##accscale", &cur_acc.scale, 0.000001, 0.1, "%.6f", 0);
            LISTEN_INPUT_CHANGE(handle_accelerometer_change, EXPRESS_ACCEL_SCALE, (int)(cur_acc.scale * 1000000))
            igSameLine(0.0f, -1.0f);
            igText("Sample Frequency:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputFloat("##accsample_hz", &cur_acc.sample_hz, 0.000001, 0.1, "%.6f", 0);
            LISTEN_INPUT_CHANGE(handle_accelerometer_change, EXPRESS_ACCEL_SAMPLE_FREQUENCY, (int)(cur_acc.sample_hz * 1000000))

            igText("Temperature:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputFloat("Celsyus", &cur_acc.temperature, 0.01, 0.1, "%.2f", 0);
            LISTEN_INPUT_CHANGE(handle_accelerometer_change, EXPRESS_ACCEL_TEMPERATURE, (int)(cur_acc.temperature * 1000000))
            igSameLine(0.0f, -1.0f);
            igText("Voltage:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputFloat("v", &cur_acc.voltage, 0.000001, 0.1, "%.6f", 0);
            LISTEN_INPUT_CHANGE(handle_accelerometer_change, EXPRESS_ACCEL_VOLTAGE, (int)(cur_acc.voltage * 1000000))

            igText("x:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##accx", &cur_acc.x, 1, 5, 0);
            LISTEN_INPUT_CHANGE(handle_accelerometer_change, EXPRESS_ACCEL_X, cur_acc.x)
            igSameLine(0.0f, -1.0f);
            igText("y:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##accy", &cur_acc.y, 1, 5, 0);
            LISTEN_INPUT_CHANGE(handle_accelerometer_change, EXPRESS_ACCEL_Y, cur_acc.y)
            igSameLine(0.0f, -1.0f);
            igText("z:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##accz", &cur_acc.z, 1, 5, 0);
            LISTEN_INPUT_CHANGE(handle_accelerometer_change, EXPRESS_ACCEL_Z, cur_acc.z)
        }
        // Gyroscope
        if (igCollapsingHeader_TreeNodeFlags("Gyroscope", 0))
        {
            igText("Scale:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputFloat("##gyrocale", &cur_gyro.scale, 0.000001, 0.1, "%.6f", 0);
            LISTEN_INPUT_CHANGE(handle_gyroscope_change, EXPRESS_GYRO_SCALE, (int)(cur_gyro.scale * 1000000))
            igSameLine(0.0f, -1.0f);
            igText("Sample Frequency:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputFloat("##gyrosample_hz", &cur_gyro.sample_hz, 0.000001, 0.1, "%.6f", 0);
            LISTEN_INPUT_CHANGE(handle_gyroscope_change, EXPRESS_GYRO_SAMPLE_FREQUENCY, (int)(cur_gyro.sample_hz * 1000000))

            igText("Temperature:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputFloat("Celsyus", &cur_gyro.temperature, 0.01, 0.1, "%.2f", 0);
            LISTEN_INPUT_CHANGE(handle_gyroscope_change, EXPRESS_GYRO_TEMPERATURE, (int)(cur_gyro.temperature * 1000000))
            igSameLine(0.0f, -1.0f);
            igText("Voltage:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputFloat("v", &cur_gyro.voltage, 0.000001, 0.1, "%.6f", 0);
            LISTEN_INPUT_CHANGE(handle_gyroscope_change, EXPRESS_GYRO_VOLTAGE, (int)(cur_gyro.voltage * 1000000))

            igText("x:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##gyrox", &cur_gyro.x, 1, 5, 0);
            LISTEN_INPUT_CHANGE(handle_gyroscope_change, EXPRESS_GYRO_X, cur_gyro.x)
            igSameLine(0.0f, -1.0f);
            igText("y:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##gyroy", &cur_gyro.y, 1, 5, 0);
            LISTEN_INPUT_CHANGE(handle_gyroscope_change, EXPRESS_GYRO_Y, cur_gyro.y)
            igSameLine(0.0f, -1.0f);
            igText("z:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##gyroz", &cur_gyro.z, 1, 5, 0);
            LISTEN_INPUT_CHANGE(handle_gyroscope_change, EXPRESS_GYRO_Z, cur_gyro.z)
        }

        // GPS
        if (igCollapsingHeader_TreeNodeFlags("GPS", 0))
        {
            igText("Latitude:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputDouble("##gpslat", &cur_gps.lat, 0.0000001, 0.1, "%.7f", 0);
            LISTEN_INPUT_CHANGE(handle_gps_change, EXPRESS_GPS_LATITUDE, (int)(cur_gps.lat * 10000000))
            igSameLine(0.0f, -1.0f);
            igText("Longitude:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputDouble("##gpslon", &cur_gps.lon, 0.0000001, 0.1, "%.7f", 0);
            LISTEN_INPUT_CHANGE(handle_gps_change, EXPRESS_GPS_LONGITUDE, (int)(cur_gps.lon * 10000000))

            igText("Speed:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputFloat("km/hr", &cur_gps.ground_speed, 0.1, 1, "%.1f", 0);
            LISTEN_INPUT_CHANGE(handle_gps_change, EXPRESS_GPS_GROUND_SPEED, (int)(cur_gps.ground_speed * 10))
            igSameLine(0.0f, -1.0f);
            igText("Heading Direction:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputFloat("##gpscourse", &cur_gps.speed_dir, 0.1, 1, "%.1f", 0);
            LISTEN_INPUT_CHANGE(handle_gps_change, EXPRESS_GPS_SPEED_DIRECTION, (int)(cur_gps.speed_dir * 10))
            igSameLine(0.0f, -1.0f);
            igText("Altitude:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputFloat("m", &cur_gps.altitude, 0.1, 0.1, "%.1f", 0);
            LISTEN_INPUT_CHANGE(handle_gps_change, EXPRESS_GPS_ALTITUDE, (int)(cur_gps.altitude * 10))

            igText("Year:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##gpsyear", &cur_gps.year, 1, 5, 0);
            LISTEN_INPUT_CHANGE(handle_gps_change, EXPRESS_GPS_YEAR, cur_gps.year)
            igSameLine(0.0f, -1.0f);
            igText("Month:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##gpsmon", &cur_gps.month, 1, 5, 0);
            LISTEN_INPUT_CHANGE(handle_gps_change, EXPRESS_GPS_MONTH, cur_gps.month)
            igSameLine(0.0f, -1.0f);
            igText("Day:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##gpsmon", &cur_gps.day, 1, 5, 0);
            LISTEN_INPUT_CHANGE(handle_gps_change, EXPRESS_GPS_DAY, cur_gps.day)

            igText("Hour:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##gpshr", &cur_gps.hr, 1, 5, 0);
            LISTEN_INPUT_CHANGE(handle_gps_change, EXPRESS_GPS_HOUR, cur_gps.hr)
            igSameLine(0.0f, -1.0f);
            igText("Minutes:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##gpsmin", &cur_gps.min, 1, 5, 0);
            LISTEN_INPUT_CHANGE(handle_gps_change, EXPRESS_GPS_MINUTE, cur_gps.min)
            igSameLine(0.0f, -1.0f);
            igText("Secound:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputFloat("##gpssec", &cur_gps.sec, 0.01, 1, "%.2f", 0);
            LISTEN_INPUT_CHANGE(handle_gps_change, EXPRESS_GPS_SECOND, (int)(cur_gps.sec * 100))

            igText("HDOP:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputFloat("##gpshdop", &cur_gps.hdop, 0.1, 1, "%.1f", 0);
            LISTEN_INPUT_CHANGE(handle_gps_change, EXPRESS_GPS_HDOP, (int)(cur_gps.hdop * 10))
            igSameLine(0.0f, -1.0f);
            igText("PDOP:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputFloat("##gpspdop", &cur_gps.pdop, 0.1, 1, "%.1f", 0);
            LISTEN_INPUT_CHANGE(handle_gps_change, EXPRESS_GPS_PDOP, (int)(cur_gps.pdop * 10))
            igSameLine(0.0f, -1.0f);
            igText("VDOP:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputFloat("##gpsvdop", &cur_gps.vdop, 0.01, 0.1, "%.1f", 0);
            LISTEN_INPUT_CHANGE(handle_gps_change, EXPRESS_GPS_VDOP, (int)(cur_gps.vdop * 10))

            igText("Number of active satellites:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##gpsnumsv", &cur_gps.num_sv, 1, 5, 0);
            LISTEN_INPUT_CHANGE(handle_gps_change, EXPRESS_GPS_NUM_ACTIVE_SV, cur_gps.num_sv)

#define GPS_ACTIVE_SV(i)                                 \
    igSetNextItemWidth(window_size.x * 0.1f);            \
    igInputInt("##gpssv", &cur_gps.sv_prns[i], 1, 5, 0); \
    LISTEN_INPUT_CHANGE(handle_gps_change, EXPRESS_GPS_ACTIVE_SV_ID, cur_gps.sv_prns[i] * 100 + i)

            for (int i = 0; i < 12; ++i)
            {
                igPushID_Int(i);
                GPS_ACTIVE_SV(i)
                igPopID();
                if (i != 5 && i != 11)
                {
                    igSameLine(0.0f, -1.0f);
                }
            }

            igText("Number of satellites in view:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##gpsnumsvinview", &cur_gps.num_sv_inview, 1, 5, 0);
            LISTEN_INPUT_CHANGE(handle_gps_change, EXPRESS_GPS_NUM_SV_INVIEW, cur_gps.num_sv_inview)

#define GPS_SV_INVIEW(i)                                                                                       \
    igText("ID:");                                                                                             \
    igSameLine(0.0f, -1.0f);                                                                                   \
    igSetNextItemWidth(window_size.x * 0.1f);                                                                  \
    igInputInt("##gpssvviewid", &cur_gps.sv_inview[i][0], 1, 5, 0);                                            \
    LISTEN_INPUT_CHANGE(handle_gps_change, EXPRESS_GPS_SV_INVIEW_ID, cur_gps.sv_inview[i][0] * 100 + i)        \
    igSameLine(0.0f, -1.0f);                                                                                   \
    igText("Elevation:");                                                                                      \
    igSameLine(0.0f, -1.0f);                                                                                   \
    igSetNextItemWidth(window_size.x * 0.1f);                                                                  \
    igInputInt("##gpssvviewe", &cur_gps.sv_inview[i][1], 1, 5, 0);                                             \
    LISTEN_INPUT_CHANGE(handle_gps_change, EXPRESS_GPS_SV_INVIEW_ELEVATION, cur_gps.sv_inview[i][1] * 100 + i) \
    igSameLine(0.0f, -1.0f);                                                                                   \
    igText("Azimuth:");                                                                                        \
    igSameLine(0.0f, -1.0f);                                                                                   \
    igSetNextItemWidth(window_size.x * 0.12f);                                                                 \
    igInputInt("##gpssvviewa", &cur_gps.sv_inview[i][2], 1, 5, 0);                                             \
    LISTEN_INPUT_CHANGE(handle_gps_change, EXPRESS_GPS_SV_INVIEW_AZIMUTH, cur_gps.sv_inview[i][2] * 100 + i)   \
    igSameLine(0.0f, -1.0f);                                                                                   \
    igText("SNR:");                                                                                            \
    igSameLine(0.0f, -1.0f);                                                                                   \
    igSetNextItemWidth(window_size.x * 0.1f);                                                                  \
    igInputInt("##gpssvviews", &cur_gps.sv_inview[i][3], 1, 5, 0);                                             \
    LISTEN_INPUT_CHANGE(handle_gps_change, EXPRESS_GPS_SV_INVIEW_SNR, cur_gps.sv_inview[i][3] * 100 + i)

            for (int i = 0; i < 16; ++i)
            {
                igPushID_Int(i);
                GPS_SV_INVIEW(i)
                igPopID();
            }
        }
        // Microphone
        if (igCollapsingHeader_TreeNodeFlags("Microphone", 0))
        {
            igText("File path:");
            igSameLine(0.0f, -1.0f);
            igInputText("##micpath", cur_mic.file_path, 100, 0, NULL, NULL);
            if (cur_mic.using_mic && igGetActiveID() == igGetID_Str("##micpath"))
            {
                igClearActiveID();
            }

            igText("Using Microphone");
            igSameLine(0.0f, -1.0f);
            igToggleButton("Using microphone", &cur_mic.using_mic, handle_mic_change);
            if (cur_mic.start_capture && igGetActiveID() == igGetID_Str("Using microphone"))
            {
                igClearActiveID();
            }
            igSameLine(0.0f, -1.0f);
            ImVec2 button_size = {0, 0};
            if (igButton(cur_mic.start_capture ? "stop capture" : "start capture", button_size))
            {
                if (cur_mic.start_capture)
                {
                    stop_capture();
                }
                else
                {
                    if (cur_mic.using_mic)
                    {
                        start_capture();
                    }
                    else
                    {
                        start_capture_from_file(cur_mic.file_path);
                    }
                }
                cur_mic.start_capture = !cur_mic.start_capture;
            }
        }

        // Magnetic
        if (igCollapsingHeader_TreeNodeFlags("Magnetic", 0))
        {
            igText("Scale x:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputFloat("##magscalex", &cur_mag.scale_x, 0.5, 5, "%.2f", 0);
            LISTEN_INPUT_CHANGE(handle_magnetic_change, cur_mag.scale_x, cur_mag.scale_y,
                                cur_mag.scale_z, cur_mag.x, cur_mag.y, cur_mag.z)
            igSameLine(0.0f, -1.0f);
            igText("Scale y:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputFloat("##magscaley", &cur_mag.scale_y, 0.5, 5, "%.2f", 0);
            LISTEN_INPUT_CHANGE(handle_magnetic_change, cur_mag.scale_x, cur_mag.scale_y,
                                cur_mag.scale_z, cur_mag.x, cur_mag.y, cur_mag.z)
            igSameLine(0.0f, -1.0f);
            igText("Scale z:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputFloat("##magscalez", &cur_mag.scale_z, 0.5, 5, "%.2f", 0);
            LISTEN_INPUT_CHANGE(handle_magnetic_change, cur_mag.scale_x, cur_mag.scale_y,
                                cur_mag.scale_z, cur_mag.x, cur_mag.y, cur_mag.z)

            igText("x:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##magx", &cur_mag.x, 1, 5, 0);
            LISTEN_INPUT_CHANGE(handle_magnetic_change, cur_mag.scale_x, cur_mag.scale_y,
                                cur_mag.scale_z, cur_mag.x, cur_mag.y, cur_mag.z)
            igSameLine(0.0f, -1.0f);
            igText("y:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##magy", &cur_mag.y, 1, 5, 0);
            LISTEN_INPUT_CHANGE(handle_magnetic_change, cur_mag.scale_x, cur_mag.scale_y,
                                cur_mag.scale_z, cur_mag.x, cur_mag.y, cur_mag.z)
            igSameLine(0.0f, -1.0f);
            igText("z:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##magz", &cur_mag.z, 1, 5, 0);
            LISTEN_INPUT_CHANGE(handle_magnetic_change, cur_mag.scale_x, cur_mag.scale_y,
                                cur_mag.scale_z, cur_mag.x, cur_mag.y, cur_mag.z)
        }

        // Light
        if (igCollapsingHeader_TreeNodeFlags("Light", 0))
        {
            igText("Scale:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.3f);
            igInputFloat("##lightscale", &cur_light.scale, 0.5, 5, "%.2f", 0);
            LISTEN_INPUT_CHANGE(handle_light_change, cur_light.scale, cur_light.input)
            igSameLine(0.0f, -1.0f);
            igText("Input:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.3f);
            igInputInt("##lightinput", &cur_light.input, 1, 5, 0);
            LISTEN_INPUT_CHANGE(handle_light_change, cur_light.scale, cur_light.input)
        }

        // Radio
        if (igCollapsingHeader_TreeNodeFlags("Radio", 0))
        {
            // igText("RSSI:");
            // igSameLine(0.0f, -1.0f);
            // igSliderInt("dBm", &cur_modem.rssi, -112, -50, "%d", 0);
            // if (igIsItemDeactivatedAfterEdit()) {
            //     handle_modem_change(EXPRESS_MODEM_RSSI, cur_modem.rssi);
            // };
            int slot = 0;
#define EM_GET_ADDR(status) express_modem_get_status_field(slot, EXPRESS_MODEM_##status)

            igText("Quality:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.1f);
            igCombo_Str_arr("##signalstrength", EM_GET_ADDR(SIGNAL_QUALITY), modem_signal_quality, 5, 5);
            igSameLine(0.0f, -1.0f);
            igText("Network type:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.1f);
            if (igCombo_Str_arr("##networktype", EM_GET_ADDR(DATA_NETWORK), modem_data_network_type, 6, 6))
                handle_modem_change(slot, EXPRESS_MODEM_DATA_NETWORK);

            igText("Voice state:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.1f);
            if (igCombo_Str_arr("##voicestate", EM_GET_ADDR(VOICE_STATE), modem_registration_type, 6, 6))
                handle_modem_change(slot, EXPRESS_MODEM_VOICE_STATE);
            igSameLine(0.0f, -1.0f);
            igText("Data state:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.1f);
            if (igCombo_Str_arr("##datastate", EM_GET_ADDR(DATA_STATE), modem_registration_type, 6, 6))
                handle_modem_change(slot, EXPRESS_MODEM_DATA_STATE);

            AOperator op;
            
            op = EM_GET_ADDR(OPERATOR_HOME);
            igText("Home Operator Name:");
            igSameLine(0.0f, -1.0f);
            igText("Long:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.15f);
            igInputText("##homeOpLongName", op->name[0], 16, 0, NULL, NULL);
            igSameLine(0.0f, -1.0f);
            igText("Short:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.1f);
            igInputText("##homeOpShortName", op->name[1], 16, 0, NULL, NULL);
            igSameLine(0.0f, -1.0f);
            igText("Numeric:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.1f);
            igInputText("##homeOpNumericName", op->name[2], 16, 0, NULL, NULL);
            // TODO: Set callback function here when implementing emulated Huawei Modem

            op = EM_GET_ADDR(OPERATOR_ROAMING);
            igText("Roam Operator Name:");
            igSameLine(0.0f, -1.0f);
            igText("Long:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.15f);
            igInputText("##roamOpLongName", op->name[0], 16, 0, NULL, NULL);
            igSameLine(0.0f, -1.0f);
            igText("Short:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.1f);
            igInputText("##roamOpShortName", op->name[1], 16, 0, NULL, NULL);
            igSameLine(0.0f, -1.0f);
            igText("Numeric:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.1f);
            igInputText("##roamOpNumericName", op->name[2], 16, 0, NULL, NULL);
            // TODO: Set callback function here when implementing emulated Huawei Modem

            igText("Area Code:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.1f);
            igInputInt("##modem_ac", EM_GET_ADDR(AREA_CODE), 0, 0, 0);
            LISTEN_INPUT_CHANGE(handle_modem_change, slot, EXPRESS_MODEM_AREA_CODE);
            igSameLine(0.0f, -1.0f);
            igText("Cell ID:");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.1f);
            igInputInt("##modem_cell", EM_GET_ADDR(CELL_ID), 0, 0, 0);
            LISTEN_INPUT_CHANGE(handle_modem_change, slot, EXPRESS_MODEM_CELL_ID);

            igText("From:");
            igSameLine(0.0f, -1.0f);
            igInputText("##from_number", EM_GET_ADDR(FROM_NUMBER), 31, 0, NULL, NULL);

            ImVec2 text_input_size = {0, 0};
            igInputTextMultiline("##sms_message", EM_GET_ADDR(INPUT_SMS_STR), MAX_SMS_MSG_SIZE, text_input_size, 0, NULL, NULL);
            igText("Send SMS to emulator:");
            igSameLine(0.0f, -1.0f);
            ImVec2 button_size = {0, 0};
            if (igButton("Send", button_size)) {
                handle_modem_change(slot, EXPRESS_MODEM_RECEIVE_SMS);
            }
        }

        igText("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / igGetIO()->Framerate, igGetIO()->Framerate);
        igEnd();
    }

    igRender();
    // int display_w, display_h;
    // glfwGetFramebufferSize(window, &display_w, &display_h);
    // glViewport(0, 0, display_w, display_h);
    // glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());

    igUpdatePlatformWindows();
    igRenderPlatformWindowsDefault(NULL, NULL);
}

void *interface_window_thread(void *data)
{   
    all_interface_data.run = (int *)data;

    THREAD_CONTROL_BEGIN

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);

    THREAD_CONTROL_END

    // GLFW already be initialized in our qemu main thread
    // if (!glfwInit())
    // {
    //     fprintf(stderr, "Device_interface::Glfw init failed!!\n");
    // }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char *glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char *glsl_version = "#version 150";
    THREAD_CONTROL_BEGIN

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    glfwMakeContextCurrent(NULL);

    // Create glfw window with graphics context
    window = glfwCreateWindow(1, 1, "Device Input", NULL, NULL);
    if (window == NULL)
    {
        LOGE("Device_interface::Failed to create window");
    #ifdef __APPLE__
        exit(-1);
    #else
        return NULL;
    #endif
    }

    THREAD_CONTROL_END

    glfwMakeContextCurrent(window);
    // 这个没用
    //  glfwSwapInterval(1); // Enable vsync

    // setup imgui
    igCreateContext(NULL);

    // set docking
    ImGuiIO *ioptr = igGetIO();
    ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

    ioptr->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // igStyleColorsDark(NULL);

    bool show_imgui = true;

    window_size.x = 640;
    window_size.y = 360;

    gint64 frame_start_time = g_get_real_time();
    gint64 remain_sleep_time = 0;

    while (*(all_interface_data.run) == 1)
    {

        igSetNextWindowSize(window_size, ImGuiCond_Once);

        ioptr = igGetIO();

        // TIMER_START(draw);

        THREAD_CONTROL_BEGIN

        draw_window(&show_imgui);

        THREAD_CONTROL_END

        // TIMER_END(draw);
        // TIMER_OUTPUT(draw, 100);

        THREAD_CONTROL_BEGIN

        glfwWaitEvents();

    THREAD_CONTROL_END

        gint64 now_time = g_get_real_time();

        gint64 need_sleep_time = 1000000 / 60 - (now_time - frame_start_time) + remain_sleep_time - 1000;

        if (need_sleep_time >= 1000)
        {
            g_usleep(need_sleep_time);
        }

        now_time = g_get_real_time();

        remain_sleep_time = 1000000 / 60 - (now_time - frame_start_time);
        frame_start_time = now_time;

        // printf("need sleep %lld remain_sleep_time %lld\n",need_sleep_time,remain_sleep_time);

        // glfwSwapBuffers(window);
        if (!show_imgui || *(all_interface_data.run) == 0)
        {
            *(all_interface_data.run) = 0;
            show_imgui = false;
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        }
    }

    THREAD_CONTROL_BEGIN

    // clean up
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(NULL);

    glfwMakeContextCurrent(NULL);

    glfwDestroyWindow(window);

    THREAD_CONTROL_END

    // glfw will only terminate once, it would be terminate in our main window thread
    // glfwTerminate();
    LOGI("Device_interface::Destroy window");

    return NULL;
}
