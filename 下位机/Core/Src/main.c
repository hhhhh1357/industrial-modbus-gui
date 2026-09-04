/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "cmsis_os.h"
#include "iwdg.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "stdlib.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "24cxx.h"
#include "DS18B20.h"
#include "mbcrc.h"
#include "AD7606.h"
#include "least_square.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//开始任务
TaskHandle_t start_handler;
void start( void * pvParameters );
//喂狗任务
TaskHandle_t Running_handler;
void Running( void * pvParameters );
//任务2
TaskHandle_t task2_handler;
void task2( void * pvParameters );
//恒温任务
TaskHandle_t const_temp_handler;
void const_temp( void * pvParameters );
//任务3
TaskHandle_t task3_handler;
void task3( void * pvParameters );
//任务4
TaskHandle_t task4_handler;
void main_slave_task( void * pvParameters );
//任务Auto_send_ASCII
TaskHandle_t Auto_send_ASCII_handler;
void Auto_send_ASCII( void * pvParameters );
/************************全局变量******************************/
SemaphoreHandle_t   USART2_mess = NULL;
SemaphoreHandle_t   oneweekFlag = NULL;
SemaphoreHandle_t   startFlag = NULL;
sensor_msg          CO2_GAS_msg;
PID_msg             PID;  
uint8_t CC_SHOW_Negative=1;
/******************************************************/
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */   
	
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
	MX_IWDG_Init();     //初始化狗，间隔1s
  MX_TIM2_Init();     //与串口2一起使用，接收间隔10ms
  MX_TIM3_Init();     //加热板PWM
  MX_SPI1_Init();     //初始化SPI外设
  MX_TIM1_Init();     //开始转换PWM
	MX_USART2_UART_Init();//115200
	MX_TIM4_Init();			//PB6,红外光源PWM
  /* USER CODE BEGIN 2 */
	HAL_Delay(30);
	PID_init();         //设置加热PID参数
	AD7606_Init();      //初始化AD7606
  AT24CXX_Init();     //初始化AT24C16
		//创建开始任务
	xTaskCreate((TaskFunction_t) start,
							(const char *) "start",
							(uint16_t ) 256,
							(void *)  NULL,
							(UBaseType_t) 1,
							(TaskHandle_t *)&start_handler);
							
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */

  /* Start scheduler */
  vTaskStartScheduler();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
		
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */


void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
	Error_Handler();
	}
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
	Error_Handler();
	}
}


/* USER CODE BEGIN 4 */
void start( void *pvParameters )
{
	taskENTER_CRITICAL();			//进入临界区
	///////////////////////////////////////////
	DS18B20_Init();			//初始化DS18B20
	USART2_mess   =xSemaphoreCreateBinary( );
	oneweekFlag   =xSemaphoreCreateBinary( );
	startFlag     =xSemaphoreCreateBinary( );

	/////////////////////////////////////////////
	read_cabli_data();    //从EEPROM中读取标定参数存入结构体
	read_Rang_data();     //从EEPROM中读取%80量程值存入结构体
	read_linear_data();		//读取非线性拟合参数
  /////////////////////////////////////////////	
	//任务函数，任务名，堆栈大小，函数入口参数，优先级，任务句柄	
	xTaskCreate(Running, "Running",128, NULL,1,&Running_handler );
	xTaskCreate(task2, "task2",256, NULL,3,&task2_handler );	
	xTaskCreate(task3, "task3",256, NULL,5,&task3_handler );
	xTaskCreate(const_temp,"const_temp",128,NULL,2,&const_temp_handler );
	xTaskCreate(main_slave_task, "main_slave_task",1024, NULL,6,&task4_handler );
	xTaskCreate(Auto_send_ASCII, "Auto_send_ASCII",128, NULL,2,&Auto_send_ASCII_handler );		
	vTaskSuspend(Auto_send_ASCII_handler);  //将自动发送任务挂起								
	vTaskDelete(start_handler);
	taskEXIT_CRITICAL();   //退出临界区	
}

