cmake_minimum_required(VERSION 3.9.0)
project(cmd_parser VERSION 0.1.0 LANGUAGES C)

add_custom_command(OUTPUT bin/bison.tab.c bin/lex.yy.c COMMAND make DEPENDS src/bison.y flex.l VERBATIM)

add_library(cmd_parser bin/c/bison.tab.c bin/c/lex.yy.c)

target_include_directories(cmd_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/bin/c)