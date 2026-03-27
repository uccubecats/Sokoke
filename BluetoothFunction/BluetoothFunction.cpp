#include <Arduino.h>
#include "BluetoothFunction.h"
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
bool isConnected = false;


void connectionStatus(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
    Serial.printf("[BT] event=%d (%s)\n", (int)event, event);
    
    if (event == ESP_SPP_SRV_OPEN_EVT) {
        Serial.printf("[BT] srv_open status=%d handle=%lu\n", (int)param->srv_open.status, (unsigned long)param->srv_open.handle);
        if (param->srv_open.status == ESP_SPP_SUCCESS) {
            isConnected = true;
            Serial.println("Client connected");
        }
    } else if (event == ESP_SPP_CLOSE_EVT) {
        isConnected = false;
        Serial.printf("[BT] close status=%d port_status=%lu async=%d\n", (int)param->close.status, (unsigned long)param->close.port_status, (int)param->close.async);
        Serial.println("Client disconnected");
    }
}

void initBluetooth() {
    if (!SerialBT.begin("ESP32-Serial", false)) {  
        Serial.println("Bluetooth failed to start!");
        printf("Error code: %d\n", SerialBT.getWriteError());
        return;
    }

    SerialBT.setTimeout(50);
    SerialBT.register_callback(connectionStatus);
    Serial.println("Bluetooth Started");
}
