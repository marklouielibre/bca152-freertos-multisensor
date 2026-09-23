#ifndef RTOS_OBJECTS_H
#define RTOS_OBJECTS_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"

#include "esp_adc/adc_oneshot.h"
#include "ssd1306.h"
#include "dht.h"

struct SensorData
{
    float temperature;
    float humidity;
    int lightLevel;
    bool motionDetected;
};

#define EVENT_ACTIVE BIT0
#define EVENT_MOTION BIT1
#define EVENT_ALARM  BIT2

extern QueueHandle_t sensorQueue;
extern EventGroupHandle_t systemEvents;
extern SemaphoreHandle_t serialMutex;

extern adc_oneshot_unit_handle_t adc_handle;
extern SSD1306_t oled;
extern DHT dht;

extern volatile bool motionDetected;
extern volatile TickType_t lastMotionTick;

#endif