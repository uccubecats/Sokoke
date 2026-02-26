#include <Arduino.h>
#include "SdFunction/SdFunction.h"

void setup() {
    Serial.begin(115200);
    randomSeed((uint32_t)esp_random());

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
        if (logBufferlen > 0) {
            LogWriteBuffer();
        }
    }
}
