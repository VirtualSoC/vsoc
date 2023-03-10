/**
 * @file express_bridge.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-2-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#define STD_DEBUG_LOG

#define DEBUG_HEAD "express_bridge "

#include "hw/express-network/express_bridge.h"
#include "qemu/sockets.h"

#define WRITE_CACHE_SIZE (512 * 1024)

#define HAS_COMMING_DATA(read_data) \
    ((read_data)->guest_read_loc == (read_data)->host_write_loc)

#define GET_READ_DATA_LEN(read_data)                              \
    (((read_data)->host_write_loc - (read_data)->guest_read_loc + \
      (read_data)->data_size) %                                   \
     (read_data)->data_size)

#define BRIDGE_FUN_BIND 1
#define BRIDGE_FUN_ACCEPT 2
#define BRIDGE_FUN_CONNECT 3
#define BRIDGE_FUN_OUTPUT 4
#define BRIDGE_FUN_END 5

#define NONE_STATUS 0
#define BIND_STATUS 1
#define CONNECTED_STATUS 2
#define CLOSED_STATUS 3

typedef struct Bridge_Read_Data
{
    volatile int host_write_loc;
    volatile int guest_read_loc;
    int data_size;
    int connected_id;
    char data[];
} __attribute__((packed, aligned(4))) Bridge_Read_Data;

static GHashTable *bridge_thread_contexts = NULL;

static GHashTable *accept_fd_thread_maps = NULL;


static void bridge_buffer_register(Guest_Mem *data, uint64_t thread_id, uint64_t process_id, uint64_t unique_id)
{

    Bridge_Thread_Context *context = g_hash_table_lookup(bridge_thread_contexts, GUINT_TO_POINTER(unique_id));

    printf("bridge register buffer thread_id %llu\n", thread_id);

    if (context->connection_context.guest_data != NULL)
    {
        free_copied_guest_mem(context->connection_context.guest_data);
    }

    context->connection_context.guest_data = data;
}

static void bridge_irq_register(Teleport_Express_Call *call)
{

    Bridge_Thread_Context *context = g_hash_table_lookup(bridge_thread_contexts, GUINT_TO_POINTER(call->unique_id));
    printf("bridge register irq thread_id %llu context %llx\n", call->thread_id, (uint64_t)context);

    if(context != NULL){
        context->connection_context.irq_call = call;

    }

}

// static void bridge_irq_release(Teleport_Express_Call *call)
// {
//     Bridge_Thread_Context *context = g_hash_table_lookup(bridge_thread_contexts, GUINT_TO_POINTER(call->unique_id));

//     free_copied_guest_mem(context->connection_context.guest_data);

//     if (context->connection_context.irq_call != NULL)
//     {
//         send_express_device_irq(context->connection_context.irq_call, 0, 0);
//         context->connection_context.irq_call = NULL;

//     }
// }

static int bridge_socket_listern(int port)
{
    struct sockaddr_in saddr;

    int fd, ret;

    inet_aton("127.0.0.1", &saddr.sin_addr);

    saddr.sin_port = htons(port);
    saddr.sin_family = AF_INET;

    fd = qemu_socket(PF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        printf("can't create stream socket %d", errno);
        return -1;
    }

    ret = bind(fd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret < 0)
    {
        printf("can't bind on socket port %d %d\n", port, errno);
        closesocket(fd);

        return -1;
    }
    ret = listen(fd, 0);
    if (ret < 0)
    {
        printf("can't listen on socket port %d %d", port, errno);
        closesocket(fd);
        return -1;
    }

    printf(DEBUG_HEAD "listern port %d ok\n", port);

    return fd;
}

static int bridge_socket_accept(int fd)
{
    struct sockaddr_in saddr;
    socklen_t addrlen;
    int client_fd = qemu_accept(fd, (struct sockaddr *)&saddr, &addrlen);
    printf(DEBUG_HEAD "get one connect fd %d\n", client_fd);

    return client_fd;
}

// void bridge_context_init(struct Thread_Context *context){

// }

// void bridge_context_destroy(struct Thread_Context *context)
// {
// }



static int fd_data_to_guest_mem(int fd, Guest_Mem *guest_mem, char *read_cache)
{
    Bridge_Read_Data *head = (Bridge_Read_Data *)guest_mem->scatter_data->data;

    if (guest_mem->scatter_data->len < 12)
    {
        // 不可能发生，这种情况直接关掉连接
        return -1;
    }

    int read_cnt = 0;
    int all_read_cnt = 0;

    int max_guest_size = (head->guest_read_loc - head->host_write_loc + head->data_size) % head->data_size;

    int max_write_size = max(max_guest_size, WRITE_CACHE_SIZE);

    do
    {
        // int buf_len = 0;
        // char *buf = get_next_continue_guest_mem(guest_mem, head->host_write_loc + sizeof(Bridge_Read_Data), &buf_len);

        // if (buf_len >= (head->guest_read_loc - head->host_write_loc + head->data_size) % head->data_size)
        // {
        //     return all_read_cnt;
        // }

        read_cnt = recv(fd, read_cache, max_write_size, 0);
        if (read_cnt < 0)
        {
            int err = errno;
            // printf(DEBUG_HEAD "read fd %d get %d err %d\n", fd, read_cnt, err);
            if (err == EINTR || err == EWOULDBLOCK || err == EAGAIN)
            {
                return all_read_cnt;
            }
            return -1;
        }
        else if (read_cnt == 0)
        {
            return -1;
        }

        printf(DEBUG_HEAD "read fd %d get %d write_loc %d\n", fd, read_cnt, head->host_write_loc);

        if (head->host_write_loc + read_cnt > head->data_size)
        {
            int first_write_size = head->data_size - head->host_write_loc;
            write_to_guest_mem(guest_mem, read_cache, head->host_write_loc + sizeof(Bridge_Read_Data), first_write_size);
            head->host_write_loc = 0;
            write_to_guest_mem(guest_mem, read_cache, head->host_write_loc + sizeof(Bridge_Read_Data), read_cnt - first_write_size);
            head->host_write_loc = read_cnt - first_write_size;
        }
        else
        {
            write_to_guest_mem(guest_mem, read_cache, head->host_write_loc + sizeof(Bridge_Read_Data), read_cnt);
            head->host_write_loc = (head->host_write_loc + read_cnt) % head->data_size;
        }

        all_read_cnt += read_cnt;

    } while (read_cnt > 0);

    return -1;
}


static void *bridge_read_host_thread(void *opaque)
{
    Bridge_Thread_Context *bridge_context = (Bridge_Thread_Context *)opaque;

    bool need_send_irq = false;

    // 512k的读取缓存
    char *read_cache = g_malloc0(WRITE_CACHE_SIZE);

    while (bridge_context->connection_context.read_thread_should_running == true)
    {
        int sleep_cnt = 0;
        while (bridge_context->connection_context.guest_data == NULL)
        {
            g_usleep(2000);
            sleep_cnt++;
            if (sleep_cnt > 10000)
            {
                closesocket(bridge_context->connection_context.socket_fd);
                bridge_context->status_id = CLOSED_STATUS;
                printf("error! no guest_mem with read_host_thread\n");
                return NULL;
            }
        }

        Guest_Mem *guest_mem = bridge_context->connection_context.guest_data;

        // Bridge_Read_Data *head = guest_mem->scatter_data->data;
        // if (guest_mem->scatter_data->len < 12)
        // {
        //     // 不可能发生，这种情况直接关掉连接
        //     closesocket(bridge_context->connection_context.socket_fd);
        //     return NULL;
        // }

        int ret = fd_data_to_guest_mem(bridge_context->connection_context.socket_fd, guest_mem, read_cache);

        if (ret > 0)
        {
            // 注入中断
            printf(DEBUG_HEAD"read get data %d\n", ret);
            if (bridge_context->connection_context.irq_call != NULL)
            {
                send_express_device_irq(bridge_context->connection_context.irq_call, 0, 0);
                bridge_context->connection_context.irq_call = NULL;
                need_send_irq = false;
            }
            else
            {
                need_send_irq = true;
            }
        }
        else if (ret == 0)
        {
            if (need_send_irq == true && bridge_context->connection_context.irq_call != NULL)
            {
                send_express_device_irq(bridge_context->connection_context.irq_call, 0, 0);
                bridge_context->connection_context.irq_call = NULL;
                need_send_irq = false;
            }
            g_usleep(1000);
        }
        else
        {
            // 断开连接
            bridge_context->status_id = CLOSED_STATUS;
            break;
        }
    }

    closesocket(bridge_context->connection_context.socket_fd);

    if (bridge_context->connection_context.guest_data != NULL)
    {
        free_copied_guest_mem(bridge_context->connection_context.guest_data);
    }

    if (bridge_context->connection_context.irq_call != NULL)
    {
        printf(DEBUG_HEAD "bridge thread send end irq\n");
        send_express_device_irq(bridge_context->connection_context.irq_call, -1, 0);
        bridge_context->connection_context.irq_call = NULL;
    }

    printf(DEBUG_HEAD "bridge thread exit closefd %d\n", bridge_context->connection_context.socket_fd);


    // if(bridge_context->connection_context.read_thread_should_running == true){
    // }

    return NULL;
}

static void bridge_output_call_handle(struct Thread_Context *context, Teleport_Express_Call *call)
{

    Call_Para all_para[1];
    int para_num = get_para_from_call(call, all_para, 1);

    Bridge_Thread_Context *bridge_context = (Bridge_Thread_Context *)context;

    uint64_t fun_id = GET_FUN_ID(call->id);
    // uint64_t process_id = call->process_id;
    // uint64_t thread_id = call->thread_id;
    // unsigned long process_id=call->process_id;

    printf(DEBUG_HEAD "bridge get call_id %llu\n",fun_id);
    switch (fun_id)
    {
    case BRIDGE_FUN_BIND:
    {
        if (para_num == 1 && all_para[0].data_len == 4 && bridge_context->status_id == NONE_STATUS)
        {
            int null_flag = 0;
            int *port_ptr = get_direct_ptr(all_para[0].data, &null_flag);
            if (unlikely(port_ptr == NULL))
            {
                printf("error BRIDGE_FUN_BIND port NULL\n");
                break;
            }
            int ret_fd = bridge_socket_listern(*port_ptr);
            if (ret_fd == -1)
            {
                *port_ptr = 0;
                break;
            }
            bridge_context->status_id = BIND_STATUS;
            bridge_context->connection_context.socket_fd = ret_fd;
        }
    }
    break;
    case BRIDGE_FUN_ACCEPT:
    {
        if (para_num == 1 && all_para[0].data_len == 4 && bridge_context->status_id == BIND_STATUS)
        {
            int null_flag = 0;
            int *port_ptr = get_direct_ptr(all_para[0].data, &null_flag);
            if (unlikely(port_ptr == NULL))
            {
                printf("error BRIDGE_FUN_BIND port NULL\n");
                break;
            }
            int ret_fd = bridge_socket_accept(bridge_context->connection_context.socket_fd);

            if (ret_fd == -1)
            {
                *port_ptr = 0;
                break;
            }
            qemu_socket_set_nonblock(ret_fd);
            g_hash_table_insert(accept_fd_thread_maps, GUINT_TO_POINTER(ret_fd), (gpointer)context->thread_id);

            *port_ptr = ret_fd;
        }
    }
    break;
    case BRIDGE_FUN_CONNECT:
    {
        if (para_num == 1 && all_para[0].data_len == 4 && bridge_context->status_id == NONE_STATUS)
        {
            int null_flag = 0;
            int *port_ptr = get_direct_ptr(all_para[0].data, &null_flag);
            if (unlikely(port_ptr == NULL))
            {
                printf("error BRIDGE_FUN_BIND port NULL\n");
                break;
            }
            uint64_t thread_id = (uint64_t)g_hash_table_lookup(accept_fd_thread_maps, GUINT_TO_POINTER(*port_ptr));
            
            printf(DEBUG_HEAD "BRIDGE_FUN_CONNECT thread_id %llu %llu, port %d\n",thread_id,  context->thread_id, *port_ptr);
            if (thread_id == context->thread_id)
            {
                bridge_context->connection_context.socket_fd = *port_ptr;

                bridge_context->connection_context.read_thread_should_running = true;
                bridge_context->status_id = CONNECTED_STATUS;

                qemu_thread_create(&bridge_context->connection_context.read_thread, "bridge_read_host_thread", bridge_read_host_thread, bridge_context, QEMU_THREAD_DETACHED);
            }
            else
            {
                *port_ptr = 0;
            }
        }
    }
    break;
    case BRIDGE_FUN_OUTPUT:
    {
        if (para_num == 1 && bridge_context->status_id == CONNECTED_STATUS && bridge_context->connection_context.socket_fd != 0 &&
            all_para[0].data != NULL && all_para[0].data_len != 0)
        {
            int num = all_para[0].data->num;
            Scatter_Data *scatter_data = all_para[0].data->scatter_data;
            for (int i = 0; i < num; i++)
            {
                send(bridge_context->connection_context.socket_fd, scatter_data[i].data, scatter_data[i].len, 0);
            }
        }
    }
    break;
    case BRIDGE_FUN_END:
    {
        if(para_num == 0){
            if (bridge_context->status_id == CONNECTED_STATUS)
            {
                bridge_context->connection_context.read_thread_should_running = false;
                // closesocket(bridge_context->connection_context.socket_fd);
                // 等待线程退出
                qemu_thread_join(&bridge_context->connection_context.read_thread);

                printf(DEBUG_HEAD "wait read thread exit ok %d\n", bridge_context->connection_context.socket_fd);

            }
            else if (bridge_context->status_id == BIND_STATUS)
            {
                printf(DEBUG_HEAD "close bind socket %d\n", bridge_context->connection_context.socket_fd);
                
                closesocket(bridge_context->connection_context.socket_fd);
            }
            else if (bridge_context->connection_context.irq_call != NULL)
            {
                if (bridge_context->connection_context.irq_call != NULL)
                {
                    send_express_device_irq(bridge_context->connection_context.irq_call, -1, 0);
                    bridge_context->connection_context.irq_call = NULL;
                }
            }

            g_hash_table_remove(bridge_thread_contexts, GUINT_TO_POINTER(bridge_context->unique_id));
            bridge_context->thread_context.thread_run = 0;
        }
    }
    break;
    default:
    {
    }
    break;
    }

    call->callback(call, 1);
}

static Thread_Context *get_bridge_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    if (bridge_thread_contexts == NULL)
    {
        bridge_thread_contexts = g_hash_table_new(g_direct_hash, g_direct_equal);
    }

    if(accept_fd_thread_maps == NULL)
    {
        accept_fd_thread_maps = g_hash_table_new(g_direct_hash, g_direct_equal);
    }

    Thread_Context *context = (Thread_Context *)g_hash_table_lookup(bridge_thread_contexts, GUINT_TO_POINTER(unique_id));
    // express_printf("g_hash table lookup\n");
    // 没有context就新建线程
    if (context == NULL)
    {
        // express_printf("create new thread\n");
        express_printf("create new thread context\n");
        context = thread_context_create(thread_id, device_id, sizeof(Bridge_Thread_Context), info);
 
        Bridge_Thread_Context *b_context = (Bridge_Thread_Context *)context;
        b_context->unique_id = unique_id;

        g_hash_table_insert(bridge_thread_contexts, GUINT_TO_POINTER(unique_id), (gpointer)context);
    }
    return context;
}

static void remove_bridge_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    // Thread_Context *context = g_hash_table_lookup(bridge_thread_contexts, GUINT_TO_POINTER(unique_id));

    g_hash_table_remove(bridge_thread_contexts, GUINT_TO_POINTER(unique_id));
}

// static char *get_next_continue_guest_mem(Guest_Mem *guest_mem, int start, int *mem_len)
// {
//     Scatter_Data *scatter_data = guest_mem->scatter_data;

//     // int loc_start = 0;
//     int now_len = 0;
//     while (now_len + scatter_data->len < start)
//     {
//         now_len += scatter_data->len;
//         scatter_data++;
//     }
//     *mem_len = scatter_data->len - (start - now_len);
//     return (char *)scatter_data->data + (start - now_len);
// }


static Express_Device_Info express_bridge_info = {
    .enable_default = true,
    .name = "express-bridge",
    .option_name = "bridge",
    .driver_name = "express_bridge",
    .device_id = EXPRESS_BRIDGE_DEVICE_ID,
    .device_type = INPUT_DEVICE_TYPE | OUTPUT_DEVICE_TYPE,

    .buffer_register = bridge_buffer_register,
    .irq_register = bridge_irq_register,
    // .irq_release = bridge_irq_release,

    // .context_init = bridge_context_init,
    // .context_destroy = bridge_context_destroy,

    .call_handle = bridge_output_call_handle,
    .get_context = get_bridge_context,
    .remove_context = remove_bridge_context,

    .static_prop = NULL,
    .static_prop_size = 0,

};

EXPRESS_DEVICE_INIT(express_bridge, &express_bridge_info)
