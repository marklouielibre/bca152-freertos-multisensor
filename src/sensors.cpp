#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_adc/adc_oneshot.h"
#include "sensors.h"
#include "alarm.h"
#include "rtos_objects.h"
#include "system_state.h"

void SensorTask(void *pvParameters)
{
    // Give the DHT22 time to stabilize
    vTaskDelay(pdMS_TO_TICKS(2500));

    TickType_t lastWakeTime = xTaskGetTickCount();

    // Default values used only until the first valid reading
    float lastTemperature = 24.0f;
    float lastHumidity = 40.0f;

    for (;;)
    {
        float temperature = 0.0f;
        float humidity = 0.0f;

        int ldr_raw = 0;
        int ldr_percent = 0;

        // ---------------------------
        // Read LDR
        // ---------------------------

        if (adc_oneshot_read(
                adc_handle,
                ADC_CHANNEL_6,
                &ldr_raw) == ESP_OK)
        {
            // The photoresistor module's AO voltage falls as illumination rises.
            const int raw = (ldr_raw < 0) ? 0 : (ldr_raw > 4095 ? 4095 : ldr_raw);
            ldr_percent = ((4095 - raw) * 100 + 2047) / 4095;

            // Clamp ADC readings near either rail to the displayed endpoints.
            if (ldr_percent >= 99)
            {
                ldr_percent = 100;
            }
            else if (ldr_percent <= 1)
            {
                ldr_percent = 0;
            }
        }

        // ---------------------------
        // Read DHT22
        // ---------------------------

        esp_err_t result =
            dht.doubleRead(&temperature, &humidity);

        if (result == ESP_OK &&
            temperature >= -40.0f && temperature <= 80.0f &&
            humidity >= 0.0f && humidity <= 100.0f)
        {
            lastTemperature = temperature;
            lastHumidity = humidity;
        }

        // Use latest valid values
        temperature = lastTemperature;
        humidity = lastHumidity;

        // ---------------------------
        // Create sensor data
        // ---------------------------

        SensorData data;

        data.temperature = temperature;
        data.humidity = humidity;
        data.lightLevel = ldr_percent;
        data.motionDetected = motionDetected;

        // Send newest data to the queue
        xQueueOverwrite(
            sensorQueue,
            &data
        );

        // ---------------------------
        // Temperature alarm
        // ---------------------------

        AlarmState alarm =
            evaluateTemperature(temperature);

        if (alarm == AlarmState::HIGH_TEMPERATURE)
        {
            xEventGroupSetBits(
                systemEvents,
                EVENT_ALARM
            );
        }
        else
        {
            xEventGroupClearBits(
                systemEvents,
                EVENT_ALARM
            );
        }

        // DHT22 should not be read too quickly
        vTaskDelayUntil(
            &lastWakeTime,
            pdMS_TO_TICKS(2500)
        );
    }
}
