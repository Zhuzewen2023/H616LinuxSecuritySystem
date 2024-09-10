#ifndef __MSG_QUEUE_H
#define __MSG_QUEUE_H

#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <fcntl.h>  
#include <sys/stat.h>  
#include <mqueue.h>  
#include <errno.h>  
#include <unistd.h>  
#include <time.h>

#define MSG_QUEUE_NAME "/my_msg_queue"

mqd_t create_mqd(void);
int send_message(mqd_t mqd, void *message, int msg_len);
void destroy_mqd(mqd_t mqd);


#endif