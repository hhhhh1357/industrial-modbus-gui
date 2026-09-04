#include "DS18B20.h"
#include "FreeRTOS.h"
#include "task.h"
#include "gpio.h"
/////////////////////////////////////////////
//温度传感器，单总线传输，数据端口 PB8
/////////////////////////////////////////////

//复位脉冲
void DS18B20_Rst(void)	   
{     
	taskENTER_CRITICAL();			//进入临界区
	DS18B20_IO_OUT();   //设置为输出
	DS18B20_DQ_OUT=0;  	//拉低DQ
	delay_us(750);      //拉低750us(至少480us)
	DS18B20_DQ_OUT=1;  	//DQ=1 
	delay_us(15);       //15US
	taskEXIT_CRITICAL();   //退出临界区	
}

//复位后等待DS18B20的回应
//返回1:未检测到DS18B20的存在
//返回0:存在
uint8_t DS18B20_Check(void) 	   
{   
	uint8_t retry=0;
	DS18B20_Rst();
	DS18B20_IO_IN();    //设置为输入
  while(DS18B20_DQ_IN&&retry<200)
	{
		retry++;
		delay_us(1);
	} 
	if(retry>=200)return 1;
	else retry=0;
  while(!DS18B20_DQ_IN&&retry<240)
	{
		retry++;
		delay_us(1);
	}
	if(retry>=240)return 1;	    
	return 0;
}

//从DS18B20读取一个位
//返回值：1/0
uint8_t DS18B20_Read_Bit(void) 
{
	taskENTER_CRITICAL();			//进入临界区
	uint8_t data;
	DS18B20_IO_OUT();       //设置为输出
	DS18B20_DQ_OUT=0; 
	delay_us(2);
	DS18B20_DQ_OUT=1; 
	DS18B20_IO_IN();        //设置为输入
	delay_us(12);
	if(DS18B20_DQ_IN)data=1;
	else data=0;	 
	delay_us(50);
	taskEXIT_CRITICAL();   //退出临界区		
	return data;
}

//从DS18B20读取一个字节
//返回值：读到的数据
uint8_t DS18B20_Read_Byte(void)   
{        
	uint8_t i,j,dat;
	dat=0;
	for (i=1;i<=8;i++) 
	{
		j=DS18B20_Read_Bit();
    dat=(j<<7)|(dat>>1);
  }						    
	return dat;
}

//写一个字节到DS18B20
//dat：要写入的字节
void DS18B20_Write_Byte(uint8_t data)     
 {             
	uint8_t j;
	uint8_t testb;
	DS18B20_IO_OUT();     //设置为输出
	for (j=1;j<=8;j++) 
	{
		testb=data&0x01;
		data=data>>1;
		if(testb)       // 写1
		{
			taskENTER_CRITICAL();			//进入临界区
			DS18B20_DQ_OUT=0;
			delay_us(2);                            
			DS18B20_DQ_OUT=1;
			delay_us(60);        
			taskEXIT_CRITICAL();   //退出临界区							
		}
		else            //写0
		{
			taskENTER_CRITICAL();			//进入临界区
			DS18B20_DQ_OUT=0;
			delay_us(60);             
			DS18B20_DQ_OUT=1;
			delay_us(2);   
			taskEXIT_CRITICAL();   //退出临界区							
		}
	}
}
 
//开始温度转换
void DS18B20_Start(void)
{   						                  
	DS18B20_Check();	 
	DS18B20_Write_Byte(0xcc);// skip rom
	DS18B20_Write_Byte(0x44);// 开启内部AD转换
	delay_us(2);   
}

//初始化DS18B20的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在    	 
uint8_t DS18B20_Init(void)
{
//	GPIO_InitTypeDef GPIO_Initure;
//  __HAL_RCC_GPIOB_CLK_ENABLE();			      //开启GPIOB时钟
//  GPIO_Initure.Pin=GPIO_PIN_8;           	//PB8
//  GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
//  GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
//  GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
//  HAL_GPIO_Init(GPIOB,&GPIO_Initure);     //初始化
 //引脚已经在GPIO.C中初始化
	return  DS18B20_Check();
}

//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-55~125） 
float DS18B20_Get_Temp(void)
{
	uint8_t temp;
	uint8_t TL,TH;
	short tem;
	float c;
	DS18B20_Start();            //开始转换
	DS18B20_Check();	 
	DS18B20_Write_Byte(0xcc);   // skip rom
	DS18B20_Write_Byte(0xbe);   // 存储命令    
	TL=DS18B20_Read_Byte();     // LSB   
	TH=DS18B20_Read_Byte();     // MSB   
	if(TH>7)
	{
		TH=~TH;
		TL=~TL+1; 
		temp=0;     //温度为负  
	}
	else temp=1;  //温度为正	  	  
	tem=TH;       //获得高八位
	tem<<=8;    
	tem+=TL;      //获得低八位
	c=(double)tem*0.0625;  //数值转换，需要除2*4次方
	if(temp)return c;     
	else return -c;        //返回温度值
} 


