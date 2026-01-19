#include "receivedata.h"
#include "queue.h"
#include "data_packet.h"

#include "semphr.h"  // 必须包含信号量头文件

extern SemaphoreHandle_t uart3_tx_sem;
extern xTaskHandle rec_task_handle;
extern QueueHandle_t rec_queue_handle;


int actual_received_byte_length = 0;
int packet_data_length = 0;

char rx_buffer[64];

void reset_rxbuffer()
{
	actual_received_byte_length = 0;
	packet_data_length = 0;
	huart1.RxState = HAL_UART_STATE_READY;
	HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t *)rx_buffer,sizeof(rx_buffer));
}

void timeout_reset()
{
	printf("timeout\r\n");
	HAL_TIM_Base_Stop_IT(&htim4);
	reset_rxbuffer();
	
}

void uart_interrupt_init()
{
	//  HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
	HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t *)rx_buffer,sizeof(rx_buffer));
	
	//                                  	缓冲区的起始地址  		缓冲区接收上限
}

int16_t parse_protocol_head()
{
	pro_packet* p_head = (pro_packet*)rx_buffer;
	#ifdef DEBUG
//	printf("packetHead:%#x\r\n",__REV16(p_head->packetHead));
//	printf("addressCode:%#x\r\n",__REV(p_head->addressCode));
//	printf("packetId:%#x\r\n",p_head->packetId);
//	printf("packetLength:%#x\r\n",__REV16(p_head->packetLength));
	#endif
	if(__REV16(p_head->packetLength)>PROTOCOL_PACKET_ALL-RECEIVE_FIXED_LENGTH_PROTOCOL_HEAD)
	{
			return -1;
	}
	return __REV16(p_head->packetLength);
}


void parse_packet_conent(uint8_t* buffer)
{
		// BaseType_t xQueueGenericSendFromISR( QueueHandle_t xQueue, const void * const pvItemToQueue, BaseType_t * const pxHigherPriorityTaskWoken, const BaseType_t xCopyPosition )
		BaseType_t pxHigherPriorityTaskWoken=pdFALSE;
		BaseType_t ret = xQueueSendToBackFromISR(rec_queue_handle,buffer,&pxHigherPriorityTaskWoken);
		if(ret == pdFALSE)
		{
			printf("xQueueSendToBackFromISR failure.\r\n");
			return;
		}
	
		portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
	
//		printf("conent:");
//		for(int i=RECEIVE_FIXED_LENGTH_PROTOCOL_HEAD;i<(RECEIVE_FIXED_LENGTH_PROTOCOL_HEAD+packet_data_length)-2;i++)
//		{
//			printf("%#x ",rx_buffer[i]);
//		}
//		printf("\r\n");
//		uint16_t crc16= (uint8_t)rx_buffer[RECEIVE_FIXED_LENGTH_PROTOCOL_HEAD+packet_data_length-2] << 8 | (uint8_t)rx_buffer[RECEIVE_FIXED_LENGTH_PROTOCOL_HEAD+packet_data_length-1];
//		printf("crc16:%#x\r\n",crc16);
}


void receive_data(uint16_t size)
{
	//printf("1 actual_received_byte_length:%d\r\n",actual_received_byte_length);
	actual_received_byte_length+=size;
	//printf("2 actual_received_byte_length:%d\r\n",actual_received_byte_length);
	if(packet_data_length==0 && actual_received_byte_length>=RECEIVE_FIXED_LENGTH_PROTOCOL_HEAD)
	{
			packet_data_length = parse_protocol_head();
	}
	
	if(packet_data_length>0 && actual_received_byte_length>=RECEIVE_FIXED_LENGTH_PROTOCOL_HEAD+packet_data_length)
	{
		
		parse_packet_conent((uint8_t*)rx_buffer);   // 接收包内容  发送到队列
		
			reset_rxbuffer();
	}else if(packet_data_length<0){
			reset_rxbuffer();
	}
	
	HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t *)(rx_buffer+actual_received_byte_length),sizeof(rx_buffer)-actual_received_byte_length);
}

int pos=0;
void start_TIM()
{
	
			if(pos)
			{
				// HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET);
			}else{
				// HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET);
			}
			
			pos=!pos;
			
			__HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_UPDATE);
			__HAL_TIM_SET_COUNTER(&htim4, 0);
			HAL_TIM_Base_Start_IT(&htim4);
}

extern void uart_dma_receivedata(uint16_t size);
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	
	if(huart==&huart1)
	{
			
			start_TIM();
			receive_data(Size);
			
	}
	
	if(huart==&huart3)
	{
		
		uart_dma_receivedata(Size);
		
//		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//    xSemaphoreGiveFromISR(uart3_tx_sem, &xHigherPriorityTaskWoken);
//    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}






