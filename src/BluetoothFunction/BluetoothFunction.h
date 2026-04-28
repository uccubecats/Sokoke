#pragma once

#include <Arduino.h>
#include "BluetoothSerial.h"

extern bool bluetooth_debug_info;
extern bool isConnected;
extern BluetoothSerial SerialBT;

void initBluetooth();
