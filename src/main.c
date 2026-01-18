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

    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "raylib basic window");
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("It works!", 20, 20, 20, BLACK);
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
