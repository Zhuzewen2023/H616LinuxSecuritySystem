#ifndef __RECEIVE_INTERFACE_H
#define __RECEIVE_INTERFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "control.h"
#include "myGlobal.h"
#include "msg_queue.h"
#include "myoled.h"
#include "face.h"
#include <mqueue.h>
#include "gdevice.h"
#include <wiringPi.h>
#include "ini.h"

struct control* add_receive_control_to_control_list(struct control* phead);

#endif