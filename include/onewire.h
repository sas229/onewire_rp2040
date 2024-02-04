/**
 * For the pico-examples derived functionality:
 *
 * Copyright (c) 2023 mjcross
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * For the libcrc derived functionality:
 *
 * MIT License
 *
 * Copyright (c) 1999-2016 Lammert Bies
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _ONEWIRE_H
#define _ONEWIRE_H

#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "onewire.pio.h"

#define OW_READ_ROM         0x33    /**< Read ROM command. */
#define OW_MATCH_ROM        0x55    /**< Match ROM command. */
#define OW_SKIP_ROM         0xCC    /**< Skip ROM command. */
#define OW_ALARM_SEARCH     0xEC    /**< Alarm search command. */
#define OW_SEARCH_ROM       0xF0    /**< Search ROM command. */

#define	CRC_START_8	    	0x00    /**< 8-bit CRC start value. */
#define	CRC_START_16	    0x0000  /**< 16-bit CRC start value. */
#define	CRC_POLY_16 		0xA001  /**< 16-bit CRC polynomial value. */

static bool crc_tab16_init = false; /**< Boolean to indicate if CRC table has been computed. */

static uint16_t crc_tab16[256];     /**< 16-bit CRC table. */

static uint8_t sht75_crc_table[] = {
        0,   49,  98,  83,  196, 245, 166, 151, 185, 136, 219, 234, 125, 76,  31,  46,
        67,  114, 33,  16,  135, 182, 229, 212, 250, 203, 152, 169, 62,  15,  92,  109,
        134, 183, 228, 213, 66,  115, 32,  17,  63,  14,  93,  108, 251, 202, 153, 168,
        197, 244, 167, 150, 1,   48,  99,  82,  124, 77,  30,  47,  184, 137, 218, 235,
        61,  12,  95,  110, 249, 200, 155, 170, 132, 181, 230, 215, 64,  113, 34,  19,
        126, 79,  28,  45,  186, 139, 216, 233, 199, 246, 165, 148, 3,   50,  97,  80,
        187, 138, 217, 232, 127, 78,  29,  44,  2,   51,  96,  81,  198, 247, 164, 149,
        248, 201, 154, 171, 60,  13,  94,  111, 65,  112, 35,  18,  133, 180, 231, 214,
        122, 75,  24,  41,  190, 143, 220, 237, 195, 242, 161, 144, 7,   54,  101, 84,
        57,  8,   91,  106, 253, 204, 159, 174, 128, 177, 226, 211, 68,  117, 38,  23,
        252, 205, 158, 175, 56,  9,   90,  107, 69,  116, 39,  22,  129, 176, 227, 210,
        191, 142, 221, 236, 123, 74,  25,  40,  6,   55,  100, 85,  194, 243, 160, 145,
        71,  118, 37,  20,  131, 178, 225, 208, 254, 207, 156, 173, 58,  11,  88,  105,
        4,   53,  102, 87,  192, 241, 162, 147, 189, 140, 223, 238, 121, 72,  27,  42,
        193, 240, 163, 146, 5,   52,  103, 86,  120, 73,  26,  43,  188, 141, 222, 239,
        130, 179, 224, 209, 70,  119, 36,  21,  59,  10,  89,  104, 255, 206, 157, 172
}; /**< SHT75 CRC table for 8-bit CRC. */

/**
 * @brief OneWire PIO configuration struct.
 * 
 */
typedef struct {
    PIO pio;        /**< PIO instance. */
    uint sm;        /**< State machine. */
    uint jmp_reset; /**< Jump reset. */
    int offset;     /**< Offset of program in memory. */
    int gpio;       /**< Pin for OneWire interface. */
} OW;

/**
 * @brief Initialise OneWire via PIO. Returns a boolean indicating success status.
 * 
 * @param ow OneWire instance.
 * @param pio PIO instance.
 * @param offset Offset.
 * @param gpio GPIO pin.
 * @return true 
 * @return false 
 */
bool ow_init(OW *ow, PIO pio, uint offset, uint gpio);

/**
 * @brief Send a byte on OneWire interface. Returns a boolean indicating success status.
 * 
 * @param ow OneWire instance.
 * @param data Data to send.
 */
void ow_send(OW *ow, uint data);

/**
 * @brief Read a byte on OneWire interface. Returns a boolean indicating success status.
 * 
 * @param ow OneWire instance.
 * @return uint8_t 
 */
uint8_t ow_read(OW *ow);

/**
 * @brief Reset OneWire interface. Returns a boolean indicating success status.
 * 
 * @param ow OneWire instance.
 * @return true 
 * @return false 
 */
bool ow_reset(OW *ow);

/**
 * @brief Perform ROM search on OneWire interface. Returns number of devices found.
 *
 * @param ow OneWire instance.
 * @param romcodes Array of ROM codes found.
 * @param maxdevs Maximum number of devices (0 means no limit).
 * @param command OneWire search command (e.g. OW_SEARCHROM or OW_ALARM_SEARCH).
 * @return int 
 */
