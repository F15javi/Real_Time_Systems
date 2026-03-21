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
}
