#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "c_logger.h"
#include "definitions.h"
#include "terminal.h"
#include "print_functions.h"
#include "bison.tab.h"

#define xstr(s) str(s)
#define str(s) #s
#define foo 4

#define NUM_REGISTERS 4096

op_code_struct_t *mem = NULL;
uintmax_t registers[NUM_REGISTERS] = {0, 4, 5};
uint32_t pc = 0;
uint64_t op_num = 0;
uint64_t number_steps = 0;

int main(int argc, char **argv)
{
    uint16_t *tmp;
    // c_logger_set_level(C_LOGGER_VERBOSE | C_LOGGER_DEBUG);
    if (argc < 2)
    {
        c_logger_log(C_LOGGER_ERROR, "No input file\n");
        return 1;
    }
    FILE *in = fopen(argv[1], "rb");
    if (in == NULL)
    {
        c_logger_log(C_LOGGER_ERROR, "Could not open input\n");
        return 1;
    }
    uint32_t len = 0;
    fread(&len, 4, 1, in);
    if (len <= 1)
    {
        c_logger_log(C_LOGGER_ERROR, "Invalid format %ld\n", len);
        printf("Pos: %d\n", ftell(in));
        fclose(in);
        return 1;
    }
    c_logger_log(C_LOGGER_INFO, "Program size: %" PRIu64 "\n", len);
    tmp = (uint16_t *)malloc(sizeof(uint16_t) * len);
    if (tmp == NULL)
    {
        c_logger_log(C_LOGGER_ERROR, "Could not allocate memory\n");
        fclose(in);
        return -1;
    }
    fread(tmp, sizeof(uint16_t), len, in);
    fclose(in);
    {
        int i = 0;
        while (i < len)
        {
            switch (tmp[i])
            {
            case DEF_IF_LEQ_GOTO:
            case DEF_IF_LE_GOTO:
            case DEF_IF_EQ_GOTO:
            case DEF_IF_GE_GOTO:
            case DEF_IF_GEQ_GOTO:
                i += 4;
                break;
            default:
                i += 2;
                break;
            }
            op_num++;
        }
        mem = (op_code_struct_t *)calloc(op_num, sizeof(op_code_struct_t));
        int j = 0;
        for (i = 0; i < op_num; i++)
        {
            mem[i].breakpoint = 0;
            switch (tmp[j])
            {
            case DEF_IF_LEQ_GOTO:
            case DEF_IF_LE_GOTO:
            case DEF_IF_EQ_GOTO:
            case DEF_IF_GE_GOTO:
            case DEF_IF_GEQ_GOTO:
                mem[i].op = tmp[j];
                mem[i].value.if_v.value = tmp[j + 1];
                mem[i].value.if_v.jmp = tmp[j + 3];
                j += 4;
                break;
            default:
                mem[i].op = tmp[j];
                mem[i].value.int_v = tmp[j + 1];
                j += 2;
                break;
            }
        }
        for (i = 0; i < op_num; i++)
        {
            print_pc_line(i);
        }
    }
    int ret;
    do
    {
        ret = yyparse();
    } while (ret != 0);
    free(tmp);
    free(mem);
    return 0;
}

void add_break_point(uint32_t l)
{
    if (l >= op_num)
    {
        c_logger_log(C_LOGGER_ERROR, "Line number out of bounds\n");
        return;
    }
    if (mem[l].breakpoint)
    {
        mem[l].breakpoint = !mem[l].breakpoint;
        c_logger_log(C_LOGGER_CONFIRMATION, "Removed breakpoint in line %" PRIu32 "\n", l);
    }
    else
    {
        mem[l].breakpoint = !mem[l].breakpoint;
        c_logger_log(C_LOGGER_CONFIRMATION, "Added breakpoint in line %" PRIu32 "\n", l);
    }
}

