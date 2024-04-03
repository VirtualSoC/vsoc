/**
 * @file express_gpu_pci.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief
 * @version 0.1
 * @date 2020-9-22
 *
 * @copyright Copyright (c) 2020
 *
 */
// #define STD_DEBUG_LOG

#include "hw/teleport-express/teleport_express_pci.h"
//#include "hw/teleport-express/express_log.h"
#include "hw/teleport-express/express_device_common.h"
#include "qapi/error.h"

char *kernel_load_express_driver_names = NULL;
int driver_names_len = 0;
int kernel_load_express_driver_num = 0;

static GHashTable *all_register_device_info = NULL;

static Property teleport_express_base_properties[] = {
    // 这个vectors变量决定了是否还是用msix中断，假如没有这行，则为APIC中断
    // 若使用APIC中断，则可能出现中断不能及时处理，引发__report_bad_irq函数报错，进而导致中断速度缓慢（If 99,900 of the previous 100,000 interrupts have not been handled* then assume that the IRQ is stuck in some manner）
    DEFINE_PROP_UINT32("vectors", VirtIOPCIProxy, nvectors, 3),
    DEFINE_PROP_BOOL("gl_debug", Teleport_Express_PCI, enalbe_opengl_debug, false),
    DEFINE_PROP_BOOL("independ_window", Teleport_Express_PCI, enable_independ_window, false),
    DEFINE_PROP_BOOL("keep_window_scale", Teleport_Express_PCI, keep_window_scale, true),
    DEFINE_PROP_INT32("window_width", Teleport_Express_PCI, window_width, 1280),
    DEFINE_PROP_INT32("window_height", Teleport_Express_PCI, window_height, 720),

    DEFINE_PROP_BOOL("device_input_window", Teleport_Express_PCI, show_device_input_window, false),

    DEFINE_PROP_INT32("display_width", Teleport_Express_PCI, display_width, 1920),
    DEFINE_PROP_INT32("display_height", Teleport_Express_PCI, display_height, 1080),
    DEFINE_PROP_INT32("refresh_rate", Teleport_Express_PCI, refresh_rate, 60),

    DEFINE_PROP_INT32("phy_width", Teleport_Express_PCI, phy_width, 1920),
    DEFINE_PROP_INT32("phy_height", Teleport_Express_PCI, phy_height, 1080),


    DEFINE_PROP_INT32("gl_log_level", Teleport_Express_PCI, gpu_debug_level, 1),
    DEFINE_PROP_BOOL("gl_log_to_host", Teleport_Express_PCI, gpu_log_to_host, true),
    DEFINE_PROP_BOOL("buffer_log", Teleport_Express_PCI, gpu_log_with_buffer, false),
    DEFINE_PROP_BOOL("opengl_trace", Teleport_Express_PCI, opengl_trace, false),

    DEFINE_PROP_BOOL("scroll_is_zoom", Teleport_Express_PCI, scroll_is_zoom, true),
    DEFINE_PROP_BOOL("right_click_is_two_finger", Teleport_Express_PCI, right_click_is_two_finger, true),
    DEFINE_PROP_INT32("scroll_ratio", Teleport_Express_PCI, scroll_ratio, 20),

    DEFINE_PROP_BOOL("finger_replay", Teleport_Express_PCI, finger_replay, true),

    DEFINE_PROP_BOOL("display_switch", Teleport_Express_PCI, display_switch_open, false),

    DEFINE_PROP_BOOL("shader_binary", Teleport_Express_PCI, open_shader_binary, true),

    DEFINE_PROP_STRING("ruim_file", Teleport_Express_PCI, ruim_file),

    DEFINE_PROP_END_OF_LIST(),
};

static Property *teleport_express_all_properties = NULL;

/**
 * @brief 所有的express设备共用的init函数，这个函数会在main函数前调用
 *
 * @param info
 */
void express_device_init_common(Express_Device_Info *info)
{
    if (all_register_device_info == NULL)
    {
        all_register_device_info = g_hash_table_new(g_direct_hash, g_direct_equal);
    }

    g_hash_table_insert(all_register_device_info, GUINT_TO_POINTER(info->device_id), (gpointer)info);
}

Express_Device_Info *get_express_device_info(unsigned int device_id)
{
    if (all_register_device_info == NULL)
    {
        return NULL;
    }

    return (Express_Device_Info *)g_hash_table_lookup(all_register_device_info, GUINT_TO_POINTER(device_id));
}

