#ifndef __DOOR_LOCK_HEAD__
#define __DOOR_LOCK_HEAD__



#include <stdint.h>
#include "data_packet.h"

extern uint8_t doorlock_processing_function(uint8_t * protacol,uint8_t *packet_connect);
extern uint8_t query_sensor_DHT11(uint8_t* packet_connect);
#endif

