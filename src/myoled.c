#include "myoled.h"

struct display_info disp;

int myoled_init()
{
    int e;
    char filename[] = "/dev/i2c-3";
    
    memset(&disp, 0, sizeof(disp));
    disp.address = OLED_I2C_ADDR;
    disp.font = font2;
    
    e = oled_open(&disp, filename);

	if (e < 0) {
		printf("\nERROR: %i, %i\n\n", 1, e);
        return e;
	} else {
		e = oled_init(&disp);
    }
	if (e < 0) {
		printf("\nERROR: %i, %i\n\n", 2, e);
        return e;
	} else {
		printf("---------start--------\n");
        oled_putstrto(&disp, 0, 9+1, "Hello zzw");
        disp.font = font2;
        oled_send_buffer(&disp);
		printf("---------end----------\n");
	}
    return e;

}

int myoled_show(void *arg)
{
    oled_clear(&disp);
    unsigned char *buffer = (unsigned char *)arg;
    oled_putstrto(&disp, 0, 9+1, buffer);
    disp.font = font2;
    #if 0
    oled_putstrto(&disp, 0, 9+1, "This garbage is: ");
    disp.font = font2;
    switch(buffer[2])
    {
        case 0x41:
            oled_putstrto(&disp, 0, 18+2, "dry waste");
            break;
        case 0x42:
            oled_putstrto(&disp, 0, 18+2, "wet waste");
            break;
        case 0x43:
            oled_putstrto(&disp, 0, 18+2, "recyclable waste");
            break;
        case 0x44:
            oled_putstrto(&disp, 0, 18+2, "harzardous waste");
            break;
        case 0x45:
            oled_putstrto(&disp, 0, 20, "recongnition failed");
            break;

    }
    #endif
    oled_send_buffer(&disp);
    return 0;
}
