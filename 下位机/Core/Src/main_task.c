#include "main_task.h"
#include "usart.h"
#include "24cxx.h"
#include "math.h"
#include "gpio.h"
#include "least_square.h"

//CO2浓度传感器为CRC校验，低位在前
extern sensor_msg          CO2_GAS_msg;
extern uint8_t CC_SHOW_Negative;
//向上位机返回浓度数据
void back_CC_value(void)
{
	uint8_t buf[28]={0};
	uint16_t crc=0;
	buf[0]=CO2_MODBUS_SLAVE_ADDR;
	buf[1]=0x03;
	buf[2]=0x00;
	buf[3]=0x02;
	buf[4]=0x00;
	buf[5]=0x03;
	buf[6]=*( long *)&CO2_GAS_msg.CC_CO2;
	buf[7]=*( long *)&CO2_GAS_msg.CC_CO2>>8;
	buf[8]=*( long *)&CO2_GAS_msg.CC_CO2>>16;
	buf[9]=*( long *)&CO2_GAS_msg.CC_CO2>>24;
	buf[10]=*( long *)&CO2_GAS_msg.R;
	buf[11]=*( long *)&CO2_GAS_msg.R>>8;
	buf[12]=*( long *)&CO2_GAS_msg.R>>16;
	buf[13]=*( long *)&CO2_GAS_msg.R>>24;
	buf[14]=*( long *)&CO2_GAS_msg.Temp;
	buf[15]=*( long *)&CO2_GAS_msg.Temp>>8;
	buf[16]=*( long *)&CO2_GAS_msg.Temp>>16;
	buf[17]=*( long *)&CO2_GAS_msg.Temp>>24;
	buf[18]=*( long *)&CO2_GAS_msg.A;
	buf[19]=*( long *)&CO2_GAS_msg.A>>8;
	buf[20]=*( long *)&CO2_GAS_msg.A>>16;
	buf[21]=*( long *)&CO2_GAS_msg.A>>24;
	buf[22]=*( long *)&CO2_GAS_msg.B;
	buf[23]=*( long *)&CO2_GAS_msg.B>>8;
	buf[24]=*( long *)&CO2_GAS_msg.B>>16;
	buf[25]=*( long *)&CO2_GAS_msg.B>>24;
	crc=usMBCRC16(buf,26);
	buf[26]=(crc&0x00FF);
	buf[27]=(crc&0XFF00)>>8;
	MAX3485_2_Send_Data(buf,28);
}
//向上位机返回标定数据a,b
void back_calib_data(void)
{
	uint8_t buf[16];
	uint16_t crc=0;
	buf[0]=CO2_MODBUS_SLAVE_ADDR;
	buf[1]=0x06;
	buf[2]=0x00;
	buf[3]=0x10;
	buf[4]=0x00;
	buf[5]=0x03;
	buf[6]=*( long *)&CO2_GAS_msg.na;
	buf[7]=*( long *)&CO2_GAS_msg.na>>8;
	buf[8]=*( long *)&CO2_GAS_msg.na>>16;
	buf[9]=*( long *)&CO2_GAS_msg.na>>24;
	buf[10]=*( long *)&CO2_GAS_msg.nb;
	buf[11]=*( long *)&CO2_GAS_msg.nb>>8;
	buf[12]=*( long *)&CO2_GAS_msg.nb>>16;
	buf[13]=*( long *)&CO2_GAS_msg.nb>>24;
	crc=usMBCRC16(buf,14);
	buf[14]=crc&0x00FF;
	buf[15]=(crc&0XFF00)>>8;
	MAX3485_2_Send_Data(buf,16);
}

union float_u8
{
	float f;
	uint8_t buf[4];
};

