#include "head.h"
#include "serial.h"

using namespace std;

Serialport::Serialport(const char *dev, int boundrate)
{
    this->Dev = dev;
    this->Boundrate = boundrate;
    this->fd = 0;
    if (this->Dev == NULL)
    {
        printf("Device pointer NULL\n");
    }
    if ((this->fd = serialOpen(this->Dev, this->Boundrate)) < 0)
    {
        printf("Fail to open %s\n", dev);
        perror("Open device failed:");
    }
}

int Serialport::readBuffer(char *buf, unsigned int size)
{
    int nread = 0;
    uint16_t count = 0;

    while (nread <= 0)
    {
        try
        {
            nread = read(fd, buf, size);
            if (count++ > 1000)
            {
                cout << "read timeout\n";
                return false;
            }
        }
        catch (exception e)
        {
            cout << "Serail read failed:" << e.what() << endl; //捕获异常，然后程序结束
            return false;
        }
    }
    return nread;
}

char Serialport::readChar()
{
    return serialGetchar(this->fd);
}

int Serialport::sendBuffer(const char *buf, unsigned int len)
{
    if (buf == NULL)
    {
        printf("Buffer pointer NULL\n");
        return -1;
    }

    unsigned int i = 0;
    while (len - i)
    {
        serialPutchar(this->fd, buf[i++]);
    }

    return i;
}

void Serialport::sendChar(char c)
{
    serialPutchar(this->fd, c);
}

bool Serialport::closeSerial()
{
    if (this->fd)
    {
        serialClose(this->fd);
        return true;
    }
    else
    {
        cerr << "serial not exits" << endl;
        return false;
    }
}

Serialport::~Serialport() //析构函数 关闭串口
{
    closeSerial();
    cout << "close Serial" << endl;
}
