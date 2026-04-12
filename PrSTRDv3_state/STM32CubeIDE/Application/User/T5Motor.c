#include "T5Motor.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "statechart.h"


extern int emergency;
extern double RX;
extern double RY;
extern int active;
extern int alt_ok;

int local_active=0;

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

	//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);

  for(;;)
  {

	xSemaphoreTake( xMutexAltitudeComm, portMAX_DELAY);

	if(local_active!=active)
	{
		if(active)
		{
			Start();
		}
		else{
			Stop();
		}
		local_active=active;
	}

	xSemaphoreGive( xMutexAltitudeComm );

	if(local_active)
	{
		xSemaphoreTake( xMutexEmergencies, portMAX_DELAY);
		if(emergency)
		{
			Vibrating();
		}
		else
		{
			VibrationsOK();
		}
		xSemaphoreGive( xMutexEmergencies );

		//GPIO_PinState res=HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8);

		xSemaphoreTake( xMutexAltitudeComm, portMAX_DELAY);
		if(alt_ok)
		{
			Rise();
		}
		else
		{
			AltitudeOK();
		}
		xSemaphoreGive( xMutexAltitudeComm );

		xSemaphoreTake(xMutexBalanceComm, portMAX_DELAY);
		if(RX > 10){
			Front();
		}else if(RX < -10){
			Back();
		}

		if(RY > 10){
		  Left();
		}else if(RY < -10){
		  Right();
		}

		if(RX >= -10 && RX <= 10 && RY >= -10 && RY <= 10)
		{
			InclinationOK();
		}

		xSemaphoreGive( xMutexBalanceComm );
	}

		vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(T_TAREA5));
  }
  /* USER CODE END StartTask05 */
}

void L2On()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
}

void L2Off()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
}

void L1On()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
}

void L1Off()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
}

void M1On()
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
}

void M2On()
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
}

void M3On()
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
}

void M4On()
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}

void M1Off()
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
}

void M2Off()
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
}

void M3Off()
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
}

void M4Off()
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
}
