#pragma once

#include "c8.h"

typedef enum C8_INSTRUCTION_TYPE
{
    CLEAR_SCREEN = 0x00,
    JUMP = 0x01,
    VX_SET = 0x06,
    VX_ADD = 0x07,
    I_SET = 0x0A,
    DRAW = 0x0D,
    SUB_CALL = 0x02,
    SUB_RET = 0xEE
} C8_INSTR;

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
    C8_INSTR type;
    C8_INSTRUCTION_PARAMETERS params;
} C8_INSTRUCTION_DATA;

C8_INSTRUCTION fetch_instruction(C8 *c8);

C8_INSTRUCTION_DATA decode_instruction(C8_INSTRUCTION instruction);

void execute_instruction(C8 *c8, C8_INSTRUCTION_DATA data);
