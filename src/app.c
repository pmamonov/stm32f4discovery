#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usb_conf.h"
#include "usbd_desc.h"

#include "FreeRTOS.h"
#include "task.h"

#include "stm32f4xx_gpio.h"

#include "f4d_leds.h"

#define ARRAY_LEN(a) (sizeof(a) / sizeof(*a))

void task_chat(void* vpars);
void task_blink(void* vpars);

__ALIGN_BEGIN  USB_OTG_CORE_HANDLE  USB_OTG_dev  __ALIGN_END;


int main(void)
{
	led_init(&f4d_led_blue);
	led_init(&f4d_led_red);
	led_init(&f4d_led_orange);
	led_init(&f4d_led_green);

	led_on(&f4d_led_blue);
	led_on(&f4d_led_red);
	led_on(&f4d_led_orange);
	led_on(&f4d_led_green);

	USBD_Init(&USB_OTG_dev,
#ifdef USE_USB_OTG_HS 
		USB_OTG_HS_CORE_ID,
#else            
		USB_OTG_FS_CORE_ID,
#endif  
		&USR_desc, 
		&USBD_CDC_cb, 
		&USR_cb);

	/* disable stdio buffering */
	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

//	xTaskCreate(task_blink, "blink", 100, NULL, tskIDLE_PRIORITY+1, NULL);
	xTaskCreate(task_chat, "task_chat", 2048, NULL, tskIDLE_PRIORITY+1, NULL);
	vTaskStartScheduler();
} 

void task_chat(void *vpars)
{
#define CMD_LEN 255
	int c;
	char cmd[CMD_LEN+1];
	int pos = 0;
	char *tk;
	struct led *leds[] = {
		&f4d_led_blue,
		&f4d_led_red,
		&f4d_led_orange,
		&f4d_led_green,
	};
	int num_leds = ARRAY_LEN(leds);
	int i = 0;
	
	while (1) {
		fflush(stdout);
		c = getchar();
		putchar(c);
		if (c == '\b') {
			if (pos > 0)
				pos--;
			continue;
		}
		if (c == '\r') {
			printf("\r\n");
			/* parse cmd */
			cmd[pos] = 0;
			tk = strtok(cmd, " ");
			if (strcmp(tk, "ping") == 0) {
				printf("pong");
			} else if (!strcmp(tk, "on")) {
				tk = strtok(NULL, " ");
				if (tk) {
					i = atoi(tk);
					if (i >= 0 && i < num_leds)
						led_on(leds[i]);
				}
			} else if (!strcmp(tk, "off")) {
				tk = strtok(NULL, " ");
				if (tk) {
					i = atoi(tk);
					if (i >= 0 && i < num_leds)
						led_off(leds[i]);
				}
			} else {
				printf("nonsense!");
			}

			printf("\r\n> ");
			pos = 0;
			continue;
		}
		if (pos < CMD_LEN) {
			cmd[pos] = c;
			pos++;
		}
	}
}

void task_blink(void *vpars)
{
	struct led *leds[] = {
		&f4d_led_blue,
		&f4d_led_red,
		&f4d_led_orange,
		&f4d_led_green,
	};
	int i = 0;

	while (1) {
		led_on(leds[i]);
		vTaskDelay(100);
		led_off(leds[i]);
		i = i + 1 >= sizeof(leds) / sizeof(struct led *) ? 0 : i + 1; 
	}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
	while (1);
}
#endif

