/*
 *      Authors: Héctor Borreguero Monleón y Francisco Javier Díaz Ventura
 */

#include "T1Altitude.h"
#include "adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "main.h"

/* Variables externas */
extern int target_altitud;
extern int current_altitud;
extern int alt_ok;

/* Mutex externos */
extern SemaphoreHandle_t xMutexTargetCurrentAltitude;
extern SemaphoreHandle_t xMutexAltitudeComm;
extern SemaphoreHandle_t xCanMutex;

extern CAN_HandleTypeDef hcan1;
extern CAN_TxHeaderTypeDef pHeader1;
uint8_t ByteSent1;
uint32_t TxMailbox1;

#define T_TAREA1 300

void StartTarea1Altitude(void const * argument)
{

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

		  if(alt_ok_local)
		  {
			  ByteSent1=1;
		  }else{
			  ByteSent1=0;
		  }


		 if (xSemaphoreTake(xCanMutex, portMAX_DELAY) == pdTRUE) {
			 HAL_CAN_AddTxMessage(&hcan1, &pHeader1, &ByteSent1, &TxMailbox1);
		 	     xSemaphoreGive(xCanMutex);
		 }
		  vTaskDelayUntil( &lastWakeTime, pdMS_TO_TICKS(T_TAREA1));
	  }
	}
}
