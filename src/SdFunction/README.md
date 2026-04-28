# SdFunction

**Files:** `SdFunction.h` · `SdFunction.cpp`

## Overview

Buffers sensor readings in a heap-allocated 48 KB RAM buffer for efficient CSV logging to SD card, and maintains a parallel [RockBLOCK](../RockblockFunction/) table buffer for periodic Iridium SBD satellite transmission. The SD card is accessed via the [SdFat](https://github.com/greiman/SdFat) library. Both channels share a FreeRTOS mutex for SD access and are flushed on independent time-based intervals managed by the application.

---

## Runtime Configuration

| Constant / Variable           | Value              | Description                                           |
|-------------------------------|--------------------|-------------------------------------------------------|
| `SD_CS_PIN`                   | `5`                | SPI chip select pin for the SD card                   |
| `CSV_FILE_PATH`               | `"/sensor_log.csv"`| File path on the SD card                              |
| `WRITE_INTERVAL_MS`           | `30000 ms`         | How often `LogWriteBuffer()` should be called (30 s)  |
| `ROCKBLOCK_SEND_INTERVAL_MS`  | `300000 ms`        | Minimum interval between satellite sends (5 min)      |
| `MUTEX_TIMEOUT_MS`            | `5000 ms`          | FreeRTOS semaphore acquisition timeout                |
| `CSV_LOG_BUFFER_SIZE`         | `49152 bytes`      | 48 KB heap-allocated RAM buffer for pending CSV rows  |

**CSV columns (in order):** `timestamp_ms`, then one column per `SensorDataType` enum value from `Sensors.h` (`AhtTemperature`, `AhtHumidity`, `GyroRoll`, … `HighInaCurrent`). Each row fills in only the column matching the sensor that was written; all other columns are left blank.

---

## Dependencies

- **SdFat** — `SdFs sd` used for all SD card operations (not the standard Arduino SD library)
- **FreeRTOS** — `logMutex` (`SemaphoreHandle_t`) must be created by the application before any SD operations
- **RockblockFunction** — `Table`, `TableEntry`, `add_entry()`, `send_table()`, etc. (see [`RockblockFunction/`](../RockblockFunction/))
- **Sensors.h** — `SensorDataType` enum and `get_sensor_name()` used to build CSV column headers
- **log_wrapper** — `lineout` / `lineoutPrintf` for status messages

> `logMutex` is declared `extern` in `SdFunction.h` — the application must call `logMutex = xSemaphoreCreateMutex()` during setup. `sd` is also exported for use by `log_wrapper`.

---

## Public API

### Initialization

#### `bool initSDCard()`
Mounts the SD card on `SD_CS_PIN` at 4 MHz SPI and prints the card size. Builds the column name array from the `SensorDataType` enum. If `sensor_log.csv` does not yet exist, creates it and writes the CSV header row. Returns `false` on any failure.

#### `bool initRockblockBuffer()`
Heap-allocates `csvLogBuffer` (`CSV_LOG_BUFFER_SIZE` bytes) and allocates the internal `rockblockTable` via `new_table()` if it has not already been created. Logs allocation address and available heap. Returns `false` if either allocation fails.

---

### Data Ingestion

#### `void writeDataToBuffer(const char* name, float value)`
The main data ingestion point. Accepts a sensor name string (must match a `SensorDataType` name from `Sensors.h`) and its reading. On each call it:

1. **RockBLOCK path** — If `rockblockTable` exists and adding one `TableEntry` would not exceed 340 bytes, appends a `TableEntry` with the matched `SensorDataType` index and current `millis()` timestamp. Also calls `checkTable()` to reset if the budget is reached.
2. **CSV path** — Formats a sparse row as `timestamp_ms,<blank columns except the matching sensor>` and appends it to `csvLogBuffer`. If the buffer would overflow, automatically calls `LogWriteBuffer()` to flush to SD before appending.

Both paths acquire `logMutex` independently. Unrecognized `name` values are logged to Serial/BT and otherwise silently ignored.

---

### Flushing

#### `bool LogWriteBuffer()`
Acquires `logMutex`, appends all pending bytes in `csvLogBuffer` to `sensor_log.csv` using `O_RDWR | O_APPEND`, then clears the buffer. Returns `true` if the buffer was empty (nothing to write). Returns `false` if the mutex cannot be acquired, the file cannot be opened, or a partial write is detected.

#### `bool sendRockblockBuffer()`
If `rockblockTable` is non-empty, calls `send_table()`, frees the old table, and allocates a fresh one. Returns `true` on success or if the table is empty. Returns `false` if the new table allocation fails. The calling application is responsible for checking the `ROCKBLOCK_SEND_INTERVAL_MS` interval before calling.

---

### Helpers

#### `void randomSensorData()`
Generates random temperature and pressure readings at staggered intervals and passes them to `writeDataToBuffer()`. Used for testing the buffer and SD logging pipeline without real sensors attached.

---

## Expected Usage Flow

```cpp
// 1. Application setup
logMutex = xSemaphoreCreateMutex();
attempt_init_sdreader();       // calls initSDCard() with retries
attempt_init_rockblock_buffer(); // calls initRockblockBuffer() with retries

// 2. As sensor data arrives (from any FreeRTOS task)
writeDataToBuffer("InsBmpTemp",  readTemp());
writeDataToBuffer("InsBmpPress", readPressure());

// 3. Periodic flush (e.g. in writeCore task)
if (millis() - lastWriteTime >= WRITE_INTERVAL_MS) {
    LogWriteBuffer();
    lastWriteTime = millis();
}

if (millis() - lastRockblockSendTime >= ROCKBLOCK_SEND_INTERVAL_MS) {
    sendRockblockBuffer();
    lastRockblockSendTime = millis();
}
```

---

## Internal State

| Variable               | Type           | Description                                      |
|------------------------|----------------|--------------------------------------------------|
| `csvLogBuffer`         | `char*`        | Heap-allocated RAM buffer for pending CSV rows   |
| `logBufferlen`         | `size_t`       | Number of valid bytes currently in `csvLogBuffer`|
| `sdReady`              | `bool`         | SD mount status                                  |
| `lastWriteTime`        | `uint32_t`     | `millis()` timestamp of last SD write            |
| `rockblockTable`       | `Table*`       | Active RockBLOCK entry buffer (module-internal)  |
| `sd`                   | `SdFs`         | SdFat filesystem object (exported for log_wrapper)|
