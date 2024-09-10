#ifndef __SMOKE_INTERFACE_H
#define __SMOKE_INTERFACE_H

#include "control.h"
#include <pthread.h>
#include "myGlobal.h"
#include "msg_queue.h"
#include <wiringPi.h>
#include <unistd.h>

#define SMOKE_PIN 6
#define SMOKE_MODE INPUT

struct control* add_smoke_to_control_list(struct control* phead);


#endif