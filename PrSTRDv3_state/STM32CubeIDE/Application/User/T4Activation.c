#include "T4Activation.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "gpio.h"
#include "main.h"

extern int target_altitud;
extern int current_altitud;
extern TaskHandle_t Task04Handle;

extern SemaphoreHandle_t xMutexTargetCurrentAltitude;
extern SemaphoreHandle_t xBinaryInterruption;
#define T_TAREA4 300

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;


    xSemaphoreGiveFromISR(xBinaryInterruption, &xHigherPriorityTaskWoken);


	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


void StartTask04Activation(void const * argument)
{
    TickType_t lastWakeTime = xTaskGetTickCount();

    for(;;)
    {
    	xSemaphoreTake(xBinaryInterruption, portMAX_DELAY);

        xSemaphoreTake(xMutexTargetCurrentAltitude, portMAX_DELAY);
        target_altitud = current_altitud;
        xSemaphoreGive(xMutexTargetCurrentAltitude);

    }
}
