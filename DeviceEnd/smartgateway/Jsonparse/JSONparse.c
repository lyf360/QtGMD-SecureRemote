#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h> // 用于strtol（十六进制转换）
#include <stdint.h>
#include "data_packet.h"
#include <string.h>


void parse_nested_json(char *json_str,Json_Data_t* datajson)
{
		int a=0;
    //  解析JSON根对象（顶层{}）
    cJSON *root = cJSON_Parse(json_str);
    if (root == NULL) {
        const char *error = cJSON_GetErrorPtr();
        printf("JSON parse : %s\r\n", error ? error : "空数据/语法错误");
        return;
    }

    //  提取顶层字段：deviceType（字符串）
    cJSON *deviceType = cJSON_GetObjectItem(root, "deviceType");
    if (deviceType == NULL || !cJSON_IsString(deviceType) || deviceType->valuestring == NULL) {
        printf("deviceType error.\r\n");
        cJSON_Delete(root); // 必须释放内存
        return;
    }

    cJSON *operationCode = cJSON_GetObjectItem(root, "operationCode");
		if (operationCode == NULL || !cJSON_IsString(operationCode) || operationCode->valuestring == NULL) {
				printf("error：operationCode error.\r\n");
				cJSON_Delete(root);
				return;
		}
		
		cJSON *deviceNum = cJSON_GetObjectItem(root, "deviceNum");
		if (deviceNum == NULL || !cJSON_IsString(deviceNum) || deviceNum->valuestring == NULL) {
				printf("deviceNum error.\r\n");
				cJSON_Delete(root);
				return;
		}
		
		cJSON *JsonVerify = cJSON_GetObjectItem(root, "JsonVerify");
		if (JsonVerify == NULL || !cJSON_IsString(JsonVerify) || JsonVerify->valuestring == NULL) {
				printf("JsonVerify error.\r\n");
				cJSON_Delete(root);
				return;
		}
		if (strncmp(deviceType->valuestring, "doorlock", strlen("doorlock")) == 0) {
				a = 0;
		}else if(strncmp(deviceType->valuestring, "electrical", strlen("electrical")) == 0){
				a = 1;
		}else if(strncmp(deviceType->valuestring, "sensor", strlen("sensor")) == 0){
				a = 2;
		}else if(strncmp(deviceType->valuestring, "sniff", strlen("sniff")) == 0){
				a = 0xffffffff;
		}else{
				printf("deviceType->valuestring match failure--------------------.\r\n");
		}
		
    // 7. 打印解析结果（验证）
    printf("=== 解析结果 ===\r\n");
    printf("deviceType: %d\r\n", a);
    printf("operationCode: %ld\r\n",strtol(operationCode->valuestring, NULL, 16));
		printf("deviceNum: %ld\r\n",strtol(deviceNum->valuestring, NULL, 16));
		printf("JsonVerify: %ld\r\n",strtol(JsonVerify->valuestring, NULL, 16));
	
		datajson->deviceType = (uint32_t)a;
		datajson->operationCode = (uint8_t)strtol(operationCode->valuestring, NULL, 16);
		datajson->deviceNum = (uint8_t)strtol(deviceNum->valuestring, NULL, 16);
		datajson->JsonVerify = (uint16_t)strtol(JsonVerify->valuestring, NULL, 16);
    // 8. 释放cJSON内存（递归释放所有对象，包括嵌套的deviceData）
    cJSON_Delete(root);
}




