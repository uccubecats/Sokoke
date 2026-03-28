#pragma once

#include <Arduino.h>

extern float kp;
extern float ki;
extern float kd;
extern float targetTemperature;
extern float maxVal;

float CalculatePID(float target, float current, float dt);