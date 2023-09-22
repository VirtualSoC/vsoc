#define STD_DEBUG_LOG
#define STD_DEBUG_LOG_GLOBAL_ON

#include "hw/teleport-express/express_log.h"
#include "hw/teleport-express/express_event.h"
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32 
#include <windows.h>
#else
#include <time.h>
#include <sys/time.h>  
#include <errno.h>  
#endif

/**
 * @brief 创建一个事件
 * 
 * @param manual_reset wait之后是否需要手动重置
 * @param initial_state 初始的信号状态
 * @return void* 事件句柄
 */
void *create_event(int manual_reset, int initial_state) {
    express_printf("Create Event\n");
#ifdef _WIN32
    HANDLE event = CreateEvent(NULL, manual_reset, initial_state, NULL);
#else
    POSIX_HANDLE event = (POSIX_HANDLE)malloc(HANDLE_SIZE);
    if (event == NULL) return NULL;
    memset(event, 0, HANDLE_SIZE);
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
#endif
    return event;
}

/**
 * @brief 阻塞等待事件
 * 
 * @param event 事件句柄
 * @param milliseconds 超时时长
 * @return int 0成功等待，1超时，-1出错
 */
int wait_event(void *event, long milliseconds) {
#ifdef _WIN32
    DWORD ret = WaitForSingleObject(event, milliseconds);
    if (ret == WAIT_OBJECT_0) 
        return 0;
    else if (ret == WAIT_TIMEOUT) 
        return 1;
    return -1;
#else
    POSIX_HANDLE pevent = (POSIX_HANDLE) event;
    struct timespec target_time;
    struct timeval current_time;
    int timeout = 0;
    gettimeofday(&current_time, NULL);
    target_time.tv_sec = current_time.tv_sec + milliseconds / 1000;
    target_time.tv_nsec = current_time.tv_usec * 1000 + (milliseconds % 1000) * 1000000;

    if (pthread_mutex_lock(&pevent->event_lock)) {
        //printf("lock failed\n");
        return -1;
    }
        
    while (!pevent->signal_state)
    {
        if (timeout = pthread_cond_timedwait(&pevent->event_cond, &pevent->event_lock, &target_time)) {
            //printf("timeout in wait event %lld %lld,timeout=%d errno %d\n",(long long)target_time.tv_sec,(long long)target_time.tv_nsec,(int)timeout,errno);
            if (timeout == ETIMEDOUT) {
                
                break;
            }
            pthread_mutex_unlock(&pevent->event_lock);
            return -1;
        }
    }

    if (!timeout && !pevent->manual_reset)
        pevent->signal_state = 0;
    if (pthread_mutex_unlock(&pevent->event_lock)){
        //printf("unlock failed\n");
        return -1;
    }
        
    return 0;
#endif
}

/**
 * @brief 触发事件信号
 * 
 * @param event 事件
 * @return int 0成功触发，-1失败
 */
int set_event(void *event) {
    //express_printf("Set Event\n");
#ifdef _WIN32
    return SetEvent(event) ? 0 : -1;
#else
    POSIX_HANDLE pevent = (POSIX_HANDLE) event;
    if (pthread_mutex_lock(&pevent->event_lock)) {
        express_printf("Set Event: Failed to lock");
        return -1;
    }
    pevent->signal_state = 1;
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
#endif
}

/**
 * @brief 触发事件信号
 * 
 * @param event 事件
 * @return int 0成功触发，-1失败
 */
int reset_event(void *event) {
    //express_printf("Set Event\n");
#ifdef _WIN32
    return ResetEvent(event) ? 0 : -1;
#else
    POSIX_HANDLE pevent = (POSIX_HANDLE) event;
    if (pthread_mutex_lock(&pevent->event_lock)) {
        express_printf("Reset Event: Failed to lock");
        return -1;
    }
    pevent->signal_state = 0;
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
#endif
}

/**
 * @brief 删除一个事件
 * 
 * @param event 事件句柄
 */
void delete_event(void *event) {
    express_printf("DeleteB Event\n");
#ifdef _WIN32
    CloseHandle(event);
#else
    POSIX_HANDLE pevent = (POSIX_HANDLE) event;
    pthread_mutex_destroy(&pevent->event_lock);
    pthread_cond_destroy(&pevent->event_cond);
    free(pevent);
#endif
}
