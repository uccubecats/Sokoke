/* ---------------------------- library includes --------------------------- */

#include <sstream>
#include <string>

#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP3XX.h>
#include <Adafruit_FXAS21002C.h>
#include <Adafruit_FXOS8700.h>
#include <Adafruit_INA219.h>
#include <Adafruit_INA228.h>
#include <Adafruit_MCP9808.h>
#include <MadgwickAHRS.h>
#include <freertos/FreeRTOS.h>

#include "src/BluetoothFunction/BluetoothFunction.h"
#include "src/PIDHeatController/PIDHeatController.h"
#include "src/PWMController/PWMController.h"
#include "src/RockblockFunction/RockblockFunction.h"
#include "src/SdFunction/SdFunction.h"
#include "src/SensorInit/sensorInit.h"
#include "src/Sensors.h"
#include "src/aht30Function/aht30Function.h"
#include "src/bmp_function/bmp_function.h"
#include "src/gyro_function/gyro_function.h"
#include "src/ina_function/ina_function.h"
#include "src/log_wrapper/log_wrapper.h"
#include "src/mcp_function/mcp_function.h"


/* ----------------------------------- IO ----------------------------------- */

SemaphoreHandle_t logMutex = NULL;

int signalQuality = -1;
int err;

#define IridiumSerial Serial2
IridiumSBD IridiumModem(IridiumSerial);

constexpr uint32_t IRIDIUM_BAUD = 19200;
constexpr int IRIDIUM_RX_PIN = 16;
constexpr int IRIDIUM_TX_PIN = 17;

bool lowPressure = false;

int heaterPin = 13;
int fanPin = 12;

uint64_t lastPID = 0;
uint64_t lastTime = 0;
uint64_t lastRockblockSendTime = 0;
float dutyCycle = 0.0f;

float temp1sec;

void readCore();
void writeCore();

constexpr BaseType_t READ_CORE_ID = 1;
constexpr BaseType_t WRITE_CORE_ID = 0;

void sensorTask(void *) { readCore(); }

void sdWriteTask(void *) { writeCore(); }

/* ----------------------------- core processes ----------------------------- */

