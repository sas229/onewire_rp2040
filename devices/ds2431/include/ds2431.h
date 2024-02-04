#ifndef _DS2431_H
#define _DS2431_H

#include "pico/stdio.h"
#include "pico/time.h"
#include "onewire.h"
#include <string.h>

#define DS2431_FAMILY               0x2d    /**< OneWire family byte.*/
#define DS2431_READ_SCRATCHPAD      0xAA    /**< Read scratchpad command.*/
#define DS2431_WRITE_SCRATCHPAD     0x0F    /**< Write scratchpad command.*/
#define DS2431_COPY_SCRATCHPAD      0x55    /**< Copy scratchpad command.*/
#define DS2431_READ_MEMORY          0xF0    /**< Read memory command.*/
#define DS2431_PF_MASK              0x07    /**< PF mask byte.*/
#define DS2431_START                0x0000  /**< Memory start address.*/
#define DS2431_SUCCESS              0xAA    /**< Write success byte.*/
#define DS2431_SIZE                 128     /**< Memory size in bytes.*/
#define DS2431_PAGES                4       /**< Memory size in pages.*/
#define DS2431_ROW_SIZE             8       /**< Memory row size in bytes.*/
#define DS2431_WRITE_CMD_SIZE       3       /**< Write command size.*/
#define DS2431_READ_CMD_SIZE        4       /**< Read command size.*/
#define DS2431_COPY_CMD_SIZE        4       /**< Copy command size.*/
#define DS2431_READ_RETRY           2       /**< Maximum read retry attempts.*/

/**
 * @brief Write arbitrary length buffer to EEPROM. Returns a boolean indicating success status.
 * 
 * @param ow OneWire instance.
 * @param romcode ROM code of target device.
 * @param address Address to write to.
 * @param buffer Buffer of bytes to write.
 * @param len Length of buffer to write.
 * @return true 
 * @return false 
 */
bool ds2431_write(OW* ow, uint64_t* romcode, uint16_t address, uint8_t* buffer, size_t len);

/**
 * @brief Write a row to EEPROM. Returns a boolean indicating success status. 
 * 
 * @param ow OneWire instance.
 * @param romcode ROM code of target device.
 * @param address Address to write to.
 * @param buffer Buffer to write bytes from.
 * @param len Length of buffer.
 * @return true 
 * @return false 
 */
bool ds2431_write_row(OW* ow, uint64_t* romcode, uint16_t address, uint8_t* buffer, size_t len);

/**
 * @brief Read from EEPROM. Returns a boolean indicating success status.
 * 
 * @param ow OneWire instance.
 * @param romcode ROM code of target device.
 * @param address Address to read from.
 * @param buffer Buffer to write bytes to.
 * @param len Length of buffer.
 * @return true 
 * @return false 
 */
bool ds2431_read(OW* ow, uint64_t* romcode, uint16_t address, uint8_t* buffer, size_t len);

/**
 * @brief Clear EEPROM. Returns a boolean indicating success status.
 * 
 * @param ow OneWire instance.
 * @param romcode ROM code of target device.
 * @return true 
 * @return false 
 */
bool ds2431_clear(OW* ow, uint64_t* romcode);

#endif