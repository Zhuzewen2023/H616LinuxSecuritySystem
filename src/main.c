#include <stdio.h>
#include <pthread.h>
#include "msg_queue.h"
#include "voice_interface.h"
#include "control.h"
#include "myGlobal.h"
#include "socket_interface.h"
#include "smoke_interface.h"
#include "receive_interface.h"

int main(int argc, char const *argv[])
{   
    if(-1 == wiringPiSetup())
    {
        printf("%s|%s|%dwiringPiSetup failed\n", __FILE__, __func__, __LINE__);
        return -1;
    }

    struct control *control_phead = NULL;
    struct control* control_pcur = NULL;
    control_info_t* control_info = (control_info_t*)malloc(sizeof(control_info_t));
    control_info->mqd = -1;
    control_info->control_phead = NULL;

    int node_num = 0;
    control_info->mqd = create_mqd();
    
    if(control_info->mqd == -1)
    {
        perror("control_info->mqd");
        return -1;
    }
    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
    control_info->control_phead = add_voice_to_ctrl_list(control_info->control_phead);
    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
    control_info->control_phead = add_socket_control_to_control_list(control_info->control_phead);
    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
    control_info->control_phead = add_smoke_to_control_list(control_info->control_phead);
    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
    control_info->control_phead = add_receive_control_to_control_list(control_info->control_phead);
    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
    control_pcur = control_info->control_phead;
    while(control_pcur != NULL)
    {
        printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
        control_pcur->init();
        control_pcur = control_pcur->next;
        node_num++;
    }
    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
    //分配一块内存，用来保存pthread_t
    pthread_t *tid = malloc(sizeof(pthread_t) * node_num);
    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
    control_pcur = control_info->control_phead;
    for(int i = 0; i < node_num; i++)
    {
        //为每一个控制设备的节点开线程
        //然后开启监听动作
        printf("%s|%s|%d:i = %d\n", __FILE__, __func__, __LINE__, i);
        pthread_create(&tid[i], NULL, control_pcur->get, (void *)control_info);
        control_pcur = control_pcur->next;

    }

    //等待线程结束
    for(int i = 0; i < node_num; i++)
    {
        pthread_join(tid[i], NULL);
    }

    //回收资源
    control_pcur = control_info->control_phead;
    while(control_pcur != NULL)
    {
        control_pcur->final();
        control_pcur = control_pcur->next;
    }

    destroy_mqd(control_info->mqd);
    if(control_info != NULL)
    {
        free(control_info);
    }

    if(tid != NULL)
    {
        free(tid);
    }

    return 0;
}
