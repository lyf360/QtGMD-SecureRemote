#include "sniff_process_fun.h"


// sniff

extern uint16_t modbus_crc_fast(const uint8_t *data, uint16_t length);




void open_sniff(sniff_packet* package_data)
{
	
	package_data->addressCode = 0x00000001;
	package_data->functionCode = 0x0001;
	package_data->deviceNum = 0x01;

}

void close_sniff(sniff_packet* package_data)
{
	package_data->addressCode = 0x00000002;
	package_data->functionCode = 0x0002;
	package_data->deviceNum = 0x02;

}

void query_sniff(sniff_packet* package_data)
{
	package_data->addressCode = 0x00000003;
	package_data->functionCode = 0x0003;
	package_data->deviceNum = 0x03;

}

typedef void (*sniff_device)(sniff_packet* package_data);

sniff_device sniff_operation_fun[]=
{
	open_sniff,
	close_sniff,
	query_sniff

};


void ack_sniff_protocol_packet(sniff_packet s_protocol_data_t)
{
	
	uint8_t protocol_buf[sizeof(pro_packet) + 10]; // 栈上数组，
    
    // 将数组指针转换为结构体指针（确保对齐，此处数组天然对齐）
  pro_packet* ack_protocol = (pro_packet*)protocol_buf;
	

	ack_protocol->packetHead=__REV16(0xabcd);
	ack_protocol->addressCode=__REV(0xFFFFFFFF);
	ack_protocol->packetId = ACK_PROTOCOL_ID;
	ack_protocol->packetLength = __REV16(0x000A);
	ack_protocol->array[0] = ACK_PROTOCOL_RIGHT_CODE;
	//pro_packet* p = (pro_packet*)&ack_protocol;
	
	// uint8_t protocol_data[7] = {0x01, 0x02 , 0x03, 0x04, 0x00, 0x01,0x02};
	
	sniff_packet* p = (sniff_packet*)&s_protocol_data_t;
	

	for(int i=1;i<=4;i++)
	{
			//ack_protocol->array[i] = ( __REV(p->addressCode) >> (4-i)*8 ) & 0xff;
			ack_protocol->array[i] = ( p->addressCode >> (4-i)*8 ) & 0xff;
	}
	
	for(int i=1;i<=2;i++)
	{
			//ack_protocol->array[4+i] = ( __REV16(p->functionCode) >> (2-i)*8 ) & 0xff;
			ack_protocol->array[4+i] = ( p->functionCode >> (2-i)*8 ) & 0xff;
	}
	
	ack_protocol->array[7] = p->deviceNum  & 0xff;

	
	uint16_t crc16_check = modbus_crc_fast((uint8_t*)ack_protocol,17);
	ack_protocol->array[8] = crc16_check >> 8 & 0xff;
	ack_protocol->array[9] = crc16_check & 0xff;

	
	// pro_packet* p=(pro_packet*)ack_protocol;
  uint8_t * data = (uint8_t*)ack_protocol;

	response_protocol_packet(data,19);
	
}


uint8_t sniff_processing_function(uint8_t deviceType,uint8_t deviceNum)
{
	uint8_t status=0;
	
	void (*handle)() = sniff_operation_fun[deviceType];
	
	sniff_packet package_data;
	
	handle(&package_data);
	
	ack_sniff_protocol_packet(package_data);
	
	return status;
}


