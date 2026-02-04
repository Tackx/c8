#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "c8.h"
#include "raylib.h"

C8_FONT_SPRITE font_sprites[16] = {
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

void draw_font_sprites(void)
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
void draw_sprite(C8 *c8, uint8_t x, uint8_t y, const uint8_t *sprite, uint8_t height)
{

	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			bool bit = (*(sprite + i) >> (7 - j)) & 1;
			// TODO: Can detect collisions here
			c8->display[y + i][x + j] ^= bit;
		}
	}
}

void draw_screen(C8 *c8)
{
	for (int y = 0; y < C8_HEIGHT_PIXELS; ++y)
	{
		for (int x = 0; x < C8_WIDTH_PIXELS; ++x)
		{
			bool bit = c8->display[y][x];

			// Naive solution for now
			if (bit)
			{
				DrawRectangle(x * C8_RESOLUTION_MULTIPLIER, y * C8_RESOLUTION_MULTIPLIER, 1 * C8_RESOLUTION_MULTIPLIER, 1 * C8_RESOLUTION_MULTIPLIER, GREEN);
			}
			else
			{
				DrawRectangle(x * C8_RESOLUTION_MULTIPLIER, y * C8_RESOLUTION_MULTIPLIER, 1 * C8_RESOLUTION_MULTIPLIER, 1 * C8_RESOLUTION_MULTIPLIER, BLACK);
			}
		}
	}
}

void c8_init(void)
{
	C8 *c8 = malloc(sizeof(C8));
	*c8 = (C8){0};

	c8->ram[C8_PROGRAM_START_LOCATION] = 250;
	printf("first byte of the C8 RAM: %d\n", c8->ram[C8_PROGRAM_START_LOCATION]);

	const int screenWidth = C8_WIDTH_PIXELS * C8_RESOLUTION_MULTIPLIER;
	const int screenHeight = C8_HEIGHT_PIXELS * C8_RESOLUTION_MULTIPLIER;

	InitWindow(screenWidth, screenHeight, "C8");
	SetTargetFPS(60);

	Image img = LoadImage("resources/wabbit_alpha.png");
	SetWindowIcon(img);

	uint8_t vxn[3][5] = {
		{0xC3, 0xC3, 0xC3, 0x66, 0x18},
		{0xC3, 0x66, 0x18, 0x66, 0xC3},
		{0xC3, 0xE3, 0xF3, 0xDB, 0xC7},
	};

	for (int i = 0; i < 3; ++i)
	{
		draw_sprite(c8, i * 10, 0, vxn[i], 5);
	}

	// C8
	// draw_sprite(c8, 0, 0, font_sprites[12], 5);
	// draw_sprite(c8, 5, 0, font_sprites[8], 5);

	BeginDrawing();
	ClearBackground(BLACK);
	EndDrawing();

	while (!WindowShouldClose())
	{
		BeginDrawing();
		draw_screen(c8);
		// draw_font_sprites();
		EndDrawing();
	}

	CloseWindow();
}
