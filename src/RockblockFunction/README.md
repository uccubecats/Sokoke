# RockblockFunction

**Files:** `RockblockFunction.h` · `RockblockFunction.cpp`

## Overview

Manages the [Iridium RockBLOCK](https://www.groundcontrol.com/product/rockblock-9603/) modem and maintains an in-memory table of sensor snapshots that is serialized into a compact binary payload for transmission over the [Iridium Short Burst Data (SBD)](https://www.iridium.com/services/iridium-sbd/) protocol. Entries are batched until the serialized payload approaches the 340-byte SBD budget, at which point the table is reset automatically.

In practice, entries are added via `add_entry()` from [`SdFunction`](../SdFunction/) rather than through the deprecated `add_sensor_data()`.

---

## Dependencies

- **IridiumSBD** — mikalhart/IridiumSBD library for Iridium modem communication
- **Sensors.h** — `SensorDataType` enum used in `TableEntry`
- **log_wrapper** — `lineout` / `lineoutDebugPrintf` for status messages
- **FreeRTOS** — `vTaskDelay` used in the ISBDCallback to yield during modem waits

---

## Data Types

### `TableEntry` *(packed)*

A single sensor reading. The struct is `__attribute__((packed))` to eliminate padding and keep the serialized footprint predictable.

| Field  | Type             | Description                                              |
|--------|------------------|----------------------------------------------------------|
| `time` | `uint64_t`       | Milliseconds since device boot (from `millis()`)         |
| `type` | `SensorDataType` | Which sensor produced this reading (see `Sensors.h`)     |
| `data` | `float`          | The sensor value                                         |

Each entry is **16 bytes** (8 + 4 + 4).

---

### `Table`

A heap-allocated, dynamically-resizable array of `TableEntry` values.

| Field      | Type             | Description                                        |
|------------|------------------|----------------------------------------------------|
| `size`     | `unsigned short` | Number of entries currently stored                 |
| `capacity` | `unsigned short` | Allocated entry slots (doubles on overflow)        |
| `entries`  | `TableEntry*`    | Heap-allocated array of entries                    |

Initial capacity: **64 entries**. Capacity doubles automatically when entries are added past the current limit.

---

### `SerializedTable`

A `typedef void*` pointing to a flat binary buffer. Layout:

```
[ size (2 bytes) | capacity (2 bytes) | entries (size × 16 bytes) ]
```

---

## Public API

### Modem Initialization

#### `bool initRockblock()`
Initializes the IridiumSBD modem on the configured UART (`IridiumSerial = Serial2`). Sets the send/receive timeout to 45 seconds. Returns `true` on success, `false` on error. Sets an internal `rockblockModemReady` flag that gates all future `send_table()` calls.

---

### Table Lifecycle

#### `Table* new_table()`
Allocates and returns a new, empty `Table` with `capacity = 64`. Returns `NULL` on allocation failure.

#### `void free_table(Table* t)`
Frees both the `entries` array and the `Table` struct itself.

#### `void seal_table(Table* t)`
Shrinks the `entries` allocation to exactly `size` entries. Called automatically by `send_table()` before serialization.

---

### Adding Data

#### `void add_entry(Table* t, TableEntry e)`
Low-level append. Doubles capacity via `realloc()` if `size >= capacity`.

#### `Table* add_sensor_data(Table* t, uint64_t time, SensorDataType type, float data)` *(deprecated)*
Convenience wrapper around `checkTable()` and `add_entry()`. Returns the (possibly new) table pointer.

> **Always reassign the return value:** `t = add_sensor_data(t, time, type, value);`

#### `Table* checkTable(Table* t)`
- If `t == NULL`: allocates and returns a fresh table.
- If the next entry would push the serialized size to 340 bytes or more: frees `t` and returns a fresh table. *(Does **not** transmit the old table — callers handle transmission separately.)*
- Otherwise: returns `t` unchanged.

---

### Serialization

#### `SerializedTable serialize_table(Table* t)`
Serializes the table to a flat binary buffer. Returns `NULL` on allocation failure.

#### `Table* deserialize_table(SerializedTable t)`
Reconstructs a `Table` from a previously serialized buffer. Intended for testing only.

#### `size_t table_memsize(Table* t)`
Returns the byte length of the serialized representation: `2 * sizeof(unsigned short) + t->size * sizeof(TableEntry)`.

---

### Transmission

#### `void send_table(Table* t)`
Seals and serializes the table, then calls `IridiumModem.sendSBDBinary()` to transmit over Iridium SBD. Skips transmission and logs a warning if `rockblockModemReady` is `false`. Logs success or the error code on failure.

---

## ISBDCallback

An `ISBDCallback()` function is registered with the IridiumSBD library. It calls `vTaskDelay(1 ms)` to yield to FreeRTOS IDLE tasks during long modem blocking loops, preventing the task watchdog from triggering.

---

## Usage Example

```cpp
// Initialization
if (initRockblock()) {
    lineout("Rockblock ready");
}

// Adding entries (managed by SdFunction):
if (table_memsize(rockblockTable) + sizeof(TableEntry) + 2 <= 340) {
    add_entry(rockblockTable, (TableEntry){
        .time = millis(),
        .type = InsBmpTemp,
        .data = readTemp()
    });
}

// Periodic send (every 5 minutes):
send_table(rockblockTable);
free_table(rockblockTable);
rockblockTable = new_table();
```