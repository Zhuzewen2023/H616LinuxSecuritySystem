#include "socket.h"

int socket_init(const char* ip_addr, const char *port)
{
    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
    int server_fd = -1;
    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in));

    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
    //1.socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
    if(server_fd == -1)
    {
        perror("socket");
        exit(-1);
    }
    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port));
    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
    inet_aton(ip_addr, &server_addr.sin_addr);
    //2.bind
    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
    if(-1 == bind(server_fd, (const struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)))
    {
        perror("bind");
        return -1;
    }
    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
    //3.listen
    if(-1 == listen(server_fd, 1))
    {
        perror("listen");
        return -1;
    }
    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
    return server_fd;
}