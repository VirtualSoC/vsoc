
// #define STD_DEBUG_LOG
// #define EGL_EGLEXT_PROTOTYPES
#include "hw/express-gpu/test_trans.h"
#include "hw/teleport-express/express_log.h"

char *copy_large_buf = NULL;
size_t buf_len = 0;

void test_no_copy(void *data, size_t len)
{
    express_printf("no copy size %lld\n", len);

    if (len > buf_len)
    {
        if (copy_large_buf != NULL)
        {
            g_free(copy_large_buf);
        }
        buf_len = len;
        copy_large_buf = g_malloc(buf_len);
    }

    if (((Guest_Mem *)data)->num == 1)
    {
        return;
    }
    g_ops.read_from_guest_mem(data, copy_large_buf, 0, len);

    return;
}

void test_copy(void *data, size_t len)
{
    if (len > buf_len)
    {
        if (copy_large_buf != NULL)
        {
            g_free(copy_large_buf);
        }
        buf_len = len;
        copy_large_buf = g_malloc(buf_len);
    }
    // gint64 start_time = g_get_monotonic_time();

    // express_printf("copy %lld %lx\n", buf_len, copy_large_buf);

    g_ops.read_from_guest_mem(data, copy_large_buf, 0, len);

    // gint64 spend_time = g_get_monotonic_time() - start_time;
    // if (spend_time == 0)
    // {
    //     spend_time = 1;
    // }

    // express_printf("copy size %lld spend time %lld speed %lf M/s\n", len, spend_time, len * 1.0 * 1000000 / 1024 / 1024 / spend_time);

    return;
}

bool test_decode_invoke(void *context, uint64_t id, const Call_Para *para, int para_num)
{
    (void)context; // unused for test path
    express_printf("test id %lx\n", id);
    bool ok = true;

    switch (id)
    {
    case FUNID_test_no_copy_asyn:
    case FUNID_test_no_copy_sync:
    {
        if (para_num != 1) { ok = false; break; }
        void *data = para[0].data;
        size_t len = para[0].data_len;
        test_no_copy(data, len);
    } break;
    case FUNID_test_copy_sync:
    case FUNID_test_copy_asyn:
    {
        if (para_num != 1) { ok = false; break; }
        void *data = para[0].data;
        size_t len = para[0].data_len;
        test_copy(data, len);
    } break;
    default:
        express_printf("error id %lx", id);
        ok = false;
        break;
    }
    return ok;
}
