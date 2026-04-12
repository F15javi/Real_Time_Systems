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
    			xSemaphoreTake( xMutexEmergencies, portMAX_DELAY);
    				emergency=1;
    			xSemaphoreGive( xMutexEmergencies );
    		}
    	}
    	else
    	{
    		counter=0;
			xSemaphoreTake( xMutexEmergencies, portMAX_DELAY);
				emergency=0;
			xSemaphoreGive( xMutexEmergencies );
    	}

    	lastX=X;
		lastY=Y;
		lastZ=Z;
    }

	vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(T_TAREA3));
  }
  /* USER CODE END StartTask03 */
}
