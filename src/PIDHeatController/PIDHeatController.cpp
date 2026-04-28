#include "src/Sensors.h"
#include <Arduino.h>

float kp = 8.0f;
float ki = 0.0f;
float kd = 2.0f;
float targetTemperature = 35.0f;
float maxVal = 255.0f;

float integral = 0;
float derivative = 0;
float last_error = 0;
float filtered_derivative = 0;

const float DERIVATIVE_FILTER = 0.3f;

float CalculatePID(float target, float current, float dt) {
  if (dt <= 0) {
    return 0.0f;
  }

  float error = target - current;
  integral += error * dt;

  float maxIntegral = maxVal / (ki > 0 ? ki : 1.0f);
  integral = std::max(-maxIntegral, std::min(integral, maxIntegral));

  float raw_derivative = (error - last_error) / dt;

  filtered_derivative = (1.0f - DERIVATIVE_FILTER) * filtered_derivative +
                        DERIVATIVE_FILTER * raw_derivative;

  last_error = error;

  float output = kp * error + ki * integral + kd * filtered_derivative;
  return std::max(0.0f, std::min(output, maxVal));
}

void ResetPID() {
  integral = 0;
  derivative = 0;
  last_error = 0;
  filtered_derivative = 0;
}