#include "msg_queue.h"

mqd_t create_mqd()
{
    //创建消息队列
    mqd_t mqd = -1;
    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 256;
    attr.mq_curmsgs = 0;

    if ((mqd = mq_open( MSG_QUEUE_NAME, O_CREAT | O_RDWR, 0666, &attr)) == -1) 
    {
        perror("mq_open");
        return -1;
    }

    printf("%s|%s|%d: mqd = %d\n", __FILE__, __func__, __LINE__, mqd);

    return mqd;
}

int send_message(mqd_t mqd, void *message, int msg_len)
{
    int byte_send = -1;
    byte_send = mq_send(mqd, (char *)message, msg_len, 0);
    
    return byte_send;
}

void destroy_mqd(mqd_t mqd)
{
    if(mqd != -1)
    {
        mq_close(mqd);
        mq_unlink(MSG_QUEUE_NAME);
        mqd = -1;
    }
}
