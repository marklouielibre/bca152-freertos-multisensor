#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#ifdef PIO_UNIT_TEST
  #include <stdint.h>
  using TickType_t = uint32_t;     // Substitute for native testing
#else
  #include "freertos/FreeRTOS.h"
  #include "freertos/semphr.h"
#endif

enum class DisplayMode
{
    TEMPERATURE,
    HUMIDITY,
    LIGHT,
    MOTION
};

enum class AlarmState
{
    NORMAL,
    LOW_TEMPERATURE,
    HIGH_TEMPERATURE
};

enum class SystemState
{
    ACTIVE,
    INACTIVE
};

extern DisplayMode currentMode;
extern SystemState currentState;
extern const TickType_t INACTIVITY_TIMEOUT;

DisplayMode nextDisplayMode(DisplayMode mode);
DisplayMode previousDisplayMode(DisplayMode mode);

SystemState evaluateSystemState(
    SystemState currentState,
    bool motionDetected,
    bool timeoutOccurred
);

#endif