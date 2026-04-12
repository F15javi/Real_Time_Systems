/*
 * statechart.c
 *
 *  Created on: 12 Apr 2026
 *      Author: hector
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
		case Standby: thisState = Active; break;
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
		case Active: thisState = Standby; break;
		case Correcting: thisState = Standby; break;
		case Rising: thisState = Standby; break;
		case Risk: thisState = Risk; break;
	}
	xSemaphoreGive(xMutexState);
}

void Front()
{
	xSemaphoreTake(xMutexState, portMAX_DELAY);
	switch(thisState) {
		case Standby: thisState = Standby; break;
		case Active: thisState = Correcting; break;
		case Correcting: thisState = Correcting;/*aqui hay accion*/ break;
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
		case Active: thisState = Correcting; break;
		case Correcting: thisState = Correcting; break;
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
		case Active: thisState = Correcting; break;
		case Correcting: thisState = Correcting; break;
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
		case Active: thisState = Correcting; break;
		case Correcting: thisState = Correcting; break;
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
		case Correcting: thisState = Active; break;
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
		case Active: thisState = Rising; break;
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
		case Rising: thisState = Active; break;
		case Risk: thisState = Risk; break;
	}
	xSemaphoreGive(xMutexState);
}

void Vibrating()
{
	xSemaphoreTake(xMutexState, portMAX_DELAY);
	switch(thisState) {
		case Standby: thisState = Standby; break;
		case Active: thisState = Risk; break;
		case Correcting: thisState = Risk; break;
		case Rising: thisState = Risk; break;
		case Risk: thisState = Risk; break;
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
		case Risk: thisState = Active; break;
	}
	xSemaphoreGive(xMutexState);
}

