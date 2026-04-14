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

/* ----------------------------------- IO ----------------------------------- */

SemaphoreHandle_t logMutex = NULL;
Adafruit_MPU6050 mpu;

void readCore();
void writeCore();

constexpr BaseType_t READ_CORE_ID = 0;
constexpr BaseType_t WRITE_CORE_ID = 1;

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


        delay(10);
    }
}

void writeCore() {
    while (true) {
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
        delay(1000);
    }
}

/* ------------------------------ inital setup ------------------------------ */

// void setup() {
//     Serial.begin(115200);
    
//     while(!Serial) {
//         delay(100);
//     }

//     // randomSeed((uint32_t)esp_random());

//     /* ---------------------------------- inits --------------------------------- */


//     // Initialize AHT30 Temperature sensor

//     if (aht30_basic_init() != 0) {
//         Serial.println("Failed to initialize AHT30 sensor");
//         while (true) {
//             delay(1000);
//         }
//     }

//     // Initialize MPU6050 sensor
//     if(!mpu.begin()) {
//         Serial.println("Failed to initialize MPU6050 sensor");
//         while (true) {
//             delay(1000);
//         }
//     }

//     // Initialize BMP390 Pressure sensor
//     // initBMP390();


//     // Initialize Mutex

//     logMutex = xSemaphoreCreateMutex(); 
//     if (logMutex == NULL) {
//         Serial.println("Failed to create mutex");
//         while (true) {
//             delay(1000);
//         }
//     }


//     // Initialize SD card
    
//     sdReady = initSDCard();
//     if (!sdReady) {
//         Serial.println("Failed to initialize SD card");
//         while (true) {
//             delay(1000);
//         }
//     }

//     // Initialize rockblock buffer

//     if (!initRockblockBuffer()) {
//         Serial.println("Failed to initialize rockblock buffer");
//         while (true) {
//             delay(1000);
//         }
//     }

//     /* --------------------------- Create pinned tasks -------------------------- */

//     xTaskCreatePinnedToCore(
//         sensorTask,
//         "SensorDataTask",
//         4096,
//         NULL,
//         1,
//         NULL,
//         READ_CORE_ID
//     );
    
//     xTaskCreatePinnedToCore(
//         sdWriteTask,
//         "SDWriteTask",
//         4096,
//         NULL,
//         1,
//         NULL,
//         WRITE_CORE_ID
//     );
// }

// void setup() {
//     Serial.begin(115200);
// }

#define IridiumSerial Serial2
#define RX2_PIN 16
#define TX2_PIN 17
#define DIAGNOSTICS false // Change this to see diagnostics

// Declare the IridiumSBD object
IridiumSBD modem(IridiumSerial);

void setup() {
    Serial.begin(115200);
    Serial.println("Hello, World!");
}

// void setup()
// {
//     int signalQuality = -1;
//     int err;
// 
//     // Start the console serial port
//     Serial.begin(115200);
//     while (!Serial)
//         ;
// 
//     // Start the serial port connected to the satellite modem
//     IridiumSerial.begin(19200, SERIAL_8N1, RX2_PIN, TX2_PIN);
// 
//     // Begin satellite modem operation
//     Serial.println("Starting modem...");
//     err = modem.begin();
//     if (err != ISBD_SUCCESS)
//     {
//         Serial.print("Begin failed: error ");
//         Serial.println(err);
//         if (err == ISBD_NO_MODEM_DETECTED)
//             Serial.println("No modem detected: check wiring.");
//         return;
//     }
// 
//     // Example: Print the firmware revision
//     char version[12];
//     err = modem.getFirmwareVersion(version, sizeof(version));
//     if (err != ISBD_SUCCESS)
//     {
//         Serial.print("FirmwareVersion failed: error ");
//         Serial.println(err);
//         return;
//     }
//     Serial.print("Firmware Version is ");
//     Serial.print(version);
//     Serial.println(".");
// 
//     // Example: Test the signal quality.
//     // This returns a number between 0 and 5.
//     // 2 or better is preferred.
//     err = modem.getSignalQuality(signalQuality);
//     if (err != ISBD_SUCCESS)
//     {
//         Serial.print("SignalQuality failed: error ");
//         Serial.println(err);
//         return;
//     }
// 
//     Serial.print("On a scale of 0 to 5, signal quality is currently ");
//     Serial.print(signalQuality);
//     Serial.println(".");
// 
//     // Send the message
//     Serial.print("Trying to send the message.  This might take several minutes.\r\n");
//     err = modem.sendSBDText("Hello, world!");
//     if (err != ISBD_SUCCESS)
//     {
//         Serial.print("sendSBDText failed: error ");
//         Serial.println(err);
//         if (err == ISBD_SENDRECEIVE_TIMEOUT)
//             Serial.println("Try again with a better view of the sky.");
//     }
// 
//     else
//     {
//         Serial.println("Hey, it worked!");
//     }
// }

// don't use
void loop() {}
