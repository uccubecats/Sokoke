#pragma once

#include <Arduino.h>

#include <BluetoothSerial.h>

extern BluetoothSerial SerialBT;

extern float temperature;
extern uint8_t humidity;

void initAht30();
float readAht30();
void printAht30();