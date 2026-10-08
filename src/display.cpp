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
    char titleLine[17];
    char valueLine[17];
    char lastTitle[16] = {};
    char lastLine[20] = {};
    bool hasRendered = false;
    bool screenIsClear = true;
    bool headerDrawn = false;

    // Clear the boot screen once. Normal page updates only rewrite text rows.
    ssd1306_clear_screen(&oled, false);

    for (;;)
    {
        EventBits_t events = xEventGroupGetBits(systemEvents);

        // Wait for the first sample, then read the most recent sensor values.
        xQueuePeek(sensorQueue, &data, portMAX_DELAY);
        if ((events & EVENT_ACTIVE) == 0)
        {
            if (!screenIsClear)
            {
                ssd1306_clear_screen(&oled, false);
                screenIsClear = true;
                hasRendered = false;
                headerDrawn = false;
            }
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        const char *title = "";

        switch (currentMode)
        {
            case DisplayMode::TEMPERATURE:
                title = "Temperature";
                snprintf(line, sizeof(line), "%.1f C", data.temperature);
                break;

            case DisplayMode::HUMIDITY:
                title = "Humidity";
                snprintf(line, sizeof(line), "%.1f %%", data.humidity);
                break;

            case DisplayMode::LIGHT:
                title = "Light";
                snprintf(line, sizeof(line), "%d %%", data.lightLevel);
                break;

            case DisplayMode::MOTION:
                title = "Motion";
                snprintf(line, sizeof(line), "%s",
                         data.motionDetected ? "YES" : "NO");
                break;
        }

        // Update both displays only when the visible page or value changes.
        if (!hasRendered || strcmp(title, lastTitle) != 0 || strcmp(line, lastLine) != 0)
        {
            memset(titleLine, ' ', 16);
            titleLine[16] = '\0';
            memcpy(titleLine, title, strlen(title) < 16 ? strlen(title) : 16);
            memset(valueLine, ' ', 16);
            valueLine[16] = '\0';
            memcpy(valueLine, line, strlen(line) < 16 ? strlen(line) : 16);

            if (!headerDrawn)
            {
                ssd1306_display_text(&oled, 0, "ROOM MONITOR", 12, false);
                headerDrawn = true;
            }
            ssd1306_display_text(&oled, 2, titleLine, 16, false);
            ssd1306_display_text(&oled, 4, valueLine, 16, false);
            screenIsClear = false;

            xSemaphoreTake(serialMutex, portMAX_DELAY);
            printf("\nROOM MONITOR\n%s\n%s\n", title, line);
            xSemaphoreGive(serialMutex);

            snprintf(lastTitle, sizeof(lastTitle), "%s", title);
            snprintf(lastLine, sizeof(lastLine), "%s", line);
            hasRendered = true;
        }

        // Let sensor/input tasks run and refresh promptly after a new sample or page change.
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
