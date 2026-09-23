#include "alarm.h"

AlarmState evaluateTemperature(float temperature)
{
    if (temperature < 20.0)
    {
        return AlarmState::LOW_TEMPERATURE;
    }

    if (temperature > 30.0)
    {
        return AlarmState::HIGH_TEMPERATURE;
    }

    return AlarmState::NORMAL;
}