#include <stdint.h>

#include "raylib.h"

#include "keyboard.h"

static const int raylib_keys[16] = {
    KEY_ZERO, KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE, KEY_SIX, KEY_SEVEN, KEY_EIGHT, KEY_NINE, KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F,
};

bool c8_is_key_down(unsigned short key) { return IsKeyDown(raylib_keys[key]); }

int8_t c8_get_key()
{
    static const int C8_KEY_NONE = 0;
    static int key;

    if (key == C8_KEY_NONE)
    {
        key = GetKeyPressed();

        return -1;
    }

    if (!IsKeyDown(key))
    {

        int8_t key_index = -1;

        for (int i = 0; i < 16; i++)
        {
            if (key == raylib_keys[i])
            {
                key_index = i;

                break;
            }
        }

        key = 0;

        return key_index;
    }

    return -1;
}