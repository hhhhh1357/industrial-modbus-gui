/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

#include "stm32f1xx_ll_spi.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_utils.h"
#include "stm32f1xx_ll_pwr.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_dma.h"

#include "stm32f1xx_ll_exti.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "string.h"
#include "mbcrc.h"
#include "delay.h"
#include "main_task.h"
#include "pid.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define AD_FRSTDATA_Pin GPIO_PIN_9
#define AD_FRSTDATA_GPIO_Port GPIOE
#define AD_BUSY_Pin GPIO_PIN_10
#define AD_BUSY_GPIO_Port GPIOE
#define AD_BUSY_EXTI_IRQn EXTI15_10_IRQn
#define AD_CS_Pin GPIO_PIN_11
#define AD_CS_GPIO_Port GPIOE
#define AD_RESET_Pin GPIO_PIN_12
#define AD_RESET_GPIO_Port GPIOE
#define AD_STBY_Pin GPIO_PIN_14
#define AD_STBY_GPIO_Port GPIOE
#define IIC_SCL_Pin GPIO_PIN_12
#define IIC_SCL_GPIO_Port GPIOB
#define IIC_SDA_Pin GPIO_PIN_13
#define IIC_SDA_GPIO_Port GPIOB
#define LED_Pin GPIO_PIN_14
#define LED1_Pin GPIO_PIN_15
#define LED_GPIO_Port GPIOB
#define DS18B20_DQ_Pin GPIO_PIN_8
#define DS18B20_DQ_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

extern SemaphoreHandle_t   USART2_mess;
extern SemaphoreHandle_t   oneweekFlag;
extern SemaphoreHandle_t   startFlag; 

//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
#define PEout(n)   BIT_ADDR((GPIOE_BASE+12),n)  //输出 
#define PDout(n)   BIT_ADDR((GPIOD_BASE+12),n)  //输出 
#define PCout(n)   BIT_ADDR((GPIOC_BASE+12),n)  //输出 
#define PBout(n)   BIT_ADDR((GPIOB_BASE+12),n)  //输出 
#define PAout(n)   BIT_ADDR((GPIOA_BASE+12),n)  //输出 
#define PBin(n)    BIT_ADDR((GPIOB_BASE+8),n)   //输入 
#define PEin(n)    BIT_ADDR((GPIOE_BASE+8),n)   //输入 
/* USER CODE END Private defines */
void sort(float *buf,int num);
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
