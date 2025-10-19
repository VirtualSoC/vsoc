/**
 * @file express_log.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief
 * @version 0.1
 * @date 2020-12-31
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "hw/vsoc/express_log.h"
#include "hw/vsoc/express_platform.h"

#include <glib/gstdio.h>
#include <backtrace.h>
#include <signal.h>
#include <string.h>

#define LOG_DIR "log/"

#define LOG_BUFFER_SIZE (8 * 1024 * 1024)

Device_Log_Setting_Info express_device_log_setting_info= {
    .express_gpu_debug_level = 1,
    .express_gpu_log_to_host = 1,
    .express_gpu_log_with_buffer = 0,
    .express_gpu_open_opengl_trace = 0,
};

static const uint32_t crc_32_tab[] = {/* CRC polynomial 0xedb88320 */
                                0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
                                0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
                                0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
                                0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
                                0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
                                0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
                                0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
                                0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
                                0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
                                0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
                                0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
                                0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
                                0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
                                0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
                                0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
                                0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
                                0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
                                0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
                                0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
                                0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
                                0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
                                0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
                                0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
                                0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
                                0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
                                0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
                                0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
                                0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
                                0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
                                0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
                                0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
                                0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
                                0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
                                0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
                                0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
                                0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
                                0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
                                0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
                                0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
                                0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
                                0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
                                0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
                                0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d};

static Thread_Context *log_thread_context = NULL;

static gint64 t_last = 0;

static char *copy_test_buf = NULL;
static int copy_test_buf_len = 0;

void call_printf_flush(const char *buffer, gsize length);
void log_init(Thread_Context *context);
/**
 * @brief 用于替换printf的空函数，方便关掉标准输出
 *
 * @param a
 * @param ...
 * @return int
 */
int null_printf(const char *a, ...)
{
    return 0;
}

/**
 * @brief log设备初始化，这里只是申请了1M的缓冲空间
 *
 * @param context
 */
void log_init(Thread_Context *context)
{
    (void)context;
}

/**
 * @brief 获取log设备处理线程的context，假如context，没有就新建，新建的过程中会新建一个线程
 *
 * @param type_id
 * @param thread_id
 * @param info
 * @return Thread_Context*
 */
static Thread_Context *get_log_thread_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, uint64_t user_id, struct Express_Device_Info *info)
{

    if (log_thread_context == NULL)
    {
        log_thread_context = thread_context_create(device_id, thread_id, process_id, user_id, sizeof(Thread_Context), info);
    }
    return log_thread_context;
}

static char time_str[1024];

char *get_now_time(void)
{
    static GTimeZone *time_zone = NULL;

    if (time_zone == NULL) {
        t_last = g_get_real_time();
        GDateTime *t = g_date_time_new_from_unix_utc((gint64)t_last / 1000000);
        GDateTime *tz = g_date_time_to_local(t);

        time_zone = g_date_time_get_timezone(tz);

        g_date_time_unref(t);
        g_date_time_unref(tz);
    }

    if (!time_zone) return (char *)"";
    gint64 t_int = g_get_real_time();
    GDateTime *t = g_date_time_new_from_unix_utc((gint64)t_int / 1000000);

    //不要用这个函数，因为它运行一次要420us，相较于log设备最快每10us接收一次数据来说太慢了
    // GDateTime *tz = g_date_time_to_local(t);
    GDateTime *tz = g_date_time_to_timezone(t, time_zone);
    gchar *t_s1 = g_date_time_format(tz, "%F %T");
    sprintf(time_str, "%s.%06lld ", t_s1, t_int % 1000000);
    g_free(t_s1);
    g_date_time_unref(t);
    g_date_time_unref(tz);
    return time_str;
}

/**
 * @brief 打印debug消息，需要只有一个指针参数
 */
