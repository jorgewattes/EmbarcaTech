cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(pico_sdk_import.cmake)

project(data_collect C)

pico_sdk_init()

add_executable(data_collect data_collect.c)

target_link_libraries(data_collect pico_stdlib hardware_adc hardware_dma)

target_include_directories(data_collect PRIVATE ${CMAKE_CURRENT_LIST_DIR})

pico_add_extra_outputs(data_collect)