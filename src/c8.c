#include "raylib.h"
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c8.h"
#include "display/display.h"
#include "instructions/instructions.h"
#include "logger/logger.h"

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
    {0xF0, 0x80, 0xF0, 0x80, 0x80}  // F
};

static int load_data(C8 *c8, char *path)
{
    FILE *file = fopen(path, "rb");

    if (file == NULL)
    {
        fprintf(stderr, "Could not read file with path %s. %s\n", path, strerror(errno));

        return 1;
    }

    size_t bytes_read = fread(&c8->ram[C8_PROGRAM_START_LOCATION], sizeof(unsigned char), C8_RAM_SIZE - C8_PROGRAM_START_LOCATION, file);
    fclose(file);

    if (bytes_read > 0)
    {
        // https://stackoverflow.com/questions/8060170/printing-hexadecimal-characters-in-c
        C8_LOG("Read: %02hhX\n", c8->ram[C8_PROGRAM_START_LOCATION]);
    }

    return 0;
}

// Height is needed so that we know how many bytes to read from the start
// pointed to by sprite
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

C8 init()
{
    C8 c8 = {.pc = C8_PROGRAM_START_LOCATION};

    return c8;
}

int c8_run(int argc, char *argv[])
{
    C8 c8 = init();

    if (argc < 2)
    {
        fprintf(stderr, "No input file specified");

        return 1;
    }

    int err = load_data(&c8, argv[1]);

    if (err != 0)
    {
        return 1;
    }

    init_display();

    while (!WindowShouldClose())
    {
        // Raylib runs at target 60 fps, so each run of this loop
        // we should process around 12 instructions to achieve ~700 instructions per second.
        for (int i = 0; i < 12; i++)
        {
            execute_instruction(&c8, decode_instruction(fetch_instruction(&c8)));

            // WaitTime(1);
        }

        BeginDrawing();
        draw_screen(&c8);
        EndDrawing();

        C8_LOG("Finished Raylib frame\n");

        // WaitTime(0.1);
    }

    CloseWindow();

    return 0;
}