void Running( void *pvParameters )//任务一,以led0闪烁来表现程序运行状态
{	
	while(1)
	{
  	HAL_IWDG_Refresh(&hiwdg);  //喂狗
		//Save_calib_data(1,0);//单独保存标定参数
		//Save_Rang_data(16);//单独保存量程
		led1=!led1;
		vTaskDelay(500);
	}
}
void const_temp( void *pvParameters ) ////PTC加热板恒温控制程序
{	
	while(1)
	{
		if(DS18B20_Get_Temp()>40)
		{
			CO2_GAS_msg.Temp=PID_realize(45.0);    //配置温度控制在45摄氏度。
		}
		else 
			TIM_SetTIM3compare(3999);
//	PRINT(temp,"%f",CO2_GAS_msg.Temp);
//  PRINT(PWM,"%d",PID.voltage);
		vTaskDelay(3000);
	}
}
volatile float ADC1_L=5.0;
volatile float ADC1_H=0;
volatile float ADC2_L=5.0;
volatile float ADC2_H=0;
uint8_t ADC_STA=0; // 
float R_avebuf[15]={0};
void task2( void *pvParameters ) 
{
	float R[15]={0};
	uint8_t i=0,x=0;
	while(1)
	{
		if(xSemaphoreTake(oneweekFlag,portMAX_DELAY)==pdTRUE)            //二值信号量，portMAX_DELAY
		{
				//将采集到的（最大值-小值）=平均峰峰值。
				CO2_GAS_msg.A=(ADC1_H-ADC1_L);							
				CO2_GAS_msg.B=(ADC2_H-ADC2_L);	
				//PRINT(ADC,"%4.2f,%4.2f",CO2_GAS_msg.B,CO2_GAS_msg.A);
				R[i]=CO2_GAS_msg.A/CO2_GAS_msg.B;	i++;
				if(i==15) 
				{
					i=0;
					sort(R,15);//从小到大排序
					R_avebuf[x]=(R[5]+R[6]+R[7]+R[8]+R[9])/5.0;  x++;
					if(x==15)x=0;
					float sum=0;
					for(int k=0;k<15;k++)
					{
						sum+=R_avebuf[k];
						if(R_avebuf[k]==0) 
						{
							CO2_GAS_msg.R=sum/k;
							break;
						}
						if(k==14) CO2_GAS_msg.R=sum/15.0;
					}
					CO2_GAS_msg.CC_CO2=fx(CO2_GAS_msg.R)*CO2_GAS_msg.na+CO2_GAS_msg.nb;	
				}
				ADC1_L=5.0;
				ADC1_H=0;
				ADC2_L=5.0;
				ADC2_H=0;
				ADC_STA=0;
		}
	}
}
void task3( void *pvParameters ) 
{	
	while(1)
	{
		if(startFlag!=NULL)
		{
			if(xSemaphoreTake(startFlag,portMAX_DELAY)==pdTRUE)   //二值信号量，等待开始采集信号量
			{
				if(ADC_STA<1)//采集数量未达到1次，还可以继续采集
				{
					AD7606_start();   //开始采集电压
					vTaskDelay(75);   //采集峰峰值
					AD7606_stop();    //停止采集电压
					ADC_STA++;
					xSemaphoreGive(oneweekFlag);    //发送信号量，代表一个周期采集完成
				}
			}
		}
	}
}
void main_slave_task( void *pvParameters )//
{	
	HAL_UART_Receive_IT(&huart2, (uint8_t *)aRxBuffer2, RXBUFFERSIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
	while(1)
	{
		if(USART2_mess!=NULL)
		{
			if(xSemaphoreTake(USART2_mess,portMAX_DELAY)==pdTRUE) //二值信号量，与串口2同步portMAX_DELAY
			{
				if(main_task())            //串口2通讯指令处理函数
					led0=!led0;
				clear_BUF(USART2_RX_BUF);//统一清除USART2_RX_BUF数据，开启下一次串口接收
			}
		}
	}
}
void Auto_send_ASCII( void *pvParameters )//ASCII发送任务
{
	int num=0;
	while(1)
	{
		vTaskDelay(400);
		if((CC_SHOW_Negative==0)&&(CO2_GAS_msg.CC_CO2<0))//禁止显示负值
		{
			printf("A %.6f B %.6f R %0.6f CC 0.0000 %% T %.2f RANGE %.1f%% %d\r\n",CO2_GAS_msg.A,CO2_GAS_msg.B,(CO2_GAS_msg.A/CO2_GAS_msg.B),CO2_GAS_msg.Temp,CO2_GAS_msg.RANGE/0.8,num);
		}
		else printf("A %.6f B %.6f R %0.6f CC %.4f %% T %.2f RANGE %.1f%% %d\r\n",CO2_GAS_msg.A,CO2_GAS_msg.B,(CO2_GAS_msg.A/CO2_GAS_msg.B),CO2_GAS_msg.CC_CO2,CO2_GAS_msg.Temp,CO2_GAS_msg.RANGE/0.8,num);
		vTaskDelay(600);
		num++;
	}
	
}

void sort(float *a,int l)
{
	int i,j;
	float v;
	for(i=0;i<l-1;i++)
		for(j=i+1;j<l;j++)
		{
			if((a[i]-a[j])>0)
			{
				v=a[i];
				a[i]=a[j];
				a[j]=v;
			}
		}
	
}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */
  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
	if(htim->Instance==TIM2)
	{
    USART2_RX_STA|=0x8000;       //定时器中断，标记接收完成
	}	
  if(htim->Instance==TIM4)
	{

	}	
  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */

  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {

  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

