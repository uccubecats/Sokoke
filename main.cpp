/* ---------------------------- library includes --------------------------- */

#include <Arduino.h>
#include "SdFunction/SdFunction.h"
#include "RockblockFunction/RockblockFunction.h"
#include <freertos/FreeRTOS.h>
#include <aht30Lib/driver_aht30.h>
#include <aht30Lib/driver_aht30_basic.h>
#include <aht30Function/aht30Function.h>
#include <BMP390/BMP390Function.h>
#include <Adafruit_MPU6050.h>
#include "BluetoothFunction/BluetoothFunction.h"
#include "PIDHeatController/PIDHeatController.h"
#include "Sensors.h"
#include <sstream>

/* ----------------------------------- IO ----------------------------------- */

SemaphoreHandle_t logMutex = NULL;
Adafruit_MPU6050 mpu;

uint64_t lastPID = 0;
uint64_t lastTime = 0;

float temp1sec;

void readCore();
void writeCore();

constexpr BaseType_t READ_CORE_ID = 1;
constexpr BaseType_t WRITE_CORE_ID = 0;

void sensorTask(void*) {
    readCore();
}

void sdWriteTask(void*) {
    writeCore();
}

/* ----------------------------- core processes ----------------------------- */

void readCore() {
    while (true) {      

        // AHT30 sensor
        std::tuple<float, uint8_t, bool> sensorData = readAht30();
        float temperature = std::get<0>(sensorData);
        uint8_t humidity = std::get<1>(sensorData);
        bool success = std::get<2>(sensorData);

        if (success) {
            writeDataToBuffer("TempIns", temperature);
            writeDataToBuffer("Humidity", (float)humidity);
            
        }

        if (millis() - lastPID < 1000) {
            temp1sec = temperature;
        }
        
        // MPU6050 sensor
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);
        writeDataToBuffer("MPUTemp", temp.temperature);
        writeDataToBuffer("AccX", a.acceleration.x);
        writeDataToBuffer("AccY", a.acceleration.y);
        writeDataToBuffer("AccZ", a.acceleration.z);

        // BMP390 sensor

        // bmp3_data bmp_data = Temp_Presure_Write_To_SD();
        
        // if (bmp_data.success) {
        //     writeDataToBuffer("BMP390_temperature", (float)bmp_data.temperature);
        //     writeDataToBuffer("BMP390_pressure", (float)bmp_data.pressure);
        // }

        // Other Sensors



        delay(100);
    }
}

void writeCore() {
    while (true) {

        /* ------------------------------ SD Card Write ----------------------------- */

        uint32_t now = millis();
        if (now - lastWriteTime >= WRITE_INTERVAL_MS) {
            if (!LogWriteBuffer()) {
                Serial.println("Failed to write log buffer to SD");
            } else {
                if (!sendRockblockBuffer()) {
                    Serial.println("Failed to send rockblock buffer");
                }
                lastWriteTime = now;
            }
        }
        if (now - lastPID >= 1000) {
                float pidOutput = CalculatePID(targetTemperature, temp1sec, 1.0f);
                (void)pidOutput;

            lastPID = now;
        }
        

        /* -------------------------------- Bluetooth ------------------------------- */
        
        if (isConnected && SerialBT.hasClient()) {
            // SerialBT.println(temp1sec);
            if (SerialBT.available()) {
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
                        SerialBT.print("Updated kp: ");
                        SerialBT.println(kp);
                    } else if (command == "ki") {
                        ki = parsed;
                        SerialBT.print("Updated ki: ");
                        SerialBT.println(ki);
                    } else if (command == "kd") {
                        kd = parsed;
                        SerialBT.print("Updated kd: ");
                        SerialBT.println(kd);
                    } else if (command == "target") {
                        targetTemperature = parsed;
                        SerialBT.print("Updated target temperature: ");
                        SerialBT.println(targetTemperature);
                    }
                }
            }    
        }
        
        delay(100); 
    }
}

/* ------------------------------ inital setup ------------------------------ */

void setup() {
    Serial.begin(115200);
    
    while(!Serial) {
        delay(100);
    }

    // randomSeed((uint32_t)esp_random());

    /* ---------------------------------- inits --------------------------------- */

    // Initialize AHT30 Temperature sensor

    if (aht30_basic_init() != 0) {
        Serial.println("Failed to initialize AHT30 sensor");
        while (true) {
            delay(1000);
        }
    }

    // Initialize MPU6050 sensor
    if(!mpu.begin()) {
        Serial.println("Failed to initialize MPU6050 sensor");
        while (true) {
            delay(1000);
        }
    }

    // Initialize BMP390 Pressure sensor
    // initBMP390();


    // Initialize Mutex

    logMutex = xSemaphoreCreateMutex(); 
    if (logMutex == NULL) {
        Serial.println("Failed to create mutex");
        while (true) {
            delay(1000);
        }
    }


    // Initialize SD card
    
    sdReady = initSDCard();
    if (!sdReady) {
        Serial.println("Failed to initialize SD card");
        while (true) {
            delay(1000);
        }
    }

    // Initialize rockblock buffer

    if (!initRockblockBuffer()) {
        Serial.println("Failed to initialize rockblock buffer");
        while (true) {
            delay(1000);
        }
    }

    // Initialize Bluetooth after blocking setup work to avoid early session drops.
    
    initBluetooth();

    /* --------------------------- Create pinned tasks -------------------------- */

    xTaskCreatePinnedToCore(
        sensorTask,
        "SensorDataTask",
        4096,
        NULL,
        1,
        NULL,
        READ_CORE_ID
    );
    
    xTaskCreatePinnedToCore(
        sdWriteTask,
        "SDWriteTask",
        4096,
        NULL,
        1,
        NULL,
        WRITE_CORE_ID
    );
}

// don't use
void loop() {}
