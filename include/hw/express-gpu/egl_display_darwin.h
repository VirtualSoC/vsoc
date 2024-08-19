#include "egl_display.h"

typedef struct
{
    Egl_Display display;
} Egl_Display_DARWIN;

void parse_pixel_format(Egl_Display *display, int index);
EGLint get_pixel_format_attrib(int index, EGLint attrib);
