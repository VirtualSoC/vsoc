#include "hw/express-gpu/device_interface.h"

#define FOREACH_DEVICE(DEVICE) \
        DEVICE(Audio)          \
        DEVICE(Battery)        \
        DEVICE(GPS)            \
        DEVICE(Gravity_sensor) \
        DEVICE(Gyro_sensor)    \
        DEVICE(Magnetic_sensor)\
        DEVICE(Dev_Len)        \

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

#define FOREACH_DATA_TYPE(DATA_TYPE) \
        DATA_TYPE(String)        \
        DATA_TYPE(Int)           \
        DATA_TYPE(Long)          \
        DATA_TYPE(Float)         \
        DATA_TYPE(Double)        \
        DATA_TYPE(Datatype_Len)  \

enum DEVICE_ENUM {
    FOREACH_DEVICE(GENERATE_ENUM)
};

enum DATA_TYPE_ENUM {
    FOREACH_DATA_TYPE(GENERATE_ENUM)
};

const char *DEVICE_STRING[] = {
    FOREACH_DEVICE(GENERATE_STRING)
};

const char *DATA_TYPE_STRING[] = {
    FOREACH_DATA_TYPE(GENERATE_STRING)
};

static SDL_Window *window = NULL;
static int current_device = 0;
static int current_type = 0;
static char current_buffer[MAX_INPUT_BUFFER] = "";
static char notify_text[64] = "";
static int current_len = 1;
static device_interface_data *my_data = NULL;

static void handle_submit(void)
{
    if(my_data==NULL){
        printf("Device_interface::my_data missing!\n");
    // } else if(my_data->senddata_callback == NULL){
    //     printf("Target Device: %s\n",DEVICE_STRING[current_device]);
    //     printf("Datatype: %s\n",DATA_TYPE_STRING[current_type]);
    //     printf("Data: %s\n", current_buffer);
    //     printf("Input Data Array len: %d\n", current_len);
    } else {
        char *cur = strtok(current_buffer,",");
        for(int i=0; i<current_len; ++i){
            my_data->data[i] = cur;
            cur = strtok(NULL ,",");
            printf("%s",my_data->data[i]);
        }
        strcpy(my_data->device, DEVICE_STRING[current_device]);
        strcpy(my_data->datatype, DATA_TYPE_STRING[current_type]);
        my_data->data_len = current_len;
        my_data->senddata_callback(my_data->device, my_data->datatype, my_data->data_len, my_data->data);
    }
}

void *create_interface(void *data)
{
    my_data = (device_interface_data *)data;
    if(my_data == NULL) {
        printf("Device_interface::Failed to malloc data");
    }

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
    

    // check opengl version sdl uses
    // printf("opengl version: %s", (char*)glGetString(GL_VERSION));

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
            igSetNextItemWidth(current_width* 0.3f);
            igCombo_Str_arr("Target Device", &current_device, DEVICE_STRING, Dev_Len, Dev_Len);
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(current_width* 0.3f);
            igCombo_Str_arr("Input Datatype", &current_type, DATA_TYPE_STRING, Datatype_Len, Datatype_Len);

            
            igInputTextWithHint("##InputData", 
                                "If input is an array use comma to seperate elements, "
                                "and set up the length of array in spin button",
                                current_buffer, (size_t)MAX_INPUT_BUFFER, 0, NULL, NULL);
            igSameLine(0.0f, -1.0f);
            igSetNextItemWidth(current_width * 0.3f);
            igInputInt("##InputDataLen", &current_len, 1, 5, 0);
            if(current_len<1) current_len = 1;
            else if(current_len>MAX_ARRAY_LEN) current_len = MAX_ARRAY_LEN;

            igText("%s", notify_text);
            igSameLine(current_width - 60.0f, -1.0f);
            ImVec2 buttonSize;
            buttonSize.x = 0;
            buttonSize.y = 0;
            if (igButton("Submit", buttonSize)){
                handle_submit();
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

    free(my_data);
    my_data = NULL;
    return NULL;
}
