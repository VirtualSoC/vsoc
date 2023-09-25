#ifndef QEMU_EXPRESS_LOG_H
#define QEMU_EXPRESS_LOG_H
#include <stdio.h>
#include "qemu/osdep.h"
#include "qemu/log.h"

// define this in a .c file before including this header enables debug log for that file locally.
//#define STD_DEBUG_LOG

// uncomment the following line to disable debug logging globally. only warnings and errors will be logged then.
// #undef STD_DEBUG_LOG

// uncomment the following line to enable debug logging globally, regardless of verbosity and per-file options.
//#define STD_DEBUG_LOG_OVERRIDE_ENABLE

#define HOST_LOG_LEVEL_ERROR 1
#define HOST_LOG_LEVEL_WARN 2
#define HOST_LOG_LEVEL_INFO 3
#define HOST_LOG_LEVEL_DEBUG 4
#define HOST_LOG_LEVEL_VERBOSE 5

#ifdef CONFIG_GETTID
#define CURRENT_TID() gettid()
#elif defined(SYS_gettid)
#define CURRENT_TID() syscall(SYS_gettid)
#elif defined(__WIN32__)
#include <processthreadsapi.h>
#define CURRENT_TID() GetCurrentThreadId()
#elif defined(__APPLE__)
#define CURRENT_TID() GetCurrentThreadIdOnMac()
static int64_t GetCurrentThreadIdOnMac()
{
    int64_t pid = -1;
    pthread_threadid_np(NULL, &pid);
    return pid;
}
#else
#warning "thread id query not supported in the current system!"
#define CURRENT_TID() -1
#endif

static const char _level_chars[] = {'F', 'E', 'W', 'I', 'D', 'V'};

#define RED(a) "\033[31m" a "\033[0m"
#define GREEN(a) "\033[32m" a "\033[0m"
#define YELLOW(a) "\033[33m" a "\033[0m"

#define _host_log(level, fmt, ...)                                                      \
    {                                                                                   \
        printf("%s %lld %c [%s:%d]: " fmt "%c", get_now_time(), (int64_t)CURRENT_TID(),                   \
                 _level_chars[level], __FILE__, __LINE__, ##__VA_ARGS__, 10);                               \
    }
#define _host_log_debug_nolf(fmt, ...)                                                  \
    {                                                                                   \
        printf("%s %lld %c [%s:%d]: " fmt, get_now_time(), (int64_t)CURRENT_TID(), 'D', __FILE__, __LINE__, ##__VA_ARGS__);   \
    }

#define LOGV(fmt, ...) _host_log(HOST_LOG_LEVEL_VERBOSE, fmt, ##__VA_ARGS__)
#define LOGD(fmt, ...) _host_log(HOST_LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...) _host_log(HOST_LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) _host_log(HOST_LOG_LEVEL_WARN, YELLOW(fmt), ##__VA_ARGS__)
#define LOGE(fmt, ...) _host_log(HOST_LOG_LEVEL_ERROR, RED(fmt), ##__VA_ARGS__)

#if defined(STD_DEBUG_LOG) || defined(STD_DEBUG_LOG_OVERRIDE_ENABLE)
#define express_printf _host_log_debug_nolf
#else
#define express_printf null_printf
#undef LOGD
#define LOGD null_printf
#undef LOGV
#define LOGV null_printf
#endif

#ifdef TIMER_LOG

#define TIMER_START(a)                 \
    static gint64 timer_spend_##a = 0; \
    static gint64 timer_cnt_##a = 0;   \
    gint64 temp_timer_##a = g_get_real_time();

#define TIMER_END(a)                                       \
    timer_spend_##a += g_get_real_time() - temp_timer_##a; \
    timer_cnt_##a += 1;

#define TIMER_OUTPUT(a, fre)                  \
    if (fre == 0 || timer_cnt_##a % fre == 0) \
        printf("timer-" #a " cnt %lld all %lld avg %lf\n", timer_cnt_##a, timer_spend_##a, timer_spend_##a * 1.0 / timer_cnt_##a);

#define TIMER_RESET(a)   \
    timer_spend_##a = 0; \
    timer_cnt_##a = 0;
#else

#define TIMER_START(a)
#define TIMER_END(a)
#define TIMER_OUTPUT(a, fre)
#define TIMER_RESET(a)

#endif

typedef struct Device_Log_Setting_Info
{
    int express_gpu_debug_level;
    int express_gpu_log_to_host;
    int express_gpu_log_with_buffer;
    int express_gpu_open_opengl_trace;
} __attribute__((packed, aligned(4))) Device_Log_Setting_Info;



char *get_now_time(void);
int null_printf(const char *a, ...);

unsigned int updateCRC32(unsigned char ch, unsigned int crc);

#endif
