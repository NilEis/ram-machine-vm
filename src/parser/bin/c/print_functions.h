#ifndef PRINT_FUNCTIONS_H
#define PRINT_FUNCTIONS_H
#include <stdint.h>

void print_register(uint16_t reg);
void print_pc_line(uint32_t pc);
void print_current_line();
void print_pc();
void switch_exit_prog();
void run_end();

#endif // PRINT_FUNCTIONS_H
