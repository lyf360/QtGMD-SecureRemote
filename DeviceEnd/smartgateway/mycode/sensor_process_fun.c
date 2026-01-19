#include "sensor_process_fun.h"

//#include "electrical_process_fun.h"


#define OPERATON_FUN 3
#define DEVICENUM_FUN 2


extern uint16_t modbus_crc_fast(const uint8_t *data, uint16_t length);

uint8_t query_sensor(uint8_t* packet_connect)
{
	
	printf("device 0         *** \r\n");
	
	// packet_connect 长度
	packet_connect[1]=0;
	packet_connect[2]=2;
	
	uint16_t connect=  ((packet_connect[1] << 8)&0xff00) | ((packet_connect[2])&0xff);
	printf("query_sensor connect:%d\r\n",connect);
	ap3216_device_data_t ap3216_data;
	get_data(&ap3216_data);
	
	uint16_t ap3216_d = ap3216_data.als_data;
	//uint16_t ap3216_d = 234;
	
	packet_connect[3]=ap3216_d & 0xff00;
	packet_connect[4]=ap3216_d & 0x00ff;
	uint16_t connect_sensor=  ((packet_connect[3] << 8)&0xff00) | ((packet_connect[4])&0xff);
	printf("connect_sensor data connect:%d\r\n",connect_sensor);
	
	return 0;
}


uint8_t query_sensor_DHT11(uint8_t* packet_connect)
{
	
	printf("device 1        *** \r\n");
	
	// packet_connect 长度
	packet_connect[1]=0;
	packet_connect[2]=2;
	
	packet_connect[3]=0;
	packet_connect[4]=0;
	
		memset(uart_rx_buf, 0, RX_BUF_SIZE);
    memset(temp_humi_result, 0, sizeof(temp_humi_result));

    /* 发送 Read\r\n 指令，接收模块响应（最长32字节，超时2秒） */
		printf("\n send:Read AT \r\n");
    HAL_StatusTypeDef read_status = uart_send_cmd(&huart2, "Read\r\n", uart_rx_buf, RX_BUF_SIZE, 2000);
		
    if (read_status == HAL_OK)
    {
      printf("DHT11 ack %s\r\n", uart_rx_buf);

      /* 解析响应为 "38.0,29.8" 格式 */
      if (parse_temp_humi(uart_rx_buf, temp_humi_result) == 1)
      {
        /* 解析成功：temp_humi_result 就是 char* 类型的目标字符串 */
        printf("parse uart_rx_buf（char*） %s\r\n", temp_humi_result);
      }
      else
      {
        printf("parse uart_rx_buf failure !!! \r\n");
      }
    }
    else
    {
      printf("parse uart_rx_buf failure   chaoshi  error  !!! \r\n");
    }
	
	return 0;
}


typedef uint8_t (*sensor_fun_t)(uint8_t* packet_connect);
typedef struct{

		sensor_fun_t sensor_fun;  // 设备 0 
		// 设备 1
		 
}sensor_operation_Num_fun_t;  // 


sensor_operation_Num_fun_t sensor_Operate_Num[DEVICENUM_FUN][OPERATON_FUN]={
	{NULL,NULL,query_sensor},    // 查询 02
	{NULL,NULL,query_sensor_DHT11},
	
};



uint8_t sensor_processing_function(uint8_t * protacol,uint8_t *packet_connect)
{
	
	uint8_t status=0;
	
	pro_packet* p = (pro_packet*)protacol;
	
	
	if(p->array[1]<OPERATON_FUN && p->array[2] < DEVICENUM_FUN && (sensor_Operate_Num[p->array[2]][p->array[1]]).sensor_fun!=NULL)
	{
		status = (sensor_Operate_Num[p->array[2]][p->array[1]]).sensor_fun(packet_connect);  //   设备num   操作code
	}else{
		return 0xff;
	}
	
	printf("p->array[1]=%d p->array[2]=%d\r\n",p->array[1],p->array[2]);
	//  地址选择 码   operation  num
	// ab cd     00 00 00 00         01        00 05        00 00 01           6A 05
	packet_connect[0] = protacol[10];  // operationCode
	
	return status;
	
}




