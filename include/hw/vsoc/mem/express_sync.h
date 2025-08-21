#ifndef EXPRESS_SYNC_H
#define EXPRESS_SYNC_H

#include "hw/vsoc/gpu/express_gpu_snapshot.h"

void signal_express_sync(int sync_id, bool need_gpu_sync);
void wait_for_express_sync(int sync_id, bool need_gpu_sync);
void load_sync_context(QEMUFile *f);
void save_sync_context(QEMUFile *f);


#endif