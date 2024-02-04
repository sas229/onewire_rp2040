#include "ds2431.h"

bool ds2431_write(OW* ow, uint64_t* romcode, uint16_t address, uint8_t* buffer, size_t len) {
    // Check address is valid (within memory scope and divisible by 8).
    if (address >= DS2431_SIZE || address % 8 != 0) {
        return false;
    }

    // Check buffer length is less than memory size.
    if (len > DS2431_SIZE) {
        return false;
    }

    // Write row by row.
    uint8_t count = 0;
    bool success;
    uint8_t* row = &buffer[0];
    while (count < len) {
        success = ds2431_write_row(ow, romcode, address, row, DS2431_ROW_SIZE);
        address += DS2431_ROW_SIZE;
        row += DS2431_ROW_SIZE;
        count += DS2431_ROW_SIZE;
    }
    return success;
}

bool ds2431_write_row(OW* ow, uint64_t* romcode, uint16_t address, uint8_t* buffer, size_t len) {
    // Prepare command.
    uint8_t TA1 = address << 0;
    uint8_t TA2 = address << 8;
    uint8_t command[DS2431_WRITE_CMD_SIZE+len];
    command[0] = DS2431_WRITE_SCRATCHPAD;                                   // Command.
    command[1] = TA1;                                                       // Offset.
    command[2] = TA2;                                                       // Address.
    for (int i=DS2431_WRITE_CMD_SIZE; i<DS2431_WRITE_CMD_SIZE+len; i++) {   // Data.
        command[i] = buffer[i-DS2431_WRITE_CMD_SIZE];
    }

    // Select device.
    bool present = ow_reset(ow);
    if (!present) {
        return false;
    }
    ow_select(ow, romcode);

    // Send command.
    for (int i=0; i<sizeof(command); i++) {
        ow_send(ow, command[i]);
    }

    // Read inverted CRC.
    uint8_t inverted_crc_16[2];
    inverted_crc_16[0] = ow_read(ow);
    inverted_crc_16[1] = ow_read(ow);

    // Check CRC.
    bool valid = ow_check_crc_16(command, sizeof(command), inverted_crc_16);
    if (!valid) {
        return false;
    }

    // Verify scratchpad contents.
    uint8_t error_count = 0;
    bool verify = false;
    uint8_t check[DS2431_READ_CMD_SIZE+len];
    do {
        // Select device.
        present = ow_reset(ow);
        if (!present) {
            return false;
        }
        ow_select(ow, romcode);

        // Read scratchpad.
        ow_send(ow, DS2431_READ_SCRATCHPAD);
        check[0] = DS2431_READ_SCRATCHPAD;              // Command.
        check[1] = ow_read(ow);                         // TA1.
        check[2] = ow_read(ow);                         // TA2.
        check[3] = ow_read(ow);                         // E/S.
        if (check[3] != DS2431_PF_MASK) {
            verify = true;
        }

        // Verify data integrity.
        if (verify) {
            // Read the data.
            for (int i=DS2431_READ_CMD_SIZE; i<DS2431_READ_CMD_SIZE+len; i++) { // Data.
                check[i] = ow_read(ow);
            }
            // Read inverted CRC.
            inverted_crc_16[0] = ow_read(ow);
            inverted_crc_16[1] = ow_read(ow);

            // Check CRC.
            valid = ow_check_crc_16(check, sizeof(check), inverted_crc_16);
            if (!valid) {
                error_count++;
                continue;
            }

            // Check address.
            if (address != ((check[2] << 8) + check[1])) {      
                return false;       
            }

            // Check transfer length.
            if (check[3] != DS2431_PF_MASK) {
                return false;
            }

            // Compare scratchpad to buffer.
            if (memcmp(&check[4], buffer, 8) != 0) {
                return false;
            }
        }
        break;
    } while (error_count < DS2431_READ_RETRY);

    // Select device.
    present = ow_reset(ow);
    if (!present) {
        return false;
    }
    ow_select(ow, romcode);

    // Copy scratchpad.
    command[0] = DS2431_COPY_SCRATCHPAD;
    command[1] = check[1];
    command[2] = check[2];
    command[3] = check[3];
    for (int i=0; i<DS2431_COPY_CMD_SIZE; i++) {
        ow_send(ow, command[i]);
    }
    sleep_ms(15);

    // Check copy status.
    uint8_t success = ow_read(ow) & DS2431_SUCCESS;
    if (!success) {
        return false;    
    }
    return true;
}

bool ds2431_read(OW* ow, uint64_t* romcode, uint16_t address, uint8_t* buffer, size_t len) {
    // Check address is valid (within memory scope and divisible by 8).
    if (address >= DS2431_SIZE || address % 8 != 0) {
        return false;
    }
    uint8_t TA1 = address << 0;
    uint8_t TA2 = address << 8;

    // Select device.
    bool present = ow_reset(ow);
    if (!present) {
        return false;
    }
    ow_select(ow, romcode);
    
    // Send.
    ow_send(ow, DS2431_READ_MEMORY);    // Command.
    ow_send(ow, TA1);                   // Offset
    ow_send(ow, TA2);                   // Address.
    for (int i=0; i<len; i++) {              // Data.
        buffer[i] = ow_read(ow);
    }
}

bool ds2431_clear(OW* ow, uint64_t* romcode) {
    uint8_t buffer[DS2431_SIZE] = {0};
    return ds2431_write(ow, romcode, DS2431_START, buffer, DS2431_SIZE);
}