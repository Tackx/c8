#pragma once

#include "c8.h"

typedef enum C8_OP_HN
{
    C8_OP_HN_0 = 0x0,
    C8_OP_HN_1 = 0x1,
    C8_OP_HN_2 = 0x2,
    C8_OP_HN_6 = 0x6,
    C8_OP_HN_7 = 0x7,
    C8_OP_HN_A = 0xA,
    C8_OP_HN_D = 0xD
} C8_OP_HN;

typedef enum C8_OP_HN_0_LB
{
    C8_OP_HN_0_LB_E0 = 0xE0,
    C8_OP_HN_0_LB_EE = 0xEE
} C8_OP_HN_0_LB;

typedef enum C8_I
{
    C8_I_GARBAGE, // :)
    C8_I_CLEAR_SCREEN,
    C8_I_JUMP,
    C8_I_VX_SET,
    C8_I_VX_ADD,
    C8_I_SET_I,
    C8_I_DRAW,
    C8_I_SUB_CALL,
    C8_I_SUB_RET,
} C8_I;

typedef uint16_t C8_INSTRUCTION;

typedef struct C8_INSTRUCTION_PARAMETERS
{
    C8_PROGRAM_COUNTER pc;
    C8_VX vx;
    uint8_t vx_value;
    uint16_t i_value;
    uint8_t draw_height;
    uint8_t x_reg;
    uint8_t y_reg;
} C8_INSTRUCTION_PARAMETERS;

typedef struct C8_INSTRUCTION_DATA
{
    C8_I type;
    C8_INSTRUCTION_PARAMETERS params;
} C8_INSTRUCTION_DATA;

C8_INSTRUCTION fetch_instruction(C8 *c8);

C8_INSTRUCTION_DATA decode_instruction(C8_INSTRUCTION instruction);

void execute_instruction(C8 *c8, C8_INSTRUCTION_DATA data);
