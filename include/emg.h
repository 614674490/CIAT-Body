#ifndef EMG_H_INCLUDED
#define EMG_H_INCLUDED

#include <fcntl.h>
#include <inttypes.h>      // uint8_t, etc
#include <linux/i2c-dev.h> // I2C bus definitions
#include <sys/ioctl.h>
#include <cinttypes>
#include <unistd.h>
#include <sys/ioctl.h>

#define ADS_ADDRESS 0x48

//以下两个预处理作为配置寄存器预处理要更改其配在这里更改
//也可以添加新的预处理对不同通道的采集或者选取不同的采样速率等
#define CONFIG_REG_H ADS1115_REG_CONFIG_OS_NULL |          \
                         ADS1115_REG_CONFIG_MUX_SINGLE_0 | \
                         ADS1115_REG_CONFIG_PGA_4 |        \
                         ADS1115_REG_CONFIG_MODE_CONTIN
#define CONFIG_REG_L ADS1115_REG_CONFIG_DR_128 |                    \
                         ADS1115_REG_CONFIG_COMP_MODE_TRADITIONAL | \
                         ADS1115_REG_CONFIG_COMP_POL_LOW |          \
                         ADS1115_REG_CONFIG_COMP_LAT_NONLATCH |     \
                         ADS1115_REG_CONFIG_COMP_QUE_DIS

/***************/
//配置寄存器说明

// config register
/*CRH[15:8](R/W)
   BIT      15      14      13      12      11      10      9       8
   NAME     OS      MUX2    MUX1    MUX0    PGA2    PGA1    PGA0    MODE
CRL[7:0] (R/W)
   BIT      7       6       5       4       3       2       1       0
   NAME    DR0     DR1     DR0   COM_MODE COM_POL COM_LAT COM_QUE1 COM_QUE0
   -----------------------------------------------------------------------------------
 * 15    | OS             |  运行状态会单词转换开始
 *       |                | 写时:
 *       |                | 0   : 无效
 *       |                | 1   : 开始单次转换处于掉电状态时
 *       |                | 读时:
 *       |                | 0   : 正在转换
 *       |                | 1   : 未执行转换
 * -----------------------------------------------------------------------------------
 * 14:12 | MUX [2:0]      | 输入复用多路配置
 *       |                | 000 : AINP = AIN0 and AINN = AIN1 (default)
 *       |                | 001 : AINP = AIN0 and AINN = AIN3
 *       |                | 010 : AINP = AIN1 and AINN = AIN3
 *       |                | 011 : AINP = AIN2 and AINN = AIN3
 *       |                | 100 : AINP = AIN0 and AINN = GND
 *       |                | 101 : AINP = AIN1 and AINN = GND
 *       |                | 110 : AINP = AIN2 and AINN = GND
 *       |                | 111 : AINP = AIN3 and AINN = GND
 * -----------------------------------------------------------------------------------
 * 11:9  | PGA [2:0]      | 可编程增益放大器配置(FSR  full scale range)
 *       |                | 000 : FSR = В±6.144 V
 *       |                | 001 : FSR = В±4.096 V
 *       |                | 010 : FSR = В±2.048 V (默认)
 *       |                | 011 : FSR = В±1.024 V
 *       |                | 100 : FSR = В±0.512 V
 *       |                | 101 : FSR = В±0.256 V
 *       |                | 110 : FSR = В±0.256 V
 *       |                | 111 : FSR = В±0.256 V
 * -----------------------------------------------------------------------------------
 * 8     | MODE           | 工作模式
 *       |                | 0   : 连续转换
 *       |                | 1   : 单词转换
 * -----------------------------------------------------------------------------------
 * 7:5   | DR [2:0]       | 采样频率
 *       |                | 000 : 8 SPS
 *       |                | 001 : 16 SPS
 *       |                | 010 : 32 SPS
 *       |                | 011 : 64 SPS
 *       |                | 100 : 128 SPS (默认)
 *       |                | 101 : 250 SPS
 *       |                | 110 : 475 SPS
 *       |                | 111 : 860 SPS
 * -----------------------------------------------------------------------------------
 * 4     | COMP_MODE      | 比较器模式
 *       |                | 0   : 传统比较器 (default)
 *       |                | 1   : 窗口比较器
 * -----------------------------------------------------------------------------------
 * 3     | COMP_POL       | Comparator polarity
 *       |                | 0   : 低电平有效 (default)
 *       |                | 1   : 高电平有效
 * -----------------------------------------------------------------------------------
 * 2     | COMP_LAT       | Latching comparator
 *       |                | 0   : 非锁存比较器. (default)
 *       |                | 1   : 锁存比较器.
 * -----------------------------------------------------------------------------------
 * 1:0   | COMP_QUE [1:0] | Comparator queue and disable
 *       |                | 00  : Assert after one conversion
 *       |                | 01  : Assert after two conversions
 *       |                | 10  : Assert after four conversions
 *       |                | 11  : 禁用比较器并将ALERT/RDY设置为高阻抗 (default)
 * -----------------------------------------------------------------------------------
*/

//地址指针寄存器
#define ADS1115_REG_POINTER_CONVERT (0x00)
#define ADS1115_REG_POINTER_CONFIG (0x01)
#define ADS1115_REG_POINTER_LOWTHRESH (0x02)
#define ADS1115_REG_POINTER_HITHRESH (0x03)

