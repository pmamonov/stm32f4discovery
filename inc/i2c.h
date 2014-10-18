#include "stm32f4xx_i2c.h"

unsigned char I2C_ByteRead(I2C_TypeDef *I2Cx,
	unsigned char *data, unsigned char dev_addr, unsigned char reg);
unsigned char I2C_ByteWrite(I2C_TypeDef *I2Cx,
	unsigned char *data, unsigned char dev_addr, unsigned char reg);
