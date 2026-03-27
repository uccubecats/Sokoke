#include <Arduino.h>
#include "../Sensors.h"

float integral   = 0;
float derivative = 0;
float last_error = 0;

float CalculatePID(float target, float current, float dt) {
    float error = target - current;
    integral += error * dt;
    derivative = (error - last_error) / dt;
    last_error = error;

    float output = kp * error + ki * integral + kd * derivative;
    return std::max(0.0f, std::min(output, maxVal));
}