int vm_step()
{
    static uint8_t running = 1;
    if (!running)
    {
        return 1;
    }
    if (pc >= op_num)
    {
        c_logger_log(C_LOGGER_ERROR, "Invalid program counter\n");
        return -1;
    }
    if (mem[pc].breakpoint)
    {
        c_logger_log(C_LOGGER_CONFIRMATION, "Reached breakpoint in line %" PRIu32 "\n", pc);
        return 3;
    }
    uint16_t opcode = mem[pc].op;
    number_steps++;
    switch (opcode)
    {
    case DEF_LOAD:
        registers[0] = registers[mem[pc].value.int_v];
        break;
    case DEF_STORE:
        registers[mem[pc].value.int_v] = registers[0];
        break;
    case DEF_ADD:
        registers[0] += registers[mem[pc].value.int_v];
        break;
    case DEF_SUB:
        registers[0] -= registers[mem[pc].value.int_v];
        break;
    case DEF_MULT:
        registers[0] *= registers[mem[pc].value.int_v];
        break;
    case DEF_DIV:
        if (registers[mem[pc].value.int_v] != 0)
        {
            registers[0] /= registers[mem[pc].value.int_v];
        }
        else
        {
            registers[0] = 0;
        }
        break;
    case DEF_INDLOAD:
        registers[0] = registers[registers[mem[pc].value.int_v]];
        break;
    case DEF_INDSTORE:
        registers[registers[mem[pc].value.int_v]] = registers[0];
        break;
    case DEF_INDADD:
        registers[0] += registers[registers[mem[pc].value.int_v]];
        break;
    case DEF_INDSUB:
        registers[0] -= registers[registers[mem[pc].value.int_v]];
        break;
    case DEF_INDMULT:
        registers[0] *= registers[registers[mem[pc].value.int_v]];
        break;
    case DEF_INDDIV:
        if (registers[registers[mem[pc].value.int_v]] != 0)
        {
            registers[0] /= registers[registers[mem[pc].value.int_v]];
        }
        else
        {
            registers[0] = 0;
        }
        break;
    case DEF_CLOAD:
        registers[0] = mem[pc].value.int_v;
        break;
    case DEF_CADD:
        registers[0] += mem[pc].value.int_v;
        break;
    case DEF_CSUB:
        registers[0] -= mem[pc].value.int_v;
        break;
    case DEF_CMULT:
        registers[0] *= mem[pc].value.int_v;
        break;
    case DEF_CDIV:
        if (mem[pc].value.int_v != 0)
        {
            registers[0] /= mem[pc].value.int_v;
        }
        else
        {
            registers[0] = 0;
        }
        break;
    case DEF_GOTO:
        pc = mem[pc].value.int_v;
        break;
    case DEF_IF_LEQ_GOTO:
        if (registers[0] <= mem[pc].value.if_v.value)
        {
            pc = mem[pc].value.if_v.jmp;
        }
        else
        {
            pc++;
        }
        break;
    case DEF_IF_LE_GOTO:
        if (registers[0] < mem[pc].value.if_v.value)
        {
            pc = mem[pc].value.if_v.jmp;
        }
        else
        {
            pc++;
        }
        break;
    case DEF_IF_EQ_GOTO:
        if (registers[0] == mem[pc].value.if_v.value)
        {
            pc = mem[pc].value.if_v.jmp;
        }
        else
        {
            pc++;
        }
        break;
    case DEF_IF_GE_GOTO:
        if (registers[0] < mem[pc].value.if_v.value)
        {
            pc = mem[pc].value.if_v.jmp;
        }
        else
        {
            pc++;
        }
        break;
    case DEF_IF_GEQ_GOTO:
        if (registers[0] >= mem[pc].value.if_v.value)
        {
            pc = mem[pc].value.if_v.jmp;
        }
        else
        {
            pc++;
        }
        break;
    case DEF_END:
        c_logger_log(C_LOGGER_INFO, "Program terminated\n");
        running = 0;
        return 1;
        break;
    default:
        c_logger_log(C_LOGGER_ERROR, "Unknown opcode: %" PRIX16 " and value %" PRIX16 "\n", opcode, mem[pc].value.int_v);
    }
    switch (opcode)
    {
    case DEF_GOTO:
    case DEF_IF_LEQ_GOTO:
    case DEF_IF_LE_GOTO:
    case DEF_IF_EQ_GOTO:
    case DEF_IF_GE_GOTO:
    case DEF_IF_GEQ_GOTO:
    case DEF_END:
        break;
    default:
        pc++;
        break;
    }
    return 0;
}

