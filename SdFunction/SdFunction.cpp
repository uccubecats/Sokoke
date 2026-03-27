#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include "RockblockFunction/RockblockFunction.h"
#include "SdFunction.h"
#include "../Sensors.h"

#define SD_CS_PIN 5
#define MUTEX_TIMEOUT_MS 5000

const uint32_t WRITE_INTERVAL_MS = 60000; // 1 minute
const char* CSV_FILE_PATH = "/sensor_log.csv";
uint32_t lastWriteTime = 0;
bool sdReady = false;

char csvLogBuffer[CSV_LOG_BUFFER_SIZE];
size_t logBufferlen = 0;

static const char *CSV_COLUMNS[SensorDataType::SENSOR_COUNT];
static const int NUM_SENSORS = SensorDataType::SENSOR_COUNT;

static const uint32_t ROCKBLOCK_SEND_INTERVAL_MS = 60000;
static Table* rockblockTable = NULL;
static uint32_t lastRockblockSendTime = 0;

bool initRockblockBuffer() {
    if (!rockblockTable) {
        rockblockTable = new_table();
    }
    return rockblockTable != NULL;
}

bool sendRockblockBuffer() {
    if (!rockblockTable || rockblockTable->size == 0) {
        return true;
    }

    uint32_t now = millis();
    if (now - lastRockblockSendTime < ROCKBLOCK_SEND_INTERVAL_MS) {
        return true;
    }

    send_table(rockblockTable);
    free_table(rockblockTable);
    rockblockTable = new_table();
    lastRockblockSendTime = now;
    return rockblockTable != NULL;
}

bool initSDCard() { // Init SD card and create CSV file with header if it doesn't exist

    // SD Card Initialization
    if (!SD.begin(SD_CS_PIN)) {
        Serial.println("Card Mount Failed");
        return false;
    }

    for (size_t i = 0; i < SensorDataType::SENSOR_COUNT; i++) {
        CSV_COLUMNS[i] = get_sensor_name((SensorDataType)i);
    }

    uint64_t cardSize = SD.cardSize() / (1000 * 1000 * 1000);
    Serial.printf("SD Card Size: %lluGB\n", cardSize);


    // CSV Creation
    if (!SD.exists(CSV_FILE_PATH)) {
        File headerFile = SD.open(CSV_FILE_PATH, FILE_WRITE);
        if (!headerFile) {
            Serial.println("Failed to create CSV file");
            return false;
        }
        headerFile.print("timestamp_ms");
        for (int i = 0; i < NUM_SENSORS; i++) {
            headerFile.print(",");
            headerFile.print(CSV_COLUMNS[i]);
        }
        headerFile.println();
        headerFile.close();
    }

    return true;
}

bool LogWriteBuffer() { // Write log buffer to SD card and clear buffer
    uint32_t now = millis();
    if (logBufferlen == 0) {
        return true;
    }
    
    if (xSemaphoreTake(logMutex, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS)) != pdTRUE) {
        Serial.println("Failed to lock log mutex");
        return false;
    }

    File logFile = SD.open(CSV_FILE_PATH, FILE_APPEND);
    if (!logFile) {
        Serial.println("Failed to open CSV file");
        xSemaphoreGive(logMutex);
        return false;
    }

    size_t bytesWritten = logFile.write((const uint8_t*)csvLogBuffer, logBufferlen);
    logFile.close();

    if (bytesWritten != logBufferlen) {
        Serial.println("partial write to CSV file");
        xSemaphoreGive(logMutex);
        return false;
    }

    // Clear the buffer while holding the mutex to avoid races with writers.
    logBufferlen = 0;

    uint32_t duration = millis() - now;
    Serial.printf("Wrote %u bytes to SD in %u ms\n", bytesWritten, duration);
    xSemaphoreGive(logMutex);
    return true;
}


void writeDataToBuffer(const char* name, float value) { // Write data to log buffer
    if (rockblockTable) {
        
        int index = -1;
        for (int i = 0; i < NUM_SENSORS; i++) {
            if (strcmp(name, CSV_COLUMNS[i]) == 0) {
                index = i;
                break;
            }
        }

        if (table_memsize(rockblockTable) + sizeof(TableEntry) + 2 <= 340 && index != -1) { // Check if adding this entry would exceed the 340 byte SBD limit (with some buffer for metadata)
            add_entry(rockblockTable, (TableEntry){ .time = millis(), .type = (SensorDataType)index, .data = value, });
        } else if (index == -1) {
            Serial.println("Unknown sensor name, not adding to rockblock buffer value for " + String(name)); 
        }
    }

    for (int i = 0; i < NUM_SENSORS; i++) { 
        if (strcmp(name, CSV_COLUMNS[i]) == 0) { 
            char entry[64];

            int pos = snprintf(entry, sizeof(entry), "%u", millis()); 

            for (int j = 0; j < NUM_SENSORS; j++) { 
                if (j == i) {
                    int written = snprintf(entry + pos, sizeof(entry) - pos, ",%.2f", value);
                    if (written < 0 || pos + written >= (int)sizeof(entry)) { 
                        Serial.println("Failed to format CSV row");
                        return;
                    }
                    pos += written;
                } else {
                    entry[pos++] = ',';
                }
            }
            entry[pos++] = '\n';

            // Mutex Error Handling and Buffer Management

            if (xSemaphoreTake(logMutex, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS)) != pdTRUE) {
                Serial.println("Failed to lock log mutex");
                return;
            }

            if (logBufferlen + pos >= sizeof(csvLogBuffer)) {
                Serial.println("Log buffer overflow, flushing to SD");
                xSemaphoreGive(logMutex);
                if (!LogWriteBuffer()) {
                    return;
                }
                if (xSemaphoreTake(logMutex, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS)) != pdTRUE) {
                    Serial.println("Failed to re-lock log mutex");
                    return;
                }
            }

            memcpy(csvLogBuffer + logBufferlen, entry, pos); // Append new entry to log buffer
            logBufferlen += pos;

            xSemaphoreGive(logMutex);
            return;
        }
    }
}

//temp

static uint32_t nextTempTime = 0;
static uint32_t nextPressureTime = 0;

void randomSensorData() {
    uint32_t now = millis();
    if (now >= nextTempTime) {
        writeDataToBuffer("ATH30_temperature", random(200, 301) / 10.0f);
        nextTempTime = now + random(5, 50);
    }
    if (now >= nextPressureTime) {
        writeDataToBuffer("BMP390_pressure", random(9000, 11001) / 10.0f);
        nextPressureTime = now + random(30, 200);
    }
}
