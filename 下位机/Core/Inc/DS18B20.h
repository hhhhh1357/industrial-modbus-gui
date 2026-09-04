#ifndef _ds18b_H
#define _ds18b_H
#include "main.h" 

//IO方向设置
#define DS18B20_IO_IN()  {GPIOB->CRH&=~(0XF<<(0*4));GPIOB->CRH|=(8<<(0*4));}	//PB8输入模式
#define DS18B20_IO_OUT() {GPIOB->CRH&=~(0XF<<(0*4));GPIOB->CRH|=(3<<(0*4));} 	//PB8输出模式
////IO操作函数											   
#define	DS18B20_DQ_OUT PBout(8) //数据端口	PB8
#define	DS18B20_DQ_IN  PBin(8)  //数据端口	PB8
   	
uint8_t DS18B20_Init(void);			//初始化DS18B20
float DS18B20_Get_Temp(void);	//获取温度
void DS18B20_Start(void);		//开始温度转换
void DS18B20_Write_Byte(uint8_t data);//写入一个字节
uint8_t DS18B20_Read_Byte(void);		//读出一个字节
uint8_t DS18B20_Read_Bit(void);		//读出一个位
uint8_t DS18B20_Check(void);			//检测是否存在DS18B20
void DS18B20_Rst(void);			//复位DS18B20 

#endif
