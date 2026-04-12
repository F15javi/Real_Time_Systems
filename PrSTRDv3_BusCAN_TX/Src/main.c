/* USER CODE BEGIN Header */
/**
 *      Authors: Héctor Borreguero Monleón y Francisco Javier Díaz Ventura
 *
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2021 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "can.h"
#include "i2c.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include <math.h>

#include "T1Altitude.h"
#include "T2Incliantion.h"
#include "T3Vibration.h"
#include "T4Activation.h"



/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define TRUE 1
#define FALSE 0
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PR_TAREA1 1
#define PR_TAREA2 2
#define PR_TAREA3 3
#define PR_TAREA4 4
#define PR_TAREA5 5


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
TaskHandle_t Task04Handle = NULL;
SemaphoreHandle_t xMutexBalanceComm = NULL;
SemaphoreHandle_t xMutexEmergencies = NULL;
SemaphoreHandle_t xMutexTargetCurrentAltitude = NULL;
SemaphoreHandle_t xMutexAltitudeComm = NULL;
SemaphoreHandle_t xBinaryInterruption = NULL;
SemaphoreHandle_t xCanMutex = NULL;

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
CAN_HandleTypeDef hcan1;
CAN_TxHeaderTypeDef pHeader1; //declare a specific header for message transmittions
CAN_TxHeaderTypeDef pHeader2; //declare a specific header for message transmittions
CAN_TxHeaderTypeDef pHeader3; //declare a specific header for message transmittions
CAN_TxHeaderTypeDef pHeader4; //declare a specific header for message transmittions

CAN_RxHeaderTypeDef pRxHeader; //declare header for message reception
uint32_t TxMailbox;
uint8_t ByteSent = 0; //declare byte to be transmitted //declare a receive byte
uint8_t ByteReceived = 0; //declare a receive byte
CAN_FilterTypeDef sFilterConfig; //declare CAN filter structure

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
int emergency= 0;

double RX = 0;
double RY = 0;

int alt_ok = 0;
int target_altitud = 0;
int current_altitud = 0;

double X = 0;
double Y = 0;
double Z = 0;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  Inicializa_Acelerometro();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  MX_CAN1_Init();
  InicializaTransmisionesCAN();
  MX_ADC2_Init();
  MX_ADC3_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */
  xMutexBalanceComm = xSemaphoreCreateMutex();
  xMutexEmergencies = xSemaphoreCreateMutex();
  xMutexTargetCurrentAltitude = xSemaphoreCreateMutex();
  xMutexAltitudeComm = xSemaphoreCreateMutex();
  xBinaryInterruption = xSemaphoreCreateBinary();
  xCanMutex = xSemaphoreCreateMutex();


  xTaskCreate(StartTarea1Altitude, "Tarea1", 128, NULL, PR_TAREA1, NULL);
  xTaskCreate(StartTask02Incliantion, "Tarea2", 128, NULL, PR_TAREA2, NULL);
  xTaskCreate(StartTask03Vibration, "Tarea3", 128, NULL, PR_TAREA3, NULL);
  xTaskCreate(StartTask04Activation, "Tarea4", 128, NULL, 4, &Task04Handle);
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void InicializaTransmisionesCAN() {

	pHeader1.DLC=1; //give message size of 1 byte
	pHeader1.IDE=CAN_ID_STD; //set identifier to standard
	pHeader1.RTR=CAN_RTR_DATA; //set data type to remote transmission request?
	pHeader1.StdId=0x2FA; //define a standard identifier, used for message identification by filters (##switch this for the other microcontroller##)

	pHeader2.DLC=4; //give message size of 1 byte
	pHeader2.IDE=CAN_ID_STD; //set identifier to standard
	pHeader2.RTR=CAN_RTR_DATA; //set data type to remote transmission request
	pHeader2.StdId=0x2FB; //define a standard identifier

	pHeader3.DLC=1; //give message size of 1 byte
	pHeader3.IDE=CAN_ID_STD; //set identifier to standard
	pHeader3.RTR=CAN_RTR_DATA; //set data type to remote transmission request
	pHeader3.StdId=0x2FC; //define a standard identifier

	pHeader4.DLC=1; //give message size of 1 byte
	pHeader4.IDE=CAN_ID_STD; //set identifier to standard
	pHeader4.RTR=CAN_RTR_DATA; //set data type to remote transmission request
	pHeader4.StdId=0x2FD; //define a standard identifier

	//filter one (stack light blink)
	sFilterConfig.FilterFIFOAssignment=CAN_FILTER_FIFO0; //set fifo assignment
	sFilterConfig.FilterIdHigh=0x2F4<<5; //the ID that the filter looks for (##switch this for the other microcontroller##)
	sFilterConfig.FilterIdLow=0;
	sFilterConfig.FilterMaskIdHigh=0;
	sFilterConfig.FilterMaskIdLow=0;
	sFilterConfig.FilterScale=CAN_FILTERSCALE_32BIT; //set filter scale
	sFilterConfig.FilterActivation=ENABLE;

	HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig); //configure CAN filter

	HAL_CAN_Start(&hcan1); //start CAN
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING); //enable interrupts
};
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

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

  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
