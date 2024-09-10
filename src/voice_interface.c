#include "voice_interface.h"

static int serial_fd = -1;

static int voice_init(void)
{
    serial_fd = myserialOpen(SERIAL_DEV, BAUD);
    if(serial_fd < 0)
    {
        perror("open serial failed");
        return -1;
    }

    printf("%s|%s|%d: serial_fd = %d\n", __FILE__, __func__, __LINE__, serial_fd);
    return serial_fd;

}

static void voice_final(void)
{
    if(-1 != serial_fd)
    {
        close(serial_fd);
        serial_fd = -1;
    }
}

//接受语音指令
static void * voice_get(void *arg) //mqd来自于arg传参
{
    printf("enter voice_get thread\n");
    unsigned char buffer[6] = {0xAA, 0x55, 0x00, 0x00, 0x55, 0xAA};
    int len = 0;
    mqd_t mqd = -1;

    if(-1 == serial_fd)
    {
        serial_fd = voice_init();
        if(serial_fd == -1)
        {
            printf("%s|%s|%d: open serial failed\n", __FILE__, __func__, __LINE__);
            pthread_exit(0);
        }
    }
    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
    pthread_detach(pthread_self());
    mqd = ((control_info_t *)arg)->mqd;
    while(1)
    {
        len = serialGetstring(serial_fd, buffer);
        printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
        if(len > 0)
        {
            if(buffer[0] == 0xAA && \
                buffer[1] == 0x55 && \
                buffer[4] == 0x55 && \
                buffer[5] == 0xAA)
            {
                printf("%s|%s|%d: send = 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", __FILE__, __func__, __LINE__, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
                send_message(mqd, buffer, len);
            }
        }
    }

}

//做语音播报
static void * voice_set(void *arg)
{
    pthread_detach(pthread_self());
    unsigned char* buffer = (unsigned char*)arg;
    if(-1 == serial_fd)
    {
        serial_fd = voice_init();
        if(serial_fd == -1)
        {
            printf("%s|%s|%d: open serial failed\n", __FILE__, __func__, __LINE__);
            pthread_exit(0);
        }
    }

    if(buffer != NULL)
    {
        serialSendstring(serial_fd, buffer, 6);
    }
    pthread_exit(0);
}

struct control voice_control = {
    .control_name = "voice",
    .init = voice_init,
    .final = voice_final,
    .get = voice_get,
    .set = voice_set
};

struct control* add_voice_to_ctrl_list(struct control* control_phead)
{
    return add_interface_to_control_list(control_phead, &voice_control);
}