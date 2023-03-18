/*
 * @Author: Ken Kaneki
 * @Date: 2022-03-08 17:16:49
 * @LastEditTime: 2022-03-10 12:15:34
 * @Description: README
 */
#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED

#include <wiringPi.h>
#include <wiringSerial.h>
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

/*UART-GPIO-BCM编码
GPIO14 = TXD0 -> ttyAMA0
GPIO0  = TXD2 -> ttyAMA1
GPIO4  = TXD3 -> ttyAMA2
GPIO8  = TXD4 -> ttyAMA3
GPIO12 = TXD5 -> ttyAMA4

GPIO15 = RXD0 -> ttyAMA0
GPIO1  = RXD2 -> ttyAMA1
GPIO5  = RXD3 -> ttyAMA2
GPIO9  = RXD4 -> ttyAMA3
GPIO13 = RXD5 -> ttyAMA4
*/

class Serialport
{
public:
    int fd;          //文件描述符
    Serialport(const char *dev, int boundrate);
    ~Serialport();

    bool closeSerial();
    int readBuffer(char *buf, unsigned int size); //读取串口数据
    char readChar();
    int sendBuffer(const char *buf, unsigned int len); //串口发送数据
    void sendChar(char c);

private:

    const char *Dev; //串口名
    int Boundrate;   //波特率
};

#endif