// 单次转换开始
#define ADS1115_REG_CONFIG_OS_START (0x1U << 7) //设备单词转换开启 高字节的最高位
#define ADS1115_REG_CONFIG_OS_NULL (0x0U << 7)

//输入引脚选择和输入方式选择
#define ADS1115_REG_CONFIG_MUX_Diff_01 (0x0U << 4)  // 差分输入0引脚和1引脚
#define ADS1115_REG_CONFIG_MUX_Diff_03 (0x1U << 4)  // 差分输入0引脚和3引脚
#define ADS1115_REG_CONFIG_MUX_Diff_13 (0x2U << 4)  // 差分输入1引脚和3引脚
#define ADS1115_REG_CONFIG_MUX_Diff_23 (0x3U << 4)  // 差分输入2引脚和3引脚
#define ADS1115_REG_CONFIG_MUX_SINGLE_0 (0x4U << 4) //单端输入 0
#define ADS1115_REG_CONFIG_MUX_SINGLE_1 (0x5U << 4) //单端输入 1
#define ADS1115_REG_CONFIG_MUX_SINGLE_2 (0x6U << 4) //单端输入 2
#define ADS1115_REG_CONFIG_MUX_SINGLE_3 (0x7U << 4) //单端输入 3

//量程选择
#define ADS1115_REG_CONFIG_PGA_6 (0x0U << 1)  // +/- 6.1144
#define ADS1115_REG_CONFIG_PGA_4 (0x1U << 1)  // +/- 4.096
#define ADS1115_REG_CONFIG_PGA_2 (0x2U << 1)  // +/- 2.048
#define ADS1115_REG_CONFIG_PGA_1 (0x3U << 1)  // +/- 1.024
#define ADS1115_REG_CONFIG_PGA_05 (0x4U << 1) // +/- 0.512
#define ADS1115_REG_CONFIG_PGA_02 (0x5U << 1) // +/- 0.256

//运行方式
#define ADS1115_REG_CONFIG_MODE_SINGLE (0x1U << 0) //  单次
#define ADS1115_REG_CONFIG_MODE_CONTIN (0x0U << 0) //连续转换

//转换速率
#define ADS1115_REG_CONFIG_DR_8 (0x0U << 5)
#define ADS1115_REG_CONFIG_DR_16 (0x1U << 5)
#define ADS1115_REG_CONFIG_DR_32 (0x2U << 5)
#define ADS1115_REG_CONFIG_DR_64 (0x3U << 5)
#define ADS1115_REG_CONFIG_DR_128 (0x4U << 5)
#define ADS1115_REG_CONFIG_DR_250 (0x5U << 5)
#define ADS1115_REG_CONFIG_DR_475 (0x6U << 5)
#define ADS1115_REG_CONFIG_DR_860 (0x7U << 5)

//比较器模式
#define ADS1115_REG_CONFIG_COMP_MODE_TRADITIONAL (0x0U << 4) //默认
#define ADS1115_REG_CONFIG_COMP_MODE_WINDOW (0x1U << 4)

#define ADS1115_REG_CONFIG_COMP_POL_LOW (0x0U << 3) //默认
#define ADS1115_REG_CONFIG_COMP_POL_HIG (0x1U << 3)

#define ADS1115_REG_CONFIG_COMP_LAT_NONLATCH (0x0U << 2)
#define ADS1115_REG_CONFIG_COMP_LAT_LATCH (0x1U << 2)

#define ADS1115_REG_CONFIG_COMP_QUE_ONE (0x0U << 0)
#define ADS1115_REG_CONFIG_COMP_QUE_TWO (0x1U << 0)
#define ADS1115_REG_CONFIG_COMP_QUE_THR (0x2U << 0)
#define ADS1115_REG_CONFIG_COMP_QUE_DIS (0x3U << 0)

#define ADS1115_PGA_6_RATE 187.5 / 1000000.0
#define ADS1115_PGA_4_RATE 125 / 1000000.0
#define ADS1115_PGA_2_RATE 62.5 / 1000000.0
#define ADS1115_PGA_1_RATE 31.25 / 1000000.0
#define ADS1115_PGA_05_RATE 15.625 / 1000000.0
#define ADS1115_PGA_02_RATE 7.8125 / 1000000.0

#define EMGBUFFLEN 2
#define CHANNELNUM 2

#define EmgAbove 0
#define EmgBelow 1

typedef struct
{
    uint16_t OS;
    uint16_t MUX;
    uint16_t PGA;
    uint16_t MODE;
    uint16_t DataRate;
    uint16_t COMP_MODE;
    uint16_t COMP_POL;
    uint16_t COMP_LAT;
    uint16_t COMP_QUE;
} ADS1115_Register;

class emgCollector
{
public:
    char buff[EMGBUFFLEN] = {0};

    ADS1115_Register ADS1115_register;
    uint16_t raw_val[CHANNELNUM] = {0};
    uint16_t emg_val[CHANNELNUM] = {0};

    emgCollector();

    void Init_ADS1115_Register();
    bool Switch_Channel(char channel);
    bool Config_ADS1115_Register();
    bool Init_EmgHardWork();
    bool ReadOneEmgDate(char channel);
    bool getEmgDate();
    bool closeI2C();
    ~emgCollector();

private:
    int I2CFile = -1;
};
float kalman_filter(float ADC_Value);
#endif
