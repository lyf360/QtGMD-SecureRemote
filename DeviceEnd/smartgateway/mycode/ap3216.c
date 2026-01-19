#include "ap3216.h"


I2C_HandleTypeDef * ap3216i2c;

uint8_t ap3216_init()
{
	
		ap3216i2c = &hi2c1;
		//uint16_t addr = ((I2C_DEVICE_ADDR << 8 ) & 0xFF00) | (SYSTEM_MODE&0x00ff);
		uint8_t buffer1[]={SYSTEM_MODE,SOFEWARE_RESET};
		Transmit_i2c_data(buffer1);  // ģʽ   00  4
		HAL_Delay(15);
		uint8_t buffer2[]={SYSTEM_MODE,RUNING_FUNCTION_ACTIVE};
		Transmit_i2c_data(buffer2);  // ģʽ   00  3 
		HAL_Delay(100);
		return 0;
}


void receive_i2c_data(uint16_t DevAddress, uint8_t *pData)
{
		HAL_StatusTypeDef ret = HAL_I2C_Master_Receive(ap3216i2c,I2C_DEVICE_ADDR,pData,1,portMAX_DELAY);
		if(ret!=HAL_OK )
		{
			printf("receive_i2c_data failure.\r\n");
			return;
		}
}	



void Transmit_i2c_data(uint8_t *pData)
{
		// HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
		
		HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(ap3216i2c,I2C_DEVICE_ADDR,pData,2,portMAX_DELAY);
		if(ret!=HAL_OK )
		{
			printf("Transmit_i2c_data failure.\r\n");
			return;
		}
}	


uint8_t read_data(uint8_t operationCode,uint8_t* data)
{
		HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(ap3216i2c,I2C_DEVICE_ADDR,&operationCode,1,portMAX_DELAY);
		if(ret!=HAL_OK )
		{
			printf("Transmit_i2c_data failure.\r\n");
		}
		
		ret = HAL_I2C_Master_Receive(ap3216i2c,I2C_DEVICE_ADDR,data,1,portMAX_DELAY);
		if(ret!=HAL_OK )
		{
			printf("receive_i2c_data failure.\r\n");
			
		}
		return 0;
}


int8_t get_data(ap3216_device_data_t* ap3216_data)
{
	uint16_t als_data=0;
	uint8_t data;
	read_data(ALS_HIGH,&data);
	als_data |= data<<8 & 0xff00;
	read_data(ALS_LOW,&data);
	als_data |= data & 0xff;
	ap3216_data->als_data = als_data;
	printf("als_data= %d",als_data);
	
	return 0;
}





