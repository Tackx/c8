#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
typedef uint16_t DWORD;
void Sleep(DWORD dwMilliseconds);
#else
#include <unistd.h>
#endif

#include "raylib.h"

#include "consts/consts.h"

typedef uint8_t C8_RAM[C8_RAM_SIZE];

// The framebuffer
typedef bool C8_DISPLAY[C8_HEIGHT_PIXELS][C8_WIDTH_PIXELS];
typedef uint16_t C8_PROGRAM_COUNTER;
typedef uint16_t C8_INSTRUCTION;

// Index register which points at locations in memory
typedef uint16_t C8_I_INDEX;

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
    {0xF0, 0x80, 0xF0, 0x80, 0x80}  // F
};

static void clear_screen(C8 *c8)
{
    printf("Executing clear_screen\n");

    for (int i = 0; i < C8_HEIGHT_PIXELS; i++)
    {
        // for (int j = 0; j < C8_WIDTH_PIXELS; j++)
        // {
        //     c8->display[i][j] = 0;
        // }

        memset(c8->display[i], 0, sizeof c8->display[0]);
    }

    printf("Executed clear_screen\n");
}

static void jump(C8 *c8, C8_PROGRAM_COUNTER pc)
{
    c8->pc = pc;
    printf("Jumped to memory address %d\n", pc);
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

        printf("Read: %d\n", c8->ram[C8_PROGRAM_START_LOCATION]);
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
    printf("Decoded instruction type: %d\n", decoded);

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
    printf("Executing instruction\n");

    switch (data.type)
    {
    case C8_INSTRUCTION_CLEAR_SCREEN:
        printf("Recognized the clear screen instruction\n");

        clear_screen(c8);

        break;
    case C8_INSTRUCTION_JUMP:
        printf("Recognized the jump instruction\n");

        jump(c8, data.params.pc);

        break;
    case C8_INSTRUCTION_VX_ADD:
        printf("Recognized the VX ADD instruction\n");
        break;
    case C8_INSTRUCTION_VX_SET:
        printf("Recognized the VX SET instruction\n");
        break;
    case C8_INSTRUCTION_I_SET:
        printf("Recognized the I SET instruction\n");
        break;
    case C8_INSTRUCTION_DRAW:
        printf("Recognized the DRAW instruction\n");
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

    // c8.ram[0] = 0x10;
    // c8.ram[1] = 0xE0;
    // 00010000
    // 11100000
    // 00010000 11100000

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
        execute_instruction(&c8, decode_instruction(fetch_instruction(&c8)));

        BeginDrawing();
        draw_screen(&c8);
        EndDrawing();

        Sleep(1000);
    }

    CloseWindow();
}
