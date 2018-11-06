# Setting variables

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(XTENSA_TOOLS_ROOT "../crosstool-NG/builds/xtensa-lx106-elf/bin/")
set(CMAKE_C_COMPILER "${XTENSA_TOOLS_ROOT}/xtensa-lx106-elf-gcc")
set(CMAKE_C_LINKER "${XTENSA_TOOLS_ROOT}/xtensa-lx106-elf-gcc")
set(CMAKE_C_LIBRARIAN "${XTENSA_TOOLS_ROOT}/xtensa-lx106-elf-ar")

###
###
###

cmake_minimum_required(VERSION 3.10)
project(bars_esp_mqtt)

include_directories("../ESP8266_NONOS_SDK-2.1.0-18-g61248df/include")
include_directories("modules/include" "mqtt/include" "driver/" "include/" "include/driver")

add_executable(bars_esp_mqtt user/user_main.c)