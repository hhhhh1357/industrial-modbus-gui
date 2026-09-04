#ifndef _PID_H
#define _PID_H
#include "main.h"

__packed typedef struct  
{										    
  float target_val;       //设定值
	float actual_val;       //实际值
	float err;             //偏差值
	float err_last;        //上一次偏差值
	float kp,ki,kd;        //PID参数
	float integral;       //积分值
	int voltage;          //控制器变量
}PID_msg; 

void PID_init(void);
float PID_realize(float temp_val);


//PID自整定
void control_variable(float output);
float process_variable(float setpoint) ;
void position_pid_autotune(float (*process_variable)(float), void (*control_variable)(float), float setpoint,float *kp,float*ki,float*kd);
#endif 
