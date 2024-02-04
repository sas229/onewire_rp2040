# Create executable.
add_executable(onewire_example
        onewire_example.c
        )

# Libraries.
target_link_libraries(onewire_example
        pico_stdlib
        onewire
        )

# Compile definitions.
target_compile_definitions(onewire_example PRIVATE
        WRITE_DS2431=${WRITE_DS2431}
        )

# Enable stdio outputs as required.
pico_enable_stdio_usb(onewire_example 0)
pico_enable_stdio_uart(onewire_example 1)

# Create output.
pico_add_extra_outputs(onewire_example)
