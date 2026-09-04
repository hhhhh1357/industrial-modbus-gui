#ifndef _MYIIC_H
#define _MYIIC_H
#include "main.h"
//////////////////////////////////////////////////////////////////////////////////	 
				  
////////////////////////////////////////////////////////////////////////////////// 	
//IO方向设置
//#define SDA_IN()  {GPIOB->CRH&=~(0XF<<(5*4));GPIOB->CRH|=(8<<(5*4));}	//PB13输入模式
//#define SDA_OUT() {GPIOB->CRH&=~(0XF<<(5*4));GPIOB->CRH|=(3<<(5*4));} //PB13输出模式
void SDA_IN(void) ;
void SDA_OUT(void);
//IO操作
#define IIC_SCL   PBout(12) //SCL
#define IIC_SDA   PBout(13) //SDA
#define READ_SDA  PBin(13)  //输入SDA

//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	 
#endif

