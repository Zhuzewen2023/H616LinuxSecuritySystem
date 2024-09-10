#ifndef __MYOLED_H
#define __MYOLED_H

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include "oled.h"
#include "font.h"

int myoled_init(void);
int myoled_show(void *arg);

#endif