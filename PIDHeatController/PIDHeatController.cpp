#include <Arduino.h>
#include <PIDHeatController/PIDHeatController.h>

float kp = 0.8f;
float ki = 0.1f;
float kd = 2.0f;
float maxVal = 255.0f;

float integral = 0.0f;
float derivative = 0.0f;
float last_error = 0.0f;

float CalculatePID(float target, float current, float dt) {
    float error = target - current;
    integral += error * dt;
    derivative = (error - last_error) / dt;
    last_error = error;

    float output = kp * error + ki * integral + kd * derivative;

    output = std::max(0.0f, std::min(output, maxVal));

    return output;
}