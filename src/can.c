#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#ifdef TARGET_F407
#include "stm32f4xx_gpio.h"
#include "misc.h"
#endif
#ifdef TARGET_F091
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx_rcc.h"
#endif
#include "can.h"
#include "can_msg.h"
#include "uqueue.h"

#define RX_QUEUE_LEN 100
CanRxMsg  rx_msg[RX_QUEUE_LEN];
CanRxMsg  rx_msg_isr[RX_QUEUE_LEN];
struct queue rx_queue;
struct queue rx_queue_isr;

unsigned int can_id = 0;
static int dump_packets = 1;

static volatile struct can_stat can_stat;

void can_stat_reset()
{
	memset(&can_stat, 0, sizeof(can_stat));
}

struct can_stat *can_stat_get()
{
	return &can_stat;
}

void can_stat_dump()
{
	printf("TX: %d (%d B)\r\n", can_stat.csent, can_stat.bsent);
	printf("RX: %d (%d B)\r\n", can_stat.crecv, can_stat.brecv);
}

static int can_ping_reply(CanRxMsg *rx_msg)
{
	struct can_msg *msg;
	unsigned int dst;

	msg = (void *)rx_msg->Data;
	if (msg->type == CAN_MSG_PING && msg->sender) {
		dst = msg->sender;
		msg->sender = 0;
		can_xmit(dst, msg, rx_msg->DLC);
		return 0;
	}
	return -1;
}


void can_dump_pkt(int on)
{
	dump_packets = on;
}

static void NVIC_Config(void)
{
	NVIC_InitTypeDef  NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = CAN_IRQ;
#ifdef TARGET_F407
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
#endif
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void CAN_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	/* CAN GPIOs configuration */

	/* Enable GPIO clock */
#ifdef TARGET_F407
	RCC_AHB1PeriphClockCmd(CAN_GPIO_CLK, ENABLE);
#endif

#ifdef TARGET_F091
	RCC_AHBPeriphClockCmd(CAN_GPIO_CLK, ENABLE);
#endif
	/* Connect CAN pins to AF9 */
	GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_RX_SOURCE, CAN_AF_PORT);
	GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_TX_SOURCE, CAN_AF_PORT); 
	
	/* Configure CAN RX and TX pins */
	GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN | CAN_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(CAN_GPIO_PORT, &GPIO_InitStructure);

	/* CAN configuration */  
	/* Enable CAN clock */
	RCC_APB1PeriphClockCmd(CAN_CLK, ENABLE);
	
	/* CAN register init */
	CAN_DeInit(CANx);

	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
#ifdef TARGET_F407
	CAN_InitStructure.CAN_Prescaler = 7;
#endif
#ifdef TARGET_F091
	CAN_InitStructure.CAN_Prescaler = 8;
#endif
	/* 1Mb */
	CAN_InitStructure.CAN_BS1 = CAN_BS1_2tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
	CAN_Init(CANx, &CAN_InitStructure);

	/* CAN filter init */
	can_filter_setup(can_id, 0);

	/* Enable FIFO 0 message pending Interrupt */
	CAN_ITConfig(CANx, CAN_IT_FMP0, ENABLE);
}

void can_init()
{
	can_stat_reset();
	queue_init(&rx_queue, sizeof(rx_msg[0]), RX_QUEUE_LEN, &rx_msg[0]);
	queue_init(&rx_queue_isr, sizeof(rx_msg_isr[0]), RX_QUEUE_LEN, &rx_msg_isr[0]);
	NVIC_Config();
	CAN_Config();
};

void task_can(void *vpars)
{
	CanRxMsg cmsg;
	struct can_msg *msg;
	unsigned int to;
	int len;

	while (1) {
		queue_swap(&rx_queue, &rx_queue_isr);
		while (queue_pop(&rx_queue, &cmsg) == 0) {
			len = cmsg.DLC;
			msg = cmsg.Data;
			if (len == sizeof(*msg) &&
			    msg->type == CAN_MSG_PING &&
			    msg->sender != 0) {
				to = msg->sender;
				msg->sender = 0;
				can_xmit(to, msg, len);
			}
		}
		vTaskDelay(1);
	};
}

void can_filter_setup(unsigned int id, unsigned int mask)
{
	CAN_FilterInitTypeDef  filter;

	id &= 0x7ff;
	mask &= 0x7ff;
	can_id = id;
	id = id << 5;
	mask = (mask & 0x7ff) << 5;

	filter.CAN_FilterNumber = 0;
	filter.CAN_FilterMode = CAN_FilterMode_IdMask;
	filter.CAN_FilterScale = CAN_FilterScale_16bit;
	filter.CAN_FilterIdLow = id;
	filter.CAN_FilterMaskIdLow = mask;
	filter.CAN_FilterFIFOAssignment = 0;
	filter.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&filter);
}

void can_xmit(unsigned int id, unsigned char *data, int len)
{
	CanTxMsg TxMessage;

	can_stat.csent += 1;
	can_stat.bsent += len;

	id &= 0x7ff;
	TxMessage.StdId = id;
	TxMessage.ExtId = id;
	TxMessage.DLC = len;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.IDE = CAN_ID_STD;
	while (len--)
		TxMessage.Data[len] = data[len];
	while (CAN_Transmit(CANx, &TxMessage) == CAN_TxStatus_NoMailBox)
		;
}

void can_dump_tx()
{
	int i;

	for (i = 0; i < CAN_NUM_MB; i++) {
		printf("MB #%d\r\n", i);
		printf("TxStatus: ");
		switch(CAN_TransmitStatus(CANx, i)) {
		case CAN_TxStatus_Ok:
			printf("OK");
			break;
		case CAN_TxStatus_Pending:
			printf("PENDING");
			break;
		case CAN_TxStatus_Failed:
			printf("FAILED");
			break;
		}
		printf("\r\n");
	}
	printf("TEC: %d\r\n", CAN_GetLSBTransmitErrorCounter(CANx));
	printf("REC: %d\r\n", CAN_GetReceiveErrorCounter(CANx));
	printf("LEC: %d\r\n", CAN_GetLastErrorCode(CANx));
	
}

#ifdef TARGET_F407
void CAN1_RX0_IRQHandler(void)
#endif
#ifdef TARGET_F091
void CEC_CAN_IRQHandler(void)
#endif
{
	CanRxMsg RxMessage;
	while (CAN_MessagePending(CANx, CAN_FIFO0)) {
		CAN_Receive(CANx, CAN_FIFO0, &RxMessage);
		if (dump_packets) {
			int i;
			printf("\r\nCAN packet received\r\n");
			printf("StdId: %x\r\n", RxMessage.StdId);
			printf("ExtId: %x\r\n", RxMessage.ExtId);
			printf("payload(%d): ", RxMessage.DLC);
			for (i = 0; i < RxMessage.DLC; i++)
				printf(" %02x", RxMessage.Data[i]);
			printf("\r\n");
		}
		if (queue_push(&rx_queue_isr, &RxMessage))
			return;
		else {
			can_stat.crecv += 1;
			can_stat.brecv += RxMessage.DLC;
		}
	}
}
