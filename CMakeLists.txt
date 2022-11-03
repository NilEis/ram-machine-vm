cmake_minimum_required(VERSION 3.9.0)
project(ram-machine-vm VERSION 0.1.0 LANGUAGES C)

add_subdirectory(extern/terminal)
add_subdirectory(extern/c-logger)
add_subdirectory(src/parser)

find_package(BISON REQUIRED)
BISON_TARGET(parser src/parser/src/bison.y src/parser/bin/bison.c COMPILE_FLAGS "-d -t")
find_package(FLEX)
FLEX_TARGET(scanner src/parser/src/flex.l src/parser/bin/flex.c)
ADD_FLEX_BISON_DEPENDENCY(scanner parser)

add_executable(ram-machine-vm ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)

target_link_libraries(ram-machine-vm c_terminal c_logger_static cmd_parser)