#include "direct-express/direct_express_distribute.h"
#include "direct-express/express_log.h"

#include "direct-express/express_device_fun_id.h"

#define LOG_DIR "call.log"


static Thread_Context *log_thread_context=NULL;


static char *print_buf=NULL;
static int loc=0;
static gint64 t_last=0;


// int null_my_print(char *a){
//     // printf("%s\n",a);
//     return 0;
// }

void log_init(struct Thread_Context *context){
    if(print_buf==NULL){
        print_buf=g_malloc(1024*1024);
        memset(print_buf,0,1024*1024);
        t_last=g_get_real_time();
    }
}

// int my_print(char *a){
//     return 0;
// }

static Thread_Context *get_log_thread_context(int type_id,int thread_id,struct Express_Device_Info *info){

    if(log_thread_context==NULL){
        log_thread_context=thread_context_create(thread_id,type_id,sizeof(Thread_Context),info);
    }
    return log_thread_context;
}



/**
 * @brief 打印debug消息，需要标号为0，并且只有一个指针参数
 * 
 * @param call 
 */
static void call_printf(Thread_Context *context,Direct_Express_Call *call)
{

        // printf("enter call printf");
    if (call->para_num == 1)
    {
        gint64 t_int=g_get_real_time();
        GDateTime *t=g_date_time_new_from_unix_utc((gint64)t_int/1000000);
        gchar *t_s1=g_date_time_format(t,"%F %T");
        // g_date_time_unref(t);
        // t=g_date_time_new_from_unix_utc((gint64)call->get_time/1000000);
        // gchar *t_s2=g_date_time_format(t,"%F %T");

        if(strlen((char *)call->elem_tail->para)+loc>1024*1024-256 || t_int-t_last>1000000){

            printf("start write\n");
            FILE *fd=fopen(LOG_DIR,"a+");
            print_buf[loc]='\n';
            print_buf[loc+1]='\n';
            fwrite(print_buf,sizeof(char),loc+2,fd);
            fclose(fd);
            memset(print_buf,0,1024*1024);
            printf("write once %d\n",loc);
            loc=0;
            t_last=t_int;
        }
        int num=snprintf(print_buf+loc,1024*1024-loc,"#LOG_GUEST %s.%06lld %d : %s\n",t_s1,t_int%1000000,call->thread_id,(char *)call->elem_tail->para);
        if(num<0){
            num=snprintf(print_buf+loc,1024*1024-loc,"#LOG_GUEST sprintf error %d",num);
        }
        
        loc+=num;
        g_free(t_s1);
        g_date_time_unref(t);
    }
    return;
}




static const Express_Device_Info express_log_info = {
    .name="express-log",
    .type_id=EXPRESS_LOG_FUN_ID,
    .context_init=log_init,
    .call_handle=call_printf,
    .get_context=get_log_thread_context,
};

EXPRESS_DEVICE_INIT(EXPRESS_LOG,&express_log_info)

