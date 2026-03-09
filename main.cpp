#include <Arduino.h>
#include "BluetoothFunction/BluetoothFunction.h"

void setup() {
    Serial.begin(115200);
    initBluetooth();
}

void loop() {
    bluetoothDebug(isConnected);
    delay(100);
}