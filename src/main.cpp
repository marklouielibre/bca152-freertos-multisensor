#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"

#include "esp_adc/adc_oneshot.h"

#include "driver/gpio.h"

#include "dht.h"
#include "ssd1306.h"

#include "alarm.h"
#include "system_state.h"
#include "rtos_objects.h"
#include "sensors.h"
#include "display.h"
#include "input.h"
#include "motion.h"
#include "alarm_task.h"

#ifndef PIO_UNIT_TESTING

extern "C" void app_main()
{
    // ---------------------------
    // OLED initialization
    // ---------------------------

    i2c_master_init(
        &oled,
        GPIO_NUM_21,
        GPIO_NUM_22,
        -1
    );

    ssd1306_init(
        &oled,
        128,
        64
    );

    ssd1306_clear_screen(
        &oled,
        false
    );

    ssd1306_display_text(
        &oled,
        0,
        "BCA152",
        6,
        false
    );

    ssd1306_display_text(
        &oled,
        1,
        "OLED Ready",
        10,
        false
    );


    // ---------------------------
    // Queue creation
    // ---------------------------

    sensorQueue = xQueueCreate(1, sizeof(SensorData));

    if (sensorQueue == NULL)
    {
        printf("Sensor queue creation failed\n");
        return;
    }


    // ---------------------------
    // Serial mutex
    // ---------------------------

    serialMutex = xSemaphoreCreateMutex();

    if (serialMutex == NULL)
    {
        printf("Serial mutex creation failed\n");
        return;
    }


    // ---------------------------
    // Event group
    // ---------------------------

    systemEvents = xEventGroupCreate();

    if (systemEvents == NULL)
    {
        printf("Event group creation failed\n");
        return;
    }

    xEventGroupSetBits(systemEvents, EVENT_ACTIVE);

    // ---------------------------
    // ADC initialization
    // ---------------------------

    adc_oneshot_unit_init_cfg_t adc_init_config = {};

    adc_init_config.unit_id = ADC_UNIT_1;

    if (adc_oneshot_new_unit(
            &adc_init_config,
            &adc_handle) != ESP_OK)
    {
        printf("LDR ADC initialization failed\n");
        return;
    }


    adc_oneshot_chan_cfg_t adc_channel_config = {};

    adc_channel_config.bitwidth = ADC_BITWIDTH_DEFAULT;
    adc_channel_config.atten = ADC_ATTEN_DB_12;

    if (adc_oneshot_config_channel(
            adc_handle,
            ADC_CHANNEL_6,
            &adc_channel_config) != ESP_OK)
    {
        printf("LDR ADC channel configuration failed\n");
        return;
    }


    // ---------------------------
    // DHT initialization
    // ---------------------------

    if (dht.begin() != ESP_OK)
    {
        printf("DHT22 initialization failed\n");
        return;
    }

    // Give the DHT22 time to stabilize
    vTaskDelay(pdMS_TO_TICKS(2000));


    // ---------------------------
    // RTOS tasks
    // ---------------------------

    // AlarmTask - Priority 2
    xTaskCreate(
        AlarmTask,
        "AlarmTask",
        2048,
        nullptr,
        2,
        nullptr
    );



    // DisplayTask - Priority 1
    xTaskCreate(
        DisplayTask,
        "DisplayTask",
        4096,
        nullptr,
        4,
        nullptr
    );


    // SensorTask - Priority 2
    xTaskCreate(
        SensorTask,
        "SensorTask",
        4096,
        nullptr,
        2,
        nullptr
    );

    // InputTask - Priority 3
    xTaskCreate(
        InputTask,
        "InputTask",
        2048,
        nullptr,
        3,
        nullptr
    );


    // MotionTask - Priority 3
    xTaskCreate(
        MotionTask,
        "MotionTask",
        2048,
        nullptr,
        3,
        nullptr
    );
}

#endif