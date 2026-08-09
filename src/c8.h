#pragma once

#include <stdbool.h>
#include <stdint.h>

// Display
#define C8_HEIGHT_PIXELS 32
#define C8_WIDTH_PIXELS 64
#define C8_RESOLUTION_MULTIPLIER 10
#define C8_ACTUAL_WIDTH (C8_WIDTH_PIXELS * C8_RESOLUTION_MULTIPLIER)
#define C8_ACTUAL_HEIGHT (C8_HEIGHT_PIXELS * C8_RESOLUTION_MULTIPLIER)

// Memory
#define C8_RAM_SIZE 4096
#define C8_FONT_START_LOCATION 0x050
#define C8_PROGRAM_START_LOCATION 0x200

typedef uint8_t C8_RAM[C8_RAM_SIZE];

// The framebuffer
typedef bool C8_DISPLAY[C8_HEIGHT_PIXELS][C8_WIDTH_PIXELS];
typedef uint16_t C8_PROGRAM_COUNTER;

// Index register which points at locations in memory
typedef uint16_t C8_I_INDEX;

typedef uint16_t C8_STACK[16];
typedef uint8_t C8_DELAY_TIMER;
typedef uint8_t C8_SOUND_TIMER;

typedef uint8_t C8_VX;

typedef C8_VX C8_VARIABLE_REGISTERS[16];

// TODO: Stack pointer?

typedef uint8_t C8_FONT_SPRITE[5];

typedef struct C8
{
    C8_RAM ram;
    C8_DISPLAY display;
    C8_PROGRAM_COUNTER pc;
    C8_I_INDEX i_index;
    C8_STACK stack;
    uint8_t sp;
    C8_DELAY_TIMER delay_timer;
    C8_SOUND_TIMER sound_timer;
    C8_VARIABLE_REGISTERS v_regs;
} C8;

int c8_run(int argc, char *argv[]);
