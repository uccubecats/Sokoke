#pragma once

#include <Arduino.h>
#include "BluetoothSerial.h"

extern bool isConnected;
extern BluetoothSerial SerialBT;

void initBluetooth();
