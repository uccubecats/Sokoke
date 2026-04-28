# log_wrapper

**Files:** `log_wrapper.h` · `log_wrapper.cpp`

## Overview

Unified output layer that simultaneously writes log messages to:
1. **`Serial`** — wired USB serial (115200 baud)
2. **`SerialBT`** — Bluetooth serial (see [`BluetoothFunction/`](../BluetoothFunction/))
3. **`/debug_log.txt`** on the SD card (optional, controlled per call)

All three outputs are written in a single call, removing the need to call `Serial.print`, `SerialBT.print`, and SD file writes separately throughout the codebase. SD writes acquire `logMutex` to prevent concurrent access from the dual-core FreeRTOS tasks.

---

## Dependencies

- **SdFat** — `SdFs sd` and `FsFile` for SD logging (from [`SdFunction/`](../SdFunction/))
- **BluetoothSerial** — `SerialBT` global (from [`BluetoothFunction/`](../BluetoothFunction/))
- **Arduino** — `Serial`, `millis()`
- **FreeRTOS** — `logMutex` semaphore for SD access

---

## Constants

| Constant          | Value              | Description                           |
|-------------------|--------------------|---------------------------------------|
| `DEBUG_LOG_FILE`  | `"/debug_log.txt"` | Path on the SD card for debug output  |

---

## Return Type

All functions return a `LineoutReturn` enum value:

| Value              | Meaning                                        |
|--------------------|------------------------------------------------|
| `Success`          | All outputs succeeded                          |
| `SerialFailure`    | `Serial` or `SerialBT` print returned 0 bytes  |
| `SerialBTFailure`  | *(reserved; currently maps to SerialFailure)*  |
| `SDFailure`        | Could not acquire mutex or write to SD file    |

---

## Public API

#### `LineoutReturn lineout(const char* output, bool debug_log = true, bool trailing_newline = true)`

Writes a plain string to Serial, SerialBT, and optionally the SD debug log.

| Parameter         | Default | Description                                               |
|-------------------|---------|-----------------------------------------------------------|
| `output`          | —       | Null-terminated string to print                           |
| `debug_log`       | `true`  | If `true`, also appends to `/debug_log.txt` with timestamp|
| `trailing_newline`| `true`  | If `true`, uses `println()`; otherwise uses `print()`     |

SD entries are prefixed with `[DEBUG <millis>] `.

---

#### `LineoutReturn lineoutPrintf(const char* format, ...)`

`printf`-style formatted output. Formats into a 256-byte stack buffer, then calls `lineout(buffer, false, false)`.

- **No** SD debug log entry.
- **No** trailing newline (format string must include `\n` if needed).

---

#### `LineoutReturn lineoutDebugPrintf(const char* format, ...)`

`printf`-style formatted output with SD debug logging. Formats into a 256-byte stack buffer, then calls `lineout(buffer, true, false)`.

- **Yes** — writes to SD debug log.
- **No** trailing newline.

---

## Usage Example

```cpp
// Plain log with newline — also writes to SD
lineout("ESP32 Started");

// Formatted output, no SD log
lineoutPrintf("Free heap: %zu bytes\n", ESP.getFreeHeap());

// Formatted output WITH SD log entry
lineoutDebugPrintf("Error code: %d\n", err);

// No newline, no SD log (useful for building up a line)
lineout("Temperature: ", false, false);
lineout(std::to_string(temp).c_str(), false, false);
```

---

## Thread Safety

`lineout()` checks whether the calling task already holds `logMutex` before attempting to acquire it, preventing deadlocks when called from within an SD critical section. If the mutex cannot be acquired within 5 seconds, the SD write is skipped and `SDFailure` is returned.
