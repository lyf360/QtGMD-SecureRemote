#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <usart.h>
#include <string.h>
#include "semphr.h"  // 必须包含信号量头文件
#include <queue.h>


SemaphoreHandle_t uart3_tx_sem = NULL;   // DMA 异步发送的资源同步缺失

extern DMA_HandleTypeDef hdma_usart3_rx;

char uart3_rx_buffer[512];

#define QUEUE_ACK_UART3_LENGTH 512

TaskHandle_t send_AT_handle;
QueueHandle_t receive_queue_handle;

void dma_receive_interrupt_init()
{
	  // HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
		HAL_UARTEx_ReceiveToIdle_DMA(&huart3,(uint8_t *)uart3_rx_buffer,sizeof(uart3_rx_buffer));
		
		//   *            @arg DMA_IT_HT:  Half transfer complete interrupt mask
		//_HAL_DMA_DISABLE_IT(__HANDLE__, __INTERRUPT__)  (CLEAR_BIT((__HANDLE__)->Instance->CCR , (__INTERRUPT__))
	
		__HAL_DMA_DISABLE_IT(&hdma_usart3_rx,DMA_IT_HT); // DMA_IT_HT
}

//  发送指令 给 设备  esp8266
int8_t dma_send_AT_commend_fun( uint8_t* data ,uint16_t size)
{
	
				HAL_StatusTypeDef ret = HAL_UART_Transmit_DMA(&huart3,data,size);
				vTaskDelay(pdMS_TO_TICKS(1000));	
				if(ret!=HAL_OK)
				{
						printf("HAL_UART_Transmit_DMA failure.\r\n");
						return -1;
				}
				return 0;

}


// 发送 ackdata  到队列
void uart_dma_receivedata(uint16_t size)
{

			BaseType_t  pxHigherPriorityTaskWoken = pdFALSE;
			BaseType_t ret;
			// BaseType_t xQueueGenericSendFromISR( QueueHandle_t xQueue, const void * const pvItemToQueue, BaseType_t * const pxHigherPriorityTaskWoken, const BaseType_t xCopyPosition )
			
			if(receive_queue_handle==NULL)
			{
					printf("receive_queue_handle==NULL \r\n");
			}
			for(int i=0;i<size;i++)
			{
					ret = xQueueSendToBackFromISR(receive_queue_handle,uart3_rx_buffer+i,&pxHigherPriorityTaskWoken);
					if(ret==pdFAIL)
					{
							printf(" xQueueSendToBackFromISR failure.\r\n ");
							break;
					}
			}
			
			dma_receive_interrupt_init();
}

// 从队列中取出数据

int esp8266_dma_receive_ack_data(uint8_t * pvBuffer,uint16_t size,TickType_t xTicksToWait)
{
		
		int index=0;
	// BaseType_t xQueueReceive( QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait )
		BaseType_t ret = xQueueReceive(receive_queue_handle,pvBuffer+index,xTicksToWait);
		if(ret==pdFAIL)
		{
				printf("xQueueReceive index=0 failure.\r\n");
				return -1;
		}
		index++;
		if(size==index)
		{
			printf("xQueueReceive index=1 success.\r\n");
			return index;
		}
		
#if 0
		
		while(xQueueReceive(receive_queue_handle,pvBuffer+index,xTicksToWait))
		{
				index++;
		}
		if(index==size)
		{
				return index;
		}else{
				return -1;
		}
		
#endif
		
		
		
#if 1
		
		while(uxQueueMessagesWaiting(receive_queue_handle))
		{
				ret = xQueueReceive(receive_queue_handle,pvBuffer+index,xTicksToWait);
				if(ret==pdFAIL)
				{
						printf("xQueueReceive index=2 failure.\r\n");
						return index;
				}
				index++;
				if(index==size)
				{
						printf("xQueueReceive receice success.\r\n");
						return index;
				}
				
		}
		
#endif
		return index;
		
}

void get_Semaphore()
{
	
				if(xSemaphoreTake(uart3_tx_sem,portMAX_DELAY) != pdTRUE)
        {
            printf("获取信号量超时！\r\n");
            
        }
				
}
#include "data_packet.h"
extern int esp8266_Exit_transparent_transmission(void);
extern int wait_user_config_network(void);
extern int esp8266_enter_station_mode(char* ssid,char *pwd);
extern int read_ssid_pwd_from_flash(char* ssid,char* pwd);
extern void reset_esp8266(void);
extern int mqtt_transport_open(char* peerport,char* peerip);
extern int mqtt_connect(char* peerip,char* peerport);
extern int mqtt_subscribe(char* subscribe);
extern int mqtt_receive_subscribe_data(unsigned char* buffer,int size);
extern int get_send_data(uint8_t * data,Json_Data_t* jsondata);

