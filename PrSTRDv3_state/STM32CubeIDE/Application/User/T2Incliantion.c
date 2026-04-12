#include "T2Incliantion.h"
#include "FreeRTOS.h"
#include "main.h"
#include "semphr.h"

extern double RX;
extern double RY;

extern SemaphoreHandle_t xMutexBalanceComm;

#define T_TAREA2 200

extern double Calculate_RotationX();
extern double Calculate_RotationY();
extern void Inicializa_Acelerometro();

extern double RX; // Inclinacion eje X
extern double RY; // Inclinacion eje Y
extern double RZ; // Inclinacion eje Z

extern double X; // eje X
extern double Y; // eje Y
extern double Z; // eje Z

void Obtain_Coordinates_XYZ()
/* Calculate acc. coordinates and stores them in global variables X Y Z */{
	int Ix, Iy, Iz;
	uint8_t Ix1, Ix2;
	uint8_t Iy1, Iy2;
	uint8_t Iz1, Iz2;
	Ix1 = SPI_Read (0x28);
	Ix2 = SPI_Read (0x29);
	Ix = (Ix2 << 8) + Ix1;
	if (Ix >= 0x8000) Ix = -(65536 - Ix);
	X = Ix/16384.0;
	Iy1 = SPI_Read (0x2A);
	Iy2 = SPI_Read (0x2B);
	Iy = (Iy2 << 8) + Iy1;
	if (Iy >= 0x8000) Iy = -(65536 - Iy);
	Y = Iy/16384.0;
	Iz1 = SPI_Read (0x2C);
	Iz2 = SPI_Read (0x2D);
	Iz = (Iz2 << 8) + Iz1;
	if (Iz >= 0x8000) Iz = -(65536 - Iz);
	Z = Iz/16384.0;
}

double Calculate_RotationX (){
	double rotX;
	Obtain_Coordinates_XYZ();
	rotX = atan2(Y, sqrt(X*X+Z*Z)) * 180.0/3.1416;
	return rotX;
}

double Calculate_RotationY (){
	double rotY;
	Obtain_Coordinates_XYZ();
	rotY = - atan2(X, sqrt(Y*Y+Z*Z)) * 180.0/3.1416;
	return rotY;
}

void StartTask02Incliantion(void const * argument)
{

	TickType_t lastWakeTime;
	lastWakeTime = xTaskGetTickCount();
	Inicializa_Acelerometro();

  for(;;)
  {
	 xSemaphoreTake( xMutexBalanceComm, portMAX_DELAY);
	 RX = Calculate_RotationX();
	 RY = Calculate_RotationY();
	 xSemaphoreGive( xMutexBalanceComm );

	vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(T_TAREA2));

  }
}
