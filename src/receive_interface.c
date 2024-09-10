#include "receive_interface.h"

typedef struct {
    int msg_len;
    unsigned char* buffer;
    control_info_t *control_info;
}receive_msg_t;

static int oled_fd = -1;
static struct gdevice* pdevhead = NULL;

static int gdevice_ini_handler(void* user, const char* section, const char* name, const char* value)
{
    struct gdevice* pdev = NULL;

    if(pdevhead == NULL)
    {
        pdevhead = (struct gdevice* )malloc(sizeof(struct gdevice));
        pdevhead->next = NULL;
        memset(pdevhead, 0, sizeof(struct gdevice));
        strcpy(pdevhead->dev_name, section);
    }
    else if(0 != strcmp(section, pdevhead->dev_name))
    {
        pdev = (struct gdevice* )malloc(sizeof(struct gdevice));
        memset(pdev, 0, sizeof(struct gdevice));
        strcpy(pdev->dev_name, section);
        pdev->next = pdevhead;
        pdevhead = pdev;
    }

    if(pdevhead != NULL)
    {
        #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
        if(MATCH(pdevhead->dev_name, "key"))
        {
            //int test_key = atoi(value);
            //printf("pdevhead->key test = %x\n", test_key);
            sscanf(value, "%x", &pdevhead->key);
            printf("pdevhead->key = %x\n", pdevhead->key);
        }
        else if(MATCH(pdevhead->dev_name, "gpio_pin"))
        {
            sscanf(value, "%d", &pdevhead->gpio_pin);
            printf("pdevhead->gpio_pin = %d\n", pdevhead->gpio_pin);
        }
        else if(MATCH(pdevhead->dev_name, "gpio_mode"))
        {
            if(strcmp(value, "OUTPUT") == 0)
            {
                pdevhead->gpio_mode = 1; //OUTPUT
            }
            else if(strcmp(value, "INPUT") == 0)
            {
                pdevhead->gpio_mode = 0; //INPUT
            }
        }
        else if(MATCH(pdevhead->dev_name, "gpio_status"))
        {
            if(strcmp(value, "HIGH") == 0)
            {
                pdevhead->gpio_status = 1; //HIGH
            }
            else if(strcmp(value, "LOW") == 0)
            {
                pdevhead->gpio_status = 0; //LOW
            }
        }
        else if(MATCH(pdevhead->dev_name, "check_face_status"))
        {
            pdevhead->check_face_status = atoi(value);
        }
        else if(MATCH(pdevhead->dev_name, "voice_set_status"))
        {
            pdevhead->voice_set_status = atoi(value);
        }
    }
    //configuration* pconfig = (configuration*)user;
    printf("section=%s, name=%s, value=%s\n", section, name, value);

    
    return 1;
}

static int receive_init(void)
{
    if(ini_parse("/home/orangepi/smartHomeProject/ini/gdevice.ini", gdevice_ini_handler, NULL) < 0)
    {
        printf("ini_parse error\n");
        return -1;
    }

    struct gdevice* pdev = pdevhead;
    while(pdev != NULL)
    {
        printf("name = %s\n", pdev->dev_name);
        printf("key = %x\n", pdev->key);
        printf("gpio_pin = %d\n", pdev->gpio_pin);
        printf("gpio_mode = %d\n", pdev->gpio_mode);
        printf("gpio_status = %d\n", pdev->gpio_status);
        printf("check_face_status = %d\n", pdev->check_face_status);
        printf("voice_set_status = %d\n", pdev->voice_set_status);
        pdev = pdev->next;
    }

    oled_fd = myoled_init();
    face_init();

    return oled_fd;
}

static void receive_final(void)
{
    face_final();
    if(oled_fd != -1)
    {
        //把oled屏"关了"
        close(oled_fd);
        oled_fd = -1;
    }
}

