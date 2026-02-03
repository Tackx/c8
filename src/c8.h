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

// TODO: Change to typedef, move the implementation to the `.c` file
uint8_t font[16][5] = {
	{0xF0, 0x90, 0x90, 0x90, 0xF0}, // 0
	{0x20, 0x60, 0x20, 0x20, 0x70}, // 1
	{0xF0, 0x10, 0xF0, 0x80, 0xF0}, // 2
	{0xF0, 0x10, 0xF0, 0x10, 0xF0}, // 3
	{0x90, 0x90, 0xF0, 0x10, 0x10}, // 4
	{0xF0, 0x80, 0xF0, 0x10, 0xF0}, // 5
	{0xF0, 0x80, 0xF0, 0x90, 0xF0}, // 6
	{0xF0, 0x10, 0x20, 0x40, 0x40}, // 7
	{0xF0, 0x90, 0xF0, 0x90, 0xF0}, // 8
	{0xF0, 0x90, 0xF0, 0x10, 0xF0}, // 9
	{0xF0, 0x90, 0xF0, 0x90, 0x90}, // A
	{0xE0, 0x90, 0xE0, 0x90, 0xE0}, // B
	{0xF0, 0x80, 0x80, 0x80, 0xF0}, // C
	{0xE0, 0x90, 0x90, 0x90, 0xE0}, // D
	{0xF0, 0x80, 0xF0, 0x80, 0xF0}, // E
	{0xF0, 0x80, 0xF0, 0x80, 0x80}	// F
};

void c8_init(void);

#endif
