#pragma once

#include <Arduino.h>
#include "SdFunction/SdFunction.h"
#include "RockblockFunction/RockblockFunction.h"
#include <freertos/FreeRTOS.h>

/* ---------------------------- Invalid Constants --------------------------- */

constexpr float InvalidTemperature = -512.0f;
constexpr float InvalidHumidity = -1.0f;
constexpr float InvalidPressure = -1.0f;
constexpr float InvalidPosition = std::numeric_limits<float>::infinity();

constexpr float const INVALID_RESPONSES[] = { 
    InvalidTemperature,
    InvalidTemperature,
    InvalidTemperature,
    InvalidTemperature,
    InvalidTemperature,

    InvalidPressure,
    InvalidPressure,
 
    InvalidHumidity,

    InvalidPosition, InvalidPosition, InvalidPosition,
    InvalidPosition, InvalidPosition,
    InvalidPosition, InvalidPosition, InvalidPosition
}; 

/* ------------------------------ Sensor Names ------------------------------ */

// Sensor count needs to be last
typedef enum {
    TempIns,
    TempOut,
    BaroTempIns,
    BaroTempOut,
    MPUTemp,

    PressIns,
    PressOut,

    Humidity,

    PosX, PosY, PosZ,
    AngAccel, AngPos,
    AccX, AccY, AccZ,

    SENSOR_COUNT,
} SensorDataType;

inline const char *get_sensor_name(SensorDataType type) {
    static const char *names[] = {
        "TempIns",
        "TempOut",
        "BaroTempIns",
        "BaroTempOut",
        "MPUTemp",

        "PressIns",
        "PressOut",

        "Humidity",

        "PosX", "PosY", "PosZ",
        "AngAccel", "AngPos",
        "AccX", "AccY", "AccZ"
    };
    return names[type];
}