/**
 * @brief 根据Express_Device_Info里的内容产生给qemu命令行用的Property
 *
 * @param key
 * @param value
 * @param userData
 */
static void fill_property(void *key, void *value, void *userData)
{
    Express_Device_Info *info = (Express_Device_Info *)value;

    int index = *(int *)userData;

    if (info->option_name != NULL)
    {
        Property p = DEFINE_PROP_BOOL(info->option_name, Teleport_Express_PCI, express_device_enable[index], info->enable_default);

        info->device_index = index;

        teleport_express_all_properties[index] = p;
        (*(int *)userData)++;
    }
    else
    {
        info->device_index = -1;
    }
}

static void copy_express_driver_name(Express_Device_Info *info)
{
    if (info->driver_name != NULL)
    {
        strcpy(kernel_load_express_driver_names + driver_names_len, info->driver_name);
        driver_names_len += strlen(info->driver_name);

        kernel_load_express_driver_names[driver_names_len] = ' ';
        driver_names_len += 1;

        kernel_load_express_driver_num += 1;
    }
}

/**
 * @brief 根据命令行的结果与是否默认开启的属性，产生要输入给内核的模块名字列表，内核将要加载这些模块。同时顺便更新info里的enable属性，方便模块内部判断
 *
 * @param key
 * @param value
 * @param userData
 */
static void fill_kernel_driver_name(void *key, void *value, void *userData)
{
    Express_Device_Info *info = (Express_Device_Info *)value;

    Teleport_Express_PCI *express_pci = (Teleport_Express_PCI *)userData;

    if (info->device_id == EXPRESS_GPU_DEVICE_ID || info->device_id == EXPRESS_BRIDGE_DEVICE_ID)
    {
        return;
    }

    if (info->device_index == -1 || express_pci->express_device_enable[info->device_index])
    {
        info->enable = true;
        copy_express_driver_name(info);
    }
    else
    {
        info->enable = false;
    }
}

static void calc_driver_names_len(void *key, void *value, void *userData)
{
    Express_Device_Info *info = (Express_Device_Info *)value;

    Teleport_Express_PCI *express_pci = (Teleport_Express_PCI *)userData;

    if (info->driver_name != NULL && (info->device_index == -1 || express_pci->express_device_enable[info->device_index]))
    {
        driver_names_len += strlen(info->driver_name) + 1;
    }

    return;
}

static void init_express_driver_names(Teleport_Express_PCI *express_pci)
{
    if (kernel_load_express_driver_names != NULL)
    {
        g_free(kernel_load_express_driver_names);
        driver_names_len = 0;
    }

    g_hash_table_foreach(all_register_device_info, calc_driver_names_len, express_pci);

    kernel_load_express_driver_names = g_malloc0(driver_names_len + 1);
    driver_names_len = 0;

    Express_Device_Info *info = (Express_Device_Info *)g_hash_table_lookup(all_register_device_info, GUINT_TO_POINTER(EXPRESS_GPU_DEVICE_ID));

    if (express_pci->express_device_enable[info->device_index])
    {
        info->enable = true;
        copy_express_driver_name(info);
    }

    info = (Express_Device_Info *)g_hash_table_lookup(all_register_device_info, GUINT_TO_POINTER(EXPRESS_BRIDGE_DEVICE_ID));

    if (express_pci->express_device_enable[info->device_index])
    {
        info->enable = true;
        copy_express_driver_name(info);
    }

    g_hash_table_foreach(all_register_device_info, fill_kernel_driver_name, express_pci);
    kernel_load_express_driver_names[driver_names_len] = 0;

    printf("init_express_driver_names | %s|\n", kernel_load_express_driver_names);

    return;
}

static Property *get_express_device_property(void)
{
    if (all_register_device_info == NULL)
    {
        return teleport_express_base_properties;
    }

    if (teleport_express_all_properties != NULL)
    {
        return teleport_express_all_properties;
    }

    teleport_express_all_properties = g_malloc0(sizeof(Property) * g_hash_table_size(all_register_device_info) + sizeof(teleport_express_base_properties));

    int index = 0;
    g_hash_table_foreach(all_register_device_info, fill_property, &index);

    for (int i = 0; i < sizeof(teleport_express_base_properties) / sizeof(Property); i++)
    {
        teleport_express_all_properties[index] = teleport_express_base_properties[i];
        index++;
    }

    return teleport_express_all_properties;
}