static void *receive_thread_handler(void* arg)
{
    pthread_detach(pthread_self());
    pthread_t tid = -1;
    int ret = -1;
    struct gdevice* pcur_gdevice = NULL;
    receive_msg_t *receive_msg = NULL;
    char success_or_failed[20] = "success";
    int smoke_status = -1;
    double face_result = 0.0;

    if(arg != NULL)
    {
        receive_msg = (receive_msg_t*)arg;
        printf("%s|%s|%d: receive_msg->msg_len = %d\n", __FILE__, __func__, __LINE__, receive_msg->msg_len);
        printf("%s|%s|%d: receive_msg->buffer =  0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", __FILE__, __func__, __LINE__, receive_msg->buffer[0], receive_msg->buffer[1], receive_msg->buffer[2], receive_msg->buffer[3], receive_msg->buffer[4], receive_msg->buffer[5]);
    }

    if(receive_msg->buffer[2] == 0x40)
    {
        printf("%s|%s|%d: do nothing...\n", __FILE__, __func__, __LINE__);
        pthread_exit(0);
    }

    if(receive_msg != NULL && receive_msg->buffer != NULL)
    {
        printf("%s|%s|%d: start find_gdevice..\n", __FILE__, __func__, __LINE__);
        pcur_gdevice = find_gdevice_by_key(pdevhead, receive_msg->buffer[2]);
    }
    else
    {
        printf("find gdevice by key failed\n");
    }

    if(pcur_gdevice != NULL)
    {
        
        pcur_gdevice->gpio_status = (receive_msg->buffer[3] == 0 ? LOW:HIGH);
        printf("%s|%s|%d: pcur_gdevice->dev_name = %s", __FILE__, __func__, __LINE__, pcur_gdevice->dev_name);
        printf("%s|%s|%d: pcur_gdevice->gpio_status = %d\n", __FILE__, __func__, __LINE__, pcur_gdevice->gpio_status);
        printf("%s|%s|%d: pcur_gdevice->check_face_status = %d\n", __FILE__, __func__, __LINE__, pcur_gdevice->check_face_status);
        printf("%s|%s|%d: pcur_gdevice->voice_set_status = %d\n", __FILE__, __func__, __LINE__, pcur_gdevice->voice_set_status);
        
        //special for lock
        if(1 == pcur_gdevice->check_face_status)
        {
            printf("%s|%s|%d: 开始人脸识别\n", __FILE__, __func__, __LINE__);
            
            face_result = face_score();
            printf("%s|%s|%d: face_result = %.2lf\n", __FILE__, __func__, __LINE__, face_result);
            if(face_result > 0.5)
            {
                printf("%s|%s|%d: 识别成功\n", __FILE__, __func__, __LINE__);
                ret = set_gpio_gdevice_status(pcur_gdevice);
                printf("%s|%s|%d: 设置人脸开锁gpio状态成功\n", __FILE__, __func__, __LINE__);
                receive_msg->buffer[2] = 0x47;
            }
            else
            {
                printf("%s|%s|%d: 识别失败\n", __FILE__, __func__, __LINE__);
                receive_msg->buffer[2] = 0x46;
                ret = -1;
            }
            
        }
        else
        {
            ret = set_gpio_gdevice_status(pcur_gdevice);
            if(ret == 0)
            {
                printf("set gpio gdevice status success\n");
            }
            else
            {
                printf("%s|%s|%d: set_gpio_gdevice_status error\n", __FILE__, __func__, __LINE__);
                memset(success_or_failed, 0, sizeof(success_or_failed));
                strncpy(success_or_failed, "failed", sizeof(success_or_failed));
            
            }
            printf("set gpio gdevice status failed\n");
        }
        
        
        
    }

    if(pcur_gdevice->voice_set_status == 1)
    {
        printf("%s|%s|%d: 开始语音播报\n", __FILE__, __func__, __LINE__);
        if(NULL != receive_msg && NULL != receive_msg->control_info && NULL != receive_msg->control_info->control_phead)
        {
                
            struct control* pcontrol = receive_msg->control_info->control_phead;
            printf("%s|%s|%d: control_name: %s\n", __FILE__, __func__, __LINE__, pcontrol->control_name);
            while(pcontrol != NULL)
            {
                if(strstr(pcontrol->control_name, "voice"))
                {
                    printf("%s|%s|%d: find control name voice\n", __FILE__, __func__, __LINE__);
                    //找到了语音模块的节点
                    //开始判断
                    
                    if(receive_msg->buffer[2] == 0x45 && receive_msg->buffer[3] == 0x00)
                    {
                        smoke_status = 1;
                        
                
                    }
                    //语音模块播报
                    pthread_create(&tid, NULL, pcontrol->set, (void *)receive_msg->buffer);
                    break;
                }    
                        
                pcontrol = pcontrol->next;
                printf("%s|%s|%d: control_name: %s\n", __FILE__, __func__, __LINE__, pcontrol->control_name);
            }    
                
            printf("find pcontrol over\n");
            
        }
                
    }

    //oled显示
    char oled_msg[512];
    memset(oled_msg, 0, sizeof(oled_msg));
    char *change_status = pcur_gdevice->gpio_status == LOW ? "Open" : "Close";
    if(smoke_status == 1)
    {
        sprintf(oled_msg, "risk of fire exists");
    }
    else
    {
        sprintf(oled_msg, "%s %s %s", pcur_gdevice->dev_name, change_status, success_or_failed);
    }
    printf("%s|%s|%d: oled_msg = %s\n", __FILE__, __func__, __LINE__, oled_msg);
    myoled_show(oled_msg);

    if(1 == pcur_gdevice->check_face_status && ret == 0 && face_result > 0.5)
    {
        sleep(3);
        pcur_gdevice->gpio_status = HIGH;
        set_gpio_gdevice_status(pcur_gdevice);
    }

    pthread_exit(0);
}

