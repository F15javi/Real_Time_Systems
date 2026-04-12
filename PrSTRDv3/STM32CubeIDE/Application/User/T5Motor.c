/*
 *      Authors: Héctor Borreguero Monleón y Francisco Javier Díaz Ventura
 */

#include "T5Motor.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stm32f4xx_hal.h"
#include "main.h"

extern int emergency;
extern double RX;
extern double RY;
extern int alt_ok;

extern SemaphoreHandle_t xMutexEmergencies;
extern SemaphoreHandle_t xMutexBalanceComm;
extern SemaphoreHandle_t xMutexAltitudeComm;

#define T_TAREA5 150

void StartTask05Motor(void const * argument)
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

	  xSemaphoreTake(xMutexBalanceComm, portMAX_DELAY);
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

