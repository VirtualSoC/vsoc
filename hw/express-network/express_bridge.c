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

//#define STD_DEBUG_LOG

// #define BRIDGE_VERBOSE_OUTPUT

#ifdef BRIDGE_VERBOSE_OUTPUT
#define LOGV LOGD
#else
#define LOGV(...)
#endif

#define DEBUG_HEAD "express_bridge "

#include "hw/express-network/express_bridge.h"
#include "qemu/sockets.h"

#define WRITE_CACHE_SIZE (1024 * 1024 + 512)

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
// #define BRIDGE_FUN_END 5

#define NONE_STATUS 0
#define BIND_STATUS 1
#define CONNECTED_STATUS 2
#define CLOSED_STATUS 3

#define NONE_ACCEPT 0
#define GET_ACCEPT 1
#define INTERRUPT_ACCEPT 2

#ifndef _WIN32
#define max(a, b)                       \
  (((a) > (b)) ? (a) : (b))
#define min(a, b)                       \
  (((a) < (b)) ? (a) : (b))
#endif

typedef struct Bridge_Read_Data
{
    volatile int host_write_loc;
    volatile int guest_read_loc;
    int data_size;
    int connected_id;
    char data[];
} __attribute__((packed, aligned(4))) Bridge_Read_Data;

typedef struct Bridge_Accept_Data
{
    volatile int accept_status;
    volatile int accept_fd;
} __attribute__((packed, aligned(4))) Bridge_Accept_Data;

static GHashTable *bridge_thread_contexts = NULL;

static GHashTable *accept_fd_thread_maps = NULL;

static void bridge_buffer_register(Guest_Mem *data, uint64_t thread_id, uint64_t process_id, uint64_t unique_id)
{

    Bridge_Thread_Context *context = g_hash_table_lookup(bridge_thread_contexts, GUINT_TO_POINTER(unique_id));

    express_printf("bridge register buffer thread_id %llu\n", thread_id);

    if (context->connection_context.guest_data != NULL)
    {
        free_copied_guest_mem(context->connection_context.guest_data);
    }

    context->connection_context.guest_data = data;
}

static int bridge_socket_listern(int port)
{
    struct sockaddr_in saddr;
    int fd, ret, opt;

    memset(&saddr, 0, sizeof(saddr));
    inet_aton("127.0.0.1", &saddr.sin_addr);

    saddr.sin_port = htons(port);
    saddr.sin_family = AF_INET;

    fd = qemu_socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0)
    {
        LOGE("can't create stream socket %d", errno);
        return -1;
    }

    ret = bind(fd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret < 0)
    {
        LOGE("can't bind on socket port %d %d", port, errno);
        closesocket(fd);

        return -1;
    }
    ret = listen(fd, 0);
    if (ret < 0)
    {
        LOGE("can't listen on socket port %d %d", port, errno);
        closesocket(fd);
        return -1;
    }

    qemu_socket_set_nonblock(fd);

    LOGI("bridge listening on localhost:%d, fd=%d", port, fd);

    return fd;
}

