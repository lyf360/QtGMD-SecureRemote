#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <usart.h>
#include <string.h>
#include "semphr.h"  // 必须包含信号量头文件
#include <queue.h>

#define PAGE_ADDR_63 0x0800F800

xTaskHandle flash_task_Handle;
int earse_flag=0;
#if 0

		uint32_t Robot_Num_Flash_Address = 0x08005000; 
		FLASH_EraseInitTypeDef My_Flash; //声明 FLASH_EraseInitTypeDef 结构体为 My_Flash
		HAL_FLASH_Unlock(); //1.解锁Flash
		My_Flash.Banks = FLASH_BANK_1; // 要擦除的存储区
		My_Flash.TypeErase = FLASH_TYPEERASE_PAGES; //页擦除
		My_Flash.PageAddress = Robot_Num_Flash_Address; //要擦除的页地址
		My_Flash.NbPages = 1; //要擦除的页数
		uint32_t PageError = 0; //设置PageError,如果出现错误这个变量会被设置为出错的FLASH地址
		HAL_FLASHEx_Erase(&My_Flash, &PageError); //2.调用擦除函数擦除
		uint16_t Write_Flash_Data = 0x1234;
		//3.对Flash进行烧写
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, Robot_Num_Flash_Address,
		Write_Flash_Data);
		//4.FLASH_等待操作完成（阻塞等待当前正在进行的FLASH操作（如页擦除、整片擦除、数据写入）完
		成,防止在操作未结束时发起新指令导致错误）
		FLASH_WaitForLastOperation(Timeout); // Timeout 超时时间（单位：毫秒）
		//5.锁住Flash
		HAL_FLASH_Lock();


#endif
extern void erase_Flash(void) ;  // 擦除

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin==GPIO_PIN_15)
	{
		earse_flag++;
		if(earse_flag%3==0)
		{
				printf("*********extern interrupt erase Flash success.\r\n");
				erase_Flash();            // 擦除
				HAL_NVIC_SystemReset();  // 系统复位
		}
		
	}
}


int flash_write_data(uint32_t pageAddr,uint8_t * buf,uint16_t size)
{
		HAL_StatusTypeDef ret;
	
		FLASH_EraseInitTypeDef flashObj;
		flashObj.PageAddress = pageAddr;
		flashObj.Banks = FLASH_BANK_1;
		flashObj.NbPages =1;
		flashObj.TypeErase = FLASH_TYPEERASE_PAGES;
	
		if(size>1024)
		{
			printf("flash_write_data size must be less than 1024.\r\n");
			return -1;
		}
	
		uint32_t PageError = 0;
		
		HAL_FLASH_Unlock(); //1.解锁Flash
	// HAL_StatusTypeDef HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *PageError)
		ret = HAL_FLASHEx_Erase(&flashObj, &PageError); //2.调用擦除函数擦除
		if(ret != HAL_OK)
		{
				printf("HAL_FLASHEx_Erase failure.\r\n");
				HAL_FLASH_Lock();
				return -1;
		}
		uint16_t Write_Flash_Data=0;
		
		for(int i=0;i<size;i+=2)
		{
				if(i+1==size)
				{
					Write_Flash_Data = 0<< 8 | buf[i];
				}else{
					Write_Flash_Data = buf[i+1]<< 8 | buf[i];
				}
				ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,pageAddr,Write_Flash_Data);//3.对Flash进行烧写         // 半字写
				if(ret != HAL_OK)
				{
						printf("HAL_FLASH_Program failure.\r\n");
						HAL_FLASH_Lock();				
						return -1;
				}
				pageAddr+=2;
		}
		
		
		//4.FLASH_等待操作完成（阻塞等待当前正在进行的FLASH操作（如页擦除、整片擦除、数据写入）完成,防止在操作未结束时发起新指令导致错误）
		ret = FLASH_WaitForLastOperation(portMAX_DELAY); // Timeout 超时时间（单位：毫秒）
		if(ret != HAL_OK)
		{
				printf("FLASH_WaitForLastOperation failure.\r\n");
				HAL_FLASH_Lock();
				return -1;
		}
		
		//5.锁住Flash
		HAL_FLASH_Lock();
		return 0;
}


