#include "assert.h"
#include "i2c.h"

#define I2C_TIMEOUT_MAX (1<<17)

unsigned char I2C_ByteRead(I2C_TypeDef *I2Cx,
	unsigned char *data, unsigned char dev_addr, unsigned char reg)
{
  uint32_t timeout = I2C_TIMEOUT_MAX;

  /* Generate the Start Condition */
  I2C_GenerateSTART(I2Cx, ENABLE);

  /* Test on I2Cx EV5 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) {
		ASSERT(0);
		return 0;
	}
  } 
  
  /* Send DCMI selcted device slave Address for write */
  I2C_Send7bitAddress(I2Cx, dev_addr, I2C_Direction_Transmitter);
 
  /* Test on I2Cx EV6 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) {
		ASSERT(0);
		return 0;
	}
  } 

  /* Send I2Cx location address LSB */
  I2C_SendData(I2Cx, reg);

  /* Test on I2Cx EV8 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) {
		ASSERT(0);
		return 0;
	}
  } 
  
  /* Clear AF flag if arised */
  I2Cx->SR1 |= (uint16_t)0x0400;

  /* Generate the Start Condition */
  I2C_GenerateSTART(I2Cx, ENABLE);
  
  /* Test on I2Cx EV6 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) {
		ASSERT(0);
		return 0;
	}
  } 
  
  /* Send DCMI selcted device slave Address for write */
  I2C_Send7bitAddress(I2Cx, dev_addr, I2C_Direction_Receiver);
   
  /* Test on I2Cx EV6 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) {
		ASSERT(0);
		return 0;
	}
  }  
 
  /* Prepare an NACK for the next data received */
  I2C_AcknowledgeConfig(I2Cx, DISABLE);

  /* Test on I2Cx EV7 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) {
		ASSERT(0);
		return 0;
	}
  }   
    
  /* Prepare Stop after receiving data */
  I2C_GenerateSTOP(I2Cx, ENABLE);

  /* Receive the Data */
  *data = I2C_ReceiveData(I2Cx);

  /* return the read data */
	return 1;
}


unsigned char I2C_ByteWrite(I2C_TypeDef *I2Cx,
	unsigned char *data, unsigned char dev_addr, unsigned char reg)
{
  uint32_t timeout = I2C_TIMEOUT_MAX;
  
  /* Generate the Start Condition */
  I2C_GenerateSTART(I2Cx, ENABLE);

  /* Test on I2Cx EV5 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) {
		ASSERT(0);
		return 0;
	}
  }
   
  /* Send DCMI selcted device slave Address for write */
  I2C_Send7bitAddress(I2Cx, dev_addr, I2C_Direction_Transmitter);
 
  /* Test on I2Cx EV6 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) {
		ASSERT(0);
		return 0;
	}
  }
 
  /* Send I2Cx location address LSB */
  I2C_SendData(I2Cx, reg);

  /* Test on I2Cx EV8 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) {
		ASSERT(0);
		return 0;
	}
  }
  
  /* Send Data */
  I2C_SendData(I2Cx, *data);

  /* Test on I2Cx EV8 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) {
		ASSERT(0);
		return 0;
	}
  }  
 
  /* Send I2Cx STOP Condition */
  I2C_GenerateSTOP(I2Cx, ENABLE);
  
  /* If operation is OK, return 0 */
	return 1;
}


