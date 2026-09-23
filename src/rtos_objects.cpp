#include "rtos_objects.h"

QueueHandle_t sensorQueue;
EventGroupHandle_t systemEvents;
SemaphoreHandle_t serialMutex;

adc_oneshot_unit_handle_t adc_handle;
SSD1306_t oled;

DHT dht(GPIO_NUM_4, DHT_TYPE_DHT22);

volatile bool motionDetected = false;
volatile TickType_t lastMotionTick = 0;