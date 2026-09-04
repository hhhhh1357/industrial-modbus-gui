#ifndef _AD7606_H
#define _AD7606_H
#include "main.h"


#define AD_CONVST   PEout(13)
#define AD_RESET    PEout(12)
#define AD_CS       PEout(11)
#define AD_STBY     PEout(14)   //输出低电平，关机AD7606
#define AD_BUSY     PEin(10)
#define AD_FRSTDATA PEin(9)

uint16_t  SPI1_ReadWriteByte(uint16_t TxData);
void AD7606_Init(void);
void AD7606_start(void);
void AD7606_stop(void);
void GET_ADCValue(float *ad7606Val,uint8_t ad7606Chl);//读取电压值，在BUSY下降沿中断中被调用。
float AD7606_convValue(uint16_t bin);//输出编码为二进制补码,转换为10进制
#endif


