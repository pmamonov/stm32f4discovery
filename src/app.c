#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usb_conf.h"
#include "usbd_desc.h"

#include "FreeRTOS.h"
#include "task.h"

#include "stm32f4xx_gpio.h"

#include "can.h"

void task_chat(void* vpars);
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

	can_init();

	xTaskCreate(task_blink, "blink", 100, NULL, tskIDLE_PRIORITY+1, NULL);
	xTaskCreate(task_can_listen, "task_can_listen", 2048, NULL, 
		    tskIDLE_PRIORITY+1, NULL);
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
			int i;

			printf("\r\n");
			/* parse cmd */
			cmd[pos] = 0;
			tk = strtok(cmd, " ");
			if (strlen(tk) == 0)
				goto cmd_finish;
			if (strcmp(tk, "stop") == 0) {
				CAN_CancelTransmit(CANx, 0);
			} else if (strcmp(tk, "status") == 0) {
				printf("TxStatus: ");
				switch(CAN_TransmitStatus(CANx, 0)) {
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
				printf("TEC: %d\r\n", CAN_GetLSBTransmitErrorCounter(CANx));
				printf("REC: %d\r\n", CAN_GetReceiveErrorCounter(CANx));
				printf("LEC: %d\r\n", CAN_GetLastErrorCode(CANx));
			} else if (strcmp(tk, "send") == 0) {
				CanTxMsg TxMessage;
				/* Transmit Structure preparation */
				tk = strtok(NULL, " ");
				if (tk == NULL)
					goto cmd_error;
				TxMessage.StdId = strtol(tk, NULL, 0x10);
				TxMessage.ExtId = TxMessage.StdId;
				printf("addr: %03x\r\n", TxMessage.StdId);
				i = 0;
				do {
					tk = strtok(NULL, " ");
					if (tk == NULL)
						break;
					TxMessage.Data[i] = strtol(tk, NULL, 0x10);
					i++;
				} while (i < 8);
				if (i == 0)
					goto cmd_error;
				TxMessage.DLC = i;
				printf("payload:");
				for (i = 0; i < TxMessage.DLC; i++)
					printf(" %02x", TxMessage.Data[i]);
				printf("\r\n");

				TxMessage.RTR = CAN_RTR_DATA;
				TxMessage.IDE = CAN_ID_EXT;

				CAN_Transmit(CANx, &TxMessage);
			} else {
				printf("nonsense!");
			}
			goto cmd_finish;
cmd_error:
			printf("check your input!");
cmd_finish:
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

