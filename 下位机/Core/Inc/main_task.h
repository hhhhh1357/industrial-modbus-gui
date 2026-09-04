#ifndef _main_task_h
#define _main_task_h

#include "main.h"

__packed typedef struct  
{										    
  float A;       //测量电压A
	float B;       //参考电压B
	float R;       //吸光度
	float CC_CO2;  //CO2浓度，单位%
	float RANGE;   //%80量程,单位%
	float Temp;    //气室温度，单位℃
	float na;       //标定参数
	float nb;	     //标定参数，单位PPM
	float fa;			//线性回归参数y=ax3+bx2+cx+d;
	float fb;
	float fc;
	float fd;
	uint8_t num;  //样本数所占字节数
}sensor_msg; 

#define CO2_MODBUS_SLAVE_ADDR  0x01

uint8_t main_task(void);    //主任务处理函数
void back_CC_value(void);//返回浓度数据
void back_calib_data(void);//返回标定参数
void Analysis_range_data(uint8_t *buf,sensor_msg *mess);//解析%80量程值并存储到结构体中
void Save_calib_data(float na,float nb);//将标定参数存入EEPROM中
void read_cabli_data(void);//从EEPROM中读取标定参数
void Save_Rang_data(float a);//将%80量程值存储到EEPROM中
void read_Rang_data(void);//从EEPROM中读取%80量程值
void read_linear_data(void);//读取非线性拟合参数
void Save_linear_data(float a,float b,float c,float d);//保存非线性拟合参数
void read_linear_point_data(uint8_t *buf,uint8_t *num);//读取线性拟合样本点
void Save_linear_point_data(uint8_t *buf,uint8_t num);//存储线性拟合样本点


#endif

