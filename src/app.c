#include <unistd.h>
#include <stdio.h>

#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usb_conf.h"
#include "usbd_desc.h"

#include "FreeRTOS.h"
#include "task.h"

#include "stm32f4xx_gpio.h"

void task_echo(void* vpars);
void task_blink(void* vpars);

__ALIGN_BEGIN  USB_OTG_CORE_HANDLE  USB_OTG_dev  __ALIGN_END;


int main(void)
{
	GPIO_InitTypeDef sGPIOinit;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	sGPIOinit.GPIO_Pin = 1<<15;
	sGPIOinit.GPIO_Mode = GPIO_Mode_OUT;
	sGPIOinit.GPIO_Speed = GPIO_Speed_25MHz;
	sGPIOinit.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOD, &sGPIOinit);

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

	xTaskCreate(task_blink, "blink", 100, NULL, tskIDLE_PRIORITY+1, NULL);
	xTaskCreate(task_echo, "echo", 2048, NULL, tskIDLE_PRIORITY+1, NULL);
	vTaskStartScheduler();
} 

void task_echo(void *vpars)
{
	int c;
	while (1) {
		putchar(getchar());
		fflush(stdout);
		vTaskDelay(10);
	}
}

void task_blink(void *vpars)
{
	while (1) {
		GPIO_SetBits(GPIOD, 1<<15);
		vTaskDelay(100);
		GPIO_ResetBits(GPIOD, 1<<15);
		vTaskDelay(100);
	}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
	while (1);
}
#endif
