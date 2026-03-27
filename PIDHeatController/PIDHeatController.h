#pragma once

#include <Arduino.h>

float kp = 20.0f;
float ki = 1.0f;
float kd = 5.0f;
float targetTemperature = 30.0f;
float maxVal = 255.0f;

float CalculatePID(float target, float current, float dt);