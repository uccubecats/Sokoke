#include "src/ina_function/ina_function.h"

InaData read_ina228(Adafruit_INA228 *ina, bool ina_alive) {
    if (!ina_alive) return {0};

    return (InaData){
        .busVoltage = ina->getBusVoltage_V() + ina->getShuntVoltage_mV() / 1000,
        .current = ina->getCurrent_mA(),
        .success = true,
    };
}

InaData read_ina219(Adafruit_INA219 *ina, bool ina_alive) {
    if (!ina_alive) return {0};

    return (InaData){
        .busVoltage = ina->getBusVoltage_V() + ina->getShuntVoltage_mV() / 1000,
        .current = ina->getCurrent_mA(),
        .success = true,
    };
}