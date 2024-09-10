#ifndef __GDEVICE_H
#define __GDEVICE_H

#include <stdio.h>
#include <wiringPi.h>

struct gdevice
{
    char dev_name[128]; //设备名称
    int key; //key值，用于匹配控制指令的值
    int gpio_pin; //控制设备的gpio引脚 6 7 8 9 -1
    int gpio_mode; //输入输出模式 INPUT OUTPUT -1
    int gpio_status; //高低电平状态 LOW HIGH -1
    int check_face_status; //是否进行人脸识别状态 0表示不需要
    int voice_set_status; //是否设置语音播报状态 0表示不需要
    struct gdevice *next; //指向下一个设备
};

//struct gdevice* add_gdevice_to_gdevice_list(struct gdevice* pdevhead, struct gdevice* gdevice_interface);
struct gdevice* find_gdevice_by_key(struct gdevice* pdevhead, int key);
int set_gpio_gdevice_status(struct gdevice* pdevhead);

#endif