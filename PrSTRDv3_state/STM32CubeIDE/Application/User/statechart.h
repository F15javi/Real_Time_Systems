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

void Start();
void Stop();
void Front();
void Back();
void Left();
void Right();
void InclinationOK();
void Rise();
void AltitudeOK();
void Vibrating();
void VibrationsOK();
