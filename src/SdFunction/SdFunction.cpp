#include <Arduino.h>
// #include <SD.h>
#include "src/RockblockFunction/RockblockFunction.h"
#include "src/SdFunction/SdFunction.h"
#include "src/Sensors.h"
#include "src/log_wrapper/log_wrapper.h"
#include <SPI.h>

#define SD_CS_PIN 5
#define MUTEX_TIMEOUT_MS 5000

SdFs sd;

const uint32_t WRITE_INTERVAL_MS = 30000; // 30 seconds
const char *CSV_FILE_PATH = "/sensor_log.csv";
uint32_t lastWriteTime = 0;
bool sdReady = false;

static const char *CSV_COLUMNS[SensorDataType::SENSOR_COUNT];
static const int NUM_SENSORS = SensorDataType::SENSOR_COUNT;

const uint32_t ROCKBLOCK_SEND_INTERVAL_MS = 60000 * 5; // 5 minutes
static Table *rockblockTable = NULL;
static uint32_t lastRockblockSendTime = 0;

bool initRockblockBuffer() {
  lineoutPrintf("Attempting to allocate: %zu\nFree heap: %zu\n",
                CSV_LOG_BUFFER_SIZE * sizeof(char), ESP.getFreeHeap());
  csvLogBuffer =
      (char *)malloc(CSV_LOG_BUFFER_SIZE *
                     sizeof(char)); // shouldn't require the sizeof part but you
                                    // never know with these things
  logBufferlen = 0;
  if (csvLogBuffer != NULL) {
    lineoutPrintf("Memory for RockBlock Buffer allocated at 0x%p\n",
                  csvLogBuffer);
  } else {
    lineout("Failed to allocate memory for csvLogBuffer");
    return false;
  }

  if (!rockblockTable) {
    rockblockTable = new_table();
  }
  return rockblockTable != NULL;
}

bool sendRockblockBuffer() {
  if (!rockblockTable || rockblockTable->size == 0) {
    lineout("Rockblock buffer empty, skipping send");
    return true;
  }
  lineout("Sending rockblock buffer indie SD Card");
  send_table(rockblockTable);
  free_table(rockblockTable);
  rockblockTable = new_table();
  return rockblockTable != NULL;
}

bool initSDCard() { // Init SD card and create CSV file with header if it
                    // doesn't exist
  for (size_t i = 0; i < SensorDataType::SENSOR_COUNT; i++) {
    CSV_COLUMNS[i] = get_sensor_name((SensorDataType)i);
    // lineoutPrintf("initializing #%zu: %s\n", i, CSV_COLUMNS[i]);
  }

  // SD Card Initialization
  if (!sd.begin(SD_CS_PIN, SD_SCK_MHZ(4))) {
    lineout("Card Mount Failed");
    lineoutPrintf("Reason: %x (%x)\n", (int)sd.card()->errorCode(),
                  (int)sd.card()->errorData());
    return false;
  }

  uint64_t sectors = sd.card()->sectorCount();
  uint64_t cardSize = (sectors * 512) / (1000 * 1000 * 1000);
  lineoutPrintf("SD Card Size: %lluGB\n", cardSize);

  // CSV Creation
  if (!sd.exists(CSV_FILE_PATH)) {
    File headerFile = sd.open(CSV_FILE_PATH, O_RDWR | O_CREAT | O_AT_END);

    if (!headerFile) {
      lineout("Failed to create CSV file");
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
    lineout("logBufferlen is zero, nothing to write... failing");
    return true;
  }

  if (xSemaphoreTake(logMutex, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS)) != pdTRUE) {
    lineout("Failed to lock log mutex");
    return false;
  }

  FsFile logFile = sd.open(CSV_FILE_PATH, O_RDWR | O_APPEND);
  if (!logFile) {
    lineoutPrintf("Failed to open CSV file: %x\n", (int)sd.card()->errorCode());
    xSemaphoreGive(logMutex);
    return false;
  }

  size_t bytesWritten =
      logFile.write((const uint8_t *)csvLogBuffer, logBufferlen);
  logFile.close();

  if (bytesWritten != logBufferlen) {
    lineoutPrintf("partial write to CSV file (%zu bytes instead of %zu)\n",
                  bytesWritten, logBufferlen);
    xSemaphoreGive(logMutex);
    return false;
  }
  lineoutPrintf("wrote %zu bytes to sd card from buffer of size %zu\n",
                bytesWritten, logBufferlen);

  // Clear the buffer while holding the mutex to avoid races with writers.
  logBufferlen = 0;

  uint32_t duration = millis() - now;
  lineoutPrintf("Wrote %u bytes to SD in %u ms\n", bytesWritten, duration);
  xSemaphoreGive(logMutex);
  return true;
}

void writeDataToBuffer(const char *name,
                       float value) { // Write data to log buffer
  if (rockblockTable) {

    int index = -1;
    for (int i = 0; i < SENSOR_COUNT; i++) {
      if (strcmp(name, CSV_COLUMNS[i]) == 0) {
        index = i;
        break;
      }
    }

    if (table_memsize(rockblockTable) + sizeof(TableEntry) + 2 <= 340 &&
        index != -1) { // Check if adding this entry would exceed the 340 byte
                       // SBD limit (with some buffer for metadata)
      add_entry(rockblockTable, (TableEntry){
                                    .time = millis(),
                                    .type = (SensorDataType)index,
                                    .data = value,
                                });
    } else if (index == -1) {
      lineoutPrintf(
          "Unknown sensor name, not adding to rockblock buffer value for %s\n",
          name);
    }

    checkTable(rockblockTable);
  }

  if (sd.fatType() == 0)
    return; // card not initialized
  for (int i = 0; i < NUM_SENSORS; i++) {
    // Serial.printf("comparing name '%s' to column '%s'\n", name,
    // CSV_COLUMNS[i]);
    if (strcmp(name, CSV_COLUMNS[i]) == 0) {
      // for (;;);
      char entry[64];

      int pos = snprintf(entry, sizeof(entry), "%u", millis());

      for (int j = 0; j < NUM_SENSORS; j++) {
        if (j == i) {
          int written =
              snprintf(entry + pos, sizeof(entry) - pos, ",%.2f", value);
          if (written < 0 || pos + written >= (int)sizeof(entry)) {
            lineout("Failed to format CSV row");
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
        lineout("Failed to lock log mutex");
        return;
      }

      if (logBufferlen + pos >= CSV_LOG_BUFFER_SIZE) {
        lineout("Log buffer overflow, flushing to SD");
        xSemaphoreGive(logMutex);
        if (!LogWriteBuffer()) {
          lineout("Failed to write to log buffer");
          Serial.print("Write Error! SD Error Code: ");
          Serial.println((int)sd.card()->errorCode(), HEX);
          Serial.print("SD Error Data: ");
          Serial.println((int)sd.card()->errorData(), HEX);
          return;
        }
        if (xSemaphoreTake(logMutex, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS)) !=
            pdTRUE) {
          lineout("Failed to re-lock log mutex");
          return;
        }
      }

      // lineoutPrintf("csvLogBuffer: %p\nlogBufferlen: %zu\n", csvLogBuffer,
      // logBufferlen);
      memcpy(csvLogBuffer + logBufferlen, entry,
             pos); // Append new entry to log buffer
      logBufferlen += pos;

      xSemaphoreGive(logMutex);
      return;
    }
  }
}
