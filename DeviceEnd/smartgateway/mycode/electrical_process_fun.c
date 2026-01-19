#include "electrical_process_fun.h"


extern uint16_t modbus_crc_fast(const uint8_t *data, uint16_t length);


#define OPERATON_FUN 3
#define DEVICENUM_FUN 2


uint8_t open_electrical(uint8_t* packet_connect)
{
	packet_connect[1]=0;
	packet_connect[2]=0;
	HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
	return 0;
}

uint8_t close_electrical(uint8_t* packet_connect)
{
	packet_connect[1]=0;
	packet_connect[2]=0;
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
	
	return 0;
}

uint8_t query_electrical(uint8_t* packet_connect)
{
	packet_connect[1]=0;
	packet_connect[2]=1;
	packet_connect[3] = htim3.Instance->CCER & TIM_CCER_CC1E ? 0:1;   // 因为是反着的  
	return 0;
}




typedef uint8_t (*electrical_fun_t)(uint8_t* packet_connect);
typedef struct{

		electrical_fun_t electrical_fun;  // 设备 0 
		// 设备 1
		 
}electrical_operation_Num_fun_t;  // 


electrical_operation_Num_fun_t electrical_Operate_Num[DEVICENUM_FUN][OPERATON_FUN]={
	{open_electrical,close_electrical,query_electrical},
	{NULL,NULL,NULL},
	
};



uint8_t electrical_processing_function(uint8_t * protacol,uint8_t *packet_connect)
{
	
	// electrical
	
	uint8_t status=0;
	
	pro_packet* p = (pro_packet*)protacol;
	
	
	if(p->array[1]<OPERATON_FUN && p->array[2] < DEVICENUM_FUN && (electrical_Operate_Num[p->array[2]][p->array[1]]).electrical_fun!=NULL)
	{
		status = (electrical_Operate_Num[p->array[2]][p->array[1]]).electrical_fun(packet_connect);  //   设备num   操作code
	}else{
		return 0xff;
	}
	
	//  地址选择 码   operation  num
	// ab cd     00 00 00 00         01        00 05        00 00 01           6A 05
	packet_connect[0] = protacol[10];  // operationCode
	
	return status;
	
}




