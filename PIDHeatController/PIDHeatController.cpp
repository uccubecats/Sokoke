#include <Arduino.h>
#include "../Sensors.h"

float integral, derivative;
float last_error;

float CalculatePID(float target, float current, float dt) {
    float error = target - current;
    integral += error * dt;
    derivative = (error - last_error) / dt;
    last_error = error;

    float output = kp * error + ki * integral + kd * derivative;

    output = std::max(0.0f, std::min(output, maxVal));

    return output;
}