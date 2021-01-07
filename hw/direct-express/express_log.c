/**
 * @file express_log.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief 
 * @version 0.1
 * @date 2020-12-31
 * 
 * @copyright Copyright (c) 2020
 * 
 */

// #define STD_DEBUG_LOG
#include "direct-express/express_log.h"

#include "direct-express/express_device_common.h"

#define LOG_DIR "log//call"

#define LOG_FILE_SIZE (8 * 1024 * 1024)

static Thread_Context *log_thread_context = NULL;

static char *print_buf = NULL;
static int loc = 0;
static gint64 t_last = 0;

/**
 * @brief 用于替换printf的空函数，方便关掉标准输出
 * 
 * @param a 
 * @param ... 
 * @return int 
 */
int null_printf(const char *a, ...)
{
    // printf("%s\n",a);
    return 0;
}

/**
 * @brief log设备初始化，这里只是申请了1M的缓冲空间
 * 
 * @param context 
 */
void log_init(struct Thread_Context *context)
{
    if (print_buf == NULL)
    {
        print_buf = g_malloc(LOG_FILE_SIZE);
        memset(print_buf, 0, LOG_FILE_SIZE);
        t_last = g_get_real_time();
    }
}

/**
 * @brief 获取log设备处理线程的context，假如context，没有就新建，新建的过程中会新建一个线程
 * 
 * @param type_id 
 * @param thread_id 
 * @param info 
 * @return Thread_Context* 
 */
static Thread_Context *get_log_thread_context(int type_id, int thread_id, struct Express_Device_Info *info)
{

    if (log_thread_context == NULL)
    {
        log_thread_context = thread_context_create(thread_id, type_id, sizeof(Thread_Context), info);
    }
    return log_thread_context;
}

static char time_str[1024];
static unsigned long long time_cnt = 0;
char *get_now_time()
{
    time_cnt++;
    gint64 t_int = g_get_real_time();
    GDateTime *t = g_date_time_new_from_unix_utc((gint64)t_int / 1000000);
    gchar *t_s1 = g_date_time_format(t, "%F %T");
    sprintf(time_str, "%s.%06lld  %llu ", t_s1, t_int % 1000000, time_cnt);
    g_free(t_s1);
    g_date_time_unref(t);
    return time_str;
}

/**
 * @brief 打印debug消息，需要只有一个指针参数
 * 
 * @param call 
 */
static void call_printf(Thread_Context *context, Direct_Express_Call *call)
{
    // static int print_cnt=0;
    // printf("enter call printf");
    if (call->para_num == 1)
    {
        // print_cnt++;
        gint64 t_int = g_get_real_time();
        // GDateTime *t=g_date_time_new_from_unix_utc((gint64)t_int/1000000);
        // gchar *t_s1=g_date_time_format(t,"%F %T");
        // g_date_time_unref(t);
        // t=g_date_time_new_from_unix_utc((gint64)call->get_time/1000000);
        // gchar *t_s2=g_date_time_format(t,"%F %T");

        if (strlen((char *)call->elem_tail->para) + loc > LOG_FILE_SIZE - 256 || t_int - t_last > 1000000)
        {

            // printf("start write\n");

            char file_name[100];
            sprintf(file_name, "%s_%.16s.log", LOG_DIR, get_now_time());

            for (int i = 0; file_name[i] != 0; i++)
            {
                if (file_name[i] == ':')
                    file_name[i] = '-';
            }

            FILE *fd = fopen(file_name, "a+");

            print_buf[loc] = '\n';
            print_buf[loc + 1] = '\n';
            fwrite(print_buf, sizeof(char), loc + 2, fd);
            fclose(fd);
            memset(print_buf, 0, LOG_FILE_SIZE);
            // printf("write once %d\n",loc);
            loc = 0;
            t_last = t_int;
        }
        Direct_Express_Flag_Buf *flag_buf = (Direct_Express_Flag_Buf *)call->elem_header->para;
        int num = snprintf(print_buf + loc, LOG_FILE_SIZE - loc, "#LOG_GUEST num_free %d %s %d : %s\n", flag_buf->num_free, get_now_time(), call->thread_id, (char *)call->elem_tail->para);

        // int num=snprintf(print_buf+loc,LOG_FILE_SIZE-loc,"#LOG_GUEST %s %d : %s\n",get_now_time(),call->thread_id,(char *)call->elem_tail->para);
        if (num < 0)
        {
            num = snprintf(print_buf + loc, LOG_FILE_SIZE - loc, "#LOG_GUEST sprintf error %d", num);
        }

        loc += num;
        // g_free(t_s1);
        // g_date_time_unref(t);
    }

    //注意在处理完成之后要主动调用下callback函数用以回收数据
    call->callback(call, 1);
    return;
}

void call_printf_flush()
{
    char file_name[100];
    sprintf(file_name, "%s_%.16s.log", LOG_DIR, get_now_time());

    for (int i = 0; file_name[i] != 0; i++)
    {
        if (file_name[i] == ':')
            file_name[i] = '-';
    }
    FILE *fd = fopen(file_name, "a+");
    print_buf[loc] = '\n';
    print_buf[loc + 1] = '\n';
    fwrite(print_buf, sizeof(char), loc + 2, fd);
    fclose(fd);
    memset(print_buf, 0, LOG_FILE_SIZE);
    // printf("write once %d\n",loc);
    loc = 0;
}

static const Express_Device_Info express_log_info = {
    .name = "express-log",
    .type_id = EXPRESS_LOG_FUN_ID,
    .context_init = log_init,
    .call_handle = call_printf,
    .get_context = get_log_thread_context,
};

EXPRESS_DEVICE_INIT(EXPRESS_LOG, &express_log_info)