void readCore() {
  while (true) {

    // AHT30 sensor
    AHT_Data_Return aht_data = readAht30();
    if (aht_data.success) {
      lineoutPrintf("Humidity: %.2f%\n", aht_data.humidity, false);
      lineoutPrintf("Temperature: %.2f C\n", aht_data.temperature, false);

      writeDataToBuffer("AhtTemperature", aht_data.temperature);
      writeDataToBuffer("AhtHumidity", aht_data.humidity);
    } else {
      lineout("Could not read AHT30 data");
    }

    // FXOS8700 / FXAS21002C sensor
    GyroData gyro_data = read_fxos_fxas_gyro();
    if (gyro_data.success) {
      lineoutPrintf("Roll: %.2f\n", gyro_data.angle.roll, false);
      lineoutPrintf("Pitch: %.2f\n", gyro_data.angle.pitch, false);
      lineoutPrintf("Yaw: %.2f\n", gyro_data.angle.yaw, false);
      lineoutPrintf("Linear X Acceleration: %.2f m/s^2\n", gyro_data.linacc.x,
                    false);
      lineoutPrintf("Linear Y Acceleration: %.2f m/s^2\n", gyro_data.linacc.y,
                    false);
      lineoutPrintf("Linear Z Acceleration: %.2f m/s^2\n", gyro_data.linacc.z,
                    false);

      writeDataToBuffer("GyroRoll", gyro_data.angle.roll);
      writeDataToBuffer("GyroPitch", gyro_data.angle.pitch);
      writeDataToBuffer("GyroYaw", gyro_data.angle.yaw);
      writeDataToBuffer("GyroLinAccX", gyro_data.linacc.x);
      writeDataToBuffer("GyroLinAccY", gyro_data.linacc.y);
      writeDataToBuffer("GyroLinAccZ", gyro_data.linacc.z);
    } else {
      lineout("Could not read Gyro data");
    }

    // bmp inside
    BmpData bmp_inside_data = read_bmp(&bmp_inside, bmp_inside_alive);
    if (bmp_inside_data.success) {
      lineoutPrintf("InsBMP Temperature: %.2f C\n", bmp_inside_data.temp,
                    false);
      lineoutPrintf("InsBMP Pressure: %.2f Pa\n", bmp_inside_data.pressure,
                    false);

      writeDataToBuffer("InsBmpTemp", bmp_inside_data.temp);
      writeDataToBuffer("InsBmpPress", bmp_inside_data.pressure);

      if (bmp_inside_data.pressure < 12000.0f) {
        if (!lowPressure) {
          lowPressure = true;
          targetTemperature = 0.0f;
          ResetPID(); // Reset PID when disabling heater due to low pressure
        }
      } else {
        lowPressure = false;
      }
    } else {
      lineout("Could not read Inside BMP390 data");
    }

    // bmp outside
    BmpData bmp_outside_data = read_bmp(&bmp_outside, bmp_outside_alive);
    if (bmp_outside_data.success) {
      lineoutPrintf("OutBMP Temperature: %.2f C\n", bmp_outside_data.temp,
                    false);
      lineoutPrintf("OutBMP Pressure: %.2f Pa\n", bmp_outside_data.pressure,
                    false);

      writeDataToBuffer("OutBmpTemp", bmp_outside_data.temp);
      writeDataToBuffer("OutBmpPress", bmp_outside_data.pressure);
    } else {
      lineout("Could not read Outside BMP390 data");
    }

    // mcp
    McpData mcp_data = read_mcp();
    if (mcp_data.success) {
      lineoutPrintf("MCP TempF: %f F\n", mcp_data.temp_f, false);
      lineoutPrintf("MCP TempC: %f C\n", mcp_data.temp_c, false);

      writeDataToBuffer("McpTempF", mcp_data.temp_f);
      writeDataToBuffer("McpTempC", mcp_data.temp_c);
    } else {
      lineout("Could not read MCP9808 data");
    }

    // ina low
    InaData ina_low_data = read_ina228(&ina_low, ina_low_alive);
    if (ina_low_data.success) {
      lineoutPrintf("LowINA228 Bus Voltage: %.2f V\n", ina_low_data.busVoltage,
                    false);
      lineoutPrintf("LowINA228 Current: %.2f mA\n", ina_low_data.current,
                    false);

      writeDataToBuffer("LowInaBusVolt", ina_low_data.busVoltage);
      writeDataToBuffer("LowInaCurrent", ina_low_data.current);
    } else {
      lineout("Could not read Low Voltage INA228");
    }

    // ina high
    InaData ina_high_data = read_ina219(&ina_high, ina_high_alive);
    if (ina_high_data.success) {
      lineoutPrintf("HighINA219 Bus Voltage: %.2f V\n",
                    ina_high_data.busVoltage, false);
      lineoutPrintf("HighINA219 Current: %.2f mA\n", ina_high_data.current,
                    false);

      writeDataToBuffer("HighInaBusVolt", ina_high_data.busVoltage);
      writeDataToBuffer("HighInaCurrent", ina_high_data.current);
    } else {
      lineout("Could not read High Voltage INA219");
    }

    lineout("");

    delay(100);
  }
}

