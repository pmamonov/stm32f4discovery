#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED 
#pragma     data_alignment = 4 
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

#include "usbd_cdc_vcp.h"
#include "usb_conf.h"
#include "newlib_stubs.h"

/* These are external variables imported from CDC core to be used for IN 
   transfer management. */
extern uint8_t  APP_Rx_Buffer[]; /* Write CDC received data in this buffer.
                                     These data will be sent over USB IN endpoint
                                     in the CDC core functions. */
extern uint32_t APP_Rx_ptr_in;    /* Increment this pointer or roll it back to
                                     start address when writing received data
                                     in the buffer APP_Rx_Buffer. */

static uint16_t VCP_Init(void);
static uint16_t VCP_DeInit(void);
static uint16_t VCP_Ctrl(uint32_t Cmd, uint8_t* Buf, uint32_t Len);
static uint16_t VCP_DataRx(uint8_t* Buf, uint32_t Len);

CDC_IF_Prop_TypeDef VCP_fops = 
{
  VCP_Init,
  VCP_DeInit,
  VCP_Ctrl,
  NULL,
  VCP_DataRx
};

static uint16_t VCP_Init(void)
{
	return USBD_OK;
}

static uint16_t VCP_DeInit(void)
{
	return USBD_OK;
}

static uint16_t VCP_Ctrl (uint32_t Cmd, uint8_t* Buf, uint32_t Len)
{ 
	return USBD_OK;
}

uint16_t VCP_DataTx(uint8_t* Buf)
{
	APP_Rx_Buffer[APP_Rx_ptr_in] = Buf[0];
	APP_Rx_ptr_in++;
	if (APP_Rx_ptr_in >= APP_RX_DATA_SIZE)
		APP_Rx_ptr_in = 0;
	return USBD_OK;
}

static uint16_t VCP_DataRx(uint8_t* Buf, uint32_t Len)
{
	uint32_t i;
  
	for (i = 0; i < Len; i++) {
		if (stdin_buffer_in >= STDIN_BUFFER_SIZE)
			stdin_buffer_in=0;
		stdin_buffer[stdin_buffer_in++] = Buf[i];
		if (stdin_buffer_len < STDIN_BUFFER_SIZE)
			stdin_buffer_len++;
	} 
  return USBD_OK;
}
