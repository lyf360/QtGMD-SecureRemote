#ifndef __LINUX_UART__
#define __LINUX_UART__

#include <iostream>
#include <cstdint>   // 提供 uint32_t, uint16_t 等类型定义
#include <string>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>   // 提供 fprintf 等函数
#include <stdlib.h>  // 提供 exit, EXIT_FAILURE 等
#include <errno.h>   // 提供 errno 定义
#include <string.h>  // 提供 strerror 函数
#include <termios.h>
#include <stdint.h>
using namespace std;

class LinuxUart
{
public:
    LinuxUart(const string &devicename,uint32_t BaudRate=115200);

    ~LinuxUart();

    bool uartInit(uint32_t BaudRate) const;

    ssize_t readuartdata(char* buf,uint16_t nbytes);
    ssize_t writeuartdata(const char* buf,uint16_t nbytes);
    bool ReceiveFixedPacket(char* buf,uint16_t size);
private:
    int fd;
    uint32_t BaudRate;
};

#endif