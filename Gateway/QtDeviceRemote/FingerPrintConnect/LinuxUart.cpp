#include "LinuxUart.h"

LinuxUart::LinuxUart(const string &devicename,const uint32_t BaudRate):BaudRate(BaudRate)
{
    //
    fd = open(devicename.c_str(), O_RDWR | O_NOCTTY); // 可读可写 | 不控制终端
    if (fd == -1)
    {
        fprintf(stderr, "open %s failure:%s\n", devicename.c_str(), strerror(errno));
        exit(EXIT_FAILURE);
    }

    bool ret = uartInit(BaudRate);
    if(ret)
    {
        cout << "uartInit successful" << endl;
    }else{
        close(fd);
        cout << "uartInit failure" << endl;
        exit(EXIT_FAILURE);
    }
}

LinuxUart::~LinuxUart()
{
    if (fd != -1)
    {
        close(fd);
    }
}

bool LinuxUart::uartInit(uint32_t BaudRate) const
{
    struct termios tio;
    bzero(&tio, sizeof(tio));
    tio.c_cflag |= CLOCAL | CREAD;
    // 清除termios结构体中数据位的掩码
    tio.c_cflag &= ~CSIZE;

    // 设置数据位为8bit
    tio.c_cflag |= CS8;
    // 设置校验位:无奇偶校验位
    tio.c_cflag &= ~PARENB;

    // 设置波特率
    switch (BaudRate)
    {
    case 4800:
        // 设置终端输入波特
        cfsetispeed(&tio, B4800);
        // 设置终端输出波特
        cfsetospeed(&tio, B4800);
        break;
    case 9600:
        cfsetispeed(&tio, B9600);
        cfsetospeed(&tio, B9600);
        break;
    case 57600:
        cfsetispeed(&tio, B57600);
        cfsetospeed(&tio, B57600);
        break;
    case 115200:
        cfsetispeed(&tio, B115200);
        cfsetospeed(&tio, B115200);
        break;
    default:
        fprintf(stderr, "The baudrate:%d is not support\n", BaudRate);
        return false;
    }
    // 设置停止位为1bit
    tio.c_cflag &= ~CSTOPB;
    // 设置等待时间和最小接收字符

    tio.c_cc[VTIME] = 30; //  3s 内无信息  判为超时返回0
    tio.c_cc[VMIN] = 0;

    tcflush(fd, TCIFLUSH); // 刷新缓冲区
    // 设置参数
    int err = tcsetattr(fd, TCSANOW, &tio);  //  TCSANOW 配置的修改立即生效
    if (err)
    {
        fprintf(stderr, "Fail to tcsetattr,err:%s\n", strerror(errno));
        return false;
    }
    return true;
}

ssize_t LinuxUart::readuartdata(char *buf, uint16_t nbytes)
{
    ssize_t ret = read(fd,buf,nbytes);
    if(ret==-1)
    {
        fprintf(stderr,"read data failure%s\n",strerror(errno));
        // fprintf(stderr, "write uart data failure. reason:%s\n", strerror(errno));
        return -1;
    }
    return ret;
}

bool LinuxUart::ReceiveFixedPacket(char* buf,uint16_t size)
{
    uint16_t alreadSize=0;

    uint8_t errorcnt=0;
    while(1)
    {
        ssize_t bytes=read(fd,buf+alreadSize,size-alreadSize);
        if(bytes==-1)
        {
            return false;
        }else if(bytes==0)
        {
            errorcnt++;
            if(errorcnt>=5)
            {
                cout << "ReceiveFixedPacket timeout (read fixed packet) " << endl;
                return false;
            }
        }
        alreadSize+=bytes;
        if(alreadSize==size)
        {
            break;
        }
    }
    return true;
}

ssize_t LinuxUart::writeuartdata(const char *buf, uint16_t nbytes)
{
    ssize_t ret = write(fd,buf,nbytes);
    if(ret==-1)
    {
        fprintf(stderr,"write data failure%s\n",strerror(errno));
        return -1;
    }
    return ret;
}
