# OneWire Library for RP2040 using PIO

This library is based on the PIO based driver provided in [Pico
Examples](https://github.com/raspberrypi/pico-examples/tree/master/pio/onewire). A few additions have been made
warranting the creation of this library, including:

- CRC verification of ROM codes for devices found in the search algorithm using various functions borrowed from [libcrc]
  (https://github.com/lammertb/libcrc);
- Functions to extract the family code from the device ROM code, which is useful when writing preocedural code for 
  plu-and-play devices; and
- A driver for the DS2431 (also works with DS28E07) 1k EEPROM with CRC verification and functions to read and write 
  buffers of arbitrary lengths from a given starting address with bounds checking. Note that functions to provide write protection for the
  DS2431 have not been provided but would it would be simple to extend the library to provide this feature.

To add the library to your project:

    git submodule add https://github.com/sas229/onewire_rp2040.git

To then use the library simply add something similar to the following in your CMakeLists.txt file:

    add_subdirectory(path_to/onewire_rp2040")

    target_link_libraries(MyProject
       ...
       onewire
    )

To build the examples, add the following CMake build options:

    cmake -DPICO_SDK_PATH=/path/to/pico-sdk -DONEWIRE_EXAMPLES=1 -DWRITE_DS2431=1 ..

The option sets the path to the Pico SDK, whilst the second and third build the example and 
selects whether to write to the DS2431 EEPROM (if present on the bus) as part of the demo, or just read. Try with
<code>-DWRITE_DS2431=1</code> first and then try with <code>-DWRITE_DS2431=0</code>.