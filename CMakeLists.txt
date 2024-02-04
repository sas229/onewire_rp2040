# Build example.
if (ONEWIRE_EXAMPLE)
    # Set CMake requirement.
    cmake_minimum_required(VERSION 3.13)

    # Set language standards.
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_CXX_STANDARD 17)

    # Import SDK.
    include(pico_sdk_import.cmake)
    project(onewire_example C CXX ASM)
    pico_sdk_init()

    # Build examples.
    add_subdirectory(example)
endif()

# Build library.
add_library(onewire INTERFACE)

target_sources(onewire INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/onewire.c
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/ds18b20/src/ds18b20.c
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/ds2431/src/ds2431.c
        )

target_include_directories(onewire INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/ds18b20/include
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/ds2431/include
        )

pico_generate_pio_header(onewire
        ${CMAKE_CURRENT_LIST_DIR}/src/onewire.pio
        )

target_link_libraries(onewire INTERFACE
        pico_stdlib
        hardware_pio
        )

target_include_directories(onewire INTERFACE
	    ${CMAKE_CURRENT_SOURCE_DIR}
	    ${CMAKE_CURRENT_BINARY_DIR}
        )
