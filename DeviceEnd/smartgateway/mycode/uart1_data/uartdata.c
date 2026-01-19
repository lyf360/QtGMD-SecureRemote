#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "usart.h"

//char rx_buffer[512];

//void uart_interrupt_init()
//{
//	//  HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
//	HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t *)rx_buffer,sizeof(rx_buffer));
//}


//// __weak void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)

//void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
//{
//	if(huart==&huart1)
//	{
//		int i=0;
//		for(i=0;i<Size;i++)
//		{
//			printf("%c",rx_buffer[i]);
//		}
//		printf("\r\n");
//		uart_interrupt_init();
//	}
//}
