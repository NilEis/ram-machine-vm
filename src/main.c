#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "c_logger.h"
#include "definitions.h"
#include "terminal.h"
#include "print_functions.h"
#include "bison.tab.h"

#define NUM_REGISTERS 4096

op_code_struct_t *mem = NULL;
uint16_t registers[NUM_REGISTERS] = {0, 4, 5};
uint32_t pc = 0;
uint64_t op_num = 0;

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
    yyparse();
    free(tmp);
    free(mem);
    return 0;
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
    uint16_t opcode = mem[pc].op;
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

void print_prog(uint32_t start, uint32_t stop)
{
    for (int i = 0; i < op_num; i++)
    {
        print_pc_line(i);
    }
}

uint32_t get_prog_size()
{
    return op_num;
}

void print_register(uint16_t reg)
{
    if (reg > NUM_REGISTERS)
    {
        c_logger_log(C_LOGGER_ERROR, "Register out of bounds\n");
        return;
    }
    printf("c(%" PRIu16 ") = %" PRIu16 ";\n", reg, registers[reg]);
}

void print_pc_line(uint32_t line)
{
    if (line > op_num)
    {
        c_logger_log(C_LOGGER_ERROR, "Line out of bounds\n");
        return;
    }
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

void print_pc()
{
    c_logger_log(C_LOGGER_INFO, "PC = %" PRIu32 "\n", pc);
}