cmake_minimum_required(VERSION 3.9.0)
project(cmd_parser VERSION 0.1.0 LANGUAGES C)

add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/bin/c/bison.tab.c ${CMAKE_CURRENT_SOURCE_DIR}/bin/c/lex.yy.c DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/bison.y ${CMAKE_CURRENT_SOURCE_DIR}/src/flex.l COMMAND make WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/)

add_library(cmd_parser ${CMAKE_CURRENT_SOURCE_DIR}/bin/c/bison.tab.c ${CMAKE_CURRENT_SOURCE_DIR}/bin/c/lex.yy.c)

target_include_directories(cmd_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/bin/c)