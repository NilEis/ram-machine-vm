#ifndef PRINT_FUNCTIONS_H
#define PRINT_FUNCTIONS_H
#include <stdint.h>

void print_register(uint16_t reg, uint16_t reg2);
void print_pc_line(uint32_t pc);
void print_current_line();
void print_pc();
void print_prog(uint32_t start, uint32_t stop);
uint32_t get_prog_size();
void switch_exit_prog();
int vm_step();
void vm_clear();
void set_register(uint16_t reg, uint16_t v);
void add_break_point(uint32_t l);
void print_help();

#endif // PRINT_FUNCTIONS_H
