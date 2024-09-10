#ifndef __VOICE_INTERFACE_H
#define __VOICE_INTERFACE_H

#include "msg_queue.h"
#include "control.h"
#include "uartTool.h"
#include "myGlobal.h"
#include <pthread.h>

struct control* add_voice_to_ctrl_list(struct control* control_phead);


#endif