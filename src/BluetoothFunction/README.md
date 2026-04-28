# BluetoothFunction

**Files:** `BluetoothFunction.h` · `BluetoothFunction.cpp`

## Overview

Sets up Classic Bluetooth Serial (SPP) on the ESP32 so that other modules can stream data wirelessly to a paired device. Once initialized, the global `SerialBT` object can be used anywhere in the project as a drop-in replacement for wired `Serial` output.

---

## Dependencies

- **BluetoothSerial** — ESP32 Arduino Bluetooth Serial library
- **log_wrapper** — `lineout` / `lineoutPrintf` for startup messages

---

## Exported Globals

| Symbol                 | Type              | Description                                                              |
|------------------------|-------------------|--------------------------------------------------------------------------|
| `SerialBT`             | `BluetoothSerial` | Bluetooth serial port; used by other modules for output                  |
| `isConnected`          | `bool`            | `true` while a remote client is connected via SPP                        |
| `bluetooth_debug_info` | `bool`            | When `true`, the SPP callback logs all events and updates `isConnected`  |

---

## Public API

#### `void initBluetooth()`
Starts the Bluetooth stack with the device name **"ESP32-Serial"** in server (non-master) mode. Sets a 50 ms read timeout and registers the internal `connectionStatus` callback. Logs an error via `lineout` and returns early if startup fails.

---

## Internal Callback

#### `void connectionStatus(esp_spp_cb_event_t event, esp_spp_cb_param_t* param)`
Registered as the SPP event callback. **Only active when `bluetooth_debug_info` is `true`** — the function returns immediately at the top if `bluetooth_debug_info` is `false`.

When active:
- Logs every SPP event with its numeric code and name (via `sppEventToString()`).
- `ESP_SPP_SRV_OPEN_EVT` with `ESP_SPP_SUCCESS` — sets `isConnected = true`.
- `ESP_SPP_CLOSE_EVT` — sets `isConnected = false` and logs "Client disconnected".

> **Note:** Because the callback short-circuits when `bluetooth_debug_info` is `false`, `isConnected` is only updated while debug mode is enabled. Enable `bluetooth_debug_info` (e.g. via the `debugbt 1` Bluetooth command) if connection state tracking is needed.

---

## Bluetooth Commands (handled in `writeCore()`)

The following commands can be sent over Bluetooth Serial at runtime:

| Command       | Example          | Effect                                                    |
|---------------|------------------|-----------------------------------------------------------|
| `debugbt 1`   | `debugbt 1`      | Enables SPP event logging and `isConnected` tracking      |
| `debugbt 0`   | `debugbt 0`      | Disables SPP event logging                                |

---

## Usage Example

```cpp
initBluetooth();

// Enable connection tracking at runtime via BT command "debugbt 1"
// or set directly in code:
bluetooth_debug_info = true;

// In any task or loop:
SerialBT.printf("Temperature: %.2f C\n", temp);
```
