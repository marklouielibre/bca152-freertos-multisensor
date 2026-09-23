#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_adc/adc_oneshot.h"

#include "sensors.h"
#include "alarm.h"
#include "rtos_objects.h"
#include "system_state.h"

void SensorTask(void *pvParameters)
{
    // Allow DHT22 to stabilize
    vTaskDelay(pdMS_TO_TICKS(2500));

    TickType_t lastWakeTime = xTaskGetTickCount();

    // Keep the last valid DHT values
    static float lastTemperature = 24.0f;
    static float lastHumidity = 40.0f;

    for (;;)
    {
        float temperature = 0.0f;
        float humidity = 0.0f;

        int ldr_raw = 0;
        int ldr_percent = 0;

        // Read LDR
        if (adc_oneshot_read(adc_handle, ADC_CHANNEL_6, &ldr_raw) == ESP_OK)
        {
            ldr_percent = (ldr_raw * 100) / 4095;
        }

        // Read DHT22
        esp_err_t result = dht.doubleRead(&temperature, &humidity);

        // Accept only valid readings
        if (result == ESP_OK &&
            temperature >= -40.0f && temperature <= 80.0f &&
            humidity >= 0.0f && humidity <= 100.0f)
        {
            lastTemperature = temperature;
            lastHumidity = humidity;
        }

        temperature = lastTemperature;
        humidity = lastHumidity;

        SensorData data;
        data.temperature = temperature;
        data.humidity = humidity;
        data.lightLevel = ldr_percent;
        data.motionDetected = motionDetected;

        // Update queue
        xQueueOverwrite(sensorQueue, &data);

        // Alarm event
        AlarmState alarm = evaluateTemperature(temperature);

        if (alarm == AlarmState::NORMAL)
            xEventGroupClearBits(systemEvents, EVENT_ALARM);
        else
            xEventGroupSetBits(systemEvents, EVENT_ALARM);


        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(2500));
    }
}