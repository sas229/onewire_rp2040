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
    int16_t temp = 0;
    temp = ow_read(ow) | (ow_read(ow) << 8);
    return temp;
}
