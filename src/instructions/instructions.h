#pragma once

#include "c8.h"

#define C8_INSTRUCTION_CLEAR_SCREEN 0x00
#define C8_INSTRUCTION_JUMP 0x01
#define C8_INSTRUCTION_VX_SET 0x06
#define C8_INSTRUCTION_VX_ADD 0x07
#define C8_INSTRUCTION_I_SET 0x0A
#define C8_INSTRUCTION_DRAW 0x0D

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
    // TODO: Could be an enum?
    uint8_t type;
    C8_INSTRUCTION_PARAMETERS params;
} C8_INSTRUCTION_DATA;

C8_INSTRUCTION fetch_instruction(C8 *c8);

C8_INSTRUCTION_DATA decode_instruction(C8_INSTRUCTION instruction);

void execute_instruction(C8 *c8, C8_INSTRUCTION_DATA data);
