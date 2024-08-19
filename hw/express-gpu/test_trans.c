
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
    read_from_guest_mem(data, copy_large_buf, 0, len);

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
    // gint64 start_time = g_get_real_time();

    // express_printf("copy %lld %lx\n", buf_len, copy_large_buf);

    read_from_guest_mem(data, copy_large_buf, 0, len);

    // gint64 spend_time = g_get_real_time() - start_time;
    // if (spend_time == 0)
    // {
    //     spend_time = 1;
    // }

    // express_printf("copy size %lld spend time %lld speed %lf M/s\n", len, spend_time, len * 1.0 * 1000000 / 1024 / 1024 / spend_time);

    return;
}

void test_decode_invoke(void *context, Teleport_Express_Call *call)
{

    // Double_Buffer *egl_context = (render_context->render_double_buffer);

    Call_Para all_para[MAX_PARA_NUM];

    express_printf("test id %lx\n", call->id);

    switch (call->id)
    {

        /******* file '1-1-1' *******/

    case FUNID_test_no_copy_asyn:
    case FUNID_test_no_copy_sync:

    {

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);

        if (para_num != 1)
        {
            break;
        }

        void *data = all_para[0].data;
        size_t len = all_para[0].data_len;
        test_no_copy(data, len);
    }
    break;

    case FUNID_test_copy_sync:
    case FUNID_test_copy_asyn:

    {

        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);

        if (para_num != 1)
        {
            break;
        }

        void *data = all_para[0].data;
        size_t len = all_para[0].data_len;
        test_copy(data, len);
    }
    break;

    default:
        express_printf("error id %lx", call->id);

        break;
    }

    // if(need_speed){
    call->callback(call, 1);
    //}else{
    //    call->callback(call, 0);
    //}
    return;
}
