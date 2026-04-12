/*
 *      Authors: Héctor Borreguero Monleón y Francisco Javier Díaz Ventura
 */

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "gpio.h"
#include "main.h"
#include "T1Altitude.h"
#include "T2Incliantion.h"
#include "T3Vibration.h"
#include "T4Activation.h"
#include "T5Motor.h"

extern SemaphoreHandle_t xMutexState;

typedef enum {
    Standby = 0,
    Active,
    Correcting,
    Rising,
	Risk
} State;

State thisState= Standby;

void Start()
{
	xSemaphoreTake(xMutexState, portMAX_DELAY);
	switch(thisState) {
		case Standby: thisState = Active; L1On() ; break;
		case Active: thisState = Active; break;
		case Correcting: thisState = Correcting; break;
		case Rising: thisState = Rising; break;
		case Risk: thisState = Risk; break;
	}
	xSemaphoreGive(xMutexState);
}

void Stop()
{
	xSemaphoreTake(xMutexState, portMAX_DELAY);
	switch(thisState) {
		case Standby: thisState = Standby; break;
		case Active: thisState = Standby; L1Off(); break;
		case Correcting: thisState = Standby; L1Off() ; break;
		case Rising: thisState = Standby; L1Off(); break;
		case Risk: thisState = Risk; break;
	}
	xSemaphoreGive(xMutexState);
}

void Front()
{
	xSemaphoreTake(xMutexState, portMAX_DELAY);
	switch(thisState) {
		case Standby: thisState = Standby; break;
		case Active: thisState = Correcting; M1On(); break;
		case Correcting: thisState = Correcting; M1On(); M2Off(); break;
		case Rising: thisState = Rising; break;
		case Risk: thisState = Risk; break;
	}
	xSemaphoreGive(xMutexState);
}

void Back()
{
	xSemaphoreTake(xMutexState, portMAX_DELAY);
	switch(thisState) {
		case Standby: thisState = Standby; break;
		case Active: thisState = Correcting; M2On(); break;
		case Correcting: thisState = Correcting; M2On(); M1Off(); break;
		case Rising: thisState = Rising; break;
		case Risk: thisState = Risk; break;
	}
	xSemaphoreGive(xMutexState);
}

void Left()
{
	xSemaphoreTake(xMutexState, portMAX_DELAY);
	switch(thisState) {
		case Standby: thisState = Standby; break;
		case Active: thisState = Correcting; M3On(); break;
		case Correcting: thisState = Correcting; M3On(); M4Off(); break;
		case Rising: thisState = Rising; break;
		case Risk: thisState = Risk; break;
	}
	xSemaphoreGive(xMutexState);
}

void Right()
{
	xSemaphoreTake(xMutexState, portMAX_DELAY);
	switch(thisState) {
		case Standby: thisState = Standby; break;
		case Active: thisState = Correcting; M4On(); break;
		case Correcting: thisState = Correcting; M4On(); M3Off(); break;
		case Rising: thisState = Rising; break;
		case Risk: thisState = Risk; break;
	}
	xSemaphoreGive(xMutexState);
}

void InclinationOK()
{
	xSemaphoreTake(xMutexState, portMAX_DELAY);
	switch(thisState) {
		case Standby: thisState = Standby; break;
		case Active: thisState = Active; break;
		case Correcting: thisState = Active; M1Off(), M2Off(), M3Off(), M4Off(); break;
		case Rising: thisState = Rising; break;
		case Risk: thisState = Risk; break;
	}
	xSemaphoreGive(xMutexState);
}

void Rise()
{
	xSemaphoreTake(xMutexState, portMAX_DELAY);
	switch(thisState) {
		case Standby: thisState = Standby; break;
		case Active: thisState = Rising; M1On(), M2On(), M3On(), M4On(); break;
		case Correcting: thisState = Correcting; break;
		case Rising: thisState = Rising; break;
		case Risk: thisState = Risk; break;
	}
	xSemaphoreGive(xMutexState);
}

void AltitudeOK()
{
	xSemaphoreTake(xMutexState, portMAX_DELAY);
	switch(thisState) {
		case Standby: thisState = Standby; break;
		case Active: thisState = Active; break;
		case Correcting: thisState = Correcting; break;
		case Rising: thisState = Active; M1Off(), M2Off(), M3Off(), M4Off(); break;
		case Risk: thisState = Risk; break;
	}
	xSemaphoreGive(xMutexState);
}

void Vibrating()
{
	xSemaphoreTake(xMutexState, portMAX_DELAY);
	switch(thisState) {
		case Standby: thisState = Standby; break;
		case Active: thisState = Risk; M1Off(), M2Off(), M3Off(), M4Off(); L2On(); break;
		case Correcting: thisState = Risk; M1Off(), M2Off(), M3Off(), M4Off();L2On();break;
		case Rising: thisState = Risk; M1Off(), M2Off(), M3Off(), M4Off();L2On();break;
		case Risk: thisState = Risk;break;
	}
	xSemaphoreGive(xMutexState);
}

void VibrationsOK()
{
	xSemaphoreTake(xMutexState, portMAX_DELAY);
	switch(thisState) {
		case Standby: thisState = Standby; break;
		case Active: thisState = Active; break;
		case Correcting: thisState = Correcting; break;
		case Rising: thisState = Rising; break;
		case Risk: thisState = Active; L2Off(); break;
	}
	xSemaphoreGive(xMutexState);
}
