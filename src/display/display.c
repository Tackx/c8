#include "raylib.h"

#include "c8.h"
#include "display.h"

void init_display(void)
{
    InitWindow(C8_ACTUAL_WIDTH, C8_ACTUAL_HEIGHT, "C8");
    SetTargetFPS(60);

    Image icon = LoadImage("resources/wabbit_alpha.png");
    SetWindowIcon(icon);
    UnloadImage(icon);
}

void draw_screen(C8 *c8)
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