static bool call_printf(Thread_Context *context, uint64_t id, const Call_Para *all_para, int para_num)
{
    if (para_num != 1) {
        LOGW("parameter number mismatch");
        return false; // 参数数量不符合预期
    }

    unsigned long fun_id = GET_FUN_ID(id);

    const Call_Para *p = all_para;

    if (fun_id == 1) {
        // 打印一条日志
        if (!p->data || p->data_len == 0) return true; // 空日志也算成功

        static char *chunk_buffer = NULL;
        static gsize chunk_capacity = 0;

        const gsize chunk_limit = LOG_BUFFER_SIZE;
        gsize remaining = (gsize)p->data_len;
        gsize offset = 0;

        while (remaining > 0) {
            gsize chunk = remaining < chunk_limit ? remaining : chunk_limit;
            if (chunk > chunk_capacity) {
                char *new_buf = chunk_buffer ? g_realloc(chunk_buffer, chunk) : g_malloc(chunk);
                if (!new_buf) {
                    return false;
                }
                chunk_buffer = new_buf;
                chunk_capacity = chunk;
            }

            g_ops.read_from_guest_mem(p->data, chunk_buffer, offset, chunk);
            call_printf_flush(chunk_buffer, chunk);
            offset += chunk;
            remaining -= chunk;
        }

        const char newline[] = "\n";
        call_printf_flush(newline, sizeof(newline) - 1);
    } else if (fun_id == 2) {
        // 复制测试模式
        if (p->data_len > copy_test_buf_len) {
            if (copy_test_buf) g_free(copy_test_buf);
            copy_test_buf = g_malloc(p->data_len);
            copy_test_buf_len = p->data_len;
        }
        gint64 start_time = g_get_monotonic_time();
        g_ops.read_from_guest_mem(p->data, copy_test_buf, 0, p->data_len);
        gint64 spend_time = g_get_monotonic_time() - start_time;
        if (spend_time == 0) spend_time = 1;
        express_printf("get copy test %lld spend time %lld speed %lf M/s\n", p->data_len, spend_time, p->data_len * 1.0 * 1000000 / 1024 / 1024 / spend_time);
    } else if (fun_id == 3) {
        // 非复制测试模式
        express_printf("get no copy test %lld\n", p->data_len);
    } else {
        return false;
    }
    return true;
}

void call_printf_flush(const char *buffer, gsize length)
{
    char file_name[100];
    static char now_file_name[100];
    static FILE *fd = NULL;
    if (buffer == NULL || length == 0) {
        return;
    }

    sprintf(file_name, "%s/call_%.16s.log", LOG_DIR, get_now_time());

    for (int i = 0; file_name[i] != 0; i++)
    {
        if (file_name[i] == ':')
            file_name[i] = '-';
    }

    if (fd == NULL || strcmp(now_file_name, file_name) != 0)
    {
        strcpy(now_file_name, file_name);
        if (fd != NULL)
        {
            fclose(fd);
        }
        fd = fopen(file_name, "a+");
    }

    if (fd == NULL) {
        return;
    }

    fwrite(buffer, sizeof(char), length, fd);
}

Thread_Context *remove_log_thread_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, uint64_t user_id, struct Express_Device_Info *info)
{
    // prevent removing the global log thread context
    return NULL;
}

#define UPDC32(octet, crc) (crc_32_tab[((crc) ^ (octet)) & 0xff] ^ ((crc) >> 8))

unsigned int updateCRC32(unsigned char ch, unsigned int crc)
{
    return UPDC32(ch, crc);
}

/* Error callback: Prints errors that occur during backtrace processing */
static void error_callback(void *data, const char *msg, int errnum) {
    fprintf(stderr, "libbacktrace error: %s (errnum: %d)\n", msg, errnum);
}

/* Full callback: Called for every stack frame retrieved.
   It prints the program counter (PC), file name, line number, and function name. */
static int full_callback(void *data, uintptr_t pc, const char *filename, int lineno, const char *function) {
    if (filename == NULL)
        filename = "unknown";
    if (function == NULL)
        function = "unknown";

    fprintf(stderr, "PC: %p, file: %s:%d, function: %s()\n", (void *)pc, filename, lineno, function);
    return 0; /* return 0 to tell libbacktrace to continue the trace */
}

/* print qemu backtrace */
void backtrace(void) {
    // todo: remove hardcoded path
    struct backtrace_state *state = backtrace_create_state("bin/qemu-system-x86_64.exe", 1, error_callback, NULL);
    if (state == NULL) {
        fprintf(stderr, "backtrace_create_state error\n");
        return;
    }

    int ret = backtrace_full(state, 0, full_callback, error_callback, NULL);
    if (ret < 0) {
        fprintf(stderr, "error: backtrace_full() failed\n");
    }
}

void segfault_handler(int signum) {
    fprintf(stderr, "*** Thread %d caught signal %d ***\n", CURRENT_TID(), signum);
    backtrace();
    exit(1);
}

void register_signal_handlers(void) {
    signal(SIGSEGV, segfault_handler);
    signal(SIGABRT, segfault_handler);
    signal(SIGILL, segfault_handler);
    signal(SIGFPE, segfault_handler);
}

static Express_Device_Info express_log_info = {
    .enable_default = true,
    .name = "express-log",
    .option_name = NULL,
    .driver_name = NULL,
    .device_id = EXPRESS_LOG_DEVICE_ID,
    .device_type = OUTPUT_DEVICE_TYPE,
    .context_init = log_init,
    .call_handler = call_printf,
    .get_context = get_log_thread_context,
    .remove_context = remove_log_thread_context,
};

EXPRESS_DEVICE_INIT(express_log, &express_log_info)
