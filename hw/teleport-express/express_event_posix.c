// #define STD_DEBUG_LOG

#include "hw/teleport-express/express_log.h"
#include "hw/teleport-express/express_event.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>  
#include <errno.h>  

/**
 * @brief 创建一个事件
 * 
 * @param manual_reset wait之后是否需要手动重置
 * @param initial_state 初始的信号状态
 * @return void* 事件句柄
 */
void *create_event(bool manual_reset, bool initial_state) {
    express_printf("Create Event\n");
    POSIX_HANDLE event = (POSIX_HANDLE)g_malloc0(HANDLE_SIZE);
    if (event == NULL) return NULL;
    event->manual_reset = manual_reset;
    event->signal_state = initial_state;
    if (pthread_mutex_init(&event->event_lock, NULL)) {
        free(event);
        return NULL;
    }
    if (pthread_cond_init(&event->event_cond, NULL)) {
        free(event);
        return NULL;
    }
    return event;
}

/**
 * @brief 阻塞等待事件
 * 
 * @param event 事件句柄
 * @param milliseconds 超时时长，0xffffffff代表无限等待
 * @return int 0成功等待，1超时，-1出错
 */
int wait_event(void *event, long milliseconds) {
    POSIX_HANDLE pevent = (POSIX_HANDLE) event;

    if (pthread_mutex_lock(&pevent->event_lock)) {
        return -1;
    }

    if (milliseconds == 0) {  // 非阻塞模式
        bool signaled = pevent->signal_state;
        if (!pevent->manual_reset && signaled) {
            pevent->signal_state = false;
        }
        pthread_mutex_unlock(&pevent->event_lock);
        return signaled;
    }

    struct timespec target_time;
    if (milliseconds != 0xffffffff) {  // 计算绝对时间
        clock_gettime(CLOCK_REALTIME, &target_time);
        target_time.tv_sec += milliseconds / 1000;
        target_time.tv_nsec += (milliseconds % 1000) * 1000000;
        if (target_time.tv_nsec >= 1000000000) {
            target_time.tv_sec++;
            target_time.tv_nsec -= 1000000000;
        }
    }

    int ret = 0;
    while (!pevent->signal_state) {
        if (milliseconds == 0xffffffff) {  // 无限等待
            ret = pthread_cond_wait(&pevent->event_cond, &pevent->event_lock);
        } else {
            ret = pthread_cond_timedwait(&pevent->event_cond, &pevent->event_lock, &target_time);
        }

        if (ret == ETIMEDOUT) {
            break;  // 超时
        } else if (ret != 0) {
            pthread_mutex_unlock(&pevent->event_lock);
            return -1;  // 错误
        }
    }

    bool success = pevent->signal_state;
    if (success && !pevent->manual_reset) {
        pevent->signal_state = false;  // 自动重置
    }

    if (pthread_mutex_unlock(&pevent->event_lock)) {
        return -1;
    }

    return success;
}

/**
 * @brief 触发事件信号
 * 
 * @param event 事件
 * @return int 0成功触发，-1失败
 */
int set_event(void *event) {
    POSIX_HANDLE pevent = (POSIX_HANDLE) event;
    if (pthread_mutex_lock(&pevent->event_lock)) {
        express_printf("Set Event: Failed to lock");
        return -1;
    }
    pevent->signal_state = true;
    if (pevent->manual_reset) {
        if (pthread_cond_broadcast(&pevent->event_cond)) {
            express_printf("Set Event: Failed to broadcast");
            return -1;
        }
    } else {
        if (pthread_cond_signal(&pevent->event_cond)) {
            express_printf("Set Event: Failed to signal");
            return -1;
        }
    }

    if (pthread_mutex_unlock(&pevent->event_lock)) {
        express_printf("Set Event: Failed to unlock");
        return -1;
    }
    return 0;
}

/**
 * @brief 触发事件信号
 * 
 * @param event 事件
 * @return int 0成功触发，-1失败
 */
int reset_event(void *event) {
    express_printf("Reset Event\n");
    POSIX_HANDLE pevent = (POSIX_HANDLE) event;
    if (pthread_mutex_lock(&pevent->event_lock)) {
        express_printf("Reset Event: Failed to lock");
        return -1;
    }
    pevent->signal_state = false;
    if (pevent->manual_reset) {
        if (pthread_cond_broadcast(&pevent->event_cond)) {
            express_printf("Reset Event: Failed to broadcast");
            return -1;
        }
    } else {
        if (pthread_cond_signal(&pevent->event_cond)) {
            express_printf("Reset Event: Failed to signal");
            return -1;
        }
    }

    if (pthread_mutex_unlock(&pevent->event_lock)) {
        express_printf("Reset Event: Failed to unlock");
        return -1;
    }
    return 0;
}

/**
 * @brief 删除一个事件
 * 
 * @param event 事件句柄
 */
void delete_event(void *event) {
    express_printf("Delete Event\n");
    POSIX_HANDLE pevent = (POSIX_HANDLE) event;
    pthread_mutex_destroy(&pevent->event_lock);
    pthread_cond_destroy(&pevent->event_cond);
    g_free(pevent);
}
