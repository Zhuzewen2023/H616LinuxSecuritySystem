#ifndef __SOCKET_INTERFACE_H
#define __SOCKET_INTERFACE_H

#include "control.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "socket.h"
#include "myGlobal.h"

struct control* add_socket_control_to_control_list(struct control* phead);

#endif