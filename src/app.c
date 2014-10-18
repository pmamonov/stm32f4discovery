#include <unistd.h>
#include <stdio.h>

#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usb_conf.h"
#include "usbd_desc.h"

#include "FreeRTOS.h"
#include "task.h"

#include "stm32f4xx_gpio.h"

#include "lsm303dlhc_driver.h"

#include "assert.h"

void task_echo(void* vpars);
void led_toggle(void);

__ALIGN_BEGIN  USB_OTG_CORE_HANDLE  USB_OTG_dev  __ALIGN_END;


int main(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitTypeDef sGPIOinit;

	sGPIOinit.GPIO_Pin = 1<<15;
	sGPIOinit.GPIO_Mode = GPIO_Mode_OUT;
	sGPIOinit.GPIO_Speed = GPIO_Speed_25MHz;
	sGPIOinit.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOD, &sGPIOinit);

	I2C_InitTypeDef  I2C_InitStruct;

 /* I2C1 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

  /* GPIOB clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  /* Connect I2C1 pins to AF4 */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
  
  /* Configure I2C1 GPIOs */  
  sGPIOinit.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  sGPIOinit.GPIO_Mode = GPIO_Mode_AF;
  sGPIOinit.GPIO_Speed = GPIO_Speed_2MHz;
  sGPIOinit.GPIO_OType = GPIO_OType_OD;
  sGPIOinit.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &sGPIOinit);

  /* Configure I2C1 */
  /* I2C DeInit */ 
  I2C_DeInit(I2C1);
    
  /* Enable the I2C peripheral */
  I2C_Cmd(I2C1, ENABLE);
 
  /* Set the I2C structure parameters */
  I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStruct.I2C_OwnAddress1 = 0xab;
  I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStruct.I2C_ClockSpeed = 10000;
  
  /* Initialize the I2C peripheral w/ selected parameters */
  I2C_Init(I2C1, &I2C_InitStruct);

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

	xTaskCreate(task_echo, "echo", 2048, NULL, tskIDLE_PRIORITY+1, NULL);
	vTaskStartScheduler();
} 

void task_echo(void *vpars)
{
	AccAxesRaw_t data;
	MagAxesRaw_t dataM;

	ASSERT(SetODR(ODR_100Hz));
	ASSERT(SetODR_M(ODR_220Hz_M));
	ASSERT(SetMode(NORMAL));
	ASSERT(SetModeMag(CONTINUOUS_MODE));
	ASSERT(SetFullScale(FULLSCALE_2));
	ASSERT(SetGainMag(GAIN_450_M));
	ASSERT(SetAxis(X_ENABLE | Y_ENABLE | Z_ENABLE));

	while (1) {
		led_toggle();

		ASSERT(GetAccAxesRaw(&data));
		ASSERT(GetMagAxesRaw(&dataM));

		iprintf("a: %6d %6d %6d\r\n",
			data.AXIS_X, data.AXIS_Y, data.AXIS_Z);
		iprintf("H: %6d %6d %6d\r\n\n",
			dataM.AXIS_X, dataM.AXIS_Y, dataM.AXIS_Z);
		fflush(stdout);

		vTaskDelay(100);
	}
}

void led_toggle(void)
{
	if (GPIO_ReadOutputDataBit(GPIOD, 1<<15) == Bit_RESET)
		GPIO_SetBits(GPIOD, 1<<15);
	else
		GPIO_ResetBits(GPIOD, 1<<15);
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
	while (1);
}
#endif

