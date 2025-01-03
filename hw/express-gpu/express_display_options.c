// #define STD_DEBUG_LOG
#include "hw/teleport-express/express_log.h"
#include "hw/express-gpu/express_display.h"

uint64_t express_display_count;
int express_display_pixel_width;
int express_display_pixel_height;
int express_display_refresh_rate;
char *express_display_options;

typedef struct DisplayInfo {
    int width;
    int height;
    int refreshRate;
} DisplayInfo;
DisplayInfo *info_array;

static void parse_display_sizes(const char *input) {
    if (input == NULL || strlen(input) == 0) {
        return;
    }

    // Count the number of displays
    int displayCount = 1; // At least one display
    for (const char *p = input; *p; p++) {
        if (*p == ';') displayCount++;
    }

    // Print the number of displays
    LOGD("Number of displays: %d", displayCount);
    express_display_count = displayCount;
    info_array = g_malloc0(sizeof(DisplayInfo) * displayCount);

    // Tokenize the input string and parse each display's dimensions and refresh rate
    char *token = strtok(input, ";");
    int displayIndex = 0;

    while (token != NULL) {
        char *xPos = strchr(token, 'x');    // Find the 'x' separating width and height
        char *atPos = strchr(token, '@');  // Find the '@' indicating refresh rate

        if (xPos == NULL || atPos == NULL || xPos > atPos) {
            LOGE("Invalid format for display %d: %s\n", displayIndex, token);
        } else {
            *xPos = '\0'; // Split the string into width and height
            *atPos = '\0'; // Split the string into height and refresh rate

            int width = atoi(token);          // Parse width
            int height = atoi(xPos + 1);      // Parse height
            int refreshRate = atoi(atPos + 1); // Parse refresh rate

            info_array[displayIndex].width = width;
            info_array[displayIndex].height = height;
            info_array[displayIndex].refreshRate = refreshRate;

            LOGD("display %d: width = %d, height = %d, refresh Rate = %dhz", 
                   displayIndex, width, height, refreshRate);
        }

        displayIndex++;
        token = strtok(NULL, ",");
    }
}

static void init_info() {
    if (express_display_options != NULL && info_array == NULL) {
        parse_display_sizes(express_display_options);
    }
    if (express_display_count < 1) {
        LOGW("at least one display is needed!");
        express_display_count = 1;
    }
} 

uint64_t get_display_count(void) {
    init_info();
    return express_display_count;
}

void get_display_info(int displayIndex, int *width, int *height, int *refreshRate) {
    init_info();
    if (displayIndex < 0 || displayIndex >= express_display_count) {
        LOGE("Invalid display index %d", displayIndex);
    }
    if (info_array == NULL) {
        *width = express_display_pixel_width;
        *height = express_display_pixel_height;
        *refreshRate = express_display_refresh_rate;
    }
    else if (displayIndex < express_display_count) {
        *width = info_array[displayIndex].width;
        *height = info_array[displayIndex].height;
        *refreshRate = info_array[displayIndex].refreshRate;
    }
}