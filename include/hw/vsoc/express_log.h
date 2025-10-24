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

#define HOST_LOG_LEVEL_FATAL 0
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
        printf("%s %" PRId64 " %c [%s:%d]: " fmt "%c", get_now_time(), (int64_t)CURRENT_TID(),                   \
                 _level_chars[level], __FILE_NAME__, __LINE__, ##__VA_ARGS__, 10);                               \
    }

#define _host_log_err(level, fmt, ...)                                                                  \
    {                                                                                                   \
        fprintf(stderr, "%s %" PRId64 " %c [%s:%d]: " fmt "%c", get_now_time(), (int64_t)CURRENT_TID(), \
                 _level_chars[level], __FILE_NAME__, __LINE__, ##__VA_ARGS__, 10);                      \
    }


#define _host_log_debug_nolf(fmt, ...)                                                  \
    {                                                                                   \
        printf("%s %" PRId64 " %c [%s:%d]: " fmt, get_now_time(), (int64_t)CURRENT_TID(), 'D', __FILE_NAME__, __LINE__, ##__VA_ARGS__);   \
    }

#define LOGV(fmt, ...) _host_log(HOST_LOG_LEVEL_VERBOSE, fmt, ##__VA_ARGS__)
#define LOGD(fmt, ...) _host_log(HOST_LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...) _host_log(HOST_LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) _host_log(HOST_LOG_LEVEL_WARN, YELLOW(fmt), ##__VA_ARGS__)
#define LOGE(fmt, ...) _host_log_err(HOST_LOG_LEVEL_ERROR, RED(fmt), ##__VA_ARGS__)
#define LOGF(fmt, ...) _host_log_err(HOST_LOG_LEVEL_FATAL, RED(fmt), ##__VA_ARGS__)

#if defined(STD_DEBUG_LOG) || defined(STD_DEBUG_LOG_OVERRIDE_ENABLE)
#define express_printf _host_log_debug_nolf
#else
#define express_printf null_printf
#undef LOGD
#define LOGD null_printf
#undef LOGV
#define LOGV null_printf
#endif

#define MONITOR_LOG(mon, fmt, ...) monitor_log(mon, fmt, ##__VA_ARGS__)
void monitor_log(void *mon, const char *fmt, ...);

#ifdef TIMER_LOG

#define TIMER_START_ON_THREAD(a)                \
    static __thread gint64 timer_spend_##a = 0; \
    static __thread gint64 timer_cnt_##a = 0;   \
    gint64 temp_timer_##a = g_get_monotonic_time();

#define TIMER_START(a)                 \
    static gint64 timer_spend_##a = 0; \
    static gint64 timer_cnt_##a = 0;   \
    gint64 temp_timer_##a = g_get_monotonic_time();

#define TIMER_END(a)                                       \
    timer_spend_##a += g_get_monotonic_time() - temp_timer_##a; \
    timer_cnt_##a += 1; \

#define TIMER_PRINT(a, freq)                    \
    if (freq == 0 || timer_cnt_##a % freq == 0) \
        LOGI("timer-" #a " cnt %lld total %.3f ms avg %.3f ms", timer_cnt_##a, (double)timer_spend_##a / 1000, (double)timer_spend_##a / timer_cnt_##a / 1000);

#define TIMER_PRINT_MOVING(a, freq) \
if (freq == 0 || timer_cnt_##a % freq == 0) { \
    LOGI("timer-" #a " cnt %lld total %.3f ms moving avg %.3f ms", timer_cnt_##a, (double)timer_spend_##a / 1000, (double)timer_spend_##a / freq / 1000); \
    timer_spend_##a = 0; \
}

#define TIMER_PRINT_MOVING_GT(a, freq, threshold) \
if (freq == 0 || timer_cnt_##a % freq == 0) { \
    if ((double)timer_spend_##a / freq / 1000 > threshold) { \
        LOGI("timer-" #a " cnt %lld total %.3f ms moving avg %.3f ms", timer_cnt_##a, (double)timer_spend_##a / 1000, (double)timer_spend_##a / freq / 1000); \
    } \
    timer_spend_##a = 0; \
}

#define TIMER_RESET(a)   \
    timer_spend_##a = 0; \
    timer_cnt_##a = 0;

#else

#define TIMER_START(a)
#define TIMER_START_ON_THREAD(a)
#define TIMER_END(a)
#define TIMER_PRINT(a, fre)
#define TIMER_PRINT_MOVING(a, fre)
#define TIMER_PRINT_MOVING_GT(a, fre, threshold)
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

void backtrace(void);
void register_signal_handlers(void);

#endif
