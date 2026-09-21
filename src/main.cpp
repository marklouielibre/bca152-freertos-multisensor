#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "dht.h"

#define DHT_GPIO GPIO_NUM_4
#define LDR_GPIO GPIO_NUM_34

struct SensorData
{
    float temperature;
    float humidity;
    int lightLevel;
    bool motionDetected;
};

DHT dht(DHT_GPIO, DHT_TYPE_DHT22);
adc_oneshot_unit_handle_t adc_handle;

void SensorTask(void *pvParameters)
{
    TickType_t lastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        float temperature = 0;
        float humidity = 0;

        esp_err_t result = dht.doubleRead(&temperature, &humidity);

        int ldr_raw = 0;

        esp_err_t ldr_result = adc_oneshot_read(
            adc_handle,
            ADC_CHANNEL_6,
            &ldr_raw
        );

        if (ldr_result == ESP_OK)
        {
            int ldr_percent = (ldr_raw * 100) / 4095;
            printf("LDR Raw: %d\n", ldr_raw);
            printf("LDR: %d %%\n", ldr_percent);
        }
        else
        {
            printf("LDR read failed: %s\n", esp_err_to_name(ldr_result));
        }

        if (result == ESP_OK)
        {
            printf("Temperature: %.2f C\n", temperature);
            printf("Humidity: %.2f %%\n", humidity);
        }
        else
        {
            printf("DHT22 read failed: %s\n", esp_err_to_name(result));
        }

        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(2000));
    }
}

extern "C" void app_main()
{
    printf("BCA152 FreeRTOS Multisensor\n");
    printf("System starting...\n");

    adc_oneshot_unit_init_cfg_t adc_init_config = {};
    adc_init_config.unit_id = ADC_UNIT_1;

    esp_err_t adc_result = adc_oneshot_new_unit(&adc_init_config, &adc_handle);

    if (adc_result != ESP_OK)
    {
        printf("LDR ADC initialization failed: %s\n", esp_err_to_name(adc_result));
        return;
    }

    adc_oneshot_chan_cfg_t adc_channel_config = {};
    adc_channel_config.bitwidth = ADC_BITWIDTH_DEFAULT;
    adc_channel_config.atten = ADC_ATTEN_DB_12;

    adc_result = adc_oneshot_config_channel(
        adc_handle,
        ADC_CHANNEL_6,
        &adc_channel_config
    );

    if (adc_result != ESP_OK)
    {
        printf("LDR ADC channel configuration failed: %s\n", esp_err_to_name(adc_result));
        return;
    }

    esp_err_t result = dht.begin();

    if (result != ESP_OK)
    {
        printf("DHT22 initialization failed: %s\n", esp_err_to_name(result));
        return;
    }

        xTaskCreate(
        SensorTask,
        "SensorTask",
        4096,
        nullptr,
        5,
        nullptr
    );
}