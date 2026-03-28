#pragma once   

#include <Arduino.h>
#include <BluetoothSerial.h>


extern BluetoothSerial SerialBT;

void initializeINA228();
void ReadINA228();
void printINA228();