#include <Arduino.h>
#include "BluetoothFunction.h"
#include "BluetoothSerial.h"
#include "src/log_wrapper/log_wrapper.h"

bool bluetooth_debug_info = 0;

BluetoothSerial SerialBT;
bool isConnected = false;

static const char* sppEventToString(esp_spp_cb_event_t event) {
    switch (event) {
        case ESP_SPP_INIT_EVT: return "INIT";
        case ESP_SPP_DISCOVERY_COMP_EVT: return "DISCOVERY_COMP";
        case ESP_SPP_OPEN_EVT: return "OPEN";
        case ESP_SPP_CLOSE_EVT: return "CLOSE";
        case ESP_SPP_START_EVT: return "START";
        case ESP_SPP_CL_INIT_EVT: return "CL_INIT";
        case ESP_SPP_DATA_IND_EVT: return "DATA_IND";
        case ESP_SPP_CONG_EVT: return "CONG";
        case ESP_SPP_WRITE_EVT: return "WRITE";
        case ESP_SPP_SRV_OPEN_EVT: return "SRV_OPEN";
        case ESP_SPP_SRV_STOP_EVT: return "SRV_STOP";
        case ESP_SPP_UNINIT_EVT: return "UNINIT";
        default: return "UNKNOWN";
    }
}


void connectionStatus(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
    if (!bluetooth_debug_info) return;
    lineoutPrintf("[BT] event=%d (%s)\n", (int)event, sppEventToString(event));

    if (param == nullptr) {
        lineout("[BT] callback param is null");
        return;
    }
    
    if (event == ESP_SPP_SRV_OPEN_EVT) {
        lineoutPrintf("[BT] srv_open status=%d handle=%lu\n", (int)param->srv_open.status, (unsigned long)param->srv_open.handle);
        if (param->srv_open.status == ESP_SPP_SUCCESS) {
            isConnected = true;
            // Serial.println("Client connected");
        }
    } else if (event == ESP_SPP_CLOSE_EVT) {
        isConnected = false;
        lineoutPrintf("[BT] close status=%d port_status=%lu async=%d\n", (int)param->close.status, (unsigned long)param->close.port_status, (int)param->close.async);
        lineout("Client disconnected");
    }
}

void initBluetooth() {
    if (!SerialBT.begin("ESP32-Serial", false)) {  
        lineout("Bluetooth failed to start!");
        lineoutPrintf("Error code: %d\n", SerialBT.getWriteError());
        return;
    }

    SerialBT.setTimeout(50);
    SerialBT.register_callback(connectionStatus);
    lineout("Bluetooth Started");
}
