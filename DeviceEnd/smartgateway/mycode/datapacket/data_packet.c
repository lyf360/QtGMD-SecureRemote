#include "data_packet.h"
#include <stdlib.h>
#include <gpio.h>
#include <string.h>
xTaskHandle rec_task_handle;
QueueHandle_t rec_queue_handle;

extern uint16_t modbus_crc_fast(const uint8_t *data, uint16_t length);

#define RX_BUF_SIZE 32  // 接收缓冲区大小（足够容纳最长响应）
uint8_t uart_rx_buf[RX_BUF_SIZE] = {0};  // 串口接收缓冲区
char temp_humi_result[16] = {0};        // 解析结果："38.0,29.8"（char*类型）

uint8_t (*device_processing_fun[])(uint8_t * protacol,uint8_t *packet_connect) =
{
	
	 // 函数指针
		doorlock_processing_function,
		electrical_processing_function,
		sensor_processing_function,
		//sniff_processing_function
};




void show_protocol(char* name_str,uint8_t* data,uint16_t size)
{
	
	#ifdef DEBUG
	printf("%s : ",name_str);
	for(int i=0;i<size;i++)
	{
			printf("%02x ",data[i]);
	}
	printf("\r\n");
	#endif
}

void response_protocol_packet(uint8_t* buffer,uint16_t size)
{
	// HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size, uint32_t Timeout)
	HAL_StatusTypeDef ret = HAL_UART_Transmit(&huart1,buffer,size,portMAX_DELAY);
	if(ret!=HAL_OK)
	{
		printf("HAL_UART_Transmit failure.\r\n");
		return;
	}
}

void ack_error_protocol_packet()
{
	
	uint8_t protocol_buf[sizeof(pro_packet) + 3]; // 栈上数组，预留12字节
    
    // 将数组指针转换为结构体指针（确保对齐，此处数组天然对齐）
  pro_packet* ack_protocol = (pro_packet*)protocol_buf;
	

	ack_protocol->packetHead=__REV16(0xabcd);
	ack_protocol->addressCode=__REV(0xf0f0f0f0);
	ack_protocol->packetId = ACK_PROTOCOL_ID;
	ack_protocol->packetLength = __REV16(0x0003);
	ack_protocol->array[0] = ACK_PROTOCOL_ERROR_CODE;

	
	uint16_t crc16_check = modbus_crc_fast((uint8_t*)ack_protocol,10);
	ack_protocol->array[1] = crc16_check >> 8 & 0xff;
	ack_protocol->array[2] = crc16_check & 0xff;

	
	//pro_packet* p=(pro_packet*)ack_protocol;
	uint8_t * data = (uint8_t*)ack_protocol;
	show_protocol("data",data,12);
	response_protocol_packet(data,12);
	
}



void queue_store_init()
{
	 // QueueHandle_t xQueueGenericCreate( const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize, const uint8_t ucQueueType )
	rec_queue_handle = xQueueCreate(QUEUE_MAX_LENGTH,PROTOCOL_PACKET_ALL);
	if(rec_queue_handle==NULL)
	{
		printf("rec_queue_handle xQueueCreate failure.\r\n");
		return;
	}
	
}


void show_rx_buffer(uint8_t* data)
{
	// void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
		//HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
		#ifdef DEBUG1
		pro_packet* p = (pro_packet*)data;
		printf("packetHead:%#x\r\n",__REV16(p->packetHead));
		printf("addressCode:%#x\r\n",__REV(p->addressCode));
		printf("packetId:%#x\r\n",p->packetId);
		printf("packetLength:%#x\r\n",__REV16(p->packetLength));
		
		uint16_t length = __REV16(p->packetLength);
		printf("conent:");
		for(int i=RECEIVE_FIXED_LENGTH_PROTOCOL_HEAD;i<(RECEIVE_FIXED_LENGTH_PROTOCOL_HEAD+length)-2;i++)
		{
			printf("%#x ",data[i]);
		}
		printf("\r\n");
		uint16_t crc16= (uint8_t)data[RECEIVE_FIXED_LENGTH_PROTOCOL_HEAD+length-2] << 8 | (uint8_t)data[RECEIVE_FIXED_LENGTH_PROTOCOL_HEAD+length-1];
		printf("crc16:%#x\r\n",crc16);
		#endif
}


uint8_t asd[5]={0};


