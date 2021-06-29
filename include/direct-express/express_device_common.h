#ifndef EXPRESS_DEVICE_COMMON_H
#define EXPRESS_DEVICE_COMMON_H

#include "direct-express/direct_express_distribute.h"

#define EXPRESS_GPU_FUN_ID ((uint64_t)1)
#define EXPRESS_LOG_FUN_ID ((uint64_t)2)


//device设备的id在高4字节，需要调用的函数id在低3字节，设备id决定到底哪个线程去处理，函数id决定怎么处理，中间一个字节的每个位决定函数处理是异步同步等信息
//设备id（4字节）|标志位（1字节）|函数id（3字节）
#define GET_DEVICE_ID(id)  ((id) >> 32)
#define GET_FUN_ID(id)     ((id)&0xffffff)
#define FUN_NEED_SYNC(id)   (((id)>>24)&0x1)
#define FUN_HAS_HOST_SYNC(id) (((id) >> 24) & 0x2)



//FUN_ID为0是保留字段

// #define FUNID_eglSwapBuffers_special ((EXPRESS_GPU_FUN_ID<<32u)+(((uint64_t)0x8+0x4)<<24u)+100000)


#endif