static int bridge_socket_accept(int fd)
{
    struct sockaddr_in saddr;
    socklen_t addrlen = sizeof(saddr);
    int client_fd = accept(fd, (struct sockaddr *)&saddr, &addrlen);
    // LOGI(DEBUG_HEAD "get one connect fd %d", client_fd);

    return client_fd;
}

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

    // LOGI("max_write_size %d",max_write_size);
    do
    {
        int max_guest_size = (head->guest_read_loc - head->host_write_loc + head->data_size - 1) % head->data_size;

        int max_write_size = min(max_guest_size, WRITE_CACHE_SIZE);

        if (max_write_size == 0)
        {
            return all_read_cnt;
        }

        read_cnt = recv(fd, read_cache, max_write_size, 0);
        if (read_cnt < 0)
        {
            int err = errno;
            if (err == EINTR || err == EWOULDBLOCK || err == EAGAIN)
            {
                return all_read_cnt;
            }
            LOGE(DEBUG_HEAD "read fd %d get %d err %d", fd, read_cnt, err);
            return -1;
        }
        else if (read_cnt == 0)
        {
            int err = errno;
            if (err == EINTR || err == EWOULDBLOCK || err == EAGAIN)
            {
                return all_read_cnt;
            }
            LOGE("recv get 0 errno %d", err);
            return -1;
        }

        // express_printf(DEBUG_HEAD "read fd %d get %d write_loc %d read_loc %d max_cnt %d max_guest %d\n", fd, read_cnt, head->host_write_loc, head->guest_read_loc, max_write_size, max_guest_size);

        if (head->host_write_loc + read_cnt > head->data_size)
        {
            int first_write_size = head->data_size - head->host_write_loc;
            write_to_guest_mem(guest_mem, read_cache, head->host_write_loc + sizeof(Bridge_Read_Data), first_write_size);
            head->host_write_loc = 0;
            write_to_guest_mem(guest_mem, read_cache + first_write_size, head->host_write_loc + sizeof(Bridge_Read_Data), read_cnt - first_write_size);
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

static void *bridge_accept_host_thread(void *opaque)
{
    Bridge_Thread_Context *bridge_context = (Bridge_Thread_Context *)opaque;

    int guest_thread_id = bridge_context->thread_id;

    int get_accept_fd = 0;
    bool need_send_irq = false;

    while (bridge_context->connection_context.read_thread_should_running == true)
    {
        if (need_send_irq == true && set_express_device_irq((Device_Context *)&bridge_context->connection_context, 0, 0) == IRQ_SET_OK)
        {
            need_send_irq = false;
        }

        if (get_accept_fd != 0)
        {
            if (bridge_context->connection_context.guest_data != NULL)
            {
                int accept_status = 0;
                read_from_guest_mem(bridge_context->connection_context.guest_data, &accept_status, __builtin_offsetof(Bridge_Accept_Data, accept_status), sizeof(int));

                if (accept_status == NONE_ACCEPT)
                {
                    LOGI("connection established, fd=%d, notifying kernel...", get_accept_fd);
                    write_to_guest_mem(bridge_context->connection_context.guest_data, &get_accept_fd, __builtin_offsetof(Bridge_Accept_Data, accept_fd), sizeof(int));
                    accept_status = GET_ACCEPT;
                    get_accept_fd = 0;
                    write_to_guest_mem(bridge_context->connection_context.guest_data, &accept_status, __builtin_offsetof(Bridge_Accept_Data, accept_status), sizeof(int));

                    int irq_status = set_express_device_irq((Device_Context *)&bridge_context->connection_context, 0, 0);
                    if (irq_status == IRQ_SET_OK)
                    {
                        LOGD("irq IRQ_SET_OK");
                        need_send_irq = false;
                    }
                    else
                    {
                        LOGD("irq_status == %d", irq_status);
                        need_send_irq = true;
                    }
                }
                else
                {
                    LOGI("kernel driver accept_data->accept_status == %d, do nothing", accept_status);
                }
            }

            if (get_accept_fd != 0)
            {
                // LOGI("ready for guest accept");
                g_usleep(2000000);
                continue;
            }
        }

        int ret_fd = bridge_socket_accept(bridge_context->connection_context.socket_fd);
        int err = errno;

        if (ret_fd == -1 && (err == EAGAIN || err == EWOULDBLOCK))
        {
            usleep(2000);
            // LOGI("ready for accept");
            continue;
        }

        if (ret_fd > 0)
        {
            LOGD("connection accepted, fd=%d", ret_fd);
            qemu_socket_set_nonblock(ret_fd);
            g_hash_table_insert(accept_fd_thread_maps, GUINT_TO_POINTER(ret_fd), (gpointer)(uint64_t)guest_thread_id);
            get_accept_fd = ret_fd;

            continue;
        }
    }

    if (get_accept_fd != 0)
    {
        closesocket(get_accept_fd);
    }

    closesocket(bridge_context->connection_context.socket_fd);
    if (bridge_context->connection_context.guest_data != NULL)
    {
        free_copied_guest_mem(bridge_context->connection_context.guest_data);
    }

    LOGW("listen thread exit. closefd %d", bridge_context->connection_context.socket_fd);
    set_express_device_irq((Device_Context *)&bridge_context->connection_context, -1, 0);

    return NULL;
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
                LOGE("error! no guest_mem with read_host_thread");
                return NULL;
            }
        }

        Guest_Mem *guest_mem = bridge_context->connection_context.guest_data;

        int ret = fd_data_to_guest_mem(bridge_context->connection_context.socket_fd, guest_mem, read_cache);

        if (ret > 0)
        {
            // 注入中断
            LOGD("transfer %d bytes from socket %d to guest mem\n", ret, bridge_context->connection_context.socket_fd);
            int irq_status = set_express_device_irq((Device_Context *)&bridge_context->connection_context, 0, 0);
            if (irq_status == IRQ_SET_OK)
            {
                need_send_irq = false;
            }
            else
            {
                need_send_irq = true;
            }
        }
        else if (ret == 0)
        {
            if (need_send_irq == true && set_express_device_irq((Device_Context *)&bridge_context->connection_context, 0, 0) == IRQ_SET_OK)
            {
                need_send_irq = false;
            }
            // LOGI("sleep 1000");
            g_usleep(1000);
        }
        else
        {
            // 断开连接
            LOGI("recv get close");
            bridge_context->status_id = CLOSED_STATUS;
            break;
        }
    }

    closesocket(bridge_context->connection_context.socket_fd);

    if (bridge_context->connection_context.guest_data != NULL)
    {
        free_copied_guest_mem(bridge_context->connection_context.guest_data);
    }

    set_express_device_irq((Device_Context *)&bridge_context->connection_context, -1, 0);

    LOGI("bridge thread exit. closefd %d", bridge_context->connection_context.socket_fd);

    return NULL;
}

