// #define STD_DEBUG_LOG

#include "hw/teleport-express/express_log.h"
#include "hw/teleport-express/express_event.h"
#include <string.h>
#include <stdlib.h>
#include <windows.h>

/**
 * @brief 创建一个事件
 * 
 * @param manual_reset wait之后是否需要手动重置
 * @param initial_state 初始的信号状态
 * @return void* 事件句柄
 */
void *create_event(int manual_reset, int initial_state) {
    express_printf("Create Event\n");
    return CreateEvent(NULL, manual_reset, initial_state, NULL);
}

/**
 * @brief 阻塞等待事件
 * 
 * @param event 事件句柄
 * @param milliseconds 超时时长
 * @return int 0成功等待，1超时，-1出错
 */
int wait_event(void *event, long milliseconds) {
    DWORD ret = WaitForSingleObject(event, milliseconds);
    if (ret == WAIT_OBJECT_0) 
        return 0;
    else if (ret == WAIT_TIMEOUT) 
        return 1;
    return -1;
}

/**
 * @brief 触发事件信号
 * 
 * @param event 事件
 * @return int 0成功触发，-1失败
 */
int set_event(void *event) {
    return SetEvent(event) ? 0 : -1;
}

/**
 * @brief 触发事件信号
 * 
 * @param event 事件
 * @return int 0成功触发，-1失败
 */
int reset_event(void *event) {
    express_printf("Reset Event\n");
    return ResetEvent(event) ? 0 : -1;
}

/**
 * @brief 删除一个事件
 * 
 * @param event 事件句柄
 */
void delete_event(void *event) {
    express_printf("Delete Event\n");
    CloseHandle(event);
}
