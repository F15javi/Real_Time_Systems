/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include <math.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "spi.h"
#include "adc.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
  #define TRUE 1
  #define FALSE 0
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
	#define T_TAREA1 300
	#define PR_TAREA1 1
	#define T_TAREA2 200
	#define PR_TAREA2 2
	#define T_TAREA3 350
	#define PR_TAREA3 3
	#define T_TAREA5 150
	#define PR_TAREA5 5
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
double Calculate_RotationX();
double Calculate_RotationY();
TaskHandle_t Task04Handle = NULL;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
	double Altitud = 0; // Altitud
	double RX = 0; // Inclinacion eje X
	double RY = 0; // Inclinacion eje Y
	double RZ = 0; // Inclinacion eje Z

	double X = 0; // eje X
	double Y = 0; // eje Y
	double Z = 0; // eje Z

	int ContTarea1 = 0;

	int target_altitud=0;
	int current_altitud=0;

	int start_signal=0;
	int alt_ok=0;
	int emergency = 0;
/* USER CODE END Variables */
osThreadId Tarea1Handle;
osThreadId myTask02Handle;
osThreadId myTask03Handle;
osThreadId myTask04Handle;
osThreadId myTask05Handle;
osMutexId mutex1Handle;
osSemaphoreId Semaforo_1Handle;
SemaphoreHandle_t xMutexBalanceComm;
SemaphoreHandle_t xMutexTargetCurrentAltitude;
SemaphoreHandle_t xMutexAltitudeComm;
SemaphoreHandle_t xMutexEmergencies;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartTarea1(void const * argument);
void StartTask02(void const * argument);
void StartTask03(void const * argument);
void StartTask04(void const * argument);
void StartTask05(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* definition and creation of mutex1 */
  osMutexDef(mutex1);
  mutex1Handle = osMutexCreate(osMutex(mutex1));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  	  xMutexBalanceComm = xSemaphoreCreateMutex();
  	  xMutexTargetCurrentAltitude = xSemaphoreCreateMutex();
  	  xMutexAltitudeComm = xSemaphoreCreateMutex();
  	  xMutexEmergencies = xSemaphoreCreateMutex();
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of Semaforo_1 */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of Tarea1 */
  osThreadDef(Tarea1, StartTarea1, osPriorityIdle, 0, 128);
  Tarea1Handle = osThreadCreate(osThread(Tarea1), NULL);

  /* definition and creation of myTask02 */
  osThreadDef(myTask02, StartTask02, osPriorityLow, 0, 128);
  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

  /* definition and creation of myTask03 */
  osThreadDef(myTask03, StartTask03, osPriorityIdle, 0, 128);
  myTask03Handle = osThreadCreate(osThread(myTask03), NULL);

  /* definition and creation of myTask04 */
  osThreadDef(myTask04, StartTask04, osPriorityIdle, 0, 128);
  myTask04Handle = osThreadCreate(osThread(myTask04), NULL);
  Task04Handle = myTask04Handle;
  /* definition and creation of myTask05 */
  osThreadDef(myTask05, StartTask05, osPriorityIdle, 0, 128);
  myTask05Handle = osThreadCreate(osThread(myTask05), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartTarea1 */
/**
  * @brief  Function implementing the Tarea1 thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTarea1 */
void StartTarea1(void const * argument)
{
  /* USER CODE BEGIN StartTarea1 */

	TickType_t lastWakeTime;
	lastWakeTime = xTaskGetTickCount();

	int alt_ok_local=0;
	int Lectura_ADC1 = 0; //Valor leido
	/* Inicializa canal 1 del ADC1 */
	ADC_ChannelConfTypeDef sConfigN = {0}; // Variable local en la tarea
	sConfigN.Channel = ADC_CHANNEL_1; // selecciona el canal 1
	sConfigN.Rank = 1;
	sConfigN.SamplingTime = ADC_SAMPLETIME_28CYCLES;
	HAL_ADC_ConfigChannel(&hadc1, &sConfigN); // configura ADC1-Canal_1

	/* Infinite loop */
	for(;;)
	{
	  // Activación de la lectura
	  HAL_ADC_Start(&hadc1); // comienza la conversón AD
	  if(HAL_ADC_PollForConversion(&hadc1, 5) == HAL_OK){
		  Lectura_ADC1 = HAL_ADC_GetValue(&hadc1); // leemos el valor

		  xSemaphoreTake( xMutexTargetCurrentAltitude, portMAX_DELAY);

		  current_altitud=Lectura_ADC1;
		  if(current_altitud<target_altitud-2){
			  alt_ok_local=1;
		  }
		  else{
			  alt_ok_local=0;
		  }

		  xSemaphoreGive( xMutexTargetCurrentAltitude );

		  xSemaphoreTake( xMutexAltitudeComm, portMAX_DELAY);
		  if(alt_ok_local)
		  {
			  alt_ok=1;
		  }else{
			  alt_ok=0;
		  }

		  xSemaphoreGive( xMutexAltitudeComm );

		  vTaskDelayUntil( &lastWakeTime, pdMS_TO_TICKS(T_TAREA1));
	  }
	}
  /* USER CODE END StartTarea1 */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
	 TickType_t lastWakeTime;
	lastWakeTime = xTaskGetTickCount();
	Inicializa_Acelerometro();

  for(;;)
  {
	 xSemaphoreTake( xMutexBalanceComm, portMAX_DELAY);
	 RX = Calculate_RotationX();
	 RY = Calculate_RotationY();
	 xSemaphoreGive( xMutexBalanceComm );

    //osDelay(1);
	vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(T_TAREA2));

  }
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the myTask03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void const * argument)
{
  /* USER CODE BEGIN StartTask03 */
  /* Infinite loop */
  static double lastX=0, lastY=0, lastZ=0;
  static int counter=0;
  TickType_t lastWakeTime;
  lastWakeTime = xTaskGetTickCount();

  for(;;)
  {
	  //if(last)
    if(!lastX&&!lastY&&!lastZ)
    {
    	lastX=X;
		lastY=Y;
		lastZ=Z;
    }
    else
    {
    	double Xdiff,Ydiff,Zdiff;
    	Xdiff=fabs(X*100-lastX*100);
		Ydiff=fabs(Y*100-lastY*100);
		Zdiff=fabs(Z*100-lastZ*100);
    	if(Xdiff>10||Ydiff>10||Zdiff>10)
    	{
    		counter++;
    		if(counter>2)
    		{
    			xSemaphoreTake( xMutexEmergencies, portMAX_DELAY);
    				emergency=1;
    			xSemaphoreGive( xMutexEmergencies );
    		}
    	}
    	else
    	{
    		counter=0;
    	}

    	lastX=X;
		lastY=Y;
		lastZ=Z;
    }

	vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(T_TAREA3));
  }
  /* USER CODE END StartTask03 */
}

