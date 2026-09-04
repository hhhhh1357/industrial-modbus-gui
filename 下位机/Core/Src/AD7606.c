#include "AD7606.h"
#include "spi.h"
#include "tim.h"
#include "24cxx.h"
extern sensor_msg   CO2_GAS_msg;

void AD7606_Init(void)
{
	AD_STBY=1;
	AD_CS=1;  //拉高，关闭结束输出
	AD_RESET=0;  
	AD_RESET=1;  //上电后需要复位一下AD7606,高电平脉冲宽度>50ns
	for(int i=0;i<20;i++)
	{
		__NOP();//1000/72 ns=13.888888ns
	}
	AD_RESET=0;  
		
}

void GET_ADCValue(float *ad7606Val,uint8_t ad7606Chl)
{
	uint16_t Val[2]={0};
  AD_CS=0;
	Val[0]=SPI1_ReadWriteByte(0xffff);
	Val[1]=SPI1_ReadWriteByte(0xffff);
  AD_CS=1;
	ad7606Val[0]=AD7606_convValue(Val[0]);
	ad7606Val[1]=AD7606_convValue(Val[1]);
}
void AD7606_start(void)
{
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);  //开始转换，CONVSTA\B(PE13),脉冲频率20Khz.
}
void AD7606_stop(void)
{
	HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_3);
  AD_CS=1;//拉高，关闭结束输出
}

float AD7606_convValue(uint16_t bin)
{
    int _val;
    float adcValue;
    _val = bin&0x8000 ? (-((~bin+1)&0x7fff)) : bin;//输出编码为二进制补码,转换为原码
    adcValue = 5.0*_val/32768.0;
    return adcValue;
}
//SPI1 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
uint16_t  SPI1_ReadWriteByte(uint16_t TxData)
{		 			 
	while((SPI1->SR&1<<1)==0);  //等待发送区空 
	SPI1->DR = TxData; 	  		//发送一个byte  
	while((SPI1->SR&1<<0)==0);		//等待接收完一个byte  
	return (uint16_t)(READ_REG(SPI1->DR)); //返回收到的数据

}

