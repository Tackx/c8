#pragma once

#include "c8.h"

// High nibble
typedef enum C8_OP_HN
{
    C8_OP_HN_0 = 0x0,
    C8_OP_HN_1 = 0x1,
    C8_OP_HN_2 = 0x2,
    C8_OP_HN_3 = 0x3,
    C8_OP_HN_4 = 0x4,
    C8_OP_HN_5 = 0x5,
    C8_OP_HN_6 = 0x6,
    C8_OP_HN_7 = 0x7,
    C8_OP_HN_8 = 0x8,
    C8_OP_HN_9 = 0x9,
    C8_OP_HN_A = 0xA,
    C8_OP_HN_D = 0xD,
    C8_OP_HN_F = 0xF
} C8_OP_HN;

// Low byte of high nibble 0
typedef enum C8_OP_HN_0_LB
{
    C8_OP_HN_0_LB_E0 = 0xE0,
    C8_OP_HN_0_LB_EE = 0xEE
} C8_OP_HN_0_LB;

// Low nibble of high nibble 5
typedef enum C8_OP_HN_5_LN
{
    C8_OP_HN_5_LN_0 = 0x0
} C8_OP_HN_5_LN;

// Low nibble of high nibble 8
typedef enum C8_OP_HN_8_LN
{
    C8_OP_HN_8_LN_0,
    C8_OP_HN_8_LN_1,
    C8_OP_HN_8_LN_2,
    C8_OP_HN_8_LN_3,
    C8_OP_HN_8_LN_4,
    C8_OP_HN_8_LN_5,
    C8_OP_HN_8_LN_6,
    C8_OP_HN_8_LN_7,
    C8_OP_HN_8_LN_E = 0xE,

} C8_OP_HN_8_LN;

// Low nibble of high nibble 9
typedef enum C8_OP_HN_9_LN
{
    C8_OP_HN_9_LN_0 = 0x0
} C8_OP_HN_9_LN;

// Low byte of high nibble F
typedef enum C8_OP_HN_F_LB
{
    C8_OP_HN_F_LB_55 = 0x55,
    C8_OP_HN_F_LB_65 = 0x65,
    C8_OP_HN_F_LB_33 = 0x33,
    C8_OP_HN_F_LB_29 = 0x29
} C8_OP_HN_F_LB;

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
    C8_I_SKIP_IF_VX,
    C8_I_SKIP_IF_NOT_VX,
    C8_I_SKIP_IF_VX_VY,
    C8_I_SKIP_IF_NOT_VX_VY,
    C8_I_SET,
    C8_I_B_OR,
    C8_I_B_AND,
    C8_I_XOR,
    C8_I_ADD,
    C8_I_SUBTRACT_Y_X,
    C8_I_SUBTRACT_X_Y,
    C8_I_SHIFT_R,
    C8_I_SHIFT_L,
    C8_I_LOAD_REGS,
    C8_I_SAVE_REGS,
    C8_I_BCDC,
    C8_I_FONT_CHAR
} C8_I;

typedef uint16_t C8_INSTRUCTION;

typedef struct C8_INSTRUCTION_PARAMETERS
{
    C8_VX vx;
    C8_VX vy;
    uint8_t n;
    uint8_t nn;
    uint16_t nnn;
} C8_INSTRUCTION_PARAMETERS;

typedef struct C8_INSTRUCTION_DATA
{
    C8_I type;
    C8_INSTRUCTION_PARAMETERS params;
} C8_INSTRUCTION_DATA;

C8_INSTRUCTION fetch_instruction(C8 *c8);

C8_INSTRUCTION_DATA decode_instruction(C8_INSTRUCTION instruction);

void execute_instruction(C8 *c8, C8_INSTRUCTION_DATA data);
