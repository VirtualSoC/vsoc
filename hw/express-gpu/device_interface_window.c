#include "hw/express-gpu/device_interface_window.h"

#define TIMER_LOG
#define STD_DEBUG_LOG
#include "hw/teleport-express/express_log.h"

static Device_Interface_Data all_interface_data;

void handle_battery_change(int current_battery)
{
    printf("Device_interface::current_battery: %d\n", current_battery);
}

void handle_accelerometer_change(float scale, int x, int y, int z)
{
    printf("Device_interface::accelerometer scale: %.2f, x: %d, y: %d, z: %d\n", scale, x, y, z);
}

void handle_magnetic_change(float scale_x, float scale_y, float scale_z, int x, int y, int z)
{
    printf("Device_interface::magnetic scale x: %.2f, scale y: %.2f, scale z: %.2f, x: %d, y: %d, z:%d\n", scale_x, scale_y, scale_z, x, y, z);
}

void handle_light_change(float scale, int input)
{
    printf("Device_interface::light scale: %.2f, input: %d\n", scale, input);
}

void handle_gyroscope_change(float scale, int x, int y, int z)
{
    printf("Device_interface::gyroscope scale: %.2f, x: %d, y: %d, z: %d\n", scale, x, y, z);
}

static void glfw_error_callback(int error, const char *description)
{
    // fprintf(stderr, "Device_interface::Glfw Error %d: %s\n", error, description);
    return;
}

static Accelerometer_Data cur_acc = {.scale = 0, .x = 0, .y = 0, .z = 0};
static Magnetic_Data cur_mag = {.scale_x = 0, .scale_y = 0, .scale_z = 0, .x = 0, .y = 0, .z = 0};
static Light_Data cur_light = {.scale = 0, .input = 0};
static Gyroscope_Data cur_gyr = {.scale = 0, .x = 0, .y = 0, .z = 0};
static int cur_battery = 100;
static ImVec2 window_size;
static float window_dpi_scale = 1.0;
GLFWwindow *window = NULL;

