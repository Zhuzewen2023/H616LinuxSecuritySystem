#ifndef __MYGLOBAL_H
#define __MYGLOBAL_H

#include "control.h"
#include "msg_queue.h"

typedef struct
{
    mqd_t mqd;
    struct control *control_phead;
}control_info_t;

#endif