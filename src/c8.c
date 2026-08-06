// #define C8_DEBUG

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

#include "c8.h"
#include "consts/consts.h"
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

static void clear_screen(C8 *c8)
{
    C8_LOG("Executing clear_screen\n");

    for (int i = 0; i < C8_HEIGHT_PIXELS; i++)
    {
        // for (int j = 0; j < C8_WIDTH_PIXELS; j++)
        // {
        //     c8->display[i][j] = 0;
        // }

        memset(c8->display[i], 0, sizeof c8->display[0]);
    }

    C8_LOG("Executed clear_screen\n");
}

static void jump(C8 *c8, C8_PROGRAM_COUNTER pc)
{
    c8->pc = pc;
    C8_LOG("Jumped to memory address %d\n", pc);
}

static void load_data(C8 *c8, char *path)
{
    FILE *file;
    errno_t err = fopen_s(&file, path, "rb");

    if (err != 0)
    {
        printf("Could not read file with path %s. Error number: %d\n", path, err);

        return;
    }

    size_t bytes_read = fread(&c8->ram[C8_PROGRAM_START_LOCATION], sizeof(unsigned char), 3000, file);
    fclose(file);

    if (bytes_read > 0)
    {

        C8_LOG("Read: %d\n", c8->ram[C8_PROGRAM_START_LOCATION]);
    }
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

// TODO
typedef struct C8_INSTRUCTION_PARAMETERS
{
    C8_PROGRAM_COUNTER pc;
} C8_INSTRUCTION_PARAMETERS;

typedef struct C8_INSTRUCTION_DATA
{
    uint8_t type;
    C8_INSTRUCTION_PARAMETERS params;
} C8_INSTRUCTION_DATA;

// TODO
static C8_INSTRUCTION_DATA decode_instruction(C8_INSTRUCTION instruction)
{
    // 00000000
    uint8_t decoded = (instruction >> 12) & 0xFF;
    C8_LOG("Decoded instruction type: %d\n", decoded);

    // TODO: Decode and insert params
    C8_INSTRUCTION_DATA d = {
        .type = decoded,
    };

    if (decoded == C8_INSTRUCTION_JUMP)
    {
        C8_PROGRAM_COUNTER pc = instruction & 0xFFF;

        d.params.pc = pc;
    }

    return d;
}

// TODO
static void execute_instruction(C8 *c8, C8_INSTRUCTION_DATA data)
{
    C8_LOG("Executing instruction\n");

    switch (data.type)
    {
    case C8_INSTRUCTION_CLEAR_SCREEN:
        C8_LOG("Recognized the clear screen instruction\n");

        clear_screen(c8);

        break;
    case C8_INSTRUCTION_JUMP:
        C8_LOG("Recognized the jump instruction\n");

        jump(c8, data.params.pc);

        break;
    case C8_INSTRUCTION_VX_ADD:
        C8_LOG("Recognized the VX ADD instruction\n");
        break;
    case C8_INSTRUCTION_VX_SET:
        C8_LOG("Recognized the VX SET instruction\n");
        break;
    case C8_INSTRUCTION_I_SET:
        C8_LOG("Recognized the I SET instruction\n");
        break;
    case C8_INSTRUCTION_DRAW:
        C8_LOG("Recognized the DRAW instruction\n");
        break;
    default:
        break;
    }
}

C8 init()
{
    C8 c8 = {.pc = C8_PROGRAM_START_LOCATION};

    InitWindow(C8_ACTUAL_WIDTH, C8_ACTUAL_HEIGHT, "C8");
    SetTargetFPS(60);

    Image icon = LoadImage("resources/wabbit_alpha.png");
    SetWindowIcon(icon);
    UnloadImage(icon);

    return c8;
}

void c8_run(int argc, char *argv[])
{
    C8 c8 = init();

    uint8_t vxn[3][5] = {
        {0xC3, 0xC3, 0xC3, 0x66, 0x18},
        {0xC3, 0x66, 0x18, 0x66, 0xC3},
        {0xC3, 0xE3, 0xF3, 0xDB, 0xC7},
    };

    for (int i = 0; i < 3; ++i)
    {
        write_sprite(&c8, i * 10, 0, vxn[i], 5);
    }

    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();

    if (argc >= 2)
    {
        load_data(&c8, argv[1]);
    }
    else
    {
        printf("No input file specified");
        return;
    }

    while (!WindowShouldClose())
    {
        // Raylib runs at target 60 fps, so each run of this loop
        // we should process around 12 instructions to achieve ~700 instructions per second.
        for (int i = 0; i < 12; i++)
        {

            execute_instruction(&c8, decode_instruction(fetch_instruction(&c8)));
        }

        BeginDrawing();
        draw_screen(&c8);
        EndDrawing();

        C8_LOG("Finished Raylib frame");

        WaitTime(1);
    }

    CloseWindow();
}
