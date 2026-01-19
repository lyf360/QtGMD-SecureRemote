#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <usart.h>
#include <string.h>
#include "queue.h"
#include "MQTTPacket.h"
#include "doorlock_process_fun.h"

extern int8_t dma_send_AT_commend_fun( uint8_t* data ,uint16_t size);
extern int esp8266_dma_receive_ack_data(uint8_t * pvBuffer,uint16_t size,TickType_t xTicksToWait);

int send_mqtt_protocol(unsigned char * data ,uint16_t size)
{
	return dma_send_AT_commend_fun((uint8_t*)data,size);
}

int transport_getdata(unsigned char * pvBuffer,int size)
{
	return esp8266_dma_receive_ack_data((uint8_t*)pvBuffer,size,3000);
}

int mqtt_transport_open(char* peerip,char* peerport)
{
	/*
	设置单连结  AT+CIPMUX=0
	*/
		char data[128]={"AT+CIPSERVER=0\r\n"};
		char ackData[256]={0};
		int8_t ret;
		int bytes;
		// HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size)
		ret = dma_send_AT_commend_fun((uint8_t*)data ,strlen(data));
		vTaskDelay(pdMS_TO_TICKS(5000));

		if(ret==0)
		{
					
				bytes = esp8266_dma_receive_ack_data((uint8_t*)ackData,sizeof(ackData),1000);
				if(bytes>0)
				{
					ackData[bytes]=0;
					printf("ack_data:%s\r\n",ackData);
				}
				
		}else{
				printf(" single connection config failure.\r\n");
				return -1;
		}
		
#if 0
		
		
此时 ESP8266 处于Station 模式（连接路由器），根本没有开启 TCP 服务器（CIPSERVER 默认是关闭的），所以执行 “关闭服务器” 指令时，模块返回ERROR—— 这是模块的正常反馈，不是故障。
后续执行AT+CIPMUX=0返回OK，说明单连接模式设置成功，不影响后续 MQTT 连接流程。
		
#endif
		
		/*
	关闭已开启的服务器   AT+CIPSERVER=0
	*/
		memset(data,0,sizeof(data));  // 清空
		strcpy(data,"AT+CIPMUX=0\r\n");
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
				printf("close server failure.\r\n");
				return -1;
		}
		
		
					/*
	建立TCP连接
	*/
		memset(data,0,sizeof(data));  // 清空
		//strcpy(data,"AT+CIPSTART=TCP,\r\n");
		sprintf(data,"AT+CIPSTART=\"TCP\",\"%s\",%s\r\n",peerip,peerport);
		ret = dma_send_AT_commend_fun((uint8_t*)data ,strlen(data));
		vTaskDelay(pdMS_TO_TICKS(10000));

		if(ret==0)
		{
				bytes = esp8266_dma_receive_ack_data((uint8_t*)ackData,sizeof(ackData),1000);
				
				if(bytes>0)
				{
					ackData[bytes]=0;
					printf("ack_data:%s\r\n",ackData);
					printf("create TCP connection success.\r\n");
				}
				
		}else{
				printf("create TCP connection failure.\r\n");
				return -1;
		}
		vTaskDelay(pdMS_TO_TICKS(2000));
			/*
	设置透传模式
	*/
		memset(data,0,sizeof(data));  // 清空
		strcpy(data,"AT+CIPMODE=1\r\n");
		ret = dma_send_AT_commend_fun((uint8_t*)data ,strlen(data));
		vTaskDelay(pdMS_TO_TICKS(1000));

		if(ret==0)
		{
					
				bytes = esp8266_dma_receive_ack_data((uint8_t*)ackData,sizeof(ackData),3000);
				
				if(bytes>0)
				{
					ackData[bytes]=0;
					printf("ack_data:%s\r\n",ackData);
					printf("transparent_transmission mode success.\r\n");
				}
				
		}else{
				printf("transparent_transmission mode failure.\r\n");
				return -1;
		}
		
	
		
					/*
	进入 wifi 透传
	*/
		memset(data,0,sizeof(data));  // 清空
		//strcpy(data,"AT+CIPSTART=TCP,\r\n");
		strcpy(data,"AT+CIPSEND\r\n");
		ret = dma_send_AT_commend_fun((uint8_t*)data ,strlen(data));
		vTaskDelay(pdMS_TO_TICKS(1000));

		bytes = esp8266_dma_receive_ack_data((uint8_t*)ackData,sizeof(ackData),5000);
		
		if(bytes>0)
		{
				ackData[bytes]=0;
		}
		
		if(strstr(ackData, ">") == NULL) { // 检查透传提示符
				printf("enter transparent mode failure!\r\n");
				return -1;
		}
		printf("enter transparent mode success.\r\n");
		return 0;
}