extern QueueHandle_t rec_queue_handle;
#include "data_packet.h"
#include "queue.h"
void uart3_send_AT_commend_taskfun(void * params)
{
		
		
		int ret;
start:
		ret = esp8266_Exit_transparent_transmission();
		if(ret<0)
		{
				goto reset_esp8266;
		}
		
		ret = wait_user_config_network();
		if(ret<0)
		{
				goto reset_esp8266;
		}
		
		// 读取 flash 中信息 enter station mode  并 配置 wifi
		
		
		goto next;
//		for(;;)
//		{

//		}
reset_esp8266:
			reset_esp8266();
			goto start;
next:
		ret = mqtt_connect("fuji.lmq.cloudamqp.com","1883");       //   fuji.lmq.cloudamqp.com    192.168.124.153      
		if(ret<0)
		{
				printf("mqtt_connect failure.\r\n");
				goto reset_esp8266;
		}
		
		ret = mqtt_subscribe("/iot/smartdevice/doorlock");   //  订阅
		if(ret<0){
			printf("mqtt_subscribe failure.\r\n");
			goto reset_esp8266;
		}
		
		unsigned char data[256]={0};
		
		while(1){
				ret = mqtt_receive_subscribe_data(data,sizeof(data));
				if(ret<0){
					printf("mqtt_receive_subscribe_data failure.\r\n");
					continue;
				}
				printf("msgbuf:%s\r\n",data);
//				printf("msgbuf hex: ");
//				for(int i=0; i<ret; i++) {
//						printf("%02x ", data[i]);
//				}
//				printf("\r\n");
				Json_Data_t jsondata;
				parse_nested_json((char*)data,&jsondata);
				printf("json  %d\r\n",jsondata.deviceNum);
				printf("json  %d\r\n",jsondata.deviceType);
				
				get_send_data(data,&jsondata);
				// pro_packet* p = (pro_packet*)data;
				// ack_protocol->addressCode
				// #define xQueueSendToBack( xQueue, pvItemToQueue, xTicksToWait )
				BaseType_t rets  = xQueueSendToBack(rec_queue_handle,data,5000);
				if(rets ==pdFAIL )
				{
					printf("xQueueSendToBack failure.\r\n");
				}			
		}
		
		//printf("end uart3_send_AT_commend_taskfun end -- -- -- \r\n");
		//vTaskDelete(NULL);
}


int get_send_data(uint8_t * data,Json_Data_t* jsondata)
{
	memset(data,0,sizeof(32));
	data[0] = 0;
	data[1] = 0;
	uint32_t deviceType = jsondata->deviceType;
	for(int i=2;i<=5;i++)
	{
		data[i] = deviceType >> 8*(5-i);
	}
	data[6] = 0x01; // 请求包
	data[7] = 0;
	data[8] = 5;  //
	data[9] = deviceType & 0xff;  // 设备型号 以设备型号  进行选择
	data[10] = jsondata->operationCode;
	data[11] = jsondata->deviceNum;
	uint16_t JsonVerify = jsondata->JsonVerify;
	data[12] = JsonVerify >> 8 & 0xff;
	data[13] = JsonVerify & 0xff;
	return 0;
}


void send_AT_commend_task()
{
	
//	 uart3_tx_sem = xSemaphoreCreateBinary();
//    if(uart3_tx_sem == NULL)
//    {
//        printf("信号量创建失败！\r\n");
//        return;
//    }
//    // 给二值信号量初始令牌（因为xSemaphoreCreateBinary创建的信号量初始值是0）
//    xSemaphoreGive(uart3_tx_sem);
	
		receive_queue_handle = xQueueCreate(QUEUE_ACK_UART3_LENGTH,1);
		
		if(receive_queue_handle==NULL){
				printf("xQueueCreate failure.\r\n");
				return;
		}
		
		BaseType_t ret = xTaskCreate(uart3_send_AT_commend_taskfun,"uart3_send_AT_commend_taskfun",512,NULL,24,&send_AT_handle);
		if(ret==pdFAIL)
		{
				printf("uart3_send_AT_commend_taskfun failure.\r\n");
				vSemaphoreDelete(uart3_tx_sem);  // 创建失败则删除信号量，避免内存泄漏
			return;
		}
	// #define xQueueCreate( uxQueueLength, uxItemSize ) xQueueGenericCreate( ( uxQueueLength ), ( uxItemSize ), ( queueQUEUE_TYPE_BASE ) )
	// QueueHandle_t xQueueGenericCreate( const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize, const uint8_t ucQueueType )
		
	
}






