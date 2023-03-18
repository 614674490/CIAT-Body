#include "head.h"
#include "emg.h"

using namespace std;

emgCollector::emgCollector()
{
    while (!(Init_EmgHardWork()))
        usleep(MS_DELAY * 5);
}

void emgCollector::Init_ADS1115_Register()
{
    ADS1115_register.OS = ADS1115_REG_CONFIG_OS_NULL;
    ADS1115_register.MUX = ADS1115_REG_CONFIG_MUX_SINGLE_0;
    ADS1115_register.PGA = ADS1115_REG_CONFIG_PGA_4;
    ADS1115_register.MODE = ADS1115_REG_CONFIG_MODE_CONTIN;
    ADS1115_register.DataRate = ADS1115_REG_CONFIG_DR_128;
    ADS1115_register.COMP_MODE = ADS1115_REG_CONFIG_COMP_MODE_TRADITIONAL;
    ADS1115_register.COMP_POL = ADS1115_REG_CONFIG_COMP_POL_LOW;
    ADS1115_register.COMP_LAT = ADS1115_REG_CONFIG_COMP_LAT_NONLATCH;
    ADS1115_register.COMP_QUE = ADS1115_REG_CONFIG_COMP_QUE_DIS;
}

bool emgCollector::Switch_Channel(char channel)
{
    switch (channel)
    {
    case 0:
        ADS1115_register.MUX = ADS1115_REG_CONFIG_MUX_SINGLE_0;
        break;
    case 1:
        ADS1115_register.MUX = ADS1115_REG_CONFIG_MUX_SINGLE_1;
        break;
    case 2:
        ADS1115_register.MUX = ADS1115_REG_CONFIG_MUX_SINGLE_2;
        break;
    case 3:
        ADS1115_register.MUX = ADS1115_REG_CONFIG_MUX_SINGLE_3;
        break;
    default:
        ADS1115_register.MUX = ADS1115_REG_CONFIG_MUX_SINGLE_0;
        break;
    }
    return Config_ADS1115_Register();
}

bool emgCollector::Config_ADS1115_Register()
{
    uint8_t writeBuf[3];
    int result = 0;

    //写入配置寄存器
    writeBuf[0] = ADS1115_REG_POINTER_CONFIG;                                                                                                                 // Pointer Byte: 01->config registe;00->conversion registe
    writeBuf[1] = ADS1115_register.OS | ADS1115_register.MUX | ADS1115_register.PGA | ADS1115_register.MODE;                                                  // config register Data high OS=1;MUX=AIN0-GND;PGA=4.096V;MODE=Continuous-conversion mode
    writeBuf[2] = ADS1115_register.DataRate | ADS1115_register.COMP_MODE | ADS1115_register.COMP_POL | ADS1115_register.COMP_LAT | ADS1115_register.COMP_QUE; // config register Data low DR=128SPS;default
    try
    {
        result = write(this->I2CFile, writeBuf, 3); //返回写入文件的字节数
    }
    catch (exception e)
    {
        cout << "ADS1115 write config reg failed:" << e.what() << endl; //捕获异常，然后程序结束
        return false;
    }

    if (result != 3)
    {
        printf("ADS1115 write config registe: result = %d, errno=%d\n", result, errno);
        return false;
    }
    // point conversion registe
    writeBuf[0] = ADS1115_REG_POINTER_CONVERT; // selects ADCS for conversion
    try
    {
        result = write(this->I2CFile, writeBuf, 1);
    }
    catch (exception e)
    {
        cout << "ADS1115 write convert reg failed:" << e.what() << endl; //捕获异常，然后程序结束
        return false;
    }

    if (result != 1)
    {
        printf("point conversion registe: result = %d, errno=%d\n", result, errno);
        return false;
    }
    usleep(EMG_SAMPLE_DELAY);
    return true;
}

bool emgCollector::Init_EmgHardWork()
{
    try
    {
        this->I2CFile = open("/dev/i2c-1", O_RDWR); //获取文件描述符
    }
    catch (exception e)
    {
        cout << "I2C open failed" << e.what() << endl; //捕获异常，然后程序结束
        return false;
    }

    if (this->I2CFile < 0)
    {
        printf("Open I2C failed\n");
        return false;
    }

    if (ioctl(this->I2CFile, I2C_TENBIT, 0) < 0)
    {
        printf("Couldn't set I2C device 7 bit\n");
        return false;
    }

    if (ioctl(this->I2CFile, I2C_SLAVE, ADS_ADDRESS) < 0)
    {
        printf("Couldn't find I2C device\n");
        return false;
    }

    Init_ADS1115_Register();
    if (Config_ADS1115_Register())
        printf("ADC Device Config Successed\n");
    else
    {
        printf("ADC Device Config Failed\n");
        return false;
    }

    return true;
}

//建议采用经过稳压后的直流电压3.3V-5V，切勿使用插排的交流电压！
bool emgCollector::ReadOneEmgDate(char channel)
{
    int nread = 0;
    uint16_t count = 0;
    while (nread <= 0)
    {
        try
        {
            nread = read(this->I2CFile, buff, EMGBUFFLEN); //返回读取到的字节数(2 byte)
            if (count++ > 1000)
            {
                cout << "read timeout\n";
                return false;
            }
        }
        catch (exception e)
        {
            cout << "Emg read failed" << e.what() << endl; //捕获异常，然后程序结束
            return false;
        }
    }
    raw_val[channel] = buff[0] << 8 | buff[1];                       //高位在前 低位在后
    if ((raw_val[channel] == 0x7FFF) | (raw_val[channel] == 0X8000)) //是否超量程了
    {
        printf("emg signal over PGA\r\n");
    }
    emg_val[channel] = (int)(((float)raw_val[channel] * ADS1115_PGA_4_RATE) * 1000);

    // emg_kalman_val = kalman_filter(emg_val);
    return true;
}

bool emgCollector::getEmgDate()
{
    char channel = 0;
    for (channel = 0; channel < CHANNELNUM; channel++)
    {
        if (Switch_Channel(channel))
        {
            if (!ReadOneEmgDate(channel))
                return false;
        }
        else
            return false;
    }
    return true;
}

bool emgCollector::closeI2C()
{
    if (this->I2CFile)
    {
        close(this->I2CFile);
        return true;
    }
    else
    {
        cerr << "I2CFile not exits" << endl;
        return false;
    }
}

emgCollector::~emgCollector()
{
    closeI2C();
    cout << "close sEMG" << endl;
}

float kalman_filter(float ADC_Value)
{
    float LastData;
    float NowData;
    float kalman_adc;
    static float kalman_adc_old = 0;
    static float P1;
    static float Q = 0.0003;
    static float R = 5;
    static float Kg = 0;
    static float P = 1;
    NowData = ADC_Value;
    LastData = kalman_adc_old;
    P = P1 + Q;
    Kg = P / (P + R);
    kalman_adc = LastData + Kg * (NowData - kalman_adc_old);
    P1 = (1 - Kg) * P;
    P = P1;
    kalman_adc_old = kalman_adc;
    return kalman_adc;
}
