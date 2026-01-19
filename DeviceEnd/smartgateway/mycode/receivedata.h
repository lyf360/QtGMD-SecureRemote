#ifndef __RECEIVEDATA_HEAD__
#define __RECEIVEDATA_HEAD__

#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "usart.h"
#include "tim.h"


#define DEBUG


#define RECEIVE_FIXED_LENGTH_PROTOCOL_HEAD 9

#define PROTOCOL_PACKET_ALL 64



struct protocol_packet{
	uint16_t packetHead;
	uint32_t addressCode;
	uint8_t packetId;
	uint16_t packetLength;
	uint8_t array[];
}__attribute__((packed));

typedef struct protocol_packet pro_packet;




#endif

