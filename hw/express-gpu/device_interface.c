#include "hw/express-gpu/device_interface.h"

#define FOREACH_DEVICE_DATA(DATA) \
        DATA(accelerometer_data)  \
        DATA(magnetic_data)       \
        DATA(light_data)          \
        DATA(gyroscope_data)      \

#define DEVICE_DEFINE(DATA)  DATA *cur_##DATA;

static SDL_Window *window = NULL;
static device_interface_data *my_data = NULL;

void handle_battery_change(int current_battery){
    printf("current_battery: %d\n", current_battery);
}

void handle_accelerometer_change(float scale, int x, int y, int z){
    printf("accelerometer scale: %.2f, x: %d, y: %d, z: %d\n", scale, x, y, z);
}

void handle_magnetic_change(float scale_x, float scale_y, float scale_z, int x, int y, int z){
    printf("magnetic scale x: %.2f, scale y: %.2f, scale z: %.2f, x: %d, y: %d, z:%d\n",scale_x,scale_y,scale_z,x,y,z);
}

void handle_light_change(float scale,int input){
    printf("light scale: %.2f, input: %d\n",scale,input);
}

void handle_gyroscope_change(float scale, int x, int y, int z){
    printf("gyroscope scale: %.2f, x: %d, y: %d, z: %d\n", scale, x, y, z);
}

void *create_interface(void *data)
{
    my_data = (device_interface_data *)data;
    if(my_data == NULL) {
        printf("Device_interface::Failed to malloc data");
    }
    accelerometer_data cur_acc = {.scale = 0, .x = 0, .y = 0, .z = 0};
    magnetic_data cur_mag = {.scale_x = 0, .scale_y = 0, .scale_z = 0, .x = 0, .y = 0, .z = 0};
    light_data cur_light = {.scale = 0, .input = 0};
    gyroscope_data cur_gyr = {.scale = 0, .x = 0, .y = 0, .z = 0};
    int cur_battery = 100;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Device_interface::Failed to init SDL: %s\n", SDL_GetError());
        return NULL;
    }

// Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif
    // and prepare OpenGL stuff
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);


    window = SDL_CreateWindow(
        "Device_input", 0, 0, 1, 1,
        SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
        );
    if (window == NULL) {
        printf("Device_interface::Failed to create window: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window); 
    SDL_GL_SetSwapInterval(1);  // enable vsync 

    // setup imgui
    igCreateContext(NULL);

    //set docking
    ImGuiIO* ioptr = igGetIO();
    ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
#ifdef IMGUI_HAS_DOCK
    ioptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    ioptr->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
#endif

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    igStyleColorsDark(NULL);

    float current_width;
    while (my_data->run)
    {
        SDL_Event e;

        // we need to call SDL_PollEvent to let window rendered, otherwise
        // no window will be shown
        while (SDL_PollEvent(&e) != 0)
        {
        ImGui_ImplSDL2_ProcessEvent(&e);
        if (e.type == SDL_QUIT)
            my_data->run = false;
        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE && e.window.windowID == SDL_GetWindowID(window))
            my_data->run = false;
        }
        
        // start imgui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        igNewFrame();

        // show a simple window that we created ourselves
        {
            igBegin("Device Input", &(my_data->run), 0);
            current_width = igGetWindowWidth();
            // Battery
            if(igCollapsingHeader_TreeNodeFlags("Battery",0))
            {
                igSliderInt("%%", &cur_battery, 0, 100, "%d", 0);
                if(igIsItemDeactivatedAfterEdit()){
                    handle_battery_change(cur_battery);
                }
            }

            // Accelerometer
            if(igCollapsingHeader_TreeNodeFlags("Accelerometer",0))
            {
                igText("Scale: ");
                igSameLine(0.0f, -1.0f);
                igSetNextItemWidth(current_width * 0.2f);
                igInputFloat("##accscale",&cur_acc.scale,0.5,5,"%.2f",0);
                if(igIsItemDeactivatedAfterEdit()){
                    handle_accelerometer_change(cur_acc.scale,
                    cur_acc.x,cur_acc.y,cur_acc.z);
                }
                igSameLine(0.0f, -1.0f);
                igText("x: ");
                igSameLine(0.0f, -1.0f);
                igSetNextItemWidth(current_width * 0.2f);
                igInputInt("##accx", &cur_acc.x, 1, 5, 0);
                if(igIsItemDeactivatedAfterEdit()){
                    handle_accelerometer_change(cur_acc.scale,
                    cur_acc.x,cur_acc.y,cur_acc.z);
                }
                igSameLine(0.0f, -1.0f);
                igText("y: ");
                igSameLine(0.0f, -1.0f);
                igSetNextItemWidth(current_width * 0.2f);
                igInputInt("##accy", &cur_acc.y, 1, 5, 0);
                if(igIsItemDeactivatedAfterEdit()){
                    handle_accelerometer_change(cur_acc.scale,
                    cur_acc.x,cur_acc.y,cur_acc.z);
                }
                igSameLine(0.0f, -1.0f);
                igText("z: ");
                igSameLine(0.0f, -1.0f);
                igSetNextItemWidth(current_width * 0.2f);
                igInputInt("##accz", &cur_acc.z, 1, 5, 0);
                if(igIsItemDeactivatedAfterEdit()){
                    handle_accelerometer_change(cur_acc.scale,
                    cur_acc.x,cur_acc.y,cur_acc.z);
                }
            }

            // Magnetic
            if(igCollapsingHeader_TreeNodeFlags("Magnetic",0))
            {
                igText("Scale x: ");
                igSameLine(0.0f, -1.0f);
                igSetNextItemWidth(current_width * 0.2f);
                igInputFloat("##magscalex",&cur_mag.scale_x,0.5,5,"%.2f",0);
                if(igIsItemDeactivatedAfterEdit()){
                    handle_magnetic_change(cur_mag.scale_x,cur_mag.scale_y,
                    cur_mag.scale_z,cur_mag.x,cur_mag.y,cur_mag.z);
                }
                igSameLine(0.0f, -1.0f);
                igText("Scale y: ");
                igSameLine(0.0f, -1.0f);
                igSetNextItemWidth(current_width * 0.2f);
                igInputFloat("##magscaley",&cur_mag.scale_y,0.5,5,"%.2f",0);
                if(igIsItemDeactivatedAfterEdit()){
                    handle_magnetic_change(cur_mag.scale_x,cur_mag.scale_y,
                    cur_mag.scale_z,cur_mag.x,cur_mag.y,cur_mag.z);
                }
                igSameLine(0.0f, -1.0f);
                igText("Scale z: ");
                igSameLine(0.0f, -1.0f);
                igSetNextItemWidth(current_width * 0.2f);
                igInputFloat("##magscalez",&cur_mag.scale_z,0.5,5,"%.2f",0);
                if(igIsItemDeactivatedAfterEdit()){
                    handle_magnetic_change(cur_mag.scale_x,cur_mag.scale_y,
                    cur_mag.scale_z,cur_mag.x,cur_mag.y,cur_mag.z);
                }
                
                igText("x: ");
                igSameLine(0.0f, -1.0f);
                igSetNextItemWidth(current_width * 0.2f);
                igInputInt("##magx", &cur_mag.x, 1, 5, 0);
                if(igIsItemDeactivatedAfterEdit()){
                    handle_magnetic_change(cur_mag.scale_x,cur_mag.scale_y,
                    cur_mag.scale_z,cur_mag.x,cur_mag.y,cur_mag.z);
                }
                igSameLine(0.0f, -1.0f);
                igText("y: ");
                igSameLine(0.0f, -1.0f);
                igSetNextItemWidth(current_width * 0.2f);
                igInputInt("##magy", &cur_mag.y, 1, 5, 0);
                if(igIsItemDeactivatedAfterEdit()){
                    handle_magnetic_change(cur_mag.scale_x,cur_mag.scale_y,
                    cur_mag.scale_z,cur_mag.x,cur_mag.y,cur_mag.z);
                }
                igSameLine(0.0f, -1.0f);
                igText("z: ");
                igSameLine(0.0f, -1.0f);
                igSetNextItemWidth(current_width * 0.2f);
                igInputInt("##magz", &cur_mag.z, 1, 5, 0);
                if(igIsItemDeactivatedAfterEdit()){
                    handle_magnetic_change(cur_mag.scale_x,cur_mag.scale_y,
                    cur_mag.scale_z,cur_mag.x,cur_mag.y,cur_mag.z);
                }
            }


            // Light
            if(igCollapsingHeader_TreeNodeFlags("Light",0))
            {
                igText("Scale: ");
                igSameLine(0.0f, -1.0f);
                igSetNextItemWidth(current_width * 0.3f);
                igInputFloat("##lightscale",&cur_light.scale,0.5,5,"%.2f",0);
                if(igIsItemDeactivatedAfterEdit()){
                    handle_light_change(cur_light.scale,cur_light.input);
                }
                igSameLine(0.0f, -1.0f);
                igText("Input: ");
                igSameLine(0.0f, -1.0f);
                igSetNextItemWidth(current_width * 0.3f);
                igInputInt("##lightinput", &cur_light.input, 1, 5, 0);
                if(igIsItemDeactivatedAfterEdit()){
                    handle_light_change(cur_light.scale,cur_light.input);
                }
            }


            // Gyroscope
            if(igCollapsingHeader_TreeNodeFlags("Gyroscope",0))
            {
                igText("Scale: ");
                igSameLine(0.0f, -1.0f);
                igSetNextItemWidth(current_width * 0.2f);
                igInputFloat("##gyroscale",&cur_gyr.scale,0.5,5,"%.2f",0);
                if(igIsItemDeactivatedAfterEdit()){
                    handle_gyroscope_change(cur_gyr.scale, 
                    cur_gyr.x, cur_gyr.y, cur_gyr.z);
                }
                igSameLine(0.0f, -1.0f);
                igText("x: ");
                igSameLine(0.0f, -1.0f);
                igSetNextItemWidth(current_width * 0.2f);
                igInputInt("##gyrox", &cur_gyr.x, 1, 5, 0);
                if(igIsItemDeactivatedAfterEdit()){
                    handle_gyroscope_change(cur_gyr.scale, 
                    cur_gyr.x, cur_gyr.y, cur_gyr.z);
                }
                igSameLine(0.0f, -1.0f);
                igText("y: ");
                igSameLine(0.0f, -1.0f);
                igSetNextItemWidth(current_width * 0.2f);
                igInputInt("##gyroy", &cur_gyr.y, 1, 5, 0);
                if(igIsItemDeactivatedAfterEdit()){
                    handle_gyroscope_change(cur_gyr.scale, 
                    cur_gyr.x, cur_gyr.y, cur_gyr.z);
                }
                igSameLine(0.0f, -1.0f);
                igText("z: ");
                igSameLine(0.0f, -1.0f);
                igSetNextItemWidth(current_width * 0.2f);
                igInputInt("##gyroz", &cur_gyr.z, 1, 5, 0);
                if(igIsItemDeactivatedAfterEdit()){
                    handle_gyroscope_change(cur_gyr.scale, 
                    cur_gyr.x, cur_gyr.y, cur_gyr.z);
                }
            }

            igText("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / igGetIO()->Framerate, igGetIO()->Framerate);
            igEnd();
        }

        // render
        igRender();
        SDL_GL_MakeCurrent(window, gl_context);
        // glViewport(0, 0, (int)ioptr->DisplaySize.x, (int)ioptr->DisplaySize.y);
        // glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        // glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
    #ifdef IMGUI_HAS_DOCK
        if (ioptr->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
                SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
                igUpdatePlatformWindows();
                igRenderPlatformWindowsDefault(NULL,NULL);
                SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
            }
    #endif
        SDL_GL_SwapWindow(window);
    }

    // clean up
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    igDestroyContext(NULL);

    SDL_GL_DeleteContext(gl_context);
    if (window != NULL)
    {
        SDL_DestroyWindow(window);
        window = NULL;
    }
    SDL_Quit();
    printf("Device_interface::Destroy window\n");

    free(my_data);
    my_data = NULL;
    return NULL;
}
