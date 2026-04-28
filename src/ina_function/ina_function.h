#pragma once

#include <Adafruit_INA228.h>
#include <Adafruit_INA219.h>

typedef struct {
    float busVoltage;
    float current;
    bool success;
} InaData;

InaData read_ina228(Adafruit_INA228 *ina, bool ina_alive);
InaData read_ina219(Adafruit_INA219 *ina, bool ina_alive);