#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

xTaskHandle smartgatewayhandle01;

void testsmartgateway001(void *pvParameters)
{
	for(;;)
	{
		printf("A-->.\r\n");
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}


void testsmartgateway001init()
{
	BaseType_t ret = xTaskCreate(testsmartgateway001,"testsmartgateway001",128,NULL,24,&smartgatewayhandle01);
	if(ret == pdFAIL)
	{
		printf("xTaskCreate testsmartgateway001 failure.\r\n");
		return;
	}
		
}