static void update_window_scale(void)
{
    float new_dpi_scale = igGetWindowDpiScale();
    if(window_dpi_scale != new_dpi_scale)
    {
        window_size.x *= new_dpi_scale/(window_dpi_scale);
        window_size.y *= new_dpi_scale/(window_dpi_scale);
        window_dpi_scale = new_dpi_scale;
        igSetWindowFontScale(new_dpi_scale);
    }
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
            igSliderInt("%%", &cur_battery, 0, 100, "%d", 0);
            if (igIsItemDeactivatedAfterEdit())
            {
                handle_battery_change(cur_battery);
            }
        }

        // Accelerometer
        if (igCollapsingHeader_TreeNodeFlags("Accelerometer", 0))
        {
            igText("Scale: ");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputFloat("##accscale", &cur_acc.scale, 0.5, 5, "%.2f", 0);
            if (igIsItemDeactivatedAfterEdit())
            {
                handle_accelerometer_change(cur_acc.scale,
                                            cur_acc.x, cur_acc.y, cur_acc.z);
            }
            igSameLine(0.0f, -1.0f);
            igText("x: ");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##accx", &cur_acc.x, 1, 5, 0);
            if (igIsItemDeactivatedAfterEdit())
            {
                handle_accelerometer_change(cur_acc.scale,
                                            cur_acc.x, cur_acc.y, cur_acc.z);
            }
            igSameLine(0.0f, -1.0f);
            igText("y: ");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##accy", &cur_acc.y, 1, 5, 0);
            if (igIsItemDeactivatedAfterEdit())
            {
                handle_accelerometer_change(cur_acc.scale,
                                            cur_acc.x, cur_acc.y, cur_acc.z);
            }
            igSameLine(0.0f, -1.0f);
            igText("z: ");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##accz", &cur_acc.z, 1, 5, 0);
            if (igIsItemDeactivatedAfterEdit())
            {
                handle_accelerometer_change(cur_acc.scale,
                                            cur_acc.x, cur_acc.y, cur_acc.z);
            }
        }

        // Magnetic
        if (igCollapsingHeader_TreeNodeFlags("Magnetic", 0))
        {
            igText("Scale x: ");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputFloat("##magscalex", &cur_mag.scale_x, 0.5, 5, "%.2f", 0);
            if (igIsItemDeactivatedAfterEdit())
            {
                handle_magnetic_change(cur_mag.scale_x, cur_mag.scale_y,
                                       cur_mag.scale_z, cur_mag.x, cur_mag.y, cur_mag.z);
            }
            igSameLine(0.0f, -1.0f);
            igText("Scale y: ");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputFloat("##magscaley", &cur_mag.scale_y, 0.5, 5, "%.2f", 0);
            if (igIsItemDeactivatedAfterEdit())
            {
                handle_magnetic_change(cur_mag.scale_x, cur_mag.scale_y,
                                       cur_mag.scale_z, cur_mag.x, cur_mag.y, cur_mag.z);
            }
            igSameLine(0.0f, -1.0f);
            igText("Scale z: ");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputFloat("##magscalez", &cur_mag.scale_z, 0.5, 5, "%.2f", 0);
            if (igIsItemDeactivatedAfterEdit())
            {
                handle_magnetic_change(cur_mag.scale_x, cur_mag.scale_y,
                                       cur_mag.scale_z, cur_mag.x, cur_mag.y, cur_mag.z);
            }

            igText("x: ");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##magx", &cur_mag.x, 1, 5, 0);
            if (igIsItemDeactivatedAfterEdit())
            {
                handle_magnetic_change(cur_mag.scale_x, cur_mag.scale_y,
                                       cur_mag.scale_z, cur_mag.x, cur_mag.y, cur_mag.z);
            }
            igSameLine(0.0f, -1.0f);
            igText("y: ");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##magy", &cur_mag.y, 1, 5, 0);
            if (igIsItemDeactivatedAfterEdit())
            {
                handle_magnetic_change(cur_mag.scale_x, cur_mag.scale_y,
                                       cur_mag.scale_z, cur_mag.x, cur_mag.y, cur_mag.z);
            }
            igSameLine(0.0f, -1.0f);
            igText("z: ");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##magz", &cur_mag.z, 1, 5, 0);
            if (igIsItemDeactivatedAfterEdit())
            {
                handle_magnetic_change(cur_mag.scale_x, cur_mag.scale_y,
                                       cur_mag.scale_z, cur_mag.x, cur_mag.y, cur_mag.z);
            }
        }

        // Light
        if (igCollapsingHeader_TreeNodeFlags("Light", 0))
        {
            igText("Scale: ");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.3f);
            igInputFloat("##lightscale", &cur_light.scale, 0.5, 5, "%.2f", 0);
            if (igIsItemDeactivatedAfterEdit())
            {
                handle_light_change(cur_light.scale, cur_light.input);
            }
            igSameLine(0.0f, -1.0f);
            igText("Input: ");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.3f);
            igInputInt("##lightinput", &cur_light.input, 1, 5, 0);
            if (igIsItemDeactivatedAfterEdit())
            {
                handle_light_change(cur_light.scale, cur_light.input);
            }
        }

        // Gyroscope
        if (igCollapsingHeader_TreeNodeFlags("Gyroscope", 0))
        {
            igText("Scale: ");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputFloat("##gyroscale", &cur_gyr.scale, 0.5, 5, "%.2f", 0);
            if (igIsItemDeactivatedAfterEdit())
            {
                handle_gyroscope_change(cur_gyr.scale,
                                        cur_gyr.x, cur_gyr.y, cur_gyr.z);
            }
            igSameLine(0.0f, -1.0f);
            igText("x: ");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##gyrox", &cur_gyr.x, 1, 5, 0);
            if (igIsItemDeactivatedAfterEdit())
            {
                handle_gyroscope_change(cur_gyr.scale,
                                        cur_gyr.x, cur_gyr.y, cur_gyr.z);
            }
            igSameLine(0.0f, -1.0f);
            igText("y: ");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##gyroy", &cur_gyr.y, 1, 5, 0);
            if (igIsItemDeactivatedAfterEdit())
            {
                handle_gyroscope_change(cur_gyr.scale,
                                        cur_gyr.x, cur_gyr.y, cur_gyr.z);
            }
            igSameLine(0.0f, -1.0f);
            igText("z: ");
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(window_size.x * 0.2f);
            igInputInt("##gyroz", &cur_gyr.z, 1, 5, 0);
            if (igIsItemDeactivatedAfterEdit())
            {
                handle_gyroscope_change(cur_gyr.scale,
                                        cur_gyr.x, cur_gyr.y, cur_gyr.z);
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

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    glfwMakeContextCurrent(NULL);

    // Create glfw window with graphics context
    window = glfwCreateWindow(123, 456, "Device Input", NULL, NULL);
    if (window == NULL)
    {
        printf("Device_interface::Failed to create window\n");
        return NULL;
    }

    glfwMakeContextCurrent(window);
    //这个没用
    // glfwSwapInterval(1); // Enable vsync

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
        draw_window(&show_imgui);
        // TIMER_END(draw);
        // TIMER_OUTPUT(draw, 100);

        glfwWaitEvents();
            
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

    // clean up
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(NULL);

    glfwMakeContextCurrent(NULL);
    glfwDestroyWindow(window);
    // glfw will only terminate once, it would be terminate in our main window thread
    // glfwTerminate();
    printf("Device_interface::Destroy window\n");

    return NULL;
}
