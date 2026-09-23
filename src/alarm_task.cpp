#include "alarm_task.h"

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "rtos_objects.h"

#define BUZZER_GPIO GPIO_NUM_26

void AlarmTask(void *pvParameters)
{
    gpio_set_direction(BUZZER_GPIO, GPIO_MODE_OUTPUT);

    bool alarmPrinted = false;

    for (;;)
    {
        EventBits_t events = xEventGroupGetBits(systemEvents);

        if (events & EVENT_ALARM)
        {
            gpio_set_level(BUZZER_GPIO, 1);

            if (!alarmPrinted)
            {
                xSemaphoreTake(serialMutex, portMAX_DELAY);
                printf("\n*** ALARM: HIGH TEMPERATURE ***\n\n");
                xSemaphoreGive(serialMutex);
                alarmPrinted = true;
            }
        }
        else
        {
            gpio_set_level(BUZZER_GPIO, 0);
            alarmPrinted = false;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}