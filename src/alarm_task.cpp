#include "alarm_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

#include "rtos_objects.h"

#define BUZZER_GPIO GPIO_NUM_26

void AlarmTask(void *pvParameters)
{
    ledc_timer_config_t timerConfig = {};
    timerConfig.speed_mode = LEDC_LOW_SPEED_MODE;
    timerConfig.duty_resolution = LEDC_TIMER_10_BIT;
    timerConfig.timer_num = LEDC_TIMER_0;
    timerConfig.freq_hz = 2000;
    timerConfig.clk_cfg = LEDC_AUTO_CLK;
    ledc_timer_config(&timerConfig);

    ledc_channel_config_t channelConfig = {};
    channelConfig.gpio_num = BUZZER_GPIO;
    channelConfig.speed_mode = LEDC_LOW_SPEED_MODE;
    channelConfig.channel = LEDC_CHANNEL_0;
    channelConfig.timer_sel = LEDC_TIMER_0;
    channelConfig.duty = 0;
    channelConfig.hpoint = 0;
    ledc_channel_config(&channelConfig);

    bool alarmOn = false;

    for (;;)
    {
        EventBits_t events = xEventGroupGetBits(systemEvents);
        const bool highTemperature = (events & EVENT_ALARM) != 0;

        if (highTemperature != alarmOn)
        {
            ledc_set_duty(
                LEDC_LOW_SPEED_MODE,
                LEDC_CHANNEL_0,
                highTemperature ? 512 : 0
            );
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            alarmOn = highTemperature;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
