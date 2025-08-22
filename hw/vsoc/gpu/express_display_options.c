// #define STD_DEBUG_LOG
#include "hw/vsoc/express_log.h"
#include "hw/vsoc/gpu/express_display.h"

typedef struct DisplayInfo {
    int width;
    int height;
    int refreshRate;
} DisplayInfo;
DisplayInfo *info_array;

static void parse_display_sizes(const char *input) {
    if (input == NULL || input[0] == 0) {
        return;
    }

    // Estimate the max number of displays
    int displayCount = 1; // At least one display
    for (const char *p = input; *p; p++) {
        if (*p == ';') displayCount++;
    }
    info_array = g_malloc0(sizeof(DisplayInfo) * displayCount);

    // Tokenize the input string and parse each display's dimensions and refresh rate
    char *token = strtok((char *)input, ";");
    int displayIndex = 0;

    while (token != NULL && displayIndex < displayCount) {
        char *xPos = strchr(token, 'x');    // Find the 'x' separating width and height
        char *atPos = strchr(token, '@');  // Find the '@' indicating refresh rate

        if (xPos == NULL || atPos == NULL || xPos > atPos) {
            LOGE("Invalid options for display: %s", token);
        } else {
            *xPos = '\0'; // Split the string into width and height
            *atPos = '\0'; // Split the string into height and refresh rate

            int width = atoi(token);          // Parse width
            int height = atoi(xPos + 1);      // Parse height
            int refreshRate = atoi(atPos + 1); // Parse refresh rate

            if (width > 0 && height > 0 && refreshRate > 0) {
                info_array[displayIndex].width = width;
                info_array[displayIndex].height = height;
                info_array[displayIndex].refreshRate = refreshRate;

                LOGD("display %d: width = %d, height = %d, refresh Rate = %dhz", 
                    displayIndex, width, height, refreshRate);

                displayIndex++;
            }
            else {
                LOGE("Invalid arguments for display: w %s h %s refresh rate %s", token, xPos + 1, atPos + 1);
            }
        }
        token = strtok(NULL, ";");
    }
    displayCount = displayIndex;
    
    // Print the number of displays
    LOGD("Number of displays: %d", displayCount);
    g_ops.express_display_count = displayCount;

    if (displayCount > 0 && g_ops.express_display_refresh_rate != info_array[0].refreshRate) {
        g_ops.express_display_refresh_rate = info_array[0].refreshRate;
    }
}

static void init_info(void) {
    if (g_ops.express_display_options[0] != 0 && info_array == NULL) {
        parse_display_sizes(g_ops.express_display_options);
    }
    if (g_ops.express_display_count < 1) {
        LOGW("at least one display is needed!");
        g_ops.express_display_count = 1;
    }
} 

uint64_t get_display_count(void) {
    init_info();
    return g_ops.express_display_count;
}

void get_display_info(int displayIndex, int *width, int *height, int *refreshRate) {
    init_info();
    if (displayIndex < 0 || displayIndex >= g_ops.express_display_count) {
        LOGE("Invalid display index %d", displayIndex);
    }
    if (info_array == NULL) {
        if (width) *width = g_ops.express_display_pixel_width;
        if (height) *height = g_ops.express_display_pixel_height;
        if (refreshRate) *refreshRate = g_ops.express_display_refresh_rate;
    }
    else if (displayIndex < g_ops.express_display_count) {
        if (width) *width = info_array[displayIndex].width;
        if (height) *height = info_array[displayIndex].height;
        if (refreshRate) *refreshRate = info_array[displayIndex].refreshRate;
    }
}