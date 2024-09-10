#ifndef __SOCKET_H
#define __SOCKET_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>

#define IPADDR "192.168.0.110"
#define IPPORT "8192"
#define BUF_SIZE 6

int socket_init(const char* ip_addr, const char *port);

#endif