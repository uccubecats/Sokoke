#include <Arduino.h>
#include "SdFunction/SdFunction.h"

SemaphoreHandle_t logMutex = NULL;

void setup() {
    Serial.begin(115200);
    randomSeed((uint32_t)esp_random());

    logMutex = xSemaphoreCreateMutex();
    if (logMutex == NULL) {
        Serial.println("Failed to create mutex");
        while (true) {
            delay(1000);
        }
    }

    

    sdReady = initSDCard();
}

void loop() {
    if (!sdReady) {
        delay(1000);
        return;
    }

    uint32_t now = millis();
    if (now - lastWriteTime >= WRITE_INTERVAL_MS) {
        lastWriteTime = now;
        LogWriteBuffer();
    }
}
