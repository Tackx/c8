#include <stdlib.h>
#include <stdio.h>
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
					DrawRectangle(x + letter_index * 6, y, 1, 1, GREEN);
				}
			}
		}
	}
}

// TODO
// Height is needed so that we know how many bytes to read from the start pointed to by sprite
void draw_sprite(uint8_t x, uint8_t y, uint8_t *sprite, uint8_t height);

void c8_init(void)
{
	C8 *c8 = malloc(sizeof(C8));
	*c8 = (C8){0};

	c8->ram[C8_PROGRAM_START_LOCATION] = 250;
	printf("first byte of the C8 RAM: %d\n", c8->ram[C8_PROGRAM_START_LOCATION]);

	for (int i = 0; i < C8_HEIGHT_PIXELS; i++)
	{
		for (int j = 0; j < C8_WIDTH_PIXELS; j++)
		{
			printf("%x", c8->display[i][j]);
		}
		printf("\n");
	}

	const int screenWidth = C8_WIDTH_PIXELS * C8_RESOLUTION_MULTIPLIER;
	const int screenHeight = C8_HEIGHT_PIXELS * C8_RESOLUTION_MULTIPLIER;

	InitWindow(screenWidth, screenHeight, "C8");
	SetTargetFPS(60);

	Image img = LoadImage("resources/wabbit_alpha.png");
	SetWindowIcon(img);

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(BLACK);
		// DrawText("Hello from C8!", 20, 20, 20, WHITE);
		EndDrawing();

		// draw_font_sprites();
	}

	CloseWindow();
}
