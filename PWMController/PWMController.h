#pragma once

#include <Arduino.h>

/****************************************************
 * ESP32 MOSFET PWM Control via Serial Monitor
 * MOSFET: FQP30N06L
 ****************************************************/

// PWM Settings
const int pwmPin = 13;      // Connect to the Gate of the MOSFET
const int freq = 5000;      // 5 kHz frequency
const int ledChannel = 0;   // PWM channel (0-15)
const int resolution = 8;   // 8-bit resolution (0-255)

void PWMSetup();
