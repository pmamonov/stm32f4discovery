#ifndef __USBD_CDC_VCP_H
#define __USBD_CDC_VCP_H

#include "usbd_cdc_core.h"
#include "usbd_conf.h"

#define DEFAULT_CONFIG                  0
#define OTHER_CONFIG                    1

uint16_t VCP_DataTx(uint8_t* Buf);

#endif /* __USBD_CDC_VCP_H */