extern int ack_correct_protocol_packet(uint8_t * protocol,uint8_t* packetconnect);
void task_data_receive_fun(void* parameter)
{

		for(;;)
		{
			uint8_t data[PROTOCOL_PACKET_ALL]={0};
			
			// BaseType_t xQueueReceive( QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait )
			BaseType_t ret = xQueueReceive(rec_queue_handle,data,portMAX_DELAY);
			if(ret==pdFAIL)
			{
				printf("rec_queue_handle xQueueReceive failure.\r\n");
				return;
			}
			
			// 解析数据 并处理  校验  应答
			
			show_rx_buffer(data);
			pro_packet* p = (pro_packet*)data;
			uint16_t length=0; 
			
			length = __REV16(p->packetLength);
			printf("length = %d\r\n",length);
			uint16_t check_before_crc16 = data[RECEIVE_FIXED_LENGTH_PROTOCOL_HEAD+length-2]<<8 | data[RECEIVE_FIXED_LENGTH_PROTOCOL_HEAD+length-1];
			uint16_t check_after_crc16 = modbus_crc_fast(data,RECEIVE_FIXED_LENGTH_PROTOCOL_HEAD+length-2);
			
			
			#ifdef DEBUG
			printf("check_before_crc16:%#x\r\n",check_before_crc16);
			printf("check_after_crc16:%#x\r\n",check_after_crc16);
			#endif
			
			uint8_t deviceNum = data[11];
			if(check_before_crc16!=check_after_crc16 || (__REV(p->addressCode) >= ARRAY_VALID(device_processing_fun) && __REV(p->addressCode)!=0xffffffff))
			{
					if(__REV16(p->packetHead) ==0xABCD )
					{
							ack_error_protocol_packet();
					}else{
							mqtt_response_json_str_protocol_packet(data,strlen((char*)data),deviceNum,1,"error");
					}
				
			}else{
				
				pro_packet* p = (pro_packet*)data;
				
				//uint8_t (*handle)(uint8_t deviceType,uint8_t deviceNum);
				
				uint8_t (*handle)(uint8_t * protacol,uint8_t *packet_connect);
					
				handle= device_processing_fun[__REV(p->addressCode)];
				
				#ifdef DEBUG1
						HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
						vTaskDelay(pdMS_TO_TICKS(1000));
						HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET);
				#endif
				if(__REV(p->addressCode) == 0xffffffff)
				{
						handle = device_processing_fun[ARRAY_VALID(device_processing_fun)-1];
				}
				show_protocol("data",data,14);
				uint8_t packet_connect[64]={0};
				
				uint8_t ret = handle(data,packet_connect);
				show_protocol("packet_connect",packet_connect,6);
				if(ret==0xff)
				{
						printf("operation failure no fun.\r\n");
				}
				ack_correct_protocol_packet(data,packet_connect);
				
				
			}
			
		}
}


HAL_StatusTypeDef uart_send_cmd(UART_HandleTypeDef *huart, const char *cmd, uint8_t *rx_buf, uint16_t rx_len, uint32_t timeout)
{
    HAL_StatusTypeDef send_status;
    uint16_t rx_cnt = 0;  // 已接收的字节数
    uint32_t start_tick = HAL_GetTick();  // 记录超时起始时间

    /* 1. 清空接收缓冲区（避免残留数据干扰） */
    memset(rx_buf, 0, rx_len);

    /* 2. 发送指令（发送超时设为100ms即可，命令本身很短） */
    send_status = HAL_UART_Transmit(huart, (uint8_t*)cmd, strlen(cmd), 100);
    if (send_status != HAL_OK)
    {
        printf("命令发送失败！状态：%d\r\n", send_status);
        return send_status;  // 发送失败，直接返回
    }

    /* 3. 接收响应：轮询+检测\r\n结束符（核心修改） */
    while (1)
    {
        /* 检查是否超时 */
        if (HAL_GetTick() - start_tick > timeout)
        {
            rx_buf[rx_cnt] = '\0';  // 给缓冲区加字符串结束符，方便打印排查
            printf("接收超时！已收到的部分数据：%s\r\n", rx_buf);
            return HAL_TIMEOUT;
        }

        /* 检查UART是否有数据可读（非阻塞查询） */
        if (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE) != RESET)
        {
            rx_buf[rx_cnt++] = (uint8_t)huart->Instance->DR;  // 读取1字节到缓冲区

            /* 关键：检测是否收到行结束符 \r\n（模块响应的结尾标志） */
            if (rx_cnt >= 2 && rx_buf[rx_cnt-2] == '\r' && rx_buf[rx_cnt-1] == '\n')
            {
                rx_buf[rx_cnt] = '\0';  // 添加字符串结束符，避免打印乱码
                return HAL_OK;  // 接收完整一行，返回成功
            }

            /* 防止缓冲区溢出（留1字节存结束符） */
            if (rx_cnt >= rx_len - 1)
            {
                rx_buf[rx_len - 1] = '\0';
                printf("接收缓冲区即将溢出！已接收完整数据：%s\r\n", rx_buf);
                return HAL_OK;
            }
        }
    }
}

