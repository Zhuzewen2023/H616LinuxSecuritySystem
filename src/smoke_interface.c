#include "smoke_interface.h"

static int smoke_init(void)
{
    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);    
    pinMode(SMOKE_PIN, SMOKE_MODE);
    return 0;
}

static void smoke_final(void)
{

}

static void* smoke_get(void* arg)
{
    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
    int status = HIGH;
    int switch_status = 0;
    unsigned char buffer[6] = {0xAA, 0x55, 0x00, 0x00, 0x55, 0xAA};
    ssize_t byte_send = -1;
    mqd_t mqd = -1;
    mqd = ((control_info_t *)arg)->mqd;
    
    if(mqd == -1)
    {
        pthread_exit(0);
    }
    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
    pthread_detach(pthread_self());
    while(1)
    {
        printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
        status = digitalRead(SMOKE_PIN);
        if(LOW == status)
        {
            //表示有状态反转
            switch_status = 1;
            buffer[2] = 0x45;
            buffer[3] = 0x00;
            printf("smoke_interface发送启动警报信号\n");
            //触发蜂鸣器
            //语音播报
            byte_send = mq_send(mqd, buffer, 6, 0);
            if(-1 == byte_send)
            {
                printf("发送启动警报信号失败\n");
                continue;
            }
            printf("发送启动警报信号成功\n");
        }
        //在状态有反转并且是高电平的状态下
        else if(HIGH == status && 1 == switch_status)
        {
            printf("smoke_interface发送解除警报信号\n");
            printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);            
            switch_status = 0;
            buffer[2] = 0x45;
            buffer[3] = 0x01;
            byte_send = mq_send(mqd, buffer, 6, 0);
            if(-1 == byte_send)
            {
                printf("smoke_interface发送解除警报信号失败\n");
                continue;
            }

            printf("smoke_interface发送解除警报信号成功\n");
        }
        sleep(5);
    }

    pthread_exit(0);
}

struct control smoke_control = {
    .control_name = "smoke",
    .init = smoke_init,
    .final = smoke_final,
    .get = smoke_get,
};

struct control* add_smoke_to_control_list(struct control* phead)
{
   return add_interface_to_control_list(phead, &smoke_control);
}