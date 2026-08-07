#include <string.h>

#include "c8.h"
#include "instructions.h"
#include "logger/logger.h"

static void clear_screen(C8 *c8)
{
    C8_LOG("Executing clear_screen\n");

    for (int i = 0; i < C8_HEIGHT_PIXELS; i++)
    {
        memset(c8->display[i], 0, sizeof c8->display[0]);
    }

    C8_LOG("Executed clear_screen\n");
}

static void jump(C8 *c8, C8_PROGRAM_COUNTER pc)
{
    c8->pc = pc;
    C8_LOG("Jumped to memory address %d\n", pc);
}

static void set_vx(C8 *c8, C8_VX vx, uint8_t value) { c8->v_regs[vx] = value; };

C8_INSTRUCTION fetch_instruction(C8 *c8)
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

C8_INSTRUCTION_DATA decode_instruction(C8_INSTRUCTION instruction)
{
    uint8_t decoded = (instruction >> 12) & 0xFF;
    C8_LOG("Decoded instruction type: %d\n", decoded);

    C8_INSTRUCTION_DATA d = {
        .type = decoded,
    };

    switch (d.type)
    {
    case C8_INSTRUCTION_JUMP:
    {
        C8_PROGRAM_COUNTER pc = instruction & 0xFFF;

        d.params.pc = pc;

        break;
    }
    case C8_INSTRUCTION_VX_SET:
    {
        // 0110 1111 0000 0001

        C8_VX vx = instruction >> 8 & 0xF;
        uint8_t value = instruction & 0xFF;

        d.params.vx = vx;
        d.params.vx_value = value;
    }
    break;

    default:
        break;
    }

    return d;
}

void execute_instruction(C8 *c8, C8_INSTRUCTION_DATA data)
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