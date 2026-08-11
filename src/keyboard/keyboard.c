#include "raylib.h"

#include "keyboard.h"

bool c8_is_key_down(unsigned short key)
{
    static const int raylib_keys[16] = {
        KEY_ZERO, KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE, KEY_SIX, KEY_SEVEN, KEY_EIGHT, KEY_NINE, KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F,
    };

    return IsKeyDown(raylib_keys[key]);
}