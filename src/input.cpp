#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include "input.h"
#include "rtos_objects.h"
#include "system_state.h"

#define ENC_CLK GPIO_NUM_18
#define ENC_DT  GPIO_NUM_19
#define ENC_SW  GPIO_NUM_5

void InputTask(void *pvParameters)
{
    gpio_set_direction(ENC_CLK, GPIO_MODE_INPUT);
    gpio_set_direction(ENC_DT, GPIO_MODE_INPUT);
    gpio_set_direction(ENC_SW, GPIO_MODE_INPUT);

    gpio_set_pull_mode(ENC_CLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(ENC_DT, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(ENC_SW, GPIO_PULLUP_ONLY);

    int lastCLK = gpio_get_level(ENC_CLK);

    for (;;)
    {
        EventBits_t events = xEventGroupGetBits(systemEvents);

        if ((events & EVENT_ACTIVE) == 0)
        {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        int currentCLK = gpio_get_level(ENC_CLK);

        if (currentCLK != lastCLK && currentCLK == 1)
        {
            if (gpio_get_level(ENC_DT) != currentCLK)
            {
                switch (currentMode)
                {
                    case DisplayMode::TEMPERATURE:
                        currentMode = DisplayMode::HUMIDITY;
                        break;

                    case DisplayMode::HUMIDITY:
                        currentMode = DisplayMode::LIGHT;
                        break;

                    case DisplayMode::LIGHT:
                        currentMode = DisplayMode::MOTION;
                        break;

                    case DisplayMode::MOTION:
                        currentMode = DisplayMode::TEMPERATURE;
                        break;
                }
            }
            else
            {
                switch (currentMode)
                {
                    case DisplayMode::TEMPERATURE:
                        currentMode = DisplayMode::MOTION;
                        break;

                    case DisplayMode::HUMIDITY:
                        currentMode = DisplayMode::TEMPERATURE;
                        break;

                    case DisplayMode::LIGHT:
                        currentMode = DisplayMode::HUMIDITY;
                        break;

                    case DisplayMode::MOTION:
                        currentMode = DisplayMode::LIGHT;
                        break;
                }
            }

            xSemaphoreTake(serialMutex, portMAX_DELAY);

            printf("\nROOM MONITOR\n");

            switch (currentMode)
            {
                case DisplayMode::TEMPERATURE:
                    printf("Page: Temperature\n");
                    break;

                case DisplayMode::HUMIDITY:
                    printf("Page: Humidity\n");
                    break;

                case DisplayMode::LIGHT:
                    printf("Page: Light\n");
                    break;

                case DisplayMode::MOTION:
                    printf("Page: Motion\n");
                    break;
            }

            xSemaphoreGive(serialMutex);
        }

        lastCLK = currentCLK;

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}