void erase_Flash()   // 擦除
{
		HAL_StatusTypeDef ret;
	
		FLASH_EraseInitTypeDef flashObj;
		flashObj.PageAddress = PAGE_ADDR_63;
		flashObj.Banks = FLASH_BANK_1;
		flashObj.NbPages =1;
		flashObj.TypeErase = FLASH_TYPEERASE_PAGES;
	
		uint32_t PageError = 0;
		
		HAL_FLASH_Unlock(); //1.解锁Flash
	// HAL_StatusTypeDef HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *PageError)
		ret = HAL_FLASHEx_Erase(&flashObj, &PageError); //2.调用擦除函数擦除
		if(ret != HAL_OK)
		{
				printf("HAL_FLASHEx_Erase failure.\r\n");
				HAL_FLASH_Lock();
				return ;
		}
	
		
		
		//3.FLASH_等待操作完成（阻塞等待当前正在进行的FLASH操作（如页擦除、整片擦除、数据写入）完成,防止在操作未结束时发起新指令导致错误）
		ret = FLASH_WaitForLastOperation(portMAX_DELAY); // Timeout 超时时间（单位：毫秒）
		if(ret != HAL_OK)
		{
				printf("FLASH_WaitForLastOperation failure.\r\n");
				HAL_FLASH_Lock();
				return ;
		}
		
		//4.锁住Flash
		HAL_FLASH_Lock();
		return ;
}


int flash_read_data(uint32_t pageAddr,uint8_t * buf,uint16_t size)
{
		memcpy(buf,(uint8_t *)pageAddr,size);
		return 0;
}



int saveData_ssid_pwd_To_flash(char* ssid,char* pwd)
{
		char temp_data[256]={0};
		int ret =snprintf(temp_data,sizeof(temp_data),"ssid=%s&pwd=%s",ssid,pwd);
		if(ret<0)
		{
			printf("snprintf data failure.\r\n");
			return -1;
		}
		
		ret= flash_write_data(PAGE_ADDR_63,(uint8_t*)temp_data,sizeof(temp_data)+1);
		if(ret<0)
		{
			printf("flash_write_data failure.\r\n");
			return -1;
		}
		
		
		return 0;
}

int read_ssid_pwd_from_flash(char* ssid,char* pwd)
{
		uint8_t buf1[128]={0};
		int ret =flash_read_data(PAGE_ADDR_63,buf1,sizeof(buf1));
		if(ret==-1)
		{
			printf("flash_read_data failure.\r\n");
			return -1;
		}
		printf("buf1 hex: ");
		for(int i=0; i<strlen((char*)buf1)+2; i++){ // 多打印2位，排查末尾隐藏字符
				printf("%02x ", buf1[i]);
		}
		printf("\r\n");
		//ret = sscanf((char*)buf1,"ssid=%63[^&]&pwd=%63[^\r\n]",ssid,pwd);
		ret = sscanf((char*)buf1,"ssid=%63[^&]&pwd=%63[^\r\n]",ssid,pwd);
		printf("ret = %d\r\n",ret);
		if(ret!=2)
		{
				printf("sscanf failure.\r\n");
				return -1;
				
		}else{
				printf("sscanf success ----.\r\n");
		}
		
		#if 0
		
		ret = sscanf((char*)data,"%*[^:]:ssid=%64[^&]&pwd=%64s",ssid,pwd);
		if(ret!=2)
					{
					printf("sscanf failure.\r\n");
					continue;
		}
		printf("ssid=%s,pwd=%s\r\n",ssid,pwd);
		
		#endif
		
		
		printf("ssid:%s,pwd:%s\r\n",ssid,pwd);
		return 0;
}

int flash_test()
{
		
		uint8_t buf2[7] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77};
		
		int ret =  flash_write_data(PAGE_ADDR_63,buf2,sizeof(buf2));
		if(ret==-1)
		{
			printf("flash_write_data failure.\r\n");
			return -1;
		}
		
		uint8_t buf1[16]={0};
		ret =flash_read_data(PAGE_ADDR_63,buf1,sizeof(buf1));
		if(ret==-1)
		{
			printf("flash_read_data failure.\r\n");
			return -1;
		}
		for(int i=0;i<strlen((char*)buf1);i++)
		{
				printf("%4x",buf1[i]);
		}
		
		printf("\r\n");
		return 0;
		
}

void flash_task_fun(void* params)
{
		for(;;)
		{
			flash_test();
			vTaskDelay(pdMS_TO_TICKS(2000));
		}
}

void flash_task_init()
{
		BaseType_t ret = xTaskCreate(flash_task_fun,"flash_task_fun",256,NULL,24,&flash_task_Handle);
		if(ret==pdFAIL)
		{
				printf("uart3_send_AT_commend_taskfun failure.\r\n");
				return;
		}
}



