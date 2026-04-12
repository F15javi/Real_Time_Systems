/*
 *      Authors: Héctor Borreguero Monleón y Francisco Javier Díaz Ventura
 */

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
extern SemaphoreHandle_t xCanMutex;

extern CAN_HandleTypeDef hcan1;
extern CAN_TxHeaderTypeDef pHeader4;
uint8_t ByteSent4;
uint32_t TxMailbox4;

#define T_TAREA4 300

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;


    xSemaphoreGiveFromISR(xBinaryInterruption, &xHigherPriorityTaskWoken);


	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


void StartTask04Activation(void const * argument)
{
    TickType_t lastWakeTime = xTaskGetTickCount();
    ByteSent4 = 1;
	if (xSemaphoreTake(xCanMutex, portMAX_DELAY) == pdTRUE) {
		HAL_CAN_AddTxMessage(&hcan1, &pHeader4, &ByteSent4, &TxMailbox4);
		xSemaphoreGive(xCanMutex);
	 }

    for(;;)
    {
    	xSemaphoreTake(xBinaryInterruption, portMAX_DELAY);

        xSemaphoreTake(xMutexTargetCurrentAltitude, portMAX_DELAY);
        target_altitud = current_altitud;
        xSemaphoreGive(xMutexTargetCurrentAltitude);


        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(T_TAREA4));

    }
}
