#include <Arduino.h>
#include "BluetoothFunction.h"
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
bool isConnected = false;

void connectionStatus(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
    if (event == ESP_SPP_SRV_OPEN_EVT) {
        isConnected = true;
    } else if (event == ESP_SPP_CLOSE_EVT) {
        isConnected = false;
    }
}

void bluetoothDebug(bool connected) {
    static unsigned long lastTime = 0;
    if (connected) {
        if (millis() - lastTime > 1000) {
            lastTime = millis();
            SerialBT.println("Hello from ESP32");
            Serial.println("Sent over Bluetooth");
        }
    } 
}

void initBluetooth() {
    if (!SerialBT.begin("ESP32-Serial")) {  
        Serial.println("Bluetooth failed to start!");
        while (true) {
            delay(1000); 
        }
    }
    SerialBT.register_callback(connectionStatus);
    Serial.println("Bluetooth Started");
}
