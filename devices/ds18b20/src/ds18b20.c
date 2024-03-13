#include "ds18b20.h"

void ds18b20_convert_temperature_all(OW *ow) {
    // Send conversion command.
    ow_reset(ow);
    ow_select(ow, NULL);
    ow_send(ow, DS18B20_CONVERT_T);

    // Wait for the conversions to finish.
    while (ow_read(ow) == 0) {
        sleep_ms(10);
    }
}

void ds18b20_convert_temperature(OW *ow, uint64_t *romcode) {
    // Send conversion command.
    ow_reset(ow);
    ow_select(ow, romcode);
    ow_send(ow, DS18B20_CONVERT_T);

    // Wait for the conversions to finish.
    while (ow_read(ow) == 0) {
        sleep_ms(10);
    }
}

int16_t ds18b20_read_temperature(OW *ow, uint64_t *romcode) {
    // Send read command.
    ow_reset(ow);
    ow_select(ow, romcode);
    ow_send(ow, DS18B20_READ_SCRATCHPAD);

    // Read temperature bytes from the DS18B20 scratchpad.
    uint8_t data[2];
    data[0] = ow_read(ow); // LSB
    data[1] = ow_read(ow); // MSB
    uint16_t temp12 = (data[1] << 8) + data[0]; // 12-bit temperature.

    // Check if temperature is negative.
    int sign = 1;
    if (temp12 > 2047) {
        temp12 = (~temp12) + 1; // Two's complement to find the magnitude.
        sign = -1;
    }
    int16_t temp = sign * temp12;

    return temp;
}
