#ifndef __AP3216_HEAD__
#define __AP3216_HEAD__


#include "stdint.h"
#include "i2c.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include <stdio.h>

#define I2C_DEVICE_ADDR 0x1E << 1


#define SYSTEM_MODE 0x00
#define RUNING_FUNCTION_ACTIVE 0x03
#define SOFEWARE_RESET 0x04

#define ALS_LOW 0x0C
#define ALS_HIGH 0x0D


struct ap3216_device_data{

	uint16_t als_data;

}__attribute__((packed));


typedef struct ap3216_device_data ap3216_device_data_t;


uint8_t ap3216_init(void);
void Transmit_i2c_data(uint8_t *pData);
int8_t get_data(ap3216_device_data_t* ap3216_data);



#endif
