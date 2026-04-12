#include "T5Motor.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stm32f4xx_hal.h"
#include "main.h"

extern int emergency;
extern double RX;
extern double RY;
extern uint8_t alt_ok;
extern CAN_HandleTypeDef hcan1;

extern SemaphoreHandle_t xMutexEmergencies;
extern SemaphoreHandle_t xMutexBalanceComm;
extern SemaphoreHandle_t xMutexAltitudeComm;
extern uint8_t ByteReceived1, ByteReceived2, ByteReceived3, ByteReceived4;
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
	if(ByteReceived4 == 1){
		xSemaphoreTake( xMutexEmergencies, portMAX_DELAY);
		if(ByteReceived3)
		{

			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
		}
		xSemaphoreGive( xMutexEmergencies );

		GPIO_PinState res=HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8);

		xSemaphoreTake( xMutexAltitudeComm, portMAX_DELAY);
		if(ByteReceived1)
		{
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
		}
		xSemaphoreGive( xMutexAltitudeComm );

		xSemaphoreTake(xMutexBalanceComm, portMAX_DELAY);



		ByteReceived2;


		if(ByteReceived2 = 0x2){
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);

		}else if(ByteReceived2 = 0x1){
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
		}else{
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
		}

		if(ByteReceived2 = 0x8){
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);

		}else if(ByteReceived2 = 0x4){
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
		}
		else{
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
		}
		xSemaphoreGive( xMutexBalanceComm );
	}




		vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(T_TAREA5));
  }
  /* USER CODE END StartTask05 */
}

