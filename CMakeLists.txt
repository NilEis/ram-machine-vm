cmake_minimum_required(VERSION 3.9.0)
project(ram-machine-vm VERSION 0.1.0 LANGUAGES C)

add_subdirectory(extern/terminal)
add_subdirectory(extern/c-logger)
add_subdirectory(src/parser)

add_executable(ram-machine-vm ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c)

configure_file(${CMAKE_SOURCE_DIR}/include/print_functions.h ${CMAKE_SOURCE_DIR}/src/parser/bin/c/print_functions.h COPYONLY)

set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/include/print_functions.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)

target_link_libraries(ram-machine-vm c_terminal c_logger_static cmd_parser)