void print_help()
{
    c_logger_log(C_LOGGER_INFO, "Help:\n");
    c_logger_log(C_LOGGER_INFO, "- print\n");
    c_logger_log(C_LOGGER_INFO, "    - print [register|reg] [0-" xstr(NUM_REGISTERS) "]             -    prints register x\n");
    c_logger_log(C_LOGGER_INFO, "    - print [register|reg] [0-" xstr(NUM_REGISTERS) "] [0-" xstr(NUM_REGISTERS) "]    -    prints registers in range\n");
    c_logger_log(C_LOGGER_INFO, "    - print line                                -    prints current line\n");
    c_logger_log(C_LOGGER_INFO, "    - print line [0-%4" PRIu32 "]                       -    prints line\n", op_num);
    c_logger_log(C_LOGGER_INFO, "    - print [program|prog] [0-%4" PRIu32 "] [0-%4" PRIu32 "]    -    prints lines in range\n", op_num, op_num);
    c_logger_log(C_LOGGER_INFO, "    - print [program|prog]                      -    prints the program\n");
    c_logger_log(C_LOGGER_INFO, "    - print pc                                  -    prints program counter (line number)\n");
    c_logger_log(C_LOGGER_INFO, "- set\n");
    c_logger_log(C_LOGGER_INFO, "    - set [register|reg] [0-" xstr(NUM_REGISTERS) "]=[0-" xstr(UINTMAX_MAX) "]     -    sets register to value\n");
    c_logger_log(C_LOGGER_INFO, "- step\n");
    c_logger_log(C_LOGGER_INFO, "    - step                                      -    runs one iteration of the machine\n");
    c_logger_log(C_LOGGER_INFO, "- run\n");
    c_logger_log(C_LOGGER_INFO, "    - run                                       -    runs until termination\n");
    c_logger_log(C_LOGGER_INFO, "    - run [0-" xstr(INT_MAX) "]                        -    runs specified number of steps\n");
    c_logger_log(C_LOGGER_INFO, "- exit\n");
    c_logger_log(C_LOGGER_INFO, "    - exit                                      -    exits the program\n");
    c_logger_log(C_LOGGER_INFO, "- clear\n");
    c_logger_log(C_LOGGER_INFO, "    - [clear|cls]                               -    Clears the screen\n");
    c_logger_log(C_LOGGER_INFO, "- break\n");
    c_logger_log(C_LOGGER_INFO, "    - [break|brk] [0-%4" PRIu32 "]                      -    Switches a breakpoint on/off\n", op_num);
}

void print_num_steps()
{
    c_logger_log(C_LOGGER_INFO, "Number of steps: %" PRIu64 "\n", number_steps);
}

void print_prog(uint32_t start, uint32_t stop)
{
    if (stop >= op_num)
    {
        stop = op_num - 1;
    }
    if (start >= stop)
    {
        return;
    }
    for (int i = start; i <= stop; i++)
    {
        print_pc_line(i);
    }
}

uint32_t get_prog_size()
{
    return op_num;
}

void print_register(uint16_t reg, uint16_t reg2)
{
    if (reg > NUM_REGISTERS || reg2 > NUM_REGISTERS || reg > reg2)
    {
        c_logger_log(C_LOGGER_ERROR, "Register out of bounds\n");
        return;
    }
    for (int i = reg; i <= reg2; i++)
    {
        printf("c(%" PRIu16 ") = %" PRIuMAX ";\n", i, registers[i]);
    }
}

void set_register(uint16_t reg, uintmax_t v)
{
    if (reg >= NUM_REGISTERS)
    {
        c_logger_log(C_LOGGER_ERROR, "Register index out of bounds\n");
        return;
    }
    printf("c(%" PRIu16 ") = %" PRIuMAX " -> ", reg, registers[reg]);
    registers[reg] = v;
    printf("c(%" PRIu16 ") = %" PRIuMAX ";\n", reg, registers[reg]);
}