// mqtt connect
int mqtt_connect(char* peerip,char* peerport)
{
	
	int ret = mqtt_transport_open(peerip,peerport);
	if(ret<0)
	{
				printf("mqtt_transport_open failure.\r\n");
				return -1;
	}
	printf("mqtt_transport_open success.\r\n");
	
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	
	unsigned char buf[200]={0};
	int buflen = sizeof(buf);
	
	data.clientID.cstring="esp8266-123";	
	data.keepAliveInterval = 60;
	data.cleansession = 1;
	
	
	#if 0
	data.username.cstring = NULL;
	data.password.cstring = NULL;
	#else
	data.username.cstring = "xjrdqaaw:xjrdqaaw";  // 你的 MQTT 用户名
	data.password.cstring = "tqGudw_cSIJj5mSMrAdL7XwwdIiRv1D2";  // 你的 MQTT 密码
	#endif
	
	data.MQTTVersion = 3;
	
	
	int len = MQTTSerialize_connect(buf,buflen,&data);
	ret = send_mqtt_protocol(buf,len);               // 发送连接 Broker 协议 
	if(ret<0){
		printf("send_mqtt_protocol failure.\r\n");
		return -1;
	}
	printf("send_mqtt_protocol success.\r\n");
	if(MQTTPacket_read(buf,buflen,transport_getdata)!=CONNACK)
	{
		printf("connct ack failure.\r\n");
		return -1;
	}
	printf("connct ack success.\r\n");
	printf("mqtt_connect broker success.\r\n");
	return 0;
}


// 订阅主题
int mqtt_subscribe(char* subscribe)
{
	
	MQTTString topicString = MQTTString_initializer;
	int req_qos = 0;
	int msgid = 1;
	topicString.cstring = subscribe;
	unsigned char buf[200];
	int buflen = sizeof(buf);
	
	int len  = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);
	
	if(len<=0)
	{
		printf("MQTTSerialize_subscribe failure.\r\n");
		return -1;
	}
	printf("MQTTSerialize_subscribe succcess.\r\n");
	int ret = send_mqtt_protocol(buf,len);
	if(ret<0){
		return -1;
	}
	
	unsigned char buffer[256]={0};
	
	if(MQTTPacket_read(buffer,sizeof(buffer),transport_getdata)!=SUBACK)
	{
		printf("MQTTSerialize_subscribe ack failure.\r\n");
		return -1;
	}
	
	printf("MQTTSerialize_subscribe topic ack success.\r\n");
	return 0;
}

int transport_getdata_block_mode(unsigned char * pvBuffer,int size)
{
	return esp8266_dma_receive_ack_data((uint8_t*)pvBuffer,size,portMAX_DELAY);
}

int mqtt_receive_subscribe_data(unsigned char* buffer,int size)
{
	unsigned char buf[512];
	
	if(MQTTPacket_read(buf, sizeof(buf), transport_getdata_block_mode) == PUBLISH)
		{
			unsigned char dup;
			int qos;
			unsigned char retained;
			unsigned short msgid;
			int payloadlen_in;
			unsigned char* payload_in;
			int rc;
			MQTTString receivedTopic;

			rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
					&payload_in, &payloadlen_in, buf, sizeof(buf));
			if(rc)
			{
				
			}
			memcpy(buffer,payload_in,payloadlen_in);
			printf("mqtt_receive_subscribe_data success.\r\n");
			buffer[payloadlen_in]=0;
			return payloadlen_in;
		}else{
				printf("mqtt_receive_subscribe_data failure.\r\n");
			return -1;
		}

}


void mqtt_response_protocol_packet(uint8_t * payload,uint16_t size)
{

	unsigned char buf[200];
	int buflen = sizeof(buf);
	MQTTString topicString = MQTTString_initializer;
	topicString.cstring = "/iot/smartdevice/doorlock/ack";   // 发布
	int len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, (unsigned char *)payload, size);
	int ret = send_mqtt_protocol(buf,len);
	
	if(ret<0)
	{
		printf("send_mqtt_protocol failure.reson :%d\r\n",ret);
		
	}
	if(ret!=HAL_OK)
	{
		printf("HAL_UART_transmit failure.\r\n");
		
	}

}

