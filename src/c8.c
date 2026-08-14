#include "raylib.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "c8.h"
#include "display/display.h"
#include "instructions/instructions.h"
#include "loader/loader.h"
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

C8 c8_init()
{
    C8 c8 = {.ram = {}, .pc = C8_PROGRAM_START_LOCATION};

    memcpy(&c8.ram[C8_FONT_START_LOCATION], &font_sprites, 80);

    return c8;
}

int c8_run(int argc, char *argv[])
{
#ifdef C8_DEBUG
    c8_log_init();
#endif

    C8 c8 = c8_init();

    if (argc < 2)
    {
        int err = load_data_input(&c8);

        if (err != 0)
        {
            return 1;
        }
    }
    else
    {
        int err = load_data_path(&c8, argv[1]);

        if (err != 0)
        {
            return 1;
        }
    }

    init_display();
    InitAudioDevice();
    Sound beep = LoadSound("resources/beep.wav");

    while (!WindowShouldClose())
    {
        // Raylib runs at target 60 fps, so each run of the main loop
        // we should process around 12 instructions to achieve ~700 instructions per second.
        uint8_t draw_quirk = 0;
        for (int i = 0; i < 12; i++)
        {
            C8_INSTRUCTION f = fetch_instruction(&c8);
            C8_INSTRUCTION_DATA d = decode_instruction(f);

            if (d.type == C8_I_DRAW)
            {
                draw_quirk++;
            }

            execute_instruction(&c8, d);

            if (draw_quirk >= 1)
            {
                break;
            }
            // WaitTime(0.5);
        }

        if (c8.delay_timer > 0)
        {
            --c8.delay_timer;
        }

        if (c8.sound_timer > 0)
        {
            PlaySound(beep);
            --c8.sound_timer;
        }

        BeginDrawing();
        draw_screen(&c8);
        EndDrawing();

        C8_LOG("Finished Raylib frame\n");

        // WaitTime(0.4);
    }

    UnloadSound(beep);
    CloseWindow();

#ifdef C8_DEBUG
    c8_log_close();
#endif

    return 0;
}
