cmake_minimum_required(VERSION 3.12)

include(${PICO_SDK_PATH}/lib/tinyusb/hw/bsp/rp2040/family.cmake)
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)
include(PicoSDKConfig.cmake)


set(PROJECT_NAME S1000_Sniffinator)
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_BUILD 1)

project(${PROJECT_NAME} C CXX ASM)

set(FAMILY rp2040)
set(PICO_BOARD_HEADER_DIRS ${CMAKE_SOURCE_DIR}/Application/configuration)
set(PICO_BOARD S1000Sniffinator)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_subdirectory(Application)

