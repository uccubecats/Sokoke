#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include "SdFunction.h"

#define SD_CS_PIN 5
#define MUTEX_TIMEOUT_MS 5000

const uint32_t LOG_INTERVAL_MS = 100;
const uint32_t WRITE_INTERVAL_MS = 1000;
const char* CSV_FILE_PATH = "/sensor_log.csv";

uint32_t lastLogTime = 0;
uint32_t lastWriteTime = 0;
bool sdReady = false;

char logBuffer[512];
size_t logBufferlen = 0;

bool initSDCard() {
    if (!SD.begin(SD_CS_PIN)) {
        Serial.println("Card Mount Failed");
        return false;
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    if (!SD.exists(CSV_FILE_PATH)) {
        File headerFile = SD.open(CSV_FILE_PATH, FILE_WRITE);
        if (!headerFile) {
            Serial.println("Failed to create CSV file");
            return false;
        }
        headerFile.println("timestamp_ms,temperature_c,pressure_hpa");
        headerFile.close();
    }

    return true;
}

bool LogWriteBuffer() { 
    if (logBufferlen == 0) {
        return true;
    }
    
    if (xSemaphoreTake(logMutex, MUTEX_TIMEOUT_MS) != pdTRUE) {
        Serial.println("Failed to lock log mutex");
        return false;
    }

    File logFile = SD.open(CSV_FILE_PATH, FILE_APPEND);
    if (!logFile) {
        Serial.println("Failed to open CSV file");
        return false;
    }

    size_t bytesWritten = logFile.write((const uint8_t*)logBuffer, logBufferlen);
    logFile.close();

    xSemaphoreGive(logMutex);

        if (bytesWritten != logBufferlen) {
        Serial.println("partial write to CSV file");
        return false;
    }

    logBufferlen = 0;
    return true;
}


void writeDataToBuffer(uint32_t timestamp, float temperature, float pressure) { 
    char entry[64];
    int entryLen = snprintf(entry, sizeof(entry), "%lu,%.1f,%.1f\n", timestamp, temperature, pressure);
    if (entryLen < 0 || entryLen >= sizeof(entry)) {
        Serial.println("Failed to format log entry");
        return;
    }

    if (xSemaphoreTake(logMutex, MUTEX_TIMEOUT_MS) != pdTRUE) {
        Serial.println("Failed to lock log mutex");
        return;
    }

    if (logBufferlen + entryLen >= sizeof(logBuffer)) {
        Serial.println("Log buffer overflow, flushing to SD");
        if (!LogWriteBuffer()) {
            xSemaphoreGive(logMutex);
            return;
        }
    }

    memcpy(logBuffer + logBufferlen, entry, entryLen);
    logBufferlen += entryLen;

    xSemaphoreGive(logMutex);
}

