#ifndef EXPRESS_CODEC_H
#define EXPRESS_CODEC_H

#include "hw/teleport-express/express_platform.h"
#include "hw/teleport-express/teleport_express_register.h"
#include "hw/teleport-express/teleport_express_distribute.h"
#include "hw/express-codec/dcodec_component.h"

typedef struct
{
    Thread_Context thread_context;

    uint64_t unique_id;

    DCodecComponent *component;

} Codec_Thread_Context;


#endif