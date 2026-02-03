#ifndef C8_H
#define C8_H

#include <stdint.h>
#include <stdbool.h>

// Display
#define C8_HEIGHT_PIXELS 32
#define C8_WIDTH_PIXELS 64
#define C8_RESOLUTION_MULTIPLIER 10

// Memory offsets
#define C8_FONT_START_LOCATION 0x050
#define C8_PROGRAM_START_LOCATION 0x200

typedef uint8_t C8_RAM[4096];

// The framebuffer
typedef bool C8_DISPLAY[C8_HEIGHT_PIXELS][C8_WIDTH_PIXELS];

typedef uint16_t C8_PROGRAM_COUNTER;

typedef uint16_t C8_I_INDEX;

// TODO: What should the stack size be?
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

typedef uint8_t C8_FONT_SPRITE[5];

void c8_init(void);

#endif
