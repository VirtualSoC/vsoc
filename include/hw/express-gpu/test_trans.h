#ifndef TEST_TRANS_H
#define TEST_TRANS_H

#include "hw/teleport-express/express_device_common.h"
#include "hw/teleport-express/teleport_express_call.h"

#define MAX_OUT_BUF_LEN 4096

#define FUNID_test_no_copy_asyn ((EXPRESS_GPU_DEVICE_ID << 32u) + 200000)
#define FUNID_test_no_copy_sync ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 200001)
#define FUNID_test_copy_asyn ((EXPRESS_GPU_DEVICE_ID << 32u) + 200002)
#define FUNID_test_copy_sync ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 200003)

void test_decode_invoke(void *context, Teleport_Express_Call *call);

void test_no_copy(void *data, size_t len);

void test_copy(void *data, size_t len);

#endif
