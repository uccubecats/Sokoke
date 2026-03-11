#pragma once

#include <Arduino.h>

float temperature;
uint8_t humidity;

void initAht30();
void readAht30();