void print_pc_line(uint32_t line)
{
    if (line > op_num)
    {
        c_logger_log(C_LOGGER_ERROR, "Line out of bounds\n");
        return;
    }
    c_logger_log(C_LOGGER_INFO, "%" PRIu32 ": ", line);
    switch (mem[line].op)
    {
    case DEF_LOAD:
        c_logger_log(C_LOGGER_INFO, "LOAD %" PRIu16 "\n", mem[line].value.int_v);
        break;
    case DEF_STORE:
        c_logger_log(C_LOGGER_INFO, "STORE %" PRIu16 "\n", mem[line].value.int_v);
        break;
    case DEF_ADD:
        c_logger_log(C_LOGGER_INFO, "ADD %" PRIu16 "\n", mem[line].value.int_v);
        break;
    case DEF_SUB:
        c_logger_log(C_LOGGER_INFO, "SUB %" PRIu16 "\n", mem[line].value.int_v);
        break;
    case DEF_MULT:
        c_logger_log(C_LOGGER_INFO, "MULT %" PRIu16 "\n", mem[line].value.int_v);
        break;
    case DEF_DIV:
        c_logger_log(C_LOGGER_INFO, "DIV %" PRIu16 "\n", mem[line].value.int_v);
        break;
    case DEF_INDLOAD:
        c_logger_log(C_LOGGER_INFO, "INDLOAD %" PRIu16 "\n", mem[line].value.int_v);
        break;
    case DEF_INDSTORE:
        c_logger_log(C_LOGGER_INFO, "INDSTORE %" PRIu16 "\n", mem[line].value.int_v);
        break;
    case DEF_INDADD:
        c_logger_log(C_LOGGER_INFO, "INDADD %" PRIu16 "\n", mem[line].value.int_v);
        break;
    case DEF_INDSUB:
        c_logger_log(C_LOGGER_INFO, "INDSUB %" PRIu16 "\n", mem[line].value.int_v);
        break;
    case DEF_INDMULT:
        c_logger_log(C_LOGGER_INFO, "INDMULT %" PRIu16 "\n", mem[line].value.int_v);
        break;
    case DEF_INDDIV:
        c_logger_log(C_LOGGER_INFO, "INDDIV %" PRIu16 "\n", mem[line].value.int_v);
        break;
    case DEF_CLOAD:
        c_logger_log(C_LOGGER_INFO, "CLOAD %" PRIu16 "\n", mem[line].value.int_v);
        break;
    case DEF_CADD:
        c_logger_log(C_LOGGER_INFO, "CADD %" PRIu16 "\n", mem[line].value.int_v);
        break;
    case DEF_CSUB:
        c_logger_log(C_LOGGER_INFO, "CSUB %" PRIu16 "\n", mem[line].value.int_v);
        break;
    case DEF_CMULT:
        c_logger_log(C_LOGGER_INFO, "CMULT %" PRIu16 "\n", mem[line].value.int_v);
        break;
    case DEF_CDIV:
        c_logger_log(C_LOGGER_INFO, "CDIV %" PRIu16 "\n", mem[line].value.int_v);
        break;
    case DEF_GOTO:
        c_logger_log(C_LOGGER_INFO, "GOTO %" PRIu16 "\n", mem[line].value.int_v);
        break;
    case DEF_IF_LEQ_GOTO:
        c_logger_log(C_LOGGER_INFO, "IF c(0) <= %" PRIu16 " THEN GOTO %" PRIu16 "\n", mem[line].value.if_v.value, mem[line].value.if_v.jmp);
        break;
    case DEF_IF_LE_GOTO:
        c_logger_log(C_LOGGER_INFO, "IF c(0) < %" PRIu16 " THEN GOTO %" PRIu16 "\n", mem[line].value.if_v.value, mem[line].value.if_v.jmp);
        break;
    case DEF_IF_EQ_GOTO:
        c_logger_log(C_LOGGER_INFO, "IF c(0) = %" PRIu16 " THEN GOTO %" PRIu16 "\n", mem[line].value.if_v.value, mem[line].value.if_v.jmp);
        break;
    case DEF_IF_GE_GOTO:
        c_logger_log(C_LOGGER_INFO, "IF c(0) > %" PRIu16 " THEN GOTO %" PRIu16 "\n", mem[line].value.if_v.value, mem[line].value.if_v.jmp);
        break;
    case DEF_IF_GEQ_GOTO:
        c_logger_log(C_LOGGER_INFO, "IF c(0) >= %" PRIu16 " THEN GOTO %" PRIu16 "\n", mem[line].value.if_v.value, mem[line].value.if_v.jmp);
        break;
    case DEF_END:
        c_logger_log(C_LOGGER_INFO, "END: %" PRIu16 "\n", mem[line].value.int_v);
        break;
    default:
        c_logger_log(C_LOGGER_ERROR, "Unknown opcode: %" PRIX16 " and value %" PRIX16 "\n", mem[line].op, mem[line].value.int_v);
    }
}

void print_current_line()
{
    print_pc_line(pc);
}

void vm_clear()
{
    terminal_clear();
}

void print_pc()
{
    c_logger_log(C_LOGGER_INFO, "PC = %" PRIu32 "\n", pc);
}