/*
 *      Authors: Héctor Borreguero Monleón y Francisco Javier Díaz Ventura
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