//存储标定参数到EEPROM中
//16-19四个字节存a,20-23四个字节存b
void Save_calib_data(float na,float nb)
{
	union float_u8 aa;
	union float_u8 bb;
	aa.f=na;
	bb.f=nb;
	AT24CXX_Write(16,aa.buf,4);
	AT24CXX_Write(20,bb.buf,4);
}
//读取标定数据到结构体变量中
void read_cabli_data(void)
{
	union float_u8 aa;
	union float_u8 bb;
	AT24CXX_Read(16,aa.buf,4);
	AT24CXX_Read(20,bb.buf,4);
	CO2_GAS_msg.na=aa.f;
	CO2_GAS_msg.nb=bb.f;
}
//存储零点标定吸收比到EEPROM中
//24-27四个字节存吸收比
void Save_Zero_calib_vol_data(float r)
{
	union float_u8 rr;
	rr.f=r;
	AT24CXX_Write(24,rr.buf,4);

}
//读取零点标定吸收比
void read_Zero_calib_vol_data(float *r)
{
	union float_u8 rr;
	AT24CXX_Read(24,rr.buf,4);
	*r=rr.f;

}
//存储量程标定吸收比到EEPROM中
//32-35四个字节存吸收比
void Save_Rang_calib_vol_data(float r)
{
	union float_u8 rr;
	rr.f=r;
	AT24CXX_Write(32,rr.buf,4);

}
//读取量程标定吸收比
void read_Rang_calib_vol_data(float *r)
{
	union float_u8 rr;
	AT24CXX_Read(32,rr.buf,4);
	*r=rr.f;
}
//存储%80量程值
//40-43四个字节存
void Save_Rang_data(float a)
{
	union float_u8 aa;
	aa.f=a;
	AT24CXX_Write(40,aa.buf,4);
}
//读取%80量程值
void read_Rang_data(void)
{
	union float_u8 aa;
	AT24CXX_Read(40,aa.buf,4);
	CO2_GAS_msg.RANGE=aa.f;
}

//存储线性拟合系数
//44-59 
void Save_linear_data(float a,float b,float c,float d)
{
	union float_u8 aa;
	aa.f=a;
	AT24CXX_Write(44,aa.buf,4);
	aa.f=b;
	AT24CXX_Write(48,aa.buf,4);
	aa.f=c;
	AT24CXX_Write(52,aa.buf,4);
	aa.f=d;
	AT24CXX_Write(56,aa.buf,4);
	
}
//读取线性拟合系数
void read_linear_data(void)
{
	union float_u8 aa;
	AT24CXX_Read(44,aa.buf,4);
	CO2_GAS_msg.fa=aa.f;
	AT24CXX_Read(48,aa.buf,4);
	CO2_GAS_msg.fb=aa.f;
	AT24CXX_Read(52,aa.buf,4);
	CO2_GAS_msg.fc=aa.f;
	AT24CXX_Read(56,aa.buf,4);
	CO2_GAS_msg.fd=aa.f;
}

//存储线性拟合样本点
//100---- 
void Save_linear_point_data(uint8_t *buf,uint8_t num)
{
	AT24CXX_Write(99,&num,1);
	AT24CXX_Write(100,buf,num);
}
//读取线性拟合样本点
void read_linear_point_data(uint8_t *buf,uint8_t *num)
{
	AT24CXX_Read(99,num,1);
	AT24CXX_Read(100,buf,*num);
}
//解析%80量程数据
void Analysis_range_data(uint8_t *buf,sensor_msg *mess)
{
	float range=0.0;
	memcpy(&range,&buf[6],4);
	mess->RANGE=range;
}