void writeCore() {
  while (true) {

    /* ------------------------------ SD Card Write
     * ----------------------------- */

    uint32_t now = millis();

    if (now - lastRockblockSendTime >= ROCKBLOCK_SEND_INTERVAL_MS) {
      if (!sendRockblockBuffer()) {
        lineout("Failed to send rockblock buffer");
      } else {
        lineout("Sent rockblock buffer", false);
      }
      lastRockblockSendTime = now;
    }

    if (now - lastWriteTime >= WRITE_INTERVAL_MS) {
      if (!LogWriteBuffer()) {
        lineout("Failed to write log buffer to SD");
      }
      lastWriteTime = now;
    }
    if (now - lastPID >= 1000) {
      float pidOutput = CalculatePID(targetTemperature, temp1sec, 1.0f);
      uint32_t appliedHeaterDuty =
          lowPressure
              ? 25u
              : std::max<uint32_t>(25u, static_cast<uint32_t>(pidOutput));

      uint32_t appliedFanDuty =
          lowPressure ? 0u : static_cast<uint32_t>(pidOutput);

      ledcWrite(heaterPin, appliedHeaterDuty);
      ledcWrite(fanPin, appliedFanDuty);

      lineoutPrintf("PID Output: %.2f\n", pidOutput, false);

      (void)pidOutput;

      lastPID = now;
    }

    /* -------------------------------- Bluetooth
     * ------------------------------- */

    if (SerialBT.available() > 0) {
      // Clear buffer

      String incoming = SerialBT.readStringUntil('\n');
      incoming.trim();

      int splitIndex = incoming.indexOf(' ');
      if (splitIndex > 0) {
        String command = incoming.substring(0, splitIndex);
        String value = incoming.substring(splitIndex + 1);
        value.trim();
        float parsed = value.toFloat();

        if (command == "kp") {
          kp = parsed;
          lineout("Updated kp: ", false);
          lineout(std::to_string(kp).c_str(), false);
        } else if (command == "ki") {
          ki = parsed;
          lineout("Updated ki: ", false);
          lineout(std::to_string(ki).c_str(), false);
        } else if (command == "kd") {
          kd = parsed;
          lineout("Updated kd: ", false);
          lineout(std::to_string(kd).c_str(), false);
        } else if (command == "target") {
          targetTemperature = parsed;
          lineout("Updated target temperature: ", false);
          lineout(std::to_string(targetTemperature).c_str(), false);
        } else if (command == "cycle") {
          ledcWrite(heaterPin, parsed);
          ledcWrite(fanPin, parsed);
          lineout("Updated duty cycle: ", false);
          lineout(std::to_string(parsed).c_str(), false);
        } else if (command == "debugbt") {
          bluetooth_debug_info = parsed == 1;
          lineoutPrintf("Updated bluetooth_debug_info: %s\n",
                        bluetooth_debug_info ? "true" : "false", false);
        } else if (incoming == "initaht") {
          lineout("manually initializing aht30", false);
          attempt_init_aht30();
        } else if (incoming == "status") {
          lineout("Sensor statuses:", false);
          lineoutPrintf("Inside BMP390: %d\n", bmp_inside_alive, false);
          lineoutPrintf("Outside BMP390: %d\n", bmp_outside_alive, false);
          lineoutPrintf("FXOS8700/FXAS21002: %d\n", fxos_fxas_alive, false);
          lineoutPrintf("MCP9808: %d\n", mcp_alive, false);
          lineoutPrintf("AHT30: %d\n", aht_alive, false);
          lineoutPrintf("INA228 Low Voltage: %d\n", ina_low_alive, false);
          lineoutPrintf("INA219 High Voltage: %d\n", ina_high_alive, false);
        } else if (incoming == "fullreset") {
          lineout("Restarting ESP32", false);
          ESP.restart();
        } else {
          lineout("Unknown command", false);

          while (SerialBT.available() > 0)
            SerialBT.read();
        }

        delay(100);
      }
    }

    // Always yield so IDLE0 can run and service the task watchdog.
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void setup() {
  Serial.begin(115200);

  delay(2000);

  IridiumSerial.begin(19200, SERIAL_8N1, 16, 17);

  initBluetooth();

  lineout("ESP32 Started");
  lineoutPrintf("Total heap available post-startup: %zu\n", ESP.getFreeHeap());
  lineoutPrintf("Rockblock UART configured at %lu baud, RX=%d TX=%d\n", 19200,
                16, 17);

  // Initialize Mutex
  if (attempt_init_mutex()) {
    lineout("Mutex Initialized\n");
  } else {
    lineout("Failed to create mutex\n");
    ESP.restart();
  }

  // Initialize SD card
  if (sdReady = attempt_init_sdreader()) {
    lineout("SD Card Reader Initialized\n");
  } else {
    lineout("Failed to initialize SD Card Reader\n");
  }

  // Initialize rockblock buffer
  if (attempt_init_rockblock_buffer()) {
    lineout("RockBlock Buffer Initialized\n");
  } else {
    lineout("RockBlock Buffer failed to initialize\n");
  }

  /* ---------------------------------- inits ---------------------------------
   */

  // Initialize Rockblock
  lineout("Initializing Rockblock...");
  if (initRockblock()) {
    lineout("Rockblock Initialized\n");
  } else {
    lineout("Failed to initialize Rockblock\n");
  }

  // Initialize AHT30 Temperature sensor
  if (aht_alive = attempt_init_aht30()) {
    lineout("AHT30 Initialized\n");
  } else {
    lineout("Failed to initialize AHT30 sensor\n");
  }

  // Initialize FXOS8700/FXAS21002 sensor
  if (fxos_fxas_alive = (attempt_init_fxos8700() && attempt_init_fxas21002())) {
    lineout("FXOS8700/FXAS21002 Initialized\n");
  } else {
    lineout("Failed to initialize FXOS8700/FXAS21002 sensor\n");
    madgwick.begin(100);
  }

  // Initialize outside BMP390 Pressure sensor
  if (bmp_outside_alive =
          attempt_init_bmp390(&bmp_outside, 0x77)) { // outside temp/pres
    lineout("Outside BMP390 on I2C Address 0x77 Initialized\n");
  } else {
    lineout("Could not find a valid outside BMP sensor, check wiring!\n");
  }

  // Initialize inside BMP390 Pressure sensor
  if (bmp_inside_alive =
          attempt_init_bmp390(&bmp_inside, 0x76)) { // inside temp/pres
    lineout("Inside BMP390 on I2C Address 0x76 Initialized\n");
  } else {
    lineout("Could not find a valid inside BMP sensor, check wiring!\n");
  }

  // Initialize MCP9808 Temperature sensor
  if (mcp_alive = attempt_init_mcp9808()) {
    lineout("MCP9808 Initialized\n");
  } else {
    lineout("Failed to initialize MCP9808 sensor\n");
  }

  // Initialize INA228 Low Voltage Power Monitor
  if (ina_low_alive = attempt_init_ina228(&ina_low, 0x41)) {
    lineout("Low Voltage INA228 Initialized\n");
  } else {
    lineout("Failed to initialize Low Voltage INA228 sensor\n");
  }

  // Initialize INA219 High Voltage Power Monitor
  if (ina_high_alive = attempt_init_ina219(&ina_high, 0x40)) {
    lineout("High Voltage INA219 Initialized\n");
  } else {
    lineout("Failed to initialize High Voltage INA219 sensor\n");
  }

  // Initialize PID controller
  PWMSetup(13, 5000, 8);
  lineout("PWM Heater Controller Initialized\n");

  PWMSetup(12, 5000, 8);
  lineout("PWM Fan Controller Initialized\n");

  // Reset PID state at startup
  ResetPID();

  /* --------------------------- Create pinned tasks --------------------------
   */

  xTaskCreatePinnedToCore(sensorTask, "SensorDataTask", 4096, NULL, 1, NULL,
                          READ_CORE_ID);

  xTaskCreatePinnedToCore(sdWriteTask, "SDWriteTask", 4096, NULL, 1, NULL,
                          WRITE_CORE_ID);
}

// don't use
void loop() {}
