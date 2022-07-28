cmake_minimum_required(VERSION 3.12)

set(FAMILY rp2040)
set(PICO_BOARD_HEADER_DIRS ${CMAKE_SOURCE_DIR}/Application/configuration)
set(PICO_BOARD flashprogrammer)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(${PICO_SDK_PATH}/lib/tinyusb/hw/bsp/rp2040/family.cmake)
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)
include(PicoSDKConfig.cmake)

project(RP2040FlashProgrammer C CXX ASM)

pico_sdk_init()


add_subdirectory(Application)