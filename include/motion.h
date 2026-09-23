#ifndef MOTION_H
#define MOTION_H

#include "freertos/FreeRTOS.h"

extern const TickType_t INACTIVITY_TIMEOUT;

void MotionTask(void *pvParameters);

#endif