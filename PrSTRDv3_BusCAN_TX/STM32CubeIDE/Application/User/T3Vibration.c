#include "T3Vibration.h"
#include "FreeRTOS.h"
#include "math.h"
#include "main.h"
#include "semphr.h"

extern double X;
extern double Y;
extern double Z;

extern int emergency;

extern SemaphoreHandle_t xMutexEmergencies;
extern SemaphoreHandle_t xCanMutex;

extern CAN_HandleTypeDef hcan1;
extern CAN_TxHeaderTypeDef pHeader3;
uint8_t ByteSent3;
uint32_t TxMailbox3;

#define T_TAREA3 100

void StartTask03Vibration(void const * argument)
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
    				emergency=1;
    		}
    	}
    	else
    	{
    		counter=0;
				emergency=0;
    	}
    	ByteSent3 = emergency;
		if (xSemaphoreTake(xCanMutex, portMAX_DELAY) == pdTRUE) {
			HAL_CAN_AddTxMessage(&hcan1, &pHeader3, &ByteSent3, &TxMailbox3);
			xSemaphoreGive(xCanMutex);
		}
    	lastX=X;
		lastY=Y;
		lastZ=Z;
    }

	vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(T_TAREA3));
  }
  /* USER CODE END StartTask03 */
}