void DHT11_DH()
{
	printf("DHT11 start AT Read ...\r\n");
  HAL_StatusTypeDef hand_status = uart_send_cmd(&huart2, "Hand\r\n", uart_rx_buf, RX_BUF_SIZE, 1000);
  printf("DHT11   uart_rx_buf %s\r\n",uart_rx_buf);
  if (hand_status == HAL_OK && strstr((char*)uart_rx_buf, "OK") != NULL)
	{
			printf("DHT11 start success %s\r\n", uart_rx_buf);
			return ;
	}
	else
	{
			printf("DHT11 start failure !!! \r\n");
			return ;
	}
}

uint8_t parse_temp_humi(const uint8_t *rx_data, char *result)
{
  /* 步骤1：查找关键标记位，验证格式 */
  char *r_tag = strstr((char*)rx_data, "R:");    // 找到"R:"（湿度起始标记）
  char *rh_tag = strstr((char*)rx_data, "RH");   // 找到"RH"（湿度结束标记）
  char *c_tag = strstr((char*)rx_data, "C");     // 找到"C"（温度结束标记）

  // 格式错误：缺少关键标记
  if (r_tag == NULL || rh_tag == NULL || c_tag == NULL)
  {
    return 0;
  }

  /* 步骤2：截取湿度值（R: → RH 之间的内容） */
  char humidity_str[8] = {0};
  strncpy(humidity_str, r_tag + 2, rh_tag - (r_tag + 2));  // 跳过"R:"，截取到RH前

  /* 步骤3：截取温度值（RH空格 → C 之间的内容） */
  char temp_str[8] = {0};
  strncpy(temp_str, rh_tag + 2, c_tag - (rh_tag + 2));     // 跳过"RH "，截取到C前

  /* 步骤4：组合为 "湿度,温度" 格式（char*类型） */
  sprintf(result, "%s,%s\r\n", humidity_str, temp_str);

  return 1;  // 解析成功
}


void task_receive_init()
{
	
	DHT11_DH();
	
	//HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
	queue_store_init();
	BaseType_t ret = xTaskCreate(task_data_receive_fun,"task_data_receive_fun",TASK_STACK_DEPTH,NULL,osPriorityNormal,&rec_task_handle);
	if(ret==pdFAIL)
	{
		
		printf("rec_task_handle xTaskCreate falure. \r\n");
		return;
	}
}

//  00 00 00 00 00 00 02 00 04 00         AB CD  
//  包头  设备地址        长度  确认码      校验 
int ack_correct_protocol_packet(uint8_t * protocol,uint8_t* packetconnect)
{
		pro_packet * p = (pro_packet*)protocol;
		uint8_t deviceNum = protocol[11];
		p->packetId = 0x02;
		uint16_t packet_connect_length = ((packetconnect[1] << 8)&0xff00) | ((packetconnect[2])&0xff);   // 包长度 计入 array
	
		uint16_t length=6+packet_connect_length;
		p->packetLength = __REV16(length);
		p->array[0]=0;  // 确认码 // 10
		p->array[1]=packetconnect[0];  // operation
		p->array[2]=packetconnect[1]; 
		p->array[3]=packetconnect[2]; 
		
		for(int i=0;i<packet_connect_length;i++)
		{
			p->array[4+i] = packetconnect[i+3];
		}
		show_protocol("pre_protocol_packet",protocol,RECEIVE_FIXED_LENGTH_PROTOCOL_HEAD+4+packet_connect_length);
		uint16_t check_crc16 = modbus_crc_fast(protocol,RECEIVE_FIXED_LENGTH_PROTOCOL_HEAD+4+packet_connect_length);   // 9 2 2 length 
		p->array[4+packet_connect_length]= check_crc16 >> 8 & 0xff;
		p->array[5+packet_connect_length]= check_crc16 & 0xff;
		show_protocol("ack_correct_protocol_packet_connect",protocol,RECEIVE_FIXED_LENGTH_PROTOCOL_HEAD+6+packet_connect_length);
		
		// 00 00 00 00 00 02 02 00 08 00 02 00 02 00 16 28 b9
		if(__REV(p->packetHead) == 0xABCD)
		{	
			response_protocol_packet(protocol,6+packet_connect_length);
		}else{
			mqtt_response_json_str_protocol_packet(protocol,strlen((char*)protocol),deviceNum,0,"");  // 错误码为空
		}
		return 0;
}

