#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#include "raylib.h"

// Display
#define C8_HEIGHT_PIXELS 32
#define C8_WIDTH_PIXELS 64
#define C8_RESOLUTION_MULTIPLIER 10
#define C8_ACTUAL_WIDTH (C8_WIDTH_PIXELS * C8_RESOLUTION_MULTIPLIER)
#define C8_ACTUAL_HEIGHT (C8_HEIGHT_PIXELS * C8_RESOLUTION_MULTIPLIER)

// Memory offsets
#define C8_FONT_START_LOCATION 0x050
#define C8_PROGRAM_START_LOCATION 0x200

typedef uint8_t C8_RAM[4096];

// The framebuffer
typedef bool C8_DISPLAY[C8_HEIGHT_PIXELS][C8_WIDTH_PIXELS];

typedef uint16_t C8_PROGRAM_COUNTER;
typedef uint16_t C8_INSTRUCTION;

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

static C8_FONT_SPRITE font_sprites[16] = {
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

static void draw_font_sprites(void)
{
	for (int letter_index = 0; letter_index < sizeof(font_sprites) / sizeof(font_sprites[0]); letter_index++)
	{
		for (int y = 0; y < sizeof(font_sprites[letter_index]); y++)
		{
			for (int x = 0; x < 8; x++)
			{
				bool bit = (font_sprites[letter_index][y] >> (7 - x)) & 1;
				if (bit)
				{

					int posX = x * C8_RESOLUTION_MULTIPLIER + letter_index * 50;
					int overflow = floor((posX / (C8_WIDTH_PIXELS * C8_RESOLUTION_MULTIPLIER)));

					if (overflow > 0)
					{
						posX -= overflow * C8_WIDTH_PIXELS * C8_RESOLUTION_MULTIPLIER;
					}

					int posY = (y + overflow) * C8_RESOLUTION_MULTIPLIER + overflow * 50;

					DrawRectangle(posX, posY, 1 * C8_RESOLUTION_MULTIPLIER, 1 * C8_RESOLUTION_MULTIPLIER, GREEN);
				}
			}
		}
	}
}

// Height is needed so that we know how many bytes to read from the start pointed to by sprite
static void write_sprite(C8 *c8, uint8_t x, uint8_t y, const uint8_t *sprite, uint8_t height)
{
	for (int i = 0; i < height; ++i)
	{
		// TODO: Fix magic number
		for (int j = 0; j < 8; ++j)
		{
			bool bit = (*(sprite + i) >> (7 - j)) & 1;
			// TODO: Can detect collisions here
			c8->display[y + i][x + j] ^= bit;
		}
	}
}

static void draw_screen(C8 *c8)
{
	for (int y = 0; y < C8_HEIGHT_PIXELS; ++y)
	{
		for (int x = 0; x < C8_WIDTH_PIXELS; ++x)
		{
			bool bit = c8->display[y][x];

			// Naive solution for now
			Color c;
			if (bit)
			{
				c = GREEN;
			}
			else
			{
				c = BLACK;
			}
			DrawRectangle(x * C8_RESOLUTION_MULTIPLIER, y * C8_RESOLUTION_MULTIPLIER, 1 * C8_RESOLUTION_MULTIPLIER, 1 * C8_RESOLUTION_MULTIPLIER, c);
		}
	}
}

static C8_INSTRUCTION fetch_instruction(C8 *c8)
{
	C8_PROGRAM_COUNTER counter_value = c8->pc;
	uint8_t first_byte = c8->ram[counter_value];
	uint8_t second_byte = c8->ram[counter_value + 1];

	// Example:
	// 00000001
	// 00000010
	// 00000001 00000010

	C8_INSTRUCTION instruction = (first_byte << 8) | second_byte;

	// Increment the Program Counter to be ready to fetch the next instruction
	c8->pc += 2;

	return instruction;
}

static void initialize_resources(void)
{
	Image icon = LoadImage("resources/wabbit_alpha.png");
	SetWindowIcon(icon);
	UnloadImage(icon);
}

void c8_init(void)
{
	C8 c8 = {0};
	c8.ram[0] = 1;
	c8.ram[1] = 2;

	InitWindow(C8_ACTUAL_WIDTH, C8_ACTUAL_HEIGHT, "C8");
	SetTargetFPS(60);

	initialize_resources();

	uint8_t vxn[3][5] = {
		{0xC3, 0xC3, 0xC3, 0x66, 0x18},
		{0xC3, 0x66, 0x18, 0x66, 0xC3},
		{0xC3, 0xE3, 0xF3, 0xDB, 0xC7},
	};

	for (int i = 0; i < 3; ++i)
	{
		write_sprite(&c8, i * 10, 0, vxn[i], 5);
	}

	// C8
	// write_sprite(c8, 0, 0, font_sprites[12], 5);
	// write_sprite(c8, 5, 0, font_sprites[8], 5);

	BeginDrawing();
	ClearBackground(BLACK);
	EndDrawing();

	printf("Instruction: %d", fetch_instruction(&c8));

	while (!WindowShouldClose())
	{
		BeginDrawing();
		// draw_screen(c8);
		draw_font_sprites();
		EndDrawing();
	}

	CloseWindow();
}
