## Description

An ESP-IDF library for the DHT series of low-cost temperature/humidity sensors.

You can find DHT tutorials [here](https://learn.adafruit.com/dht).

# Requirements
 * Library built as c++. In case of ESP-IDF examples (written in pure C) you need to rename main.c into main.cpp and add <code>extern "C"</code> before app_main() function

        /**
         *  @file main.cpp
         *  
         */
        
        #include "DHT.h"
    
        extern "C" void app_main() {
          DHT dht( GPIO_NUM_4, DHT_TYPE_AM2301, 0 );
        }

# Example
    #include "esp_sleep.h"

    #include "DHT.h"

    static const char *TAG = "ESP32-DHT";
    RTC_DATA_ATTR uint32_t wakeup_count = 0;

    extern "C" void app_main() {
        DHT dht( GPIO_NUM_4, DHT_TYPE_AM2301 );
        dht.debug_enabled = true;
        dht.begin();

        uint32_t sleep_time_ms;
        if ( wakeup_count % 2 == 0 ) { // Zero and Even runs we "wakeup" sensor. Odd runs (after double_read_interval_ms sleep) - reads fresh data
            dht.sensorWakeup();
            sleep_time_ms = dht.double_read_interval_ms;
            ESP_LOGI( TAG, "dht.sensorWakeup(). Going to deep sleep while sensor not ready to read fresh data for %dms", sleep_time_ms );
        } else {
            float t;
            float h;
            esp_err_t ret = dht.singleRead( &t, &h );
            if ( ret != ESP_OK ) {
                ESP_LOGE( TAG, "dht.singleRead() failed with error 0x%x", ret );
            } else {
                ESP_LOGI( TAG, "dht.singleRead() done successfully. t=%f; h=%f", t, h );
            }
            sleep_time_ms = 30 * 1000;
            ESP_LOGI( TAG, "Going to deep sleep for %dms", sleep_time_ms );
        }
        wakeup_count++;
        esp_sleep_enable_timer_wakeup( sleep_time_ms * 1000 );
        esp_deep_sleep_start();
    }

# Dependencies
 * Espressif ESP-IDF framework

# Contributing
Contributions are welcome!  Not only you’ll encourage the development of the library, but you’ll also learn how to best use the library and probably some C++ too

MIT license, check LICENSE for more information
All text above must be included in any redistribution
