/*
 *      Authors: Héctor Borreguero Monleón y Francisco Javier Díaz Ventura
 */

#include "T4Activation.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "gpio.h"
#include "main.h"

extern int target_altitud;
extern int current_altitud;
extern TaskHandle_t Task04Handle;

extern SemaphoreHandle_t xMutexTargetCurrentAltitude;

#define T_TAREA4 300

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if(GPIO_Pin == GPIO_PIN_0)
	{
	  vTaskNotifyGiveFromISR(Task04Handle, &xHigherPriorityTaskWoken);
	}

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


void StartTask04Activation(void const * argument)
{
    TickType_t lastWakeTime = xTaskGetTickCount();

    for(;;)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        xSemaphoreTake(xMutexTargetCurrentAltitude, portMAX_DELAY);
        target_altitud = current_altitud;
        xSemaphoreGive(xMutexTargetCurrentAltitude);

        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(T_TAREA4));
    }
}
