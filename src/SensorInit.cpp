#include "SensorInit.h"
#include <arduino.h>

bool bmp_outside_alive;
bool bmp_inside_alive;
bool fxos_fxas_alive;
bool mcp_alive;
bool aht_alive;
bool ina_low_alive;
bool ina_high_alive;
bool pwm_fan_alive;
bool pwm_heater_alive;
bool rockblock_alive;

Adafruit_FXOS8700 fxos = Adafruit_FXOS8700(0x1F);
Adafruit_FXAS21002C fxas = Adafruit_FXAS21002C(0x0021002C);
Madgwick madgwick;
float mag_offsets[3] = {0.0, 0.0, 0.0};

Adafruit_BMP3XX bmp_outside;
Adafruit_BMP3XX bmp_inside;

Adafruit_MCP9808 mcp = Adafruit_MCP9808();

Adafruit_AHTX0 aht;

Adafruit_INA228 ina_low = Adafruit_INA228();
Adafruit_INA219 ina_high;

int attempt_init_fxos8700(int current_attempt) {
  if (current_attempt > MAX_INIT_ATTEMPTS) {
    return 0;
  }
  lineoutPrintf("Attempt %d to initialize FXOS8700\n", current_attempt);

  if (!fxos.begin()) {
    return attempt_init_fxos8700(current_attempt + 1);
  }
  return 1;
}

int attempt_init_fxas21002(int current_attempt) {
  if (current_attempt > MAX_INIT_ATTEMPTS) {
    return 0;
  }
  lineoutPrintf("Attempt %d to initialize FXAS21002\n", current_attempt);

  if (!fxas.begin()) {
    return attempt_init_fxas21002(current_attempt + 1);
  }
  madgwick.begin(100);
  return 1;
}

int attempt_init_bmp390(Adafruit_BMP3XX *bmp, int address,
                        int current_attempt) {
  if (current_attempt > MAX_INIT_ATTEMPTS) {
    return 0;
  }
  lineoutPrintf("Attempt %d to initialize BMP390 on I2C Address 0x%x\n",
                current_attempt, address);

  if (!bmp->begin_I2C(address)) {
    return attempt_init_bmp390(bmp, address, current_attempt + 1);
  }
  return 1;
}

int attempt_init_mcp9808(int current_attempt) {
  if (current_attempt > MAX_INIT_ATTEMPTS) {
    return 0;
  }
  lineoutPrintf("Attempt %d to initialize MCP9808\n", current_attempt);

  if (!mcp.begin(0x18)) {
    return attempt_init_mcp9808(current_attempt + 1);
  }
  mcp.setResolution(3);
  mcp.wake();
  return 1;
}

int attempt_init_aht30(int current_attempt) {
  if (current_attempt > MAX_INIT_ATTEMPTS) {
    return 0;
  }
  lineoutPrintf("Attempt %d to initialize AHT30\n", current_attempt);

  if (!aht.begin()) {
    return attempt_init_aht30(current_attempt + 1);
  }
  return 1;
}

int attempt_init_mutex(int current_attempt) {
  if (current_attempt > MAX_INIT_ATTEMPTS) {
    return 0;
  }
  lineoutPrintf("Attempt %d to initialize mutex\n", current_attempt);

  logMutex = xSemaphoreCreateMutex();
  if (logMutex == NULL) {
    return attempt_init_mutex(current_attempt + 1);
  }
  return 1;
}

int attempt_init_sdreader(int current_attempt) {
  if (current_attempt > MAX_INIT_ATTEMPTS) {
    return 0;
  }
  lineoutPrintf("Attempt %d to initialize SD Card Reader\n", current_attempt);

  if (!initSDCard()) {
    // delay(1000); // maybe give it some time
    return attempt_init_sdreader(current_attempt + 1);
  }
  return 1;
}

int attempt_init_ina228(Adafruit_INA228 *ina, int address,
                        int current_attempt) {
  if (current_attempt > MAX_INIT_ATTEMPTS) {
    return 0;
  }
  lineoutPrintf("Attempt %d to initialize INA228 on I2C Address 0x%x\n",
                current_attempt, address);

  if (!ina->begin(address)) {
    return attempt_init_ina228(ina, address, current_attempt + 1);
  }
  ina->setShunt(0.015, 10.0);
  return 1;
}

int attempt_init_ina219(Adafruit_INA219 *ina, int address,
                        int current_attempt) {
  if (current_attempt > MAX_INIT_ATTEMPTS) {
    return 0;
  }
  lineoutPrintf("Attempt %d to initialize INA219 on I2C Address 0x%x\n",
                current_attempt, address);

  *ina = Adafruit_INA219(address);
  if (!ina->begin()) {
    return attempt_init_ina219(ina, address, current_attempt + 1);
  }
  return 1;
}

int attempt_init_rockblock_buffer(int current_attempt) {
  if (current_attempt > MAX_INIT_ATTEMPTS) {
    return 0;
  }
  lineoutPrintf("Attempt %d to initialize RockBlock\n", current_attempt);

  if (!initRockblockBuffer()) {
    return attempt_init_rockblock_buffer(current_attempt + 1);
  }
  return 1;
}

int attempt_init_rockblock(int current_attempt) {
  if (current_attempt > MAX_INIT_ATTEMPTS) {
    return 0;
  }
  lineoutPrintf("Attempt %d to initialize RockBlock\n", current_attempt);

  if (!initRockblock()) {
    return attempt_init_rockblock(current_attempt + 1);
  }
  return 1;
}