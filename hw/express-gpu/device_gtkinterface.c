#include "hw/express-gpu/device_gtkinterface.h"

void send_gest(int device,int datatype,char *data[],int array_len)
{
    g_print("Device Selected: %s \n",DEVICE_STRING[device]);
    g_print("Device Data Type: %s \n",DATA_TYPE_STRING[datatype]);
    g_print("Input len: %d \n",array_len);
    for(int i=0;i<array_len;++i) g_print("Input: %s \n",data[i]);
    /*TODO send data to Guest*/
}

void handle_input(GtkWidget *widget, gpointer data)
{   
    struct Listening_widget_list *widgets = (struct Listening_widget_list *)data;
    int selected_device = atoi(gtk_combo_box_get_active_id(GTK_COMBO_BOX(widgets->device_list)));
    int selected_datatype = atoi(gtk_combo_box_get_active_id(GTK_COMBO_BOX(widgets->datatype_list)));
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(widgets->entry));
    // copy to buffer removing const for strsok() slpiting array
    gchar buffer[strlen(text)];
    strcpy(buffer,text);

    int len = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widgets->spin_button));
    
    gchar *output[len];
    char *cur = strtok(buffer,",");
    for(int idx = 0; idx<len; ++idx){
        output[idx] = cur;
        cur = strtok(NULL,",");
    }
    
    send_gest(selected_device,selected_datatype,output,len);
    gtk_entry_set_text(GTK_ENTRY(widgets->entry),"");
}

void destroy_gtkinterface(GtkWidget *widget, gpointer data)
{
    free((struct Listening_widget_list *)data);
    gtk_main_quit();
}

void *create_gtkinterface(void *data)
{
    GtkWidget *window, *submit_button, *grid, *label;
    struct Listening_widget_list *widgets = malloc(sizeof(struct Listening_widget_list));

    gtk_init(NULL, NULL);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "Device_input");
    gtk_window_set_default_size (GTK_WINDOW (window), 400, 400);
    gtk_container_set_border_width (GTK_CONTAINER (window), 20);
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), widgets);

    grid = gtk_grid_new();
    gtk_container_add (GTK_CONTAINER (window), grid);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);

    widgets->device_list = gtk_combo_box_text_new();
    for(enum DEVICE_ENUM i=Audio; i<Dev_Len; ++i)
    {
        char id[4];
        snprintf(id, 4,"%d",(int)i);
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(widgets->device_list), id, DEVICE_STRING[i]);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->device_list), 1);
    gtk_grid_attach (GTK_GRID (grid), widgets->device_list, 0, 0, 4, 1);

    widgets->datatype_list = gtk_combo_box_text_new();
    for(enum DATA_TYPE_ENUM i=Char; i<Datatype_Len; ++i)
    {
        char id[4];
        snprintf(id, 4, "%d",(int)i);
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(widgets->datatype_list), id, DATA_TYPE_STRING[i]);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->datatype_list), 1);
    gtk_grid_attach_next_to (GTK_GRID (grid), widgets->datatype_list, widgets->device_list, GTK_POS_RIGHT, 2, 1);

    widgets->spin_button = gtk_spin_button_new_with_range(1,MAX_INPUT_LEN,1);
    gtk_grid_attach_next_to (GTK_GRID (grid), widgets->spin_button, widgets->datatype_list, GTK_POS_RIGHT, 2, 1);

    submit_button = gtk_button_new_with_label("Submit");
    g_signal_connect(submit_button, "clicked", G_CALLBACK(handle_input), widgets);
    gtk_grid_attach_next_to (GTK_GRID (grid), submit_button, widgets->spin_button, GTK_POS_RIGHT, 1, 1);
    
    widgets->entry = gtk_entry_new ();
    gtk_grid_attach_next_to (GTK_GRID (grid), widgets->entry, widgets->device_list, GTK_POS_BOTTOM, 9, 5);

    label = gtk_label_new("");
    gtk_label_set_markup (GTK_LABEL (label), "<small>NOTICE: If input is an array set up the length of input array in spin button, and use comma to seperate elements.</small>");
    gtk_grid_attach_next_to (GTK_GRID (grid), label, widgets->entry, GTK_POS_BOTTOM, 9, 1);

    gtk_widget_show_all(window);
    gtk_main();

    return NULL;
}