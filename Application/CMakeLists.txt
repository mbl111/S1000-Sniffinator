set(FREERTOS_KERNEL_PATH ${PROJECT_SOURCE_DIR}\\..\\..\\FreeRTOS-Kernel\\)
set(CFG_TUSB_OS_INC_PATH ${FREERTOS_KERNEL_PATH}\\include\\)

include(FreeRTOS_Kernel_import.cmake)


#project(AppMain C CXX ASM)

add_executable(AppMain usb_descriptors.c)



target_sources(AppMain PUBLIC
     ${CMAKE_CURRENT_LIST_DIR}/Application.cpp
     ${CMAKE_CURRENT_LIST_DIR}/EntryPoint.cpp
     ${CMAKE_CURRENT_LIST_DIR}/StatusLED.cpp
     ${CMAKE_CURRENT_LIST_DIR}/CPPTask.cpp
     ${CMAKE_CURRENT_LIST_DIR}/Mutex.cpp
)

target_compile_definitions(AppMain PUBLIC $<$<CONFIG:Debug>:FLASHPROGRAMMER_DEBUG>)

#USB is used to communicate with the PC software
#pico_enable_stdio_usb(AppMain 1)
pico_enable_stdio_uart(AppMain 1)
#PICO_ENABLE_STDIO_SEMIHOSTING(AppMain 1)


target_include_directories(AppMain PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(AppMain PUBLIC ${CFG_TUSB_OS_INC_PATH})
target_link_libraries(AppMain PUBLIC pico_stdlib FreeRTOS-Kernel FreeRTOS-Kernel-Heap4 tinyusb_device tinyusb_board )

    # create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(AppMain)