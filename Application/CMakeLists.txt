set(FREERTOS_KERNEL_PATH ${PROJECT_SOURCE_DIR}\\..\\..\\FreeRTOS\\Source)
set(CFG_TUSB_OS_INC_PATH ${FREERTOS_KERNEL_PATH}\\include\\)

include(FreeRTOS_Kernel_import.cmake)

add_executable(${PROJECT_NAME} usb_descriptors.c Applications/ApplicationManager.cpp Hal/HardwareVersion.cpp)

target_sources(${PROJECT_NAME} PUBLIC
     ${CMAKE_CURRENT_LIST_DIR}/EntryPoint.cpp
     ${CMAKE_CURRENT_LIST_DIR}/Applications/Application.cpp
     ${CMAKE_CURRENT_LIST_DIR}/Applications/StatusLED.cpp
     ${CMAKE_CURRENT_LIST_DIR}/Applications/UARTForwarder.cpp
     ${CMAKE_CURRENT_LIST_DIR}/Hal/PIOUartRx.cpp
     ${CMAKE_CURRENT_LIST_DIR}/Applications/Console.cpp
     ${CMAKE_CURRENT_LIST_DIR}/Hal/HardwareUart.cpp
     ${CMAKE_CURRENT_LIST_DIR}/Hal/GPIO.cpp
     ${CMAKE_CURRENT_LIST_DIR}/Hal/S1000.cpp
)

include(FreeRTOS-Addons/freertos-addons.cmake)

target_compile_definitions(${PROJECT_NAME} PUBLIC $<$<CONFIG:Debug>:SNIFFER_DEBUG>)
target_compile_definitions(${PROJECT_NAME} PUBLIC CPP_FREERTOS_NO_EXCEPTIONS)
target_compile_definitions(${PROJECT_NAME} PUBLIC FIRMWARE_VERSION_MAJOR=${VERSION_MAJOR})
target_compile_definitions(${PROJECT_NAME} PUBLIC FIRMWARE_VERSION_MINOR=${VERSION_MINOR})
target_compile_definitions(${PROJECT_NAME} PUBLIC FIRMWARE_VERSION_BUILD=${VERSION_BUILD})
target_compile_definitions(${PROJECT_NAME} PUBLIC SIMPLE_COM_PORTS)

#USB is used to communicate with the PC software
#pico_enable_stdio_usb(AppMain 1)
#pico_enable_stdio_uart(${PROJECT_NAME} 1)
#PICO_ENABLE_STDIO_SEMIHOSTING(AppMain 1)


target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${CFG_TUSB_OS_INC_PATH})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Applications)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Configuration)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/PIO)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Hal)

target_link_libraries(${PROJECT_NAME} PUBLIC pico_stdlib pico_unique_id hardware_pio hardware_dma hardware_watchdog FreeRTOS-Kernel FreeRTOS-Kernel-Heap4 tinyusb_device tinyusb_board)

# Create C header file with the name <pio program>.pio.h
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/PIO/UartRx.pio)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(${PROJECT_NAME})