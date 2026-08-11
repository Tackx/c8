#include <stdint.h>

#include "raylib.h"

#include "keyboard.h"

static const int raylib_keys[16] = {
    KEY_ZERO, KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE, KEY_SIX, KEY_SEVEN, KEY_EIGHT, KEY_NINE, KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F,
};

bool c8_is_key_down(unsigned short key) { return IsKeyDown(raylib_keys[key]); }

int8_t c8_get_key()
{
    int key = GetKeyPressed();

    int8_t key_index = -1;

    for (int i = 0; i < 16; i++)
    {
        if (key == raylib_keys[i])
        {
            key_index = i;

            break;
        }
    }

    return key_index;
}