#include <Arduino.h>
#include <Wire.h>
#include <stdarg.h>
#include <stdio.h>

#include "driver_aht30_interface.h"

/**
 * @brief  interface iic bus init
 * @return status code
 *         - 0 success
 *         - 1 iic init failed
 * @note   none
 */
extern "C" uint8_t aht30_interface_iic_init(void)
{
    Wire.begin();
    Wire.setClock(100000);

    return 0;
}

/**
 * @brief  interface iic bus deinit
 * @return status code
 *         - 0 success
 *         - 1 iic deinit failed
 * @note   none
 */
extern "C" uint8_t aht30_interface_iic_deinit(void)
{
    Wire.end();

    return 0;
}

/**
 * @brief      interface iic bus read
 * @param[in]  addr iic device write address
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len length of the data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
extern "C" uint8_t aht30_interface_iic_read_cmd(uint8_t addr, uint8_t *buf, uint16_t len)
{
    uint16_t i;
    const uint8_t dev_addr = (uint8_t)(addr >> 1);

    if (Wire.requestFrom((int)dev_addr, (int)len) != (int)len)
    {
        return 1;
    }

    for (i = 0; i < len; i++)
    {
        if (!Wire.available())
        {
            return 1;
        }
        buf[i] = (uint8_t)Wire.read();
    }

    return 0;
}

/**
 * @brief     interface iic bus write
 * @param[in] addr iic device write address
 * @param[in] *buf pointer to a data buffer
 * @param[in] len length of the data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
extern "C" uint8_t aht30_interface_iic_write_cmd(uint8_t addr, uint8_t *buf, uint16_t len)
{
    uint16_t i;
    const uint8_t dev_addr = (uint8_t)(addr >> 1);

    Wire.beginTransmission(dev_addr);
    for (i = 0; i < len; i++)
    {
        Wire.write(buf[i]);
    }

    return (Wire.endTransmission() == 0) ? 0 : 1;
}

/**
 * @brief     interface delay ms
 * @param[in] ms time
 * @note      none
 */
extern "C" void aht30_interface_delay_ms(uint32_t ms)
{
    delay(ms);
}

/**
 * @brief     interface print format data
 * @param[in] fmt format data
 * @note      none
 */
extern "C" void aht30_interface_debug_print(const char *const fmt, ...)
{
    char buffer[192];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    Serial.print(buffer);
}