/* USER CODE BEGIN Header_StartTask04 */
/**
* @brief Function implementing the myTask04 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask04 */
void StartTask04(void const * argument)
{
  /* USER CODE BEGIN StartTask04 */
  /* Infinite loop */
	TickType_t lastWakeTime;
	lastWakeTime = xTaskGetTickCount();
  for(;;)
  {
	  /* Wait for notification from ISR */
	  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	  xSemaphoreTake( xMutexTargetCurrentAltitude, portMAX_DELAY);

	  target_altitud=current_altitud;

	  xSemaphoreGive( xMutexTargetCurrentAltitude );

	  vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(T_TAREA1));
  }
  /* USER CODE END StartTask04 */
}

/* USER CODE BEGIN Header_StartTask05 */
/**
* @brief Function implementing the myTask05 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask05 */
void StartTask05(void const * argument)
{
  /* USER CODE BEGIN StartTask05 */
  /* Infinite loop */
	TickType_t lastWakeTime;
	lastWakeTime = xTaskGetTickCount();

	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
  for(;;)
  {


		xSemaphoreTake( xMutexEmergencies, portMAX_DELAY);
		if(emergency)
		{

			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
		}
		xSemaphoreGive( xMutexEmergencies );

		GPIO_PinState res=HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8);

	  xSemaphoreTake( xMutexBalanceComm, portMAX_DELAY);
	  if(RX > 10){
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);

	  }else if(RX < -10){
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
	  }else{
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
	  }

	  if(RY > 10){
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);

	  }else if(RY < -10){
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
	  }
	  else{
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	  }
	  xSemaphoreGive( xMutexBalanceComm );

	  xSemaphoreTake( xMutexAltitudeComm, portMAX_DELAY);
	  if(alt_ok)
	  {
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
	  }
	  xSemaphoreGive( xMutexAltitudeComm );

		vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(T_TAREA5));
  }
  /* USER CODE END StartTask05 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void Obtain_Coordinates_XYZ()
/* Calculate acc. coordinates and stores them in global variables X Y Z */{
	int Ix, Iy, Iz;
	uint8_t Ix1, Ix2;
	uint8_t Iy1, Iy2;
	uint8_t Iz1, Iz2;
	Ix1 = SPI_Read (0x28);
	Ix2 = SPI_Read (0x29);
	Ix = (Ix2 << 8) + Ix1;
	if (Ix >= 0x8000) Ix = -(65536 - Ix);
	X = Ix/16384.0;
	Iy1 = SPI_Read (0x2A);
	Iy2 = SPI_Read (0x2B);
	Iy = (Iy2 << 8) + Iy1;
	if (Iy >= 0x8000) Iy = -(65536 - Iy);
	Y = Iy/16384.0;
	Iz1 = SPI_Read (0x2C);
	Iz2 = SPI_Read (0x2D);
	Iz = (Iz2 << 8) + Iz1;
	if (Iz >= 0x8000) Iz = -(65536 - Iz);
	Z = Iz/16384.0;
}
double Calculate_RotationX (){
	double rotX;
	Obtain_Coordinates_XYZ();
	rotX = atan2(Y, sqrt(X*X+Z*Z)) * 180.0/3.1416;
	return rotX;
}
double Calculate_RotationY (){
	double rotY;
	Obtain_Coordinates_XYZ();
	rotY = - atan2(X, sqrt(Y*Y+Z*Z)) * 180.0/3.1416;
	return rotY;
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
	  {
	      BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	      if(GPIO_Pin == GPIO_PIN_0)
	      {
	          vTaskNotifyGiveFromISR(Task04Handle, &xHigherPriorityTaskWoken);
	      }

	      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	  }

/* USER CODE END Application */
