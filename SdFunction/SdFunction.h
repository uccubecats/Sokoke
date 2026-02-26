#pragma once

#include <Arduino.h>

extern const uint32_t LOG_INTERVAL_MS;
extern const uint32_t WRITE_INTERVAL_MS;

extern uint32_t lastLogTime;
extern uint32_t lastWriteTime;
extern bool sdReady;

extern char logBuffer[512];
extern size_t logBufferlen;

bool initSDCard();
bool LogWriteBuffer();
void writeDataToBuffer(uint32_t timestamp, float temperature, float pressure);
void randomSensorData();
