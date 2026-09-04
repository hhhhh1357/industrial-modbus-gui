#include "main.h"
#include "math.h"
#include "main_task.h"
#define RANK_  	3  		//多项式次数
/*
*********************************************************************************************************
*   函 数 名: polyfit
*   功能说明: 多项式曲线拟合（与matlab效果一致）
*   形    参: d_X	输入的数据的x值
			  d_Y	输入的数据的y值
			  d_N	输入的数据的个数
			  rank  多项式的次数
			  coeff 输出的系数
*   返 回 值: 无
*********************************************************************************************************
*/
//原理：At * A * C = At * Y	，其中 At 为 A 转置矩阵，C 为系数矩阵
void polyfit(float *d_X, float *d_Y, int d_N, int rank, float *coeff)
{
	if(RANK_ != rank)	//判断次数是否合法
		return;

	int i,j,k;	
	double aT_A[RANK_ + 1][RANK_ + 1] = {0};
	double aT_Y[RANK_ + 1] = {0};
	
	
	for(i = 0; i < rank + 1; i++)	//行
	{
		for(j = 0; j < rank + 1; j++)	//列
		{
			for(k = 0; k < d_N; k++)	
			{
				aT_A[i][j] +=  pow(d_X[k], i+j);		//At * A 线性矩阵
			}
		}
	}
	
	for(i = 0; i < rank + 1; i++)
	{
		for(k = 0; k < d_N; k++)
		{
			aT_Y[i] += pow(d_X[k], i) * d_Y[k];		//At * Y 线性矩阵
		}
	}
	
	//以下为高斯列主元素消去法解线性方程组
	for(k = 0; k < rank + 1 - 1; k++)
	{
		int row = k;
		double mainElement = fabs(aT_A[k][k]);
		double temp = 0.0;
		
		//找主元素
		for(i = k + 1; i < rank + 1 - 1; i++)
		{
			if( fabs(aT_A[i][i]) > mainElement )
			{
				mainElement = fabs(aT_A[i][i]);
				row = i;
			}
		}
		
		//交换两行
		if(row != k)
		{
			for(i = 0; i < rank + 1; i++)
			{
				temp = aT_A[k][i];
				aT_A[k][i] = aT_A[row][i];
				aT_A[row][i] = temp;
			}	
			temp = aT_Y[k];
			aT_Y[k] = aT_Y[row];
			aT_Y[row] = temp;
		}
			
		
		//消元过程
		for(i = k + 1; i < rank + 1; i++)
		{
			for(j = k + 1; j < rank + 1; j++)
			{
				aT_A[i][j] -= aT_A[k][j] * aT_A[i][k] / aT_A[k][k];
			}
			aT_Y[i] -= aT_Y[k] * aT_A[i][k] / aT_A[k][k];
		}
	}	
		
	//回代过程
	for(i = rank + 1 - 1; i >= 0; coeff[i] /= aT_A[i][i], i--)
	{
		for(j = i + 1, coeff[i] = aT_Y[i]; j < rank + 1; j++)
		{
			coeff[i] -= aT_A[i][j] * coeff[j];
		}
	}

	return;	
}
extern sensor_msg    CO2_GAS_msg;
void least_aquare(float *d_X, float *d_Y, int d_N)
{
	float coeff[4];
	polyfit(d_X,d_Y,d_N,RANK_,coeff);
	CO2_GAS_msg.fd=coeff[0];
	CO2_GAS_msg.fc=coeff[1];	
	CO2_GAS_msg.fb=coeff[2];
	CO2_GAS_msg.fa=coeff[3];
	CO2_GAS_msg.na=1;
	CO2_GAS_msg.nb=0;
	Save_linear_data(CO2_GAS_msg.fa,CO2_GAS_msg.fb,CO2_GAS_msg.fc,CO2_GAS_msg.fd);//保存非线性拟合参数
	Save_calib_data(1,0);//将标定参数存入EEPROM中
}

//R为吸收比
float fx(float R)
{
	float x=0.0;
	//x=323.19061-(1180.83599*R)+(1365.02919*R*R)-(481.62896*R*R*R);//CO20002
	//x=266.399-(1104.2*R)+(1476.28*R*R)-(626.341*R*R*R);//CO20003
	//x=292.187-(1338.38*R)+(2000.64*R*R)-(968.728*R*R*R);//CO20004
	//x=296.724-(1329.31*R)+(1932.33*R*R)-(901.201*R*R*R);//CO20005
	x=CO2_GAS_msg.fd+(CO2_GAS_msg.fc*R)+(CO2_GAS_msg.fb*R*R)+(CO2_GAS_msg.fa*R*R*R);
	return x;
}
