#pragma once

#include <Arduino.h>
#include <BluetoothSerial.h>
#include "src/log_wrapper/log_wrapper.h"

extern BluetoothSerial SerialBT;


void PWMSetup(int pwmPin, int freq, int resolution);
