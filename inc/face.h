#ifndef __FACE_H
#define __FACE_H

#define WGET_CMD "wget http://127.0.0.1:8080/?action=snapshot -O /home/orangepi/smartHomeProject/face.jpg"
#define FACE_FILE "/home/orangepi/smartHomeProject/face.jpg"

#include <Python.h>
#include <stdio.h>

void face_init(void);

void face_final(void);

double face_score();

#endif