#include "cJSON.h"
void mqtt_response_json_str_protocol_packet(uint8_t *original_payload, uint16_t original_size,uint8_t deviceNum, int ack_code, const char *msg)
{
    // 1. 构建JSON应答数据
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        printf("cJSON create object failure!\r\n");
        return;
    }
		uint16_t data_select = ((original_payload[11] << 8)&0xff00) | ((original_payload[12])&0xff);     // 根据包长度  选择 返回数据   doorlock / sensor
		
		printf("data_select : %d",data_select);
		uint32_t deviceAddrnum = 0;
		char* deviceAddr=NULL;
		for(int i=0;i<4;i++)
		{
			deviceAddrnum |= original_payload[i+2] << (3-i)*8;
		}
		if(deviceAddrnum==0)
		{
			deviceAddr = "doorlock";
		}else if(deviceAddrnum==1)
		{
			deviceAddr = "electrical";
		}else if(deviceAddrnum==2)
		{
			deviceAddr = "sensor";
		}else if(deviceAddrnum==0xffffffff)
		{
			deviceAddr = "sniff";
		}else{
			printf("deviceAddrnum select failure.\r\n");
		}
		
		uint8_t operationnum = original_payload[10];  // operatioin
		char* operationCode=NULL;
		
		if(operationnum==0)
		{
			operationCode = "open";
		}else if(operationnum==1)
		{
			operationCode = "close";
		}else if(operationnum==2)
		{
			operationCode = "query";
		}else{
			printf("operationnum select failure.\r\n");
		}
		
		
		
		
		uint16_t status = 0;       // status
		
		if(data_select==1)
		{
			status = original_payload[13];
		}else if(data_select==2){
			status = ((original_payload[13] << 8)&0xff00) | ((original_payload[14])&0xff);
		}else if(data_select==0){
			status = 0;                      // 无状态
		}else{
			printf("data_select failure.\r\n");
		}
		
    // 添加JSON字段（根据你的协议需求自定义）
    cJSON_AddStringToObject(root, "deviceAddr", deviceAddr);       // 应答  数字
    cJSON_AddStringToObject(root, "operationCode", operationCode);             // 应答字符串
		cJSON_AddNumberToObject(root, "deviceNum",deviceNum);

		
		cJSON *devicedata = cJSON_CreateObject();
    if (root == NULL) {
        printf("cJSON create object failure!\r\n");
        return;
    }
		
    cJSON_AddStringToObject(devicedata, "errmsg", msg); 
		if(strcmp(deviceAddr, "sensor") == 0 && deviceNum==1)
		{
			cJSON_AddStringToObject(devicedata, "status",temp_humi_result);
			printf("status *** :%s",temp_humi_result);
		}else{
			cJSON_AddNumberToObject(devicedata, "status",status);
		}
		
		cJSON_AddItemToObject(root, "devicedata", devicedata);
		
    // 2. 生成紧凑的JSON字符串（节省带宽和内存）
    char *json_payload = cJSON_PrintUnformatted(root);
    if (json_payload == NULL) {
        printf("cJSON print json failure!\r\n");
        cJSON_Delete(root);
        return;
    }
	
    // 3. 准备MQTT发布参数
    unsigned char buf[200];
    int buflen = sizeof(buf);
    MQTTString topicString = MQTTString_initializer;
    topicString.cstring = "/iot/smartdevice/doorlock/ack";   // 发布

    // 获取JSON字符串长度
    uint16_t json_len = strlen(json_payload);
    if (json_len >= buflen) { // 检查缓冲区是否足够（避免溢出）
        printf("JSON payload too long! len:%d > buf size:%d\r\n", json_len, buflen);
        cJSON_free(json_payload);
        cJSON_Delete(root);
        return;
    }
		
    // 4. 序列化MQTT发布包并发送
    int len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, (unsigned char *)json_payload, json_len);
    int ret = send_mqtt_protocol(buf, len);

    // 5. 错误处理
    if (ret < 0) {
        printf("send_mqtt_protocol failure. reason:%d\r\n", ret);
    }
    if (ret != HAL_OK) {
        printf("HAL_UART_transmit failure.\r\n");
    }
		
    // 6. 释放cJSON内存（关键！避免内存泄漏）
    cJSON_free(json_payload);
    cJSON_Delete(root);
}