static void teleport_express_pci_instance_init(Object *obj)
{

    Teleport_Express_PCI *dev = TELEPORT_EXPRESS_PCI(obj);

    virtio_instance_init_common(obj, &dev->teleport_express, sizeof(dev->teleport_express),
                                TYPE_TELEPORT_EXPRESS);
}

static void teleport_express_pci_realize(VirtIOPCIProxy *vpci_dev, Error **errp)
{

    Teleport_Express_PCI *express_pci = TELEPORT_EXPRESS_PCI(vpci_dev);
    Teleport_Express *e = &(express_pci->teleport_express);
    DeviceState *vdev = DEVICE(e);
    Error *local_error = NULL;

    qdev_set_parent_bus(vdev, BUS(&vpci_dev->bus), &local_error);
    if (local_error)
    {
        error_propagate(errp, local_error);
        return;
    }

    virtio_pci_force_virtio_1(vpci_dev);
    object_property_set_bool(OBJECT(vdev), "realized", true, &local_error);

    init_express_driver_names(express_pci);

    express_gpu_gl_debug_enable = express_pci->enalbe_opengl_debug;
    express_gpu_independ_window_enable = express_pci->enable_independ_window;
    express_device_input_window_enable = express_pci->show_device_input_window;
    express_gpu_keep_window_scale = express_pci->keep_window_scale;
    express_gpu_window_width = express_pci->window_width;
    express_gpu_window_height = express_pci->window_height;

    express_touchscreen_size[0] = express_pci->display_width;
    express_touchscreen_size[1] = express_pci->display_height;

    *express_display_pixel_width = express_pci->display_width;
    *express_display_pixel_height = express_pci->display_height;
    express_display_refresh_rate = express_pci->refresh_rate;

    *express_display_phy_width = express_pci->phy_width;
    *express_display_phy_height = express_pci->phy_height;

    express_device_log_setting_info.express_gpu_debug_level = express_pci->gpu_debug_level;
    express_device_log_setting_info.express_gpu_log_to_host = (int)express_pci->gpu_log_to_host;
    express_device_log_setting_info.express_gpu_log_with_buffer = (int)express_pci->gpu_log_with_buffer;
    express_device_log_setting_info.express_gpu_open_opengl_trace = (int)express_pci->opengl_trace;

    express_touchscreen_scroll_is_zoom = express_pci->scroll_is_zoom;
    express_touchscreen_right_click_is_two_finger = express_pci->right_click_is_two_finger;
    express_touchscreen_scroll_ratio = express_pci->scroll_ratio;

    express_keyboard_finger_replay = express_pci->finger_replay;

    express_display_switch_open = express_pci->display_switch_open;

    express_gpu_open_shader_binary = express_pci->open_shader_binary;

    express_ruim_file = express_pci->ruim_file;

    if (local_error)
    {
        error_propagate(errp, local_error);
        return;
    }

    // for (i = 0; i < g->conf.max_outputs; i++) {
    //     object_property_set_link(OBJECT(g->scanout[i].con),
    //                             OBJECT(vpci_dev),
    //                             "device", errp);
    // }
}

static void teleport_express_pci_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    VirtioPCIClass *k = VIRTIO_PCI_CLASS(klass);
    PCIDeviceClass *pcidev_k = PCI_DEVICE_CLASS(klass);

    set_bit(DEVICE_CATEGORY_DISPLAY, dc->categories);

    device_class_set_props(dc, get_express_device_property());

    dc->hotpluggable = false;
    k->realize = teleport_express_pci_realize;
    pcidev_k->class_id = PCI_CLASS_DISPLAY_OTHER;

    pcidev_k->revision = 0x1;
}

static void teleport_express_pci_register_types(void)
{

    const TypeInfo teleport_express_pci_info = {
        .name = TYPE_TELEPORT_EXPRESS_PCI,
        .parent = TYPE_VIRTIO_PCI,
        .instance_size = sizeof(Teleport_Express_PCI),
        .instance_init = teleport_express_pci_instance_init,
        .class_init = teleport_express_pci_class_init,
        .interfaces = (InterfaceInfo[]){
            {INTERFACE_PCIE_DEVICE},
            {INTERFACE_CONVENTIONAL_PCI_DEVICE},
            {}},
    };

    type_register_static(&teleport_express_pci_info);
}

type_init(teleport_express_pci_register_types);
