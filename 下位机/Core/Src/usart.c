/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
	USART2->DR = (uint8_t) ch;      
	return ch;
}
#endif 

uint8_t aRxBuffer2[RXBUFFERSIZE];    //HAL库USART接收Buffer
uint8_t USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个
//接收状态
//bit15，	接收完成标志
//bit14~0，	接收到的有效字节数目
uint16_t USART2_RX_STA=0;       //接收状态标记	
/* USER CODE END 0 */

UART_HandleTypeDef huart2;

/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */
  clear_BUF(USART2_RX_BUF);	
	
  /* USER CODE END USART2_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART2_MspInit 1 */
		HAL_NVIC_EnableIRQ(USART2_IRQn);
		HAL_NVIC_SetPriority(USART2_IRQn,6,0);
  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
//RS485发送len个字节.
//buf:发送区首地址
//len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
void MAX3485_2_Send_Data(uint8_t *buf,uint8_t len)
{
//	clear_BUF(USART2_RX_BUF); 
	taskENTER_CRITICAL();			//进入临界区
	HAL_UART_Transmit(&huart2,buf,len,100);//串口2发送数据
	taskEXIT_CRITICAL();   //退出临界区		
}
//清除接收寄存器的状态，开启下次接收
void clear_BUF(uint8_t* usart)
{
	if(usart==USART2_RX_BUF)
	{
		USART2_RX_STA=0;
		memset(USART2_RX_BUF,0,USART2_REC_LEN);
	}
}

//接收中断回调函数，公共处理函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART2)    //如果是串口2
	{
		if((USART2_RX_STA&0x8000)==0)//接收未完成，
		{
			if(USART2_RX_STA==0)     //接收到了第一个字符
			{
				__HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);    //使能中断前先清除中断标志位，好习惯头发多
				HAL_TIM_Base_Start_IT(&htim2);                //开启定时器2的中断
			}
			__HAL_TIM_SET_COUNTER(&htim2,0);                //将定时器计数值清零（定时器为向上计数模式）
			USART2_RX_BUF[USART2_RX_STA&0X3FFF]=aRxBuffer2[0];//存储串口2中断所接收的单个字节到数组USART2_RX_BUF[]中
			USART2_RX_STA++;    //记录总共接收数据的数量
			if((USART2_RX_STA&0x3fff)>(USART2_REC_LEN-1)) USART2_RX_STA|=0x8000;    //超出接收最大长度，不再接收了; 
		}
		
	}
}
void USART2_IRQHandler()
{	
	uint32_t timeout=0;
	uint32_t maxDelay=0x1FFFF;
	HAL_UART_IRQHandler(&huart2);	//进入HAL库中断处理公用函数
	timeout=0;
  while (HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY)//等待就绪
	{
	 timeout++;////超时处理
   if(timeout>maxDelay) while(1);		
	}
	timeout=0;
	while(HAL_UART_Receive_IT(&huart2, (uint8_t *)aRxBuffer2, RXBUFFERSIZE) != HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount为1
	{
	 timeout++; //超时处理
	 if(timeout>maxDelay) while(1);		
	}
}
/* USER CODE END 1 */
