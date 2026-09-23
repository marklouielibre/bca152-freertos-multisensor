#include "system_state.h"

DisplayMode currentMode = DisplayMode::TEMPERATURE;
SystemState currentState = SystemState::ACTIVE;

DisplayMode nextDisplayMode(DisplayMode mode)
{
    switch (mode)
    {
        case DisplayMode::TEMPERATURE: return DisplayMode::HUMIDITY;
        case DisplayMode::HUMIDITY:    return DisplayMode::LIGHT;
        case DisplayMode::LIGHT:       return DisplayMode::MOTION;
        case DisplayMode::MOTION:      return DisplayMode::TEMPERATURE;
    }
    return DisplayMode::TEMPERATURE;
}

DisplayMode previousDisplayMode(DisplayMode mode)
{
    switch (mode)
    {
        case DisplayMode::TEMPERATURE: return DisplayMode::MOTION;
        case DisplayMode::HUMIDITY:    return DisplayMode::TEMPERATURE;
        case DisplayMode::LIGHT:       return DisplayMode::HUMIDITY;
        case DisplayMode::MOTION:      return DisplayMode::LIGHT;
    }
    return DisplayMode::TEMPERATURE;
}

SystemState evaluateSystemState(
    SystemState currentState,
    bool motionDetected,
    bool timeoutOccurred)
{
    if (currentState == SystemState::ACTIVE)
    {
        if (timeoutOccurred)
            return SystemState::INACTIVE;

        return SystemState::ACTIVE;
    }

    if (motionDetected)
        return SystemState::ACTIVE;

    return SystemState::INACTIVE;
}