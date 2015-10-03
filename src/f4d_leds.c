#include "f4d_leds.h"

struct led f4d_led_blue = {
	.gpio_rcc = RCC_AHB1Periph_GPIOD,
	.gpio = GPIOD,
	.pin = GPIO_Pin_15,
};

struct led f4d_led_red = {
	.gpio_rcc = RCC_AHB1Periph_GPIOD,
	.gpio = GPIOD,
	.pin = GPIO_Pin_14,
};

struct led f4d_led_orange = {
	.gpio_rcc = RCC_AHB1Periph_GPIOD,
	.gpio = GPIOD,
	.pin = GPIO_Pin_13,
};

struct led f4d_led_green = {
	.gpio_rcc = RCC_AHB1Periph_GPIOD,
	.gpio = GPIOD,
	.pin = GPIO_Pin_12,
};
