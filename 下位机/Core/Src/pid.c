#include "pid.h"
#include "DS18B20.h"
#include "tim.h"
#include <stdio.h>
 
extern PID_msg       PID; 
extern sensor_msg    CO2_GAS_msg;
void PID_init(void)
{
	PID.target_val=0;
	PID.actual_val=0;
	PID.err=0;
	PID.err_last=0;
	PID.integral=0;
	PID.kp=250;
	PID.ki=10;
	PID.kd=1000;
	PID.voltage=0;
}
float PID_realize(float temp_val)
{
	//记录目标值
	PID.target_val=temp_val;
	//计算目标值与实际值的误差
	PID.err=PID.target_val-PID.actual_val;
	//误差累积
	PID.integral+=PID.err;
	//PID算法实现
	PID.voltage=PID.kp*PID.err+PID.ki*PID.integral+PID.kd*(PID.err-PID.err_last);
	if(PID.voltage<0) PID.voltage=1;
	//给执行器输入参数
	TIM_SetTIM3compare(PID.voltage);
	//记录实际值
	PID.actual_val=DS18B20_Get_Temp();
	//误差传递
	PID.err_last=PID.err;
	return PID.actual_val;
}




