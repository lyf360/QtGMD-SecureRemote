#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <usart.h>
#include <string.h>
#include "queue.h"



extern int8_t dma_send_AT_commend_fun( uint8_t* data ,uint16_t size);
extern int esp8266_dma_receive_ack_data(uint8_t * pvBuffer,uint16_t size,TickType_t xTicksToWait);

int esp8266_Exit_transparent_transmission(){
		char data[128]={"+++"};
		char ackData[128]={0};
		int8_t ret;
		int result;
		// HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size)
		
		
		for(int i=0;i<=3;i++)
		{

				ret = dma_send_AT_commend_fun((uint8_t*)data ,strlen(data));
			  vTaskDelay(pdMS_TO_TICKS(3000));

				if(ret==0)
				{
					
						result = esp8266_dma_receive_ack_data((uint8_t*)ackData,sizeof(ackData),1000);
						
						if(result>0)
						{
							ackData[result]=0;
							printf("ack_data:%s\r\n",ackData);
						}else{
							return -1;
						}
						printf("dma_send_AT_commend_fun +++ success.\r\n");
						break;
				}
				
				
		}
		

		//    CIPMUX and CIPSERVER must be 0
		memset(data,0,sizeof(data));  // 清空
		strcpy(data,"AT+CIPSERVER=0\r\n");
		ret = dma_send_AT_commend_fun((uint8_t*)data ,strlen(data));
		vTaskDelay(pdMS_TO_TICKS(2000));
		if(ret!=0)
		{
				return -1;
		}
		result = esp8266_dma_receive_ack_data((uint8_t*)ackData,sizeof(ackData),1000);
		
		if(result>0)
		{
				ackData[result]=0;
				if(strstr(ackData,"ERROR")) {
						printf("CIPSERVER未开启，忽略ERROR\r\n");
				}else{
						printf("ack_data:%s\r\n",ackData);
				}
				
		}else{
				return -1;
		}
		
		
		memset(data,0,sizeof(data));  // 清空
		strcpy(data,"AT+CIPMUX=0\r\n");
		ret = dma_send_AT_commend_fun((uint8_t*)data ,strlen(data));
		vTaskDelay(pdMS_TO_TICKS(2000));
		if(ret!=0)
		{
				return -1;
		}
		result = esp8266_dma_receive_ack_data((uint8_t*)ackData,sizeof(ackData),1000);
		
		if(result>0)
		{
				ackData[result]=0;
				printf("ack_data:%s\r\n",ackData);
		}else{
				return -1;
		}
		
		
		
		memset(data,0,sizeof(data));  // 清空
		strcpy(data,"AT+CIPMODE=0\r\n");
		ret = dma_send_AT_commend_fun((uint8_t*)data ,strlen(data));
		vTaskDelay(pdMS_TO_TICKS(1000));
		if(ret!=0)
		{
				return -1;
		}
		result = esp8266_dma_receive_ack_data((uint8_t*)ackData,sizeof(ackData),1000);
		
		if(result>0)
		{
				ackData[result]=0;
				printf("ack_data:%s\r\n",ackData);
		}else{
				return -1;
		}
		
	
		

		memset(data,0,sizeof(data));  // 清空
		strcpy(data,"AT\r\n");
				
		ret = dma_send_AT_commend_fun((uint8_t*)data ,strlen(data));
		vTaskDelay(pdMS_TO_TICKS(3000));	
		if(ret!=0)
		{
				return -1;
		}
				
		result = esp8266_dma_receive_ack_data((uint8_t*)ackData,sizeof(ackData),3000);
		if(result>0)
		{
				ackData[result]=0;
				printf("ack_data:%s\r\n",ackData);
		}else{
				return -1;
		}
		
		if(strstr(ackData,"OK"))
		{
				printf("esp8266_Exit_transparent_transmission success.\r\n");
				
		}else{
				printf("esp8266_Exit_transparent_transmission failure.\r\n");
				return -1;
		}
		
		
	return 0;
		
}
extern QueueHandle_t receive_queue_handle;
extern void dma_receive_interrupt_init(void);


int clean_queue()
{
	uint8_t data;
	while(uxQueueMessagesWaiting(receive_queue_handle))
		{
				int ret = xQueueReceive(receive_queue_handle,&data,3000);
				if(ret==pdFAIL)
				{
						printf("clean_queue failure.\r\n");
						return -1;
				}
				
		}
		return 0;
}

