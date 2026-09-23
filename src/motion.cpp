#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include "motion.h"
#include "rtos_objects.h"
#include "system_state.h"

#define PIR_GPIO GPIO_NUM_27

const TickType_t INACTIVITY_TIMEOUT = pdMS_TO_TICKS(15000);

void MotionTask(void *pvParameters)
{
    gpio_set_direction(PIR_GPIO, GPIO_MODE_INPUT);

    lastMotionTick = xTaskGetTickCount();

    for (;;)
    {
        if (gpio_get_level(PIR_GPIO))
        {
            motionDetected = true;

            lastMotionTick = xTaskGetTickCount();

            xEventGroupSetBits(
                systemEvents,
                EVENT_ACTIVE | EVENT_MOTION
            );

            currentState = SystemState::ACTIVE;
        }

        if ((xTaskGetTickCount() - lastMotionTick) >
            INACTIVITY_TIMEOUT)
        {
            motionDetected = false;

            xEventGroupClearBits(
                systemEvents,
                EVENT_ACTIVE | EVENT_MOTION
            );

            currentState = SystemState::INACTIVE;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}