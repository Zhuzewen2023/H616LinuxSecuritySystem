#include "socket_interface.h"

static int server_fd = -1;

static int socket_control_init()
{
    //创建套接字
    server_fd = socket_init(IPADDR, IPPORT);
    if(server_fd < 0)
    {
        perror("socket");
        return -1;
    }
    return server_fd;
}

static void socket_control_final()
{
    close(server_fd);
    server_fd = -1;
}

static void * socket_control_get(void *arg)
{
    printf("enter socket_control_get thread\n");
    mqd_t mqd = -1;
    control_info_t *control_info = (control_info_t *)arg;
    printf("socket_control_get now running\n");    
    pthread_detach(pthread_self());
    if(server_fd == -1)
    {
        server_fd = socket_control_init();
        if(-1 == server_fd)
        {
            perror("socket_control_init");
            pthread_exit(0);
        }
    }

    int connected_server_fd = -1;
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    char buffer[BUF_SIZE] = {0};
    int sockaddr_len = sizeof(struct sockaddr_in);

    printf("accept start\n");
    while(1)
    {
        //接受客户端连接
        connected_server_fd = accept(server_fd, (struct sockaddr *)&client_addr, &sockaddr_len);
        if(connected_server_fd < 0)
        {
            perror("accept");
            return NULL;
        }
        printf("%s|%s|%d:Accept connection from %s:%d\n", __FILE__, __func__, __LINE__, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        //该变量用于开启TCP KEEPALIVE心跳包功能
        int keepalive = 1;
        //该变量用于设置5s内没收到数据就开始发送心跳包
        int keepidle = 5;
        //发送三次
        int keepcnt = 3;
        //发送间隔三秒钟
        int keepintvl = 3;

        if(-1 == setsockopt(connected_server_fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive, sizeof(keepalive)))
        {
            perror("setsockopt SO_KEEPALIVE");
        }
    
        if(-1 == setsockopt(connected_server_fd, SOL_TCP, TCP_KEEPIDLE, &keepidle, sizeof(keepidle)))
        {
            perror("setsockopt TCP_KEEPIDLE");
        }

        if(-1 == setsockopt(connected_server_fd, SOL_TCP, TCP_KEEPINTVL, &keepintvl, sizeof(keepintvl)))
        {
            perror("setsockopt TCP_KEEPINTVL");
        }

        if(-1 == setsockopt(connected_server_fd, SOL_TCP, TCP_KEEPCNT, &keepcnt, sizeof(keepcnt)))
        {
            perror("setsockopt TCP_KEEPCNT");
        }

        //读取客户端数据
        while(1)
        {
            memset(buffer, 0, BUF_SIZE);
            int n_read = recv(connected_server_fd, buffer, BUF_SIZE, 0);
            if(n_read <= 0)
            {
                break;
            }
            else if(n_read > 0)
            {
                mqd = control_info->mqd;
                printf("received data from client: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
                if(buffer[0] == 0xAA && buffer[1] == 0x55 \
                && buffer[5] == 0xAA && buffer[4] == 0x55)
                {
                    //把消息发送给posix消息队列
                    if(mqd == -1)
                    {
                        perror("get mqd");
                        pthread_exit(0);
                    }
                    int byte_send = send_message(mqd, buffer, n_read);
                    if(byte_send == -1)
                    {
                        perror("send_message");
                    }else
                    {
                        printf("send_message to posix queue success\n");
                    }
                    
                }
            
            }
        }
    }
}

struct control socket_control = {
    .control_name = "socket_control",
    .init = socket_control_init,
    .final = socket_control_final,
    .get = socket_control_get

};

struct control *add_socket_control_to_control_list(struct control* phead) 
{
    return add_interface_to_control_list(phead, &socket_control);
}