static void* receive_get(void *control_info)
{    
    pthread_detach(pthread_self());
    ssize_t receive_len = -1;
    pthread_t tid = -1;
    unsigned char *buffer = NULL;
    receive_msg_t *receive_msg = NULL;
    mqd_t mqd = -1;
    struct mq_attr attr;
    if(control_info != NULL)
    {
        receive_msg = (receive_msg_t *)malloc(sizeof(receive_msg_t));
        memset(receive_msg, 0, sizeof(receive_msg_t));
        receive_msg->control_info = (control_info_t *)control_info; //获取到mqd和phead(struct control 链表的头结点)
        receive_msg->msg_len = -1;
        receive_msg->buffer = NULL;
    }
    else
    {
        pthread_exit(0);
    }

    mqd = receive_msg->control_info->mqd;

    if(mq_getattr(mqd, &attr) == -1)
    {
        perror("mq_getattr");
        pthread_exit(0);
    }

    receive_msg->msg_len = attr.mq_msgsize;
    printf("%s|%s|%d: receive_msg->msg_len = %d", __FILE__, __func__, __LINE__, receive_msg->msg_len);
    receive_msg->buffer = (unsigned char *)malloc(receive_msg->msg_len);
    memset(receive_msg->buffer, 0, receive_msg->msg_len);
    buffer = (unsigned char *)malloc(receive_msg->msg_len);
    memset(buffer, 0, receive_msg->msg_len);

    while(1)
    {
        receive_len = mq_receive(mqd, buffer, attr.mq_msgsize, NULL);
        printf("%s|%s|%d: receive 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", __FILE__, __func__, __LINE__, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
        printf("%s|%s|%d: receive_len = %ld\n", __FILE__, __func__, __LINE__, receive_len);
        
        if(receive_len == -1)
        {
            
            perror("mq_receive");
            
        }
        else if(buffer[0] == 0xAA && buffer[1] == 0x55 && buffer[5] == 0xAA && buffer[4] == 0x55)
        {
            receive_msg->msg_len = receive_len;
            memcpy(receive_msg->buffer, buffer, receive_len);
            pthread_create(&tid, NULL, receive_thread_handler, (void *)receive_msg);
        }
    }



    pthread_exit(0);
}



struct control receive_control = {
    .control_name = "receive",
    .init = receive_init,
    .final = receive_final,
    .get = receive_get,
    
};

struct control* add_receive_control_to_control_list(struct control* phead)
{
    return add_interface_to_control_list(phead, &receive_control);
}