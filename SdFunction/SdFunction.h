#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

extern SemaphoreHandle_t logMutex;

extern const uint32_t WRITE_INTERVAL_MS;

extern uint32_t lastWriteTime;
extern bool sdReady;

constexpr size_t CSV_LOG_BUFFER_SIZE = 96 * 1024; // 96KB 
extern char csvLogBuffer[CSV_LOG_BUFFER_SIZE];
extern size_t logBufferlen;

bool initSDCard();
bool LogWriteBuffer();
bool initRockblockBuffer();
bool sendRockblockBuffer();
void writeDataToBuffer(const char* name, float value);
void randomSensorData();
