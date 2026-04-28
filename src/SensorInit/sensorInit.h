#pragma once

#include "BluetoothFunction/BluetoothFunction.h"
#include "PIDHeatController/PIDHeatController.h"
#include "PWMController/PWMController.h"
#include "RockblockFunction/RockblockFunction.h"
#include "SdFunction/SdFunction.h"
#include "Sensors.h"
#include "aht30Function/aht30Function.h"
#include "bmp_function/bmp_function.h"
#include "gyro_function/gyro_function.h"
#include "ina_function/ina_function.h"
#include "log_wrapper/log_wrapper.h"
#include "mcp_function/mcp_function.h"


#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP3XX.h>
#include <Adafruit_FXAS21002C.h>
#include <Adafruit_FXOS8700.h>
#include <Adafruit_INA219.h>
#include <Adafruit_INA228.h>
#include <Adafruit_MCP9808.h>
#include <Arduino.h>
#include <MadgwickAHRS.h>

#define MAX_INIT_ATTEMPTS 10

extern bool bmp_outside_alive;
extern bool bmp_inside_alive;
extern bool fxos_fxas_alive;
extern bool mcp_alive;
extern bool aht_alive;
extern bool ina_low_alive;
extern bool ina_high_alive;
extern bool pwm_fan_alive;
extern bool pwm_heater_alive;
extern bool rockblock_alive;

extern Adafruit_FXOS8700 fxos;
extern Adafruit_FXAS21002C fxas;
extern Madgwick madgwick;
extern float mag_offsets[3];

extern Adafruit_BMP3XX bmp_outside;
extern Adafruit_BMP3XX bmp_inside;

extern Adafruit_MCP9808 mcp;

extern Adafruit_AHTX0 aht;

extern Adafruit_INA228 ina_low;
extern Adafruit_INA219 ina_high;

int attempt_init_fxos8700(int current_attempt = 1);
int attempt_init_fxas21002(int current_attempt = 1);
int attempt_init_bmp390(Adafruit_BMP3XX *bmp, int address,
                        int current_attempt = 1);
int attempt_init_mcp9808(int current_attempt = 1);
int attempt_init_aht30(int current_attempt = 1);
int attempt_init_mutex(int current_attempt = 1);
int attempt_init_sdreader(int current_attempt = 1);
int attempt_init_ina228(Adafruit_INA228 *ina, int address,
                        int current_attempt = 1);
int attempt_init_ina219(Adafruit_INA219 *ina, int address,
                        int current_attempt = 1);
int attempt_init_rockblock_buffer(int current_attempt = 1);
int attempt_init_rockblock(int current_attempt = 1);
