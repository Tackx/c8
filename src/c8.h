#pragma once

#include "consts/consts.h"
#include <stdbool.h>
#include <stdint.h>

typedef uint8_t C8_RAM[C8_RAM_SIZE];

// The framebuffer
typedef bool C8_DISPLAY[C8_HEIGHT_PIXELS][C8_WIDTH_PIXELS];
typedef uint16_t C8_PROGRAM_COUNTER;
typedef uint16_t C8_INSTRUCTION;

// Index register which points at locations in memory
typedef uint16_t C8_I_INDEX;

typedef uint16_t C8_STACK[16];
typedef uint8_t C8_DELAY_TIMER;
typedef uint8_t C8_SOUND_TIMER;
typedef uint8_t C8_VARIABLE_REGISTERS[16];

typedef enum
{
    V0,
    V1,
    V2,
    V3,
    V4,
    V5,
    V6,
    V7,
    V8,
    V9,
    VA,
    VB,
    VC,
    VD,
    VE,
    VF
} C8_VARIABLE_REGISTER;

// TODO: Stack pointer?

typedef uint8_t C8_FONT_SPRITE[5];

typedef struct C8
{
    C8_RAM ram;
    C8_DISPLAY display;
    C8_PROGRAM_COUNTER pc;
    C8_I_INDEX i_index;
    C8_STACK stack;
    C8_DELAY_TIMER delay_timer;
    C8_SOUND_TIMER sound_timer;
    C8_VARIABLE_REGISTERS v_regs;
} C8;

void c8_run(int argc, char *argv[]);
