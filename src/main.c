// https://tobiasvl.github.io/blog/write-a-chip-8-emulator/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "c8.h"
#include "raylib.h"

int main(void)
{
    c8_init();

    // Heap allocated
    C8 *c8 = malloc(sizeof(C8));

    c8->ram[C8_PROGRAM_START_LOCATION] = 250;

    printf("first byte of the C8 RAM: %d\n", c8->ram[C8_PROGRAM_START_LOCATION]);

    const int screenWidth = C8_WIDTH_PIXELS * C8_RESOLUTION_MULTIPLIER;
    const int screenHeight = C8_HEIGHT_PIXELS * C8_RESOLUTION_MULTIPLIER;

    InitWindow(screenWidth, screenHeight, "C8");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        // DrawText("Hello from C8!", 20, 20, 20, WHITE);
        EndDrawing();

        for (int letter_index = 0; letter_index < 16; letter_index++)
        {
            for (int y = 0; y < 5; y++)
            {
                for (int x = 0; x < 8; x++)
                {
                    bool bit = (font[letter_index][y] >> (7 - x)) & 1;
                    if (bit)
                    {
                        DrawRectangle(x + letter_index * 6, y, 1, 1, GREEN);
                    }
                }
            }
        }
    }
    CloseWindow();

    return 0;
}
