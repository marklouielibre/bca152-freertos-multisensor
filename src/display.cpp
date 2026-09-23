#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ssd1306.h"

#include "display.h"
#include "rtos_objects.h"
#include "system_state.h"

void DisplayTask(void *pvParameters)
{
    SensorData data = {};
    char line[20];

    for (;;)
    {
        EventBits_t events = xEventGroupGetBits(systemEvents);

        // Get the latest data if available (don't block)
        xQueuePeek(sensorQueue, &data, portMAX_DELAY);
        if ((events & EVENT_ACTIVE) == 0)
        {
            ssd1306_clear_screen(&oled, false);
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        ssd1306_clear_screen(&oled, false);
        ssd1306_display_text(&oled, 0, "ROOM MONITOR", 12, false);

        switch (currentMode)
        {
            case DisplayMode::TEMPERATURE:
                ssd1306_display_text(&oled, 2, "Temperature", 11, false);
                snprintf(line, sizeof(line), "%.1f C", data.temperature);
                break;

            case DisplayMode::HUMIDITY:
                ssd1306_display_text(&oled, 2, "Humidity", 8, false);
                snprintf(line, sizeof(line), "%.1f %%", data.humidity);
                break;

            case DisplayMode::LIGHT:
                ssd1306_display_text(&oled, 2, "Light", 5, false);
                snprintf(line, sizeof(line), "%d %%", data.lightLevel);
                break;

            case DisplayMode::MOTION:
                ssd1306_display_text(&oled, 2, "Motion", 6, false);
                snprintf(line, sizeof(line), "%s",
                         data.motionDetected ? "YES" : "NO");
                break;
        }

        ssd1306_display_text(&oled, 4, line, strlen(line), false);

        //vTaskDelay(pdMS_TO_TICKS(1000));
    }
}