#include "pico/stdlib.h"
#include "onewire.h"
#include "ds18b20.h"
#include "ds2431.h"
#include <stdio.h>

int main() {
    // Serial interface.
    stdio_init_all();
    sleep_ms(2000);

    // Setup PIO OneWire.
    PIO pio = pio0;
    uint gpio = 15;
    OW ow;
    uint offset;

    // Add the program to the PIO shared address space.
    if (!pio_can_add_program (pio, &onewire_program)) {
        printf("Could not add the program.\n");
    } else {
        offset = pio_add_program (pio, &onewire_program);

        // Claim a state machine and initialise a driver instance.
        if (!ow_init(&ow, pio, offset, gpio)) {
            printf("Could not initialise the driver.\n");
        } else {
            // Find and display 64-bit device addresses.
            int max_devices = 10;
            uint64_t romcode[max_devices];
            int num_devices = ow_romsearch(&ow, romcode, max_devices, OW_SEARCH_ROM);

            printf("Found %d device(s)\n", num_devices);
            for (int i = 0; i < num_devices; i += 1) {
                printf("Family: 0x%x ROM: 0x%llx\n", ow_family(&romcode[i]), romcode[i]);
            }
            for (int i=0; i < num_devices; i++) {
                uint8_t family = ow_family(&romcode[i]);
                if (family == DS2431_FAMILY) {
                    uint16_t address = DS2431_START;
                    bool success;
                    char write_buffer[128];
                    char str[] = "This is a test of the OneWire EEPROM on device with ROM: 0x%llx";
                    size_t len = snprintf(write_buffer, sizeof(write_buffer), str, romcode[i]);
#if WRITE_DS2431
                    {
                        // Erase EEPROM.
                        success = ds2431_clear(&ow, &romcode[i]);
                        if (success) {
                            printf("Successfully erased EEPROM contents!\n");
                        }

                        // Write to EEPROM.
                        success = ds2431_write(&ow, &romcode[i], address, write_buffer, len);
                        if (success) {
                            printf("Buffer successfully written to EEPROM!\n");
                        }
                    }
#endif
                    {
                        // Read back from EEPROM.
                        uint8_t read_buffer[len];
                        success = ds2431_read(&ow, &romcode[i], address, read_buffer, len);
                        if (success) {
                            printf("Buffer successfully read from EEPROM!\n");
                        }

                        // Print EEPROM contents.
                        printf("EEPROM: ");
                        for (int j=0; j<len; j++) {
                            printf("%c", read_buffer[j]);
                        }
                        printf("\n");
                    }
                }
            }
            if (num_devices > 0) {
                printf("Printing temperature from all DS18B20 devices on OneWire bus...\n");
                while (true) {
                    // Start temperature conversion in parallel on all DS18B20 devices.
                    ds18b20_convert_temperature_all(&ow);

                    // Read the result from each DS18B20 device.
                    for (int i = 0; i < num_devices; i += 1) {
                        uint8_t family = ow_family(&romcode[i]);
                        if (family == DS18B20_FAMILY) {
                            printf("ROM: 0x%llx ", romcode[i]);
                            int16_t temp = ds18b20_read_temperature(&ow, &romcode[i]);
                            printf("%f; ", temp / 16.0);
                        }

                    }
                    printf("\n");
                }
            }
        }
    }
}