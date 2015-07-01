#ifndef __CAN_H__
#define __CAN_H__

#include "FreeRTOS.h"
#include "task.h"

#define CANx                       CAN1
#define CAN_CLK                    RCC_APB1Periph_CAN1
#define CAN_RX_PIN                 GPIO_Pin_0
#define CAN_TX_PIN                 GPIO_Pin_1
#define CAN_GPIO_PORT              GPIOD
#define CAN_GPIO_CLK               RCC_AHB1Periph_GPIOD
#define CAN_AF_PORT                GPIO_AF_CAN1
#define CAN_RX_SOURCE              GPIO_PinSource0
#define CAN_TX_SOURCE              GPIO_PinSource1 

extern CanRxMsg RxMessage;
extern TaskHandle_t can_listen_handle;

void can_init();
void task_can_listen(void *vpars);
#endif