void reset_esp8266()
{
		char data[128]={"+++"};
		char ackData[128]={0};
		int8_t ret;
		int result;
		
		do{
				memset(data,0,sizeof(data));  
				strcpy(data,"AT+RST=0\r\n");
				ret = dma_send_AT_commend_fun((uint8_t*)data ,strlen(data));
				vTaskDelay(pdMS_TO_TICKS(10000));
				if(ret!=0)
				{
						continue;
				}
				result = esp8266_dma_receive_ack_data((uint8_t*)ackData,sizeof(ackData),1000);
				
				if(result>0)
				{
						ackData[result]=0;
						printf("RST ack_data:%s\r\n",ackData);
				}else{
						continue;
				}
				
				
				clean_queue();        // 
				MX_USART3_UART_Init();
				dma_receive_interrupt_init();
				
				
				memset(data,0,sizeof(data)); 
				strcpy(data,"AT\r\n");
						
				ret = dma_send_AT_commend_fun((uint8_t*)data ,strlen(data));
				vTaskDelay(pdMS_TO_TICKS(3000));	
				if(ret!=0)
				{
						continue;
				}
						
				result = esp8266_dma_receive_ack_data((uint8_t*)ackData,sizeof(ackData),3000);
				if(result>0)
				{
						ackData[result]=0;
						printf("ack_data:%s\r\n",ackData);
				}else{
						continue;
				}
				
				if(strstr(ackData,"OK"))
				{
						
						
				}else{
						printf("reset_esp8266 failure.\r\n");
						continue;
				}
				break;
				
		}while(1);
		
		printf("RST reset_esp8266 successful.\r\n");
		
}


int esp8266_enter_ap_mode()
{
	
	 // 进入 ap  mode
		char data[128]={"AT+CWMODE=2\r\n"};
		char ackData[256]={0};
		int8_t ret;
		int bytes;
		// HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size)
		ret = dma_send_AT_commend_fun((uint8_t*)data ,strlen(data));
		vTaskDelay(pdMS_TO_TICKS(1000));

		if(ret==0)
		{
					
				bytes = esp8266_dma_receive_ack_data((uint8_t*)ackData,sizeof(ackData),500);
					//ackData[bytes]=0;
				if(bytes>0)
				{
					ackData[bytes]=0;
					printf("ack_data:%s\r\n",ackData);
				}
				
		}else{
				printf("esp8266_enter_ap_mode AT+CWMODE=2 config failure.\r\n");
				return -1;
		}
		
		//  配置 密码 id
		memset(data,0,sizeof(data));  // 清空
		strcpy(data,"AT+CWSAP=\"xiaodou\",\"12345678\",11,4\r\n");
		ret = dma_send_AT_commend_fun((uint8_t*)data ,strlen(data));
		vTaskDelay(pdMS_TO_TICKS(5000));

		if(ret==0)
		{
					
				bytes = esp8266_dma_receive_ack_data((uint8_t*)ackData,sizeof(ackData),500);
				
				if(bytes>0)
				{
					ackData[bytes]=0;
					printf("ack_data:%s\r\n",ackData);
				}
				
		}else{
				printf("esp8266_enter_ap_mode password id config failure.\r\n");
				return -1;
		}
		
		
		
		//  查询
		memset(data,0,sizeof(data));  // 清空
		strcpy(data,"AT+CWSAP?\r\n");
		ret = dma_send_AT_commend_fun((uint8_t*)data ,strlen(data));
		vTaskDelay(pdMS_TO_TICKS(2000));

		if(ret==0)
		{
					
				bytes = esp8266_dma_receive_ack_data((uint8_t*)ackData,sizeof(ackData),1000);
				
				if(bytes>0)
				{
					ackData[bytes]=0;
					printf("ack_data:%s\r\n",ackData);
					printf("esp8266_enter_ap_mode success.\r\n");
				}
				
		}else{
				printf("esp8266_enter_ap_mode failure.\r\n");
				return -1;
		}
		
		return 0;
			
}


