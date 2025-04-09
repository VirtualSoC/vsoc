#ifndef EXPRESS_EVENT_H
#define EXPRESS_EVENT_H

#include <stdbool.h>

#ifndef _WIN32
#include <pthread.h>
typedef struct
{
    bool signal_state; // 0代表事件未触发，1代表触发
    bool manual_reset;   // 0代表每次wait后自动重置，1代表需要手动重置
    pthread_mutex_t event_lock;
    pthread_cond_t event_cond;
} POSIX_Event;
#define POSIX_HANDLE POSIX_Event *
#define HANDLE_SIZE sizeof(POSIX_Event)
#endif

void *create_event(bool manual_reset, bool initial_state);

int wait_event(void *event, long milliseconds);

int set_event(void *event);

int reset_event(void *event);

void delete_event(void *event);

#endif
