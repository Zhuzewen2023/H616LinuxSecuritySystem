#include "gdevice.h"

#if 0
struct gdevice* add_gdevice_to_gdevice_list(struct gdevice* pdevhead, struct gdevice* gdevice_interface)
{
    if(pdevhead == NULL)
    {
        pdevhead = gdevice_interface;
    }
    else
    {
        gdevice_interface->next = pdevhead;
        pdevhead = gdevice_interface;
    }

    return pdevhead;
}

#endif

struct gdevice* find_gdevice_by_key(struct gdevice* pdevhead, int key)
{
    struct gdevice* pcur = NULL;
    if(NULL == pdevhead)
    {
        return NULL;
    }
    else
    {
        pcur = pdevhead;
        while(pcur != NULL)
        {
            if(pcur->key == key)
            {
                return pcur;
            }
            pcur = pcur->next;
        }
        return NULL;
    }
}

int set_gpio_gdevice_status(struct gdevice* pdev)
{
    if(pdev == NULL)
    {
        return -1;
    }

    if(-1 != pdev->gpio_pin)
    {
        if(-1 != pdev->gpio_mode)
        {
            pinMode(pdev->gpio_pin, pdev->gpio_mode);
        }

        if(-1 != pdev->gpio_status)
        {
            digitalWrite(pdev->gpio_pin, pdev->gpio_status); //当配置为输出模式时引脚的高低电平状态
        }
    }

    return 0;

}