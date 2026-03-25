#pragma once

#include <Arduino.h>

float temperature;
uint8_t humidity;

bool initAht30();
std::tuple<float, uint8_t, bool> readAht30();