int ow_romsearch(OW *ow, uint64_t *romcodes, int maxdevs, uint command);

/**
 * @brief Get OneWire family byte from ROM code.
 *
 * @note This function can take a NULL value for the romcode argument and will then use the OW_SKIP_ROM command.
 * Ensure that this is only used when a single device is on the bus.
 *
 * @param romcode Device ROM code.
 * @return uint8_t 
 */
uint8_t ow_family(const uint64_t* romcode);

/**
 * @brief Function to select a device based on the ROM code.
 *
 * @note This function can take a NULL value for the romcode argument and will then use the OW_SKIP_ROM command.
 * Ensure that this is only used when a single device is on the bus.
 *
 * @param ow
 * @param romcode
 */
void ow_select(OW *ow, uint64_t *romcode);

/**
 * @brief Initialise OneWire PIO state machine.
 *
 * @param pio PIO instance.
 * @param sm State machine.
 * @param offset Program offset.
 * @param pin_num GPIO pin.
 * @param bits_per_word Number of bits per word.
 */
static inline void ow_sm_init(PIO pio, uint sm, uint offset, uint pin_num, uint bits_per_word) {
    // Create a new state machine configuration.
    pio_sm_config c = onewire_program_get_default_config(offset);

    // Input Shift Register configuration settings.
    sm_config_set_in_shift(
            &c,
            true,               // Shift direction: right.
            true,               // Autopush: enabled.
            bits_per_word   // Autopush threshold.
    );

    // Output Shift Register configuration settings.
    sm_config_set_out_shift(
            &c,
            true,                  // Shift direction: right.
            true,                   // Autopull: enabled.
            bits_per_word       // Autopull threshold.
    );

    // Configure the input and sideset pin groups to start at `pin_num`.
    sm_config_set_in_pins(&c, pin_num);
    sm_config_set_sideset_pins(&c, pin_num);

    // Configure the clock divider for 1 usec per instruction.
    float div;
    div = (float) (clock_get_hz(clk_sys) * 1e-6);
    sm_config_set_clkdiv (&c, div);

    // Apply the configuration and initialise the program counter.
    pio_sm_init(pio, sm, offset + onewire_offset_fetch_bit, &c);

    // Enable the state machine.
    pio_sm_set_enabled(pio, sm, true);
}

/**
 * @brief Function to reset the OneWire instructions.
 *
 * @param offset Program offset.
 * @return uint
 */
static inline uint ow_reset_instr(uint offset) {
    // Encode a "jmp reset_bus side 0" instruction for the state machine.
    return pio_encode_jmp(offset + onewire_offset_reset_bus) | pio_encode_sideset (1, 0);
}

/**
 * @brief Function to compute an 8-bit CRC value from a buffer of bytes.
 *
 * @param buffer Buffer for which to calculate CRC.
 * @param len Length of buffer.
 * @return uint8_t
 */
uint8_t ow_crc_8(const uint8_t* buffer, size_t len);

/**
 * @brief Function to update an 8-bit CRC value using a given byte.
 *
 * @param crc Value of CRC.
 * @param byte Byte with which to update CRC.
 * @return uint8_t
 */
uint8_t ow_update_crc_8(uint8_t crc, uint8_t val);

/**
 * @brief Function to calculate a 16-nit CRC value from a buffer of bytes.
 *
 * @param buffer Buffer for which to calculate CRC.
 * @param len Length of buffer.
 * @return uint16_t
 */
uint16_t ow_crc_16(const uint8_t *buffer, size_t len);

/**
 * @brief Function to update a 16-bit CRC value using a given byte.
 *
 * @param crc Value of CRC.
 * @param byte Byte with which to update CRC.
 * @return uint16_t
 */
uint16_t update_crc_16(uint16_t crc, uint8_t c);

/**
 * @brief Perform cyclic redundancy check (CRC). Returns boolean status of check.
 *
 * @param buffer Buffer of data to check.
 * @param len Buffer length.
 * @param inverted_crc Inverted CRC from device.
 * @return true
 * @return false
 */
bool ow_check_crc_16(uint8_t* buffer, size_t len, const uint8_t* inverted_crc);

/**
 * @brief Function to initialise the 16-bit CRC table.
 */
static void ow_init_crc16_tab(void) {
    uint16_t i;
    uint16_t j;
    uint16_t crc;
    uint16_t c;
    for (i=0; i<256; i++) {
        crc = 0;
        c = i;
        for (j=0; j<8; j++) {
            if ((crc ^ c) & 0x0001) {
                crc = (crc >> 1) ^ CRC_POLY_16;
            } else {
                crc = crc >> 1;
            }
            c = c >> 1;
        }
        crc_tab16[i] = crc;
    }
    crc_tab16_init = true;
}

#endif