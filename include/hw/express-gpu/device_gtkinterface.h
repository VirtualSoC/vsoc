#include <gtk/gtk.h>

#define MAX_INPUT_LEN 20
#define FOREACH_DEVICE(DEVICE) \
        DEVICE(Audio)       \
        DEVICE(Bluetooth)   \
        DEVICE(Battery)     \
        DEVICE(Camera)      \
        DEVICE(GPS)         \
        DEVICE(Dev_Len)     \

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

enum DEVICE_ENUM {
    FOREACH_DEVICE(GENERATE_ENUM)
};

static const char *DEVICE_STRING[] = {
    FOREACH_DEVICE(GENERATE_STRING)
};

#define FOREACH_DATA_TYPE(DATA_TYPE) \
        DATA_TYPE(Char)          \
        DATA_TYPE(Int)           \
        DATA_TYPE(Long)          \
        DATA_TYPE(Float)         \
        DATA_TYPE(Double)        \
        DATA_TYPE(Datatype_Len)  \

enum DATA_TYPE_ENUM {
    FOREACH_DATA_TYPE(GENERATE_ENUM)
};

static const char *DATA_TYPE_STRING[] = {
    FOREACH_DATA_TYPE(GENERATE_STRING)
};

struct gtkinterface_data{
    enum DEVICE_ENUM device;       //check FOREACH_DATA_TYPE
    enum DATA_TYPE_ENUM datatype;  // check DATA_TYPE_STRING
    char **data;                   // retrun data array
    int data_len;                  // the len of data array
};

struct Listening_widget_list{
    GtkWidget *device_list;
    GtkWidget *datatype_list;
    GtkWidget *spin_button;
    GtkWidget *entry;
};

//init & create window using gtk
void *create_gtkinterface(void *data);

//should be called inside gtk main thread
void quit_device_window(GtkWidget *widget, gpointer data);

void handle_input(GtkWidget *widget, gpointer data);

void send_gest(int device,int datatype,char *data[],int array_len);