static void bridge_output_call_handle(struct Thread_Context *context, Teleport_Express_Call *call)
{

    Call_Para all_para[1];
    int para_num = get_para_from_call(call, all_para, 1);

    Bridge_Thread_Context *bridge_context = (Bridge_Thread_Context *)context;

    uint64_t fun_id = GET_FUN_ID(call->id);
    LOGV("bridge get call_id=%llu process_id=%lld thread=%lld unique_id=%llx", fun_id, call->process_id, call->thread_id, call->unique_id);

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
                LOGE("error BRIDGE_FUN_BIND port NULL");
                break;
            }

            LOGD("BIND(port=%d)", *port_ptr);

            int ret_fd = bridge_socket_listern(*port_ptr);
            int try_cnt = 0;
            while (ret_fd == -1 && try_cnt < 50)
            {
                *port_ptr = *port_ptr + 1;
                try_cnt++;
                ret_fd = bridge_socket_listern(*port_ptr);
            }

            if (ret_fd == -1)
            {
                *port_ptr = 0;
                break;
            }

            bridge_context->connection_context.read_thread_should_running = true;
            bridge_context->status_id = BIND_STATUS;
            bridge_context->connection_context.socket_fd = ret_fd;
            qemu_thread_create(&bridge_context->connection_context.read_thread, "bridge_accept_host_thread", bridge_accept_host_thread, bridge_context, QEMU_THREAD_JOINABLE);
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
                LOGE("error BRIDGE_FUN_CONNECT port NULL");
                break;
            }
            LOGD("CONNECT(host_accept_fd=%d) from guest thread %llu", *port_ptr, context->thread_id);

            uint64_t thread_id = (uint64_t)g_hash_table_lookup(accept_fd_thread_maps, GUINT_TO_POINTER(*port_ptr));
            LOGD("the accept fd %d is held by guest thread %llu", *port_ptr, thread_id);
            if (thread_id == context->thread_id)
            {
                LOGD("connect successfully, starting read thread");
                bridge_context->connection_context.socket_fd = *port_ptr;

                bridge_context->connection_context.read_thread_should_running = true;
                bridge_context->status_id = CONNECTED_STATUS;

                // 注意这里创建的线程一定要是QEMU_THREAD_JOINABLE，不然数据可能被越界写入
                qemu_thread_create(&bridge_context->connection_context.read_thread, "bridge_read_host_thread", bridge_read_host_thread, bridge_context, QEMU_THREAD_JOINABLE);
            }
            else
            {
                LOGE("bridge not connected by the same thread that binds it, return error");
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
            LOGD("OUTPUT(scatter, sg_num=%d)", num);

            for (int i = 0; i < num; i++)
            {
                char temp_buf[36];
                memset(temp_buf, 0, 36);
                memcpy(temp_buf, scatter_data[i].data, scatter_data[i].len <= 32 ? scatter_data[i].len : 32);
                strcpy(temp_buf + 32, "...");
                LOGV("send(sockfd=%d, buf=\"%s\", len=%zu, flag=%d)", bridge_context->connection_context.socket_fd, temp_buf, scatter_data[i].len, 0);
                send(bridge_context->connection_context.socket_fd, scatter_data[i].data, scatter_data[i].len, 0);
            }
        }
    }
    break;
    // case BRIDGE_FUN_END:
    // {
    //     if (para_num == 0)
    //     {
    //         g_hash_table_remove(bridge_thread_contexts, GUINT_TO_POINTER(bridge_context->unique_id));
    //         bridge_context->thread_context.thread_run = 0;

    //         if (bridge_context->status_id == CONNECTED_STATUS || bridge_context->status_id == BIND_STATUS)
    //         {
    //             bridge_context->connection_context.read_thread_should_running = false;
    //             // closesocket(bridge_context->connection_context.socket_fd);
    //             // 等待线程退出
    //             qemu_thread_join(&bridge_context->connection_context.read_thread);
    //             LOGI(DEBUG_HEAD "wait read thread exit ok %d", bridge_context->connection_context.socket_fd);
    //         }
    //     }
    // }
    // break;
    default:
    {
        LOGE("error bridge fun id %lld", fun_id);
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

    if (accept_fd_thread_maps == NULL)
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
        b_context->thread_id = thread_id;

        g_hash_table_insert(bridge_thread_contexts, GUINT_TO_POINTER(unique_id), (gpointer)context);
    }
    return context;
}

