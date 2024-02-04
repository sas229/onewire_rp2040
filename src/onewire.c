#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "include/onewire.h"

bool ow_init(OW *ow, PIO pio, uint offset, uint gpio) {
    int sm = pio_claim_unused_sm(pio, false);
    if (sm == -1) {
        return false;
    }
    gpio_init(gpio);    // Enable the GPIO and clear any output value.
    pio_gpio_init(pio, gpio);      // Set the function to PIO output.
    ow->gpio = gpio;
    ow->pio = pio;
    ow->offset = offset;
    ow->sm = (uint)sm;
    ow->jmp_reset = ow_reset_instr(ow->offset);   // Assemble the bus reset instruction.
    ow_sm_init(ow->pio, ow->sm, ow->offset, ow->gpio, 8); // Set 8 bits per byte.
    return true;
}

void ow_send(OW *ow, uint data) {
    pio_sm_put_blocking(ow->pio, ow->sm, (uint32_t)data);
    pio_sm_get_blocking(ow->pio, ow->sm);  // Discard the response.
}

uint8_t ow_read(OW *ow) {
    pio_sm_put_blocking(ow->pio, ow->sm, 0xff);    // Generate read slots.
    return (uint8_t)(pio_sm_get_blocking (ow->pio, ow->sm) >> 24);  // Shift response into bits 0..7.
}

bool ow_reset(OW *ow) {
    pio_sm_exec_wait_blocking(ow->pio, ow->sm, ow->jmp_reset);
    if ((pio_sm_get_blocking(ow->pio, ow->sm) & 1) == 0) {     // Apply pin mask (see pio program).
        return true;    // A slave pulled the bus low.
    }
    return false;
}

int ow_romsearch(OW *ow, uint64_t *romcodes, int maxdevs, uint command) {
    int index;
    uint64_t romcode = 0ull;
    int branch_point;
    int next_branch_point = -1;
    int num_found = 0;
    bool finished = false;

    ow_sm_init(ow->pio, ow->sm, ow->offset, ow->gpio, 1); // Set driver to 1-bit mode.

    while (finished == false && (maxdevs == 0 || num_found < maxdevs )) {
        finished = true;
        branch_point = next_branch_point;
        if (ow_reset(ow) == false) {
            // No slaves present.
            num_found = 0;
            finished = true;
            break;
        }
        for (int i = 0; i < 8; i += 1) {
            // Send search command as single bits.
            ow_send(ow, command >> i);
        }
        for (index = 0; index < 64; index += 1) {
            // Determine ROM code bits 0..63 (see ref).
            uint a = ow_read(ow);
            uint b = ow_read(ow);
            if (a == 0 && b == 0) {         // (a, b) = (0, 0)
                if (index == branch_point) {
                    ow_send(ow, 1);
                    romcode |= (1ull << index);
                } else {
                    if (index > branch_point || (romcode & (1ull << index)) == 0) {
                        ow_send(ow, 0);
                        finished = false;
                        romcode &= ~(1ull << index);
                        next_branch_point = index;
                    } else {                // index < branch_point or romcode[index] = 1
                        ow_send(ow, 1);
                    }
                }
            } else if (a != 0 && b != 0) {  // (a, b) = (1, 1) error (e.g. device disconnected).
                num_found = -2;             // Function will return -1.
                finished = true;
                break;                      // Terminate for loop.
            } else {
                if (a == 0) {               // (a, b) = (0, 1) or (1, 0)
                    ow_send(ow, 0);
                    romcode &= ~(1ull << index);
                } else {
                    ow_send(ow, 1);
                    romcode |= (1ull << index);
                }
            }
        }                                   // End of for loop.
        if (romcodes != NULL) {
            // Check crc and add romcode to array if OK.
            uint8_t crc = 0;
            for (int i=7; i>0; i--) {
                uint8_t byte = (uint8_t)(romcode << (8*i));
                crc = ow_update_crc_8(crc, byte);
            }
            if (crc == (uint8_t)((romcode << (8*7)))) {     // Check against crc byte.
                romcodes[num_found] = romcode;              // Store the ROM code.
            }
        }
        num_found += 1;
    }                                       // End of while loop.
    ow_sm_init(ow->pio, ow->sm, ow->offset, ow->gpio, 8); // Restore 8-bit mode.
    return num_found;
}

uint8_t ow_family(const uint64_t* romcode) {
    int n = 0;
    uint8_t family = (*romcode << (8*n)) & 0xff;     // Get family byte from ROM code.
    return family;
}

void ow_select(OW* ow, uint64_t* romcode) {
    if (romcode == NULL) {
        ow_send(ow, OW_SKIP_ROM);
    } else {
        ow_send(ow, OW_MATCH_ROM);
        for (int b = 0; b < 64; b += 8) {
            ow_send(ow, *romcode >> b);
        }
    }
}

uint8_t ow_crc_8(const uint8_t* buffer, size_t len) {
    size_t a;
    uint8_t crc;
    const unsigned char *ptr;
    crc = CRC_START_8;
    ptr = buffer;
    if ( ptr != NULL ) {
        for (a=0; a<len; a++) {
            crc = sht75_crc_table[(*ptr++) ^ crc];
        }
    }
    return crc;
}

uint8_t ow_update_crc_8(uint8_t crc, uint8_t val) {
    return sht75_crc_table[val^crc];
}

uint16_t ow_crc_16(const uint8_t *buffer, size_t len) {
    uint16_t crc;
    const uint8_t *ptr;
    size_t a;
    if (!crc_tab16_init) ow_init_crc16_tab();
    crc = CRC_START_16;
    ptr = buffer;
    if (ptr != NULL) for (a=0; a<len; a++) {
            crc = (crc >> 8) ^ crc_tab16[(crc ^ (uint16_t) *ptr++) & 0x00FF];
        }
    return crc;
}

uint16_t ow_update_crc_16(uint16_t crc, uint8_t c) {
    if (!crc_tab16_init) ow_init_crc16_tab();
    return (crc >> 8) ^ crc_tab16[(crc ^ (uint16_t) c) & 0x00FF];
}

bool ow_check_crc_16(uint8_t* buffer, size_t len, const uint8_t* inverted_crc) {
    uint16_t crc = ~ow_crc_16(buffer, len);
    return (crc & 0xFF) == inverted_crc[0] && (crc >> 8) == inverted_crc[1];
}