# Sokoke — CubeCats 2026 Catise HAB Project

Sokoke is an embedded telemetry and control firmware framework for the ESP32 developed for the CubeCats 2026 high-altitude balloon (HAB) mission. The system is designed to collect atmospheric and system data, maintain internal payload temperature in extreme conditions, and record onboard video throughout flight. It supports reliable data logging, real-time GPS tracking, and modular sensor integration, while also serving as a hands-on platform to teach new members engineering design, programming, and mission operations.

![Photo of the CubeCats 2026 Catise mission and board members of the organization](https://github.com/uccubecats/Sokoke/blob/SeanDolan-FINAL-CODE/res/Team.jpg?raw=true)

Key goals:
- Educate first-year and new members in engineering design, programming, and project management through a full HAB mission lifecycle.
- Maintain internal payload temperature within a controlled range (0–10°C) using active heating and insulation systems.
- Collect and store atmospheric and system data (temperature, pressure, humidity, attitude, and power usage) throughout the entire flight.
- Record high-altitude video footage (including 360° capture) for mission analysis and outreach.
- Provide reliable onboard data logging with sufficient storage for multi-hour flights.
- Enable live GPS tracking for real-time monitoring and payload recovery.
- Ensure system survivability in extreme high-altitude conditions (low pressure, low temperature, long duration).
- Build a modular and extensible firmware system that supports multiple sensors and future mission upgrades.

Contents
--------

- [Sokoke — CubeCats 2026 Catise HAB Project](#sokoke--cubecats-2026-catise-hab-project)
  - [Contents](#contents)
  - [Features](#features)
  - [Features](#features-1)
  - [Hardware Supported](#hardware-supported)
  - [Wiring \& Power](#wiring--power)
  - [Building \& Uploading](#building--uploading)
  - [Configuration](#configuration)
  - [Runtime Behavior \& Usage](#runtime-behavior--usage)
  - [Logging \& Telemetry](#logging--telemetry)
  - [Mission Overview](#mission-overview)
  - [Flight Results](#flight-results)
  - [Repository Layout](#repository-layout)
  - [Contributing](#contributing)
  - [License](#license)
  - [Contact / Questions](#contact--questions)

> [!WARNING]
> The PID controller is not fully tuned yet. If you use the heater control, retune the PID gains before relying on it for normal operation.

Features
--------

Features
--------

- Integrated multi-sensor data acquisition for atmospheric and onboard measurements:
  temperature, pressure, humidity, attitude (IMU), and power consumption.
- Active thermal control system using PWM + PID to maintain internal payload
  temperature within operational limits (0–10°C) in extreme environments.
- Robust SD card logging with timestamped CSV output for full-flight data recovery
  and post-mission analysis.
- Real-time GPS tracking support for live position monitoring and reliable payload recovery.
- High-altitude video capture support, including integration with 360° cameras for
  full-flight visualization.
- Modular ESP32-based architecture enabling easy integration of additional sensors
  and mission upgrades.
- Fault-tolerant startup and retry-based sensor initialization to improve reliability
  during launch and flight.
- Centralized logging system that supports Serial, Bluetooth, and onboard storage
  for debugging and data access.
- Designed for high-altitude survivability, including support for low temperatures,
  low pressure, and long-duration operation (3–5 hour flights).
- Built as an educational platform to teach embedded systems, control systems,
  and aerospace mission design to new team members.

Hardware Supported
------------------

This project targets ESP32-based systems used in high-altitude balloon payloads.
The hardware stack is selected to support data collection, thermal control,
tracking, and survivability.

Core components include:

- [ESP32](https://a.co/d/07C4VmTO)
  - primary flight computer for data collection and control
- [BMP390](https://a.co/d/024NGFiK) 
  - high-accuracy barometric pressure for altitude estimation
- [AHT30](https://a.co/d/022H3fro) 
  - temperature and humidity sensing inside payload
- [INA219](https://a.co/d/0jgzTkRU) / [INA228](https://a.co/d/03YWTxpj)
  - power monitoring for battery and subsystem analysis
- [FXOS8700 / FXAS21002C](https://www.adafruit.com/product/3463) 
  - IMU for payload orientation and motion tracking
- [MCP9808](https://www.adafruit.com/product/1782) 
  - precision external temperature monitoring
- [SpotGen 4 GPS](https://www.findmespot.com/en-us/products-services/spot-gen4) 
  - live tracking and post-flight recovery
- [SD card module](https://a.co/d/0cG6BlbM)
  - primary onboard data storage
- [RockBLOCK 9603 Iridium modem](https://www.groundcontrol.com/product/rockblock-9603-compact-plug-and-play-satellite-transmitter/) 
  - satellite telemetry and secondary GPS
- [Kapton heating pads](https://a.co/d/013tWFy0) + [MOSFETs](https://a.co/d/09iMKFLd)  
  - active thermal regulation
- [GoPro MAX2](https://gopro.com/en/us/shop/cameras/learn/max2/CHDHZ-311-master.html) 
  - external high-altitude video capture

Wiring & Power
--------------

- All sensors communicate over a shared I2C bus.
- The ESP32 acts as the central controller for sensors, storage, and thermal control.
- Heating elements are driven via MOSFETs and require careful power routing due to
  higher current draw.
- The system is powered by a high-capacity battery pack sized for 3–5 hour flights
  under cold conditions, where battery efficiency is reduced.
- Ensure all components share a common ground to prevent communication issues.
- Power budgeting should account for peak loads from heaters, camera systems,
  and optional satellite communication.

Building & Uploading
--------------------

- This repository is structured for the Arduino/PlatformIO toolchains. The primary sketch is `main.ino` at the project root.
- Using Arduino IDE: open `main.ino`, select the target board (e.g., ESP32 dev module), set serial port, and upload.

> [!IMPORTANT]
> PlatformIO builds currently have issues with RockBLOCK support. Use Arduino IDE for builds and uploads.

Configuration
-------------

- Most runtime configuration (sampling intervals, logging options, RockBLOCK settings, PID gains) is set in the top-level sketch or in header files referenced by `main.ino`.
- Sensitive values such as satellite endpoint keys or Wi‑Fi credentials (if added) should be moved to a non-committed config header or use build-time environment variables.

Runtime Behavior & Usage
------------------------

- On boot, the system initializes all sensors using a retry-and-backoff strategy
  to ensure reliable startup in cold or unstable conditions.
- Once initialized, the system continuously:
  - Records atmospheric and internal sensor data
  - Logs all data to the SD card in CSV format
  - Tracks GPS position for recovery
  - Maintains a closed-loop temperature control system using PID
- The thermal control system dynamically adjusts heater output to maintain
  internal payload temperature between 0–10°C.
- If enabled, telemetry data may be queued and transmitted via satellite.
- All data streams are optionally mirrored to Serial/Bluetooth for debugging.

Logging & Telemetry
-------------------

- All sensor and system data is logged in CSV format with timestamps for
  complete post-flight reconstruction of mission conditions.
- Logged data includes:
  - Atmospheric conditions (temperature, pressure, humidity)
  - Internal payload conditions
  - Orientation and motion data
  - Power consumption over time
- GPS data is recorded for both live tracking and payload recovery.
- Optional RockBLOCK integration enables low-bandwidth satellite transmission
  of critical telemetry (e.g., position updates).
- Data redundancy is prioritized to ensure no critical flight data is lost.

Mission Overview
----------------

Project Sokoke is a high-altitude balloon mission designed to:

- Reach altitudes of 30–35 km
- Operate for approximately 3–5 hours
- Survive extreme environmental conditions (low temperature and pressure)
- Maintain internal payload temperature within operational limits
- Collect and store atmospheric and system data throughout the flight
- Record high-altitude video footage
- Provide live GPS tracking for recovery

In addition to technical goals, the project serves as a training platform
to teach new members engineering design, embedded systems, and mission operations.

Flight Results
-----------------

![Photo of the earth's horision captured during the mission](https://github.com/uccubecats/Sokoke/blob/SeanDolan-FINAL-CODE/res/Mission.jpg?raw=true)

Repository Layout
-----------------

- `main.ino` — Top-level Arduino sketch and application glue.
- `src/` — Core source files and modules.
	- `SensorInit.cpp`, `sensorInit.h` — sensor initialization helpers.
	- `Sensors.h` — shared data types and externs.
	- `aht30Function/`, `bmp_function/`, `ina_function/`, `gyro_function/`, `mcp_function/` — per-sensor drivers and docs.
	- `RockblockFunction/` — Iridium SBD modem support and queueing logic.
	- `SdFunction/` — CSV logging implementation.
	- `BluetoothFunction/` — ESP32 classic Bluetooth SPP support.
	- `PWMController/`, `PIDHeatController/` — control algorithms and PWM output.
	- `log_wrapper/` — centralized logging multiplexing to Serial/Bluetooth/SD.

Each subfolder in `src` includes a README with module-specific wiring, API, and examples.

Contributing
------------

- Open an issue to discuss major changes or new hardware support before implementing.
- Follow the existing module structure and document new modules with a `README.md` in the module folder.
- Keep API surfaces minimal and prefer composition over monolithic changes.

License
------------

This project is licensed under the MIT License. See [LICENSE.txt](LICENSE.txt) for the full license text.

Copyright (c) 2026 CubeCats


Contact / Questions
-------------------

For questions about specific modules, check the per-module READMEs in `src/`. You can also open issues on the repository to request features or report bugs.

-----

