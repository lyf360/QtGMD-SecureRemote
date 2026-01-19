#include "doorlock_process_fun.h"


#define OPERATON_FUN 3
#define DEVICENUM_FUN 2


extern uint16_t modbus_crc_fast(const uint8_t *data, uint16_t length);

uint8_t open_doorlock(uint8_t* packet_connect)
{
	packet_connect[1]=0;
	packet_connect[2]=0;
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
	return 0;
}

uint8_t close_doorlock(uint8_t* packet_connect)
{
	packet_connect[1]=0;
	packet_connect[2]=0;
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET);
	return 0;
}

uint8_t query_doorlock(uint8_t* packet_connect)
{
	packet_connect[1]=0;
	packet_connect[2]=1;
	
	GPIO_PinState ret =HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
	if(ret == GPIO_PIN_SET)
	{
		packet_connect[3]=1;
	}else{
		packet_connect[3]=0;
	}
	
	return 0;
}


typedef uint8_t (*doorlock_fun_t)(uint8_t* packet_connect);
typedef struct{

		doorlock_fun_t doorlock_fun;  // 设备 0 
		// 设备 1
		 
}doorlock_operation_Num_fun_t;  // 


doorlock_operation_Num_fun_t doorlock_Operate_Num[DEVICENUM_FUN][OPERATON_FUN]={
	{open_doorlock,close_doorlock,query_doorlock},
	{NULL,NULL,NULL},
	
};





uint8_t doorlock_processing_function(uint8_t * protacol,uint8_t *packet_connect)
{
	uint8_t status=0;
	
	pro_packet* p = (pro_packet*)protacol;
	
	
	if(p->array[1]<OPERATON_FUN && p->array[2] < DEVICENUM_FUN && (doorlock_Operate_Num[p->array[2]][p->array[1]]).doorlock_fun!=NULL)
	{
		status = (doorlock_Operate_Num[p->array[2]][p->array[1]]).doorlock_fun(packet_connect);  //   设备num   操作code
	}else{
		return 0xff;
	}
	
	//  地址选择 码   operation  num
	// ab cd     00 00 00 00         01        00 05        00 00 01           6A 05
	packet_connect[0] = protacol[10];  // operationCode
	
	return status;
}