static bool remove_bridge_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    Bridge_Thread_Context *bridge_context = (Bridge_Thread_Context *)g_hash_table_lookup(bridge_thread_contexts, GUINT_TO_POINTER(unique_id));

    g_hash_table_remove(bridge_thread_contexts, GUINT_TO_POINTER(unique_id));

    bridge_context->thread_context.thread_run = 0;

    if (bridge_context->status_id == CONNECTED_STATUS || bridge_context->status_id == BIND_STATUS)
    {
        bridge_context->connection_context.read_thread_should_running = false;
        // closesocket(bridge_context->connection_context.socket_fd);
        // 等待线程退出
        qemu_thread_join(&bridge_context->connection_context.read_thread);
        LOGI(DEBUG_HEAD "wait read thread exit ok %d", bridge_context->connection_context.socket_fd);
    }

    return true;
}

static Device_Context *get_bridge_connection_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    Bridge_Thread_Context *thread_context = g_hash_table_lookup(bridge_thread_contexts, GUINT_TO_POINTER(unique_id));
    if (thread_context == NULL)
    {
        return NULL;
    }
    return (Device_Context *)&thread_context->connection_context;
}

static Express_Device_Info express_bridge_info = {
    .enable_default = true,
    .name = "express-bridge",
    .option_name = "bridge",
    .driver_name = "express_bridge",
    .device_id = EXPRESS_BRIDGE_DEVICE_ID,
    .device_type = INPUT_DEVICE_TYPE | OUTPUT_DEVICE_TYPE,

    .get_device_context = get_bridge_connection_context,
    .buffer_register = bridge_buffer_register,
    // .irq_register = bridge_irq_register,
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
