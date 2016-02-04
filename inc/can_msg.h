#ifndef _CAN_MSG_H
#define _CAN_MSG_H

#include <stdint.h>

#define CAN_MSG_PING		0xff
#define   CAN_FLG_RET		(1 << 0)

struct can_msg {
	uint8_t type;
	uint8_t flags;
	uint16_t sender;
	uint32_t data;
};
#endif /* _CAN_MSG_H*/
