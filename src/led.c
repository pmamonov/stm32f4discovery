#include "led.h"

void led_init(struct led *led)
{
	GPIO_InitTypeDef sGPIOinit;

	RCC_AHB1PeriphClockCmd(led->gpio_rcc, ENABLE);

	sGPIOinit.GPIO_Pin = led->pin;
	sGPIOinit.GPIO_Mode = GPIO_Mode_OUT;
	sGPIOinit.GPIO_Speed = GPIO_Speed_25MHz;
	sGPIOinit.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(led->gpio, &sGPIOinit);
}

void led_on(struct led *led)
{
	GPIO_SetBits(led->gpio, led->pin);
}

void led_off(struct led *led)
{
	GPIO_ResetBits(led->gpio, led->pin);
}

void led_toggle(struct led *led)
{
	if (GPIO_ReadOutputDataBit(led->gpio, led->pin)) {
		led_off(led);
	} else {
		led_on(led);
	}
}
