#pragma once

#ifndef MAIN_SRC_SDFUNCTION_SDFUNCTION_H_
#define MAIN_SRC_SDFUNCTION_SDFUNCTION_H_

#include "SdFat.h"

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

extern SdFs sd;

extern SemaphoreHandle_t logMutex;

extern const uint32_t WRITE_INTERVAL_MS;
extern const uint32_t ROCKBLOCK_SEND_INTERVAL_MS;

extern uint32_t lastWriteTime;
extern bool sdReady;

constexpr size_t CSV_LOG_BUFFER_SIZE = 48 * 1024; // 48KB
static char *csvLogBuffer;
static size_t logBufferlen;

bool initSDCard();
bool LogWriteBuffer();
bool initRockblockBuffer();
bool sendRockblockBuffer();
void writeDataToBuffer(const char *name, float value);
void randomSensorData();

#endif // MAIN_SRC_SDFUNCTION_SDFUNCTION_H_