//开始零点标定
void Start_zero_calib(void)
{
	float VRANGER;
	float X0=0,X1=0;
	Save_Zero_calib_vol_data(CO2_GAS_msg.R);//存储当前零点标定吸收比
	read_Rang_calib_vol_data(&VRANGER);//读取量程标定得电压值
	//计算标定参数
	X0=fx(CO2_GAS_msg.R);
	X1=fx(VRANGER);
	CO2_GAS_msg.na=CO2_GAS_msg.RANGE/(X1-X0);
	CO2_GAS_msg.nb=0-CO2_GAS_msg.na*X0;
}
//开始量程标定
void Start_range_calib(void)
{
	float VZEROR;
	float X0=0,X1=0;
	Save_Rang_calib_vol_data(CO2_GAS_msg.R);//存储当前量程标定吸收比
	read_Zero_calib_vol_data(&VZEROR);//读取零点标定电压值
	//计算标定参数
	X0=fx(VZEROR);
	X1=fx(CO2_GAS_msg.R);
	CO2_GAS_msg.na=CO2_GAS_msg.RANGE/(X1-X0);
	CO2_GAS_msg.nb=0-CO2_GAS_msg.na*X0;
}
int linear_point_num=0;
extern TaskHandle_t Auto_send_ASCII_handler;
uint8_t main_task(void)
{
	uint8_t sta=0;
	switch(USART2_RX_BUF[0])//接收USART2_RX_BUF[]最大长度为50
	{
		case CO2_MODBUS_SLAVE_ADDR:        //为CO2传感器设备地址
			switch(USART2_RX_BUF[1])
			{
				case 0x03:
					switch(USART2_RX_BUF[2])
					{
						case 0x00:
							switch(USART2_RX_BUF[3])
							{
								case 0x02:  //01 03 00 02 00 03 CRCL CRCH 获取一次浓度信息
									if((USART2_RX_BUF[7]==((usMBCRC16(USART2_RX_BUF,6)&0XFF00)>>8))&&USART2_RX_BUF[6]==(usMBCRC16(USART2_RX_BUF,6)&0X00FF))//CRC校验
									{
										back_CC_value();//返回一次浓度数据
										sta=1;
									}
								break;
								case 0x08:  
									switch(USART2_RX_BUF[4])
									{
										case 0x50:
											switch(USART2_RX_BUF[5])
											{
												case 0x16:////01 03 00 08 50 16 CRCL CRCH 关闭自动发送
													if((USART2_RX_BUF[7]==((usMBCRC16(USART2_RX_BUF,6)&0XFF00)>>8))&&USART2_RX_BUF[6]==(usMBCRC16(USART2_RX_BUF,6)&0X00FF))//CRC校验
													{
														vTaskSuspend(Auto_send_ASCII_handler);  //将自动发送任务挂起
														MAX3485_2_Send_Data(USART2_RX_BUF,8);   //echo原指令
														sta=1;
													}
												break;
												case 0x35://01 03 00 08 50 35 CRCL CRCH 开启自动发送
													if((USART2_RX_BUF[7]==((usMBCRC16(USART2_RX_BUF,6)&0XFF00)>>8))&&USART2_RX_BUF[6]==(usMBCRC16(USART2_RX_BUF,6)&0X00FF))//CRC校验
													{
														vTaskResume(Auto_send_ASCII_handler);   //将自动发送任务恢复
														MAX3485_2_Send_Data(USART2_RX_BUF,8);   //echo原指令
														sta=1;
													}
												break;
											}
										break;
									}
								break;
							}
						break;
					}
				break;
				case 0x06:
					switch(USART2_RX_BUF[2])
					{
						case 0x00:
							switch(USART2_RX_BUF[3])
							{
								case 0x04:
									switch(USART2_RX_BUF[5])
									{
										case 0x00:   // 01 06 00 04 00 00 CRCL CRCH 关闭ASCII负值显示
											if((USART2_RX_BUF[7]==((usMBCRC16(USART2_RX_BUF,6)&0XFF00)>>8))&&USART2_RX_BUF[6]==(usMBCRC16(USART2_RX_BUF,6)&0X00FF))//CRC校验
											{
												CC_SHOW_Negative=0;
												MAX3485_2_Send_Data(USART2_RX_BUF,8);    //echo原指令
												sta=1;
											}		
										break;
										case 0x01:			//01 06 00 04 00 01 CRCL CRCH 开启ASCII负值显示
											if((USART2_RX_BUF[7]==((usMBCRC16(USART2_RX_BUF,6)&0XFF00)>>8))&&USART2_RX_BUF[6]==(usMBCRC16(USART2_RX_BUF,6)&0X00FF))//CRC校验
											{
												CC_SHOW_Negative=1;
												MAX3485_2_Send_Data(USART2_RX_BUF,8);    //echo原指令
												sta=1;
											}
										break;
									}
								break;
								case 0x10:
									switch(USART2_RX_BUF[5])
									{
										case 0x01: //01 06 00 10 00 01 00 00 00 00 CRCL CRCH零点标定
											if((USART2_RX_BUF[11]==((usMBCRC16(USART2_RX_BUF,10)&0XFF00)>>8))&&USART2_RX_BUF[10]==(usMBCRC16(USART2_RX_BUF,10)&0X00FF))//CRC校验
											{
												Start_zero_calib();
												MAX3485_2_Send_Data(USART2_RX_BUF,12);     //echo原指令
												sta=1;
											}
										break;
										case 0x02:  //01 06 00 10 00 02 XX XX XX XX CRCL CRCH 量程标定
											if((USART2_RX_BUF[11]==((usMBCRC16(USART2_RX_BUF,10)&0XFF00)>>8))&&USART2_RX_BUF[10]==(usMBCRC16(USART2_RX_BUF,10)&0X00FF))//CRC校验
											{
												Analysis_range_data(USART2_RX_BUF,&CO2_GAS_msg);//存储%80量程值
												Save_Rang_data(CO2_GAS_msg.RANGE);
												Start_range_calib();
												MAX3485_2_Send_Data(USART2_RX_BUF,12);  //echo原指令
												sta=1;
											}
										break;
										case 0x03: //01 06 00 10 00 03 CRCL CRCH 标定确认
											if((USART2_RX_BUF[7]==((usMBCRC16(USART2_RX_BUF,6)&0XFF00)>>8))&&USART2_RX_BUF[6]==(usMBCRC16(USART2_RX_BUF,6)&0X00FF))//CRC校验
											{
												Save_calib_data(CO2_GAS_msg.na,CO2_GAS_msg.nb);
												back_calib_data();		
												sta=1;
											}
										break;
									}
								break;
							}
						break;
					}
				break;
				case 0x10://回复时间约2.7s
					if((USART2_RX_BUF[8+USART2_RX_BUF[6]]==((usMBCRC16(USART2_RX_BUF,7+USART2_RX_BUF[6])&0XFF00)>>8))&&USART2_RX_BUF[7+USART2_RX_BUF[6]]==(usMBCRC16(USART2_RX_BUF,7+USART2_RX_BUF[6])&0X00FF))//CRC校验
					{
						float d_X[30]={0};
						float d_Y[30]={0};
						
						for(int i=0;i<30;i++)
						{
							memcpy(&d_X[i],&USART2_RX_BUF[7+i*8],4);
							memcpy(&d_Y[i],&USART2_RX_BUF[11+i*8],4);
							if(d_X[i]==0) 
							{
								linear_point_num=i;
								break;
							}
						}
						Save_linear_point_data(&USART2_RX_BUF[7],30*8);//存储线性拟合样本点
						least_aquare(d_X,d_Y,linear_point_num);
						uint8_t buf[8]={0};
						uint16_t crc=0;
						buf[0]=USART2_RX_BUF[0];
						buf[1]=USART2_RX_BUF[1];
						buf[2]=USART2_RX_BUF[2];
						buf[3]=USART2_RX_BUF[3];
						buf[4]=USART2_RX_BUF[4];
						buf[5]=USART2_RX_BUF[5];
						crc=usMBCRC16(buf,6);
						buf[6]=crc&0x00FF;
						buf[7]=(crc&0XFF00)>>8;
						MAX3485_2_Send_Data(buf,8);  
						sta=1;
					}
				break;
				case 0x04://读取样本点
					if((USART2_RX_BUF[7]==((usMBCRC16(USART2_RX_BUF,6)&0XFF00)>>8))&&USART2_RX_BUF[6]==(usMBCRC16(USART2_RX_BUF,6)&0X00FF))//CRC校验
					{
						uint8_t buf[256]={0};
						uint16_t crc=0;
						read_linear_point_data(buf,&CO2_GAS_msg.num);//读取线性拟合样本点
						uint8_t BUF[CO2_GAS_msg.num+5];
						BUF[0]=USART2_RX_BUF[0];
						BUF[1]=USART2_RX_BUF[1];
						BUF[2]=CO2_GAS_msg.num;
						for(uint8_t y=0;y<(CO2_GAS_msg.num);y++)
						{
							BUF[3+y]=buf[y];
						}
						crc=usMBCRC16(BUF,CO2_GAS_msg.num+3);
						BUF[CO2_GAS_msg.num+3]=crc&0x00FF;
						BUF[CO2_GAS_msg.num+4]=(crc&0XFF00)>>8;
						MAX3485_2_Send_Data(BUF,CO2_GAS_msg.num+5);  
						sta=1;
					}
				break;						
			}
		break;
	}
	return sta;
}



