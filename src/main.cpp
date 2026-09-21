#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "dht.h"

#define DHT_GPIO GPIO_NUM_4

extern "C" void app_main()
{
    printf("BCA152 FreeRTOS Multisensor\n");
    printf("System starting...\n");

    DHT dht(DHT_GPIO, DHT_TYPE_DHT22);

    esp_err_t result = dht.begin();

    if (result != ESP_OK)
    {
        printf("DHT22 initialization failed: %s\n", esp_err_to_name(result));
        return;
    }

    while (1)
    {
        float temperature = 0;
        float humidity = 0;

        result = dht.doubleRead(&temperature, &humidity);

        if (result == ESP_OK)
        {
            printf("Temperature: %.2f C\n", temperature);
            printf("Humidity: %.2f %%\n", humidity);
        }
        else
        {
            printf("DHT22 read failed: %s\n", esp_err_to_name(result));
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}