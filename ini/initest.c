#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ini.h"
#include "gdevice.h"

/*
typedef struct
{
    int version;
    const char* name;
    const char* email;
}configuration;
*/

static struct gdevice* pdevhead = NULL;

static int handler(void* user, const char* section, const char* name, 
                    const char* value)
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

int main(int argc, char* argv[])
{
    //configuration config;

    if(ini_parse("gdevice.ini", handler, NULL) < 0)
    {
        printf("Can't load 'gdevice.ini'\n");
        return 1;
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
    //printf("Config load from 'gdevice.ini' :version:%d, name:%s, email:%s\n",
    //    config.version, config.name, config.email);

    return 0;
}