int esp8266_enter_station_mode(char* ssid,char *pwd)
{
	
		
	
	 // 进入 station  mode
		char data[128]={"AT+CWMODE=1\r\n"};
		char ackData[256]={0};
		int8_t ret;
		int bytes;
		// HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size)
		ret = dma_send_AT_commend_fun((uint8_t*)data ,strlen(data));
		vTaskDelay(pdMS_TO_TICKS(3000));

		if(ret==0)
		{
					
				bytes = esp8266_dma_receive_ack_data((uint8_t*)ackData,sizeof(ackData),1000);
					//ackData[bytes]=0;
				if(bytes>0)
				{
					ackData[bytes]=0;
					printf("ack_data:%s\r\n",ackData);
				}
				
		}else{
				printf("AT+RST  mode failure.\r\n");
				return -1;
		}
		
		
//		//  进入 station
//		memset(data,0,sizeof(data));  // 清空
//		sprintf(data,"AT+CWMODE=1\r\n");
//		ret = dma_send_AT_commend_fun((uint8_t*)data ,strlen(data));
//		vTaskDelay(pdMS_TO_TICKS(2000));

//		if(ret==0)
//		{
//					
//				bytes = esp8266_dma_receive_ack_data((uint8_t*)ackData,sizeof(ackData),1000);
//				
//				if(bytes>0)
//				{
//					ackData[bytes]=0;
//					printf("ack_data:%s\r\n",ackData);
//				}
//				
//		}else{
//				printf("esp8266_enter_station_mode enter station  mode failure.\r\n");
//				return -1;
//		}
//		
		
		
		//  配置 密码 id
		memset(data,0,sizeof(data));  // 清空
		sprintf(data,"AT+CWDHCP=1,1\r\n");
		ret = dma_send_AT_commend_fun((uint8_t*)data ,strlen(data));
		vTaskDelay(pdMS_TO_TICKS(10000));

		if(ret==0)
		{
					
				bytes = esp8266_dma_receive_ack_data((uint8_t*)ackData,sizeof(ackData),10000);
				
				if(bytes>0)
				{
					ackData[bytes]=0;
					printf("ack_data:%s\r\n",ackData);
				}
				
		}else{
				printf("AT+CWDHCP=1,1  config failure.\r\n");
				return -1;
		}
		
		

		int connect_retry = 0;
		while(connect_retry < 10) {
     //  配置 密码 id
		memset(data,0,sizeof(data));  // 清空
		sprintf(data,"AT+CWJAP=\"%s\",\"%s\"\r\n",ssid,pwd);
		ret = dma_send_AT_commend_fun((uint8_t*)data ,strlen(data));
		vTaskDelay(pdMS_TO_TICKS(10000));
			// 重试10次，每次间隔1秒
				bytes = esp8266_dma_receive_ack_data((uint8_t*)ackData,sizeof(ackData),1000);
				if(bytes>0)
				{
					ackData[bytes]=0;
					printf("ack_data:%s\r\n",ackData);
				}
				
				// 检查是否连接成功
				if(strstr(ackData,"WIFI CONNECTED") && strstr(ackData,"WIFI GOT IP")) {
						break;
				}
				connect_retry++;
				if(connect_retry >= 10) {
				printf("WiFi connect timeout!\r\n");
				return -1;
		}
				
		}
		
		
		//  配置 密码 id
		memset(data,0,sizeof(data));  // 清空
		sprintf(data,"AT+CWDHCP?\r\n");
		ret = dma_send_AT_commend_fun((uint8_t*)data ,strlen(data));
		vTaskDelay(pdMS_TO_TICKS(10000));

		if(ret==0)
		{
					
				bytes = esp8266_dma_receive_ack_data((uint8_t*)ackData,sizeof(ackData),1000);
				
				if(bytes>0)
				{
					ackData[bytes]=0;
					printf("ack_data:%s\r\n",ackData);
				}
				
		}else{
				printf("DHCP  config failure.\r\n");
				return -1;
		}
		
		//  查询 ip
		memset(data,0,sizeof(data));  // 清空
		strcpy(data,"AT+CIFSR\r\n");
		ret = dma_send_AT_commend_fun((uint8_t*)data ,strlen(data));
		vTaskDelay(pdMS_TO_TICKS(2000));

		if(ret==0)
		{
					
				bytes = esp8266_dma_receive_ack_data((uint8_t*)ackData,sizeof(ackData),1000);
				
				if(bytes>0)
				{
					ackData[bytes]=0;
					printf("ack_data ip:%s\r\n",ackData);
					printf("search ip success \r\n");
				}
				
		}else{
				printf("search ip failure.\r\n");
				return -1;
		}
		
		//  查询
		memset(data,0,sizeof(data));  // 清空
		strcpy(data,"AT+CWJAP?\r\n");
		ret = dma_send_AT_commend_fun((uint8_t*)data ,strlen(data));
		vTaskDelay(pdMS_TO_TICKS(5000));

		if(ret==0)
		{
					
				bytes = esp8266_dma_receive_ack_data((uint8_t*)ackData,sizeof(ackData),1000);
				
				if(bytes>0)
				{
					ackData[bytes]=0;
					printf("ack_data:%s\r\n",ackData);
					printf("query WiFi success \r\n");
				}
				
		}else{
				printf("esp8266_enter_station_mode failure.\r\n");
				return -1;
		}
		
		
		
		return 0;
			
}


// 建立  TCP  server

