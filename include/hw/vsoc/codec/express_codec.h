#ifndef EXPRESS_CODEC_H
#define EXPRESS_CODEC_H

#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/codec/dcodec_component.h"

typedef struct
{
    Thread_Context thread_context;

    Device_Context device_context;

    uint64_t unique_id;

    DCodecComponent *component;

} Codec_Thread_Context;


#endif