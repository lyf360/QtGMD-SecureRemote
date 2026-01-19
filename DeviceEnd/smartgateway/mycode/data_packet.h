#ifndef __DATA_PACKET_HEAD__
#define __DATA_PACKET_HEAD__




#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "usart.h"
#include "queue.h"
#include "cmsis_os.h"
#include "receivedata.h"

#include "doorlock_process_fun.h"
#include "sensor_process_fun.h"
#include "electrical_process_fun.h"
#include "sniff_process_fun.h"


#define QUEUE_MAX_LENGTH 16   // 消息队列长度
#define TASK_STACK_DEPTH 256  // 栈深

#define ACK_PROTOCOL_ID 0x02
#define ACK_PROTOCOL_ERROR_CODE 0x02
#define ACK_PROTOCOL_RIGHT_CODE 0x00
#define ARRAY_VALID(arr) (sizeof(arr)/sizeof(arr[0]))

struct sniff_protocol_packet{
	
	uint32_t addressCode;
	uint16_t functionCode;
	uint8_t deviceNum;
}__attribute__((packed));


#define RX_BUF_SIZE 32  // 接收缓冲区大小（足够容纳最长响应）
extern uint8_t uart_rx_buf[RX_BUF_SIZE];  // 串口接收缓冲区
extern char temp_humi_result[16];        // 解析结果："38.0,29.8"（char*类型）

typedef struct Json_Data{
	uint32_t deviceType;
	uint8_t operationCode;
	uint8_t deviceNum;
	uint16_t JsonVerify;
}Json_Data_t;

typedef struct sniff_protocol_packet sniff_packet;

extern uint8_t (*device_processing_fun[])(uint8_t * protacol,uint8_t *packet_connect);
extern void response_protocol_packet(uint8_t* buffer,uint16_t size);
extern void mqtt_response_protocol_packet(uint8_t * payload,uint16_t size);
extern void mqtt_response_json_str_protocol_packet(uint8_t *original_payload, uint16_t original_size,uint8_t deviceNum, int ack_code, const char *msg);
extern void parse_nested_json(char *json_str,Json_Data_t* datajson);
extern void show_protocol(char* name_str,uint8_t* data,uint16_t size);
extern HAL_StatusTypeDef uart_send_cmd(UART_HandleTypeDef *huart, const char *cmd, uint8_t *rx_buf, uint16_t rx_len, uint32_t timeout);
extern uint8_t parse_temp_humi(const uint8_t *rx_data, char *result);
#endif