int esp_8266_creat_tcp_server()
{
		
	
		// 多连接
		char data[128]={"AT+CIPMUX=1\r\n"};
		char ackData[128]={0};
		int8_t ret;
		int bytes;
		// HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size)
		ret = dma_send_AT_commend_fun((uint8_t*)data ,strlen(data));
		vTaskDelay(pdMS_TO_TICKS(2000));

		if(ret==0)
		{
					
				bytes = esp8266_dma_receive_ack_data((uint8_t*)ackData,sizeof(ackData),500);
					//ackData[bytes]=0;
				if(bytes>0)
				{
					printf("ack_data:%s\r\n",ackData);
				}
				
		}else{
				printf("esp8266_enter_ap_mode AT+CIPMUX=1 config failure.\r\n");
				return -1;
		}
		
		
		
		//  配置 端口
		memset(data,0,sizeof(data));  // 清空
		strcpy(data,"AT+CIPSERVER=1,9799\r\n");
		ret = dma_send_AT_commend_fun((uint8_t*)data ,strlen(data));
		vTaskDelay(pdMS_TO_TICKS(2000));

		if(ret==0)
		{
					
				bytes = esp8266_dma_receive_ack_data((uint8_t*)ackData,sizeof(ackData),500);
				
				if(bytes>0)
				{
					ackData[bytes]=0;
					printf("ack_data:%s\r\n",ackData);
				}
				
		}else{
				printf("esp8266_enter_ap_mode AT+CIPSERVER=1,9799 port failure.\r\n");
				return -1;
		}
		
		
		// 查询 ip
		
		memset(data,0,sizeof(data));  // 清空
		strcpy(data,"AT+CIFSR\r\n");
		ret = dma_send_AT_commend_fun((uint8_t*)data ,strlen(data));
		vTaskDelay(pdMS_TO_TICKS(2000));

		if(ret==0)
		{
					
				bytes = esp8266_dma_receive_ack_data((uint8_t*)ackData,sizeof(ackData),500);
				
				if(bytes>0)
				{
					ackData[bytes]=0;
					printf("ack_data:%s\r\n",ackData);
					printf("esp8266_enter_ap_mode TCP server succsee.\r\n");
				}
				
		}else{
				printf("esp8266_enter_ap_modeAT+CIFSR query ip failure.\r\n");
				return -1;
		}
		
		return 0;
}


extern int read_ssid_pwd_from_flash(char* ssid,char* pwd);
extern int saveData_ssid_pwd_To_flash(char* ssid,char* pwd);
extern int flash_read_data(uint32_t pageAddr,uint8_t * buf,uint16_t size);
#define PAGE_ADDR_63 0x0800F800
// 等待用户配置自己的网络

int wait_user_config_network()
{
	
	
	char ssid[64]={0};
	char pwd[64]={0};
	int ret = read_ssid_pwd_from_flash(ssid,pwd);
	if(ret<0)
	{
				printf("read_ssid_pwd_from_flash failure reset_flash *** ***.\r\n");
				
				
				ret = esp8266_enter_ap_mode();
				if(ret<0){
						printf("esp8266_enter_ap_mode failure.\r\n");
						return -1;
				}
				
				
				
				ret = esp_8266_creat_tcp_server();
				if(ret<0){
						printf("esp_8266_creat_tcp_server failure.\r\n");
						return -1;
				}

				uint8_t data[128]={0};
				
				while(1)
				{
						memset(data,0,sizeof(data));
						ret = esp8266_dma_receive_ack_data(data,sizeof(data),portMAX_DELAY);
						if(ret<0){
							
							printf("esp8266_dma_receive_ack_data failure.\r\n");
							
							return -1;
						}
						printf("receive user network:%s\r\n",data);
						
						printf("12344321\r\n");
						printf("ssid:%s\r\n",ssid);
						printf("pwd:%s\r\n",pwd);
						ret = sscanf((char*)data,"%*[^:]:ssid=%64[^&]&pwd=%64s",ssid,pwd);
						if(ret!=2)
						{
							printf("sscanf failure.\r\n");
							continue;
						}
						printf("jjjjjjjjjjjjjjjjjjjjj\r\n");
						printf("ssid=%s,pwd=%s\r\n",ssid,pwd);
						
						break;
				}
				// 保存数据到 flash
				
				ret = saveData_ssid_pwd_To_flash(ssid,pwd);
				if(ret==0)
				{
					printf("saveData_ssid_pwd_To_flash success.\r\n");
				}else{
					printf("saveData_ssid_pwd_To_flash failure.\r\n");
				}
				
				uint8_t data_flash_d[64]={0};
				ret = flash_read_data(PAGE_ADDR_63,data_flash_d,sizeof(data_flash_d));
				if(ret==0)
				{
						printf("data_flash_d:%s\r\n",data_flash_d);
						printf("data_flash_d:02 :");
						for(int i=0; i<strlen((char*)data_flash_d)+2; i++) // 多打印2位，排查末尾隐藏字符
						{
							printf("%02x ", data_flash_d[i]);
						}
						
						printf("44 .\r\n");
				}
				
	
		}
		ret = esp8266_enter_station_mode(ssid,pwd);
		if(ret<0)
		{
				printf("esp8266_enter_station_mode failure.\r\n");
		}
		printf("WiFi connect success.\r\n");
		
		
		
		return 0;
}

