/**
 * For the pico-examples derived functionality:
 *
 * Copyright (c) 2023 mjcross
 *
 * SPDX-License-Identifier: BSD-3-Clause
**/

#ifndef _DS18B20_H
#define _DS18B20_H

#include "pico/stdlib.h"
#include "onewire.h"

#define DS18B20_FAMILY              0x28    /**< OneWire family code. */
#define DS18B20_CONVERT_T           0x44    /**< Convert temperature command.*/
#define DS18B20_WRITE_SCRATCHPAD    0x4e    /**< Write scratchpad command.*/
#define DS18B20_READ_SCRATCHPAD     0xbe    /**< Read scratchpad command.*/
#define DS18B20_COPY_SCRATCHPAD     0x48    /**< Copy scratchpad command.*/
#define DS18B20_RECALL_EE           0xb8    /**< Recall EEPROM registers command.*/
#define DS18B20_READ_POWER_SUPPLY   0xb4    /**< Read power supply command.*/

/**
 * @brief Command all DS18B20 devices on bus to convert a temperature reading.
 * 
 * @param ow OneWire instance.
 */
void ds18b20_convert_temperature_all(OW *ow);

/**
 * @brief Command a specific device to convert a temperature reading.
 * 
 * @param ow OneWire instance.
 * @param romcode ROM code of target device.
 */
void ds18b20_convert_temperature(OW *ow, uint64_t *romcode);

/**
 * @brief Read the temperature from a specific device.
 * 
 * @param ow OneWire instance
 * @param romcode ROM code of target device.
 * @return int16_t 
 */
int16_t ds18b20_read_temperature(OW *ow, uint64_t *romcode);

#endif