#ifndef DEFINITIONS_H
#define DEFINITIONS_H

typedef enum
{
    DEF_LOAD = 0x0A,
    DEF_STORE = 0x0B,
    DEF_ADD = 0x0C,
    DEF_SUB = 0x0D,
    DEF_MULT = 0x0E,
    DEF_DIV = 0x0F,

    DEF_INDLOAD = 0x1A,
    DEF_INDSTORE = 0x1B,
    DEF_INDADD = 0x1C,
    DEF_INDSUB = 0x1D,
    DEF_INDMULT = 0x1E,
    DEF_INDDIV = 0x1F,

    DEF_CLOAD = 0x2A,
    DEF_CADD = 0x2C,
    DEF_CSUB = 0x2D,
    DEF_CMULT = 0x2E,
    DEF_CDIV = 0x2F,

    DEF_GOTO = 0xAA,
    DEF_IF_LEQ_GOTO = 0xB1,
    DEF_IF_LE_GOTO = 0xB2,
    DEF_IF_EQ_GOTO = 0xB3,
    DEF_IF_GE_GOTO = 0xB4,
    DEF_IF_GEQ_GOTO = 0xB5,
    DEF_END = 0xFF
} op_code_t;

typedef struct
{
    uint16_t value;
    uint16_t jmp;
} if_struct_t;

typedef struct
{
    op_code_t op;
    uint8_t breakpoint;
    union
    {
        uint16_t int_v;
        if_struct_t if_v;
    } value;
    
} op_code_struct_t;

#endif // DEFINITIONS_H
