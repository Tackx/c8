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

    C8_LOG("Jumped to memory address %x\n", pc);
}

static void set_vx(C8 *c8, C8_VX vx, uint8_t value)
{
    c8->v_regs[vx] = value;

    C8_LOG("Set register %x to value %d\n", vx, value);
};

static void add_vx(C8 *c8, C8_VX vx, uint8_t value)
{
    c8->v_regs[vx] += value;

    C8_LOG("Added value %d to register %x\n", value, vx);
};

static void set_i(C8 *c8, uint16_t value)
{
    c8->i_index = value;

    C8_LOG("Set Index register to value %x\n", value);
};

static void draw(C8 *c8, uint8_t x_reg, uint8_t y_reg, uint8_t height)
{

    uint8_t x = c8->v_regs[x_reg];
    uint8_t y = c8->v_regs[y_reg];

    C8_LOG("Executing draw. X: %d; Y: %d; Height: %d\n", x, y, height);

    uint8_t *sprite = &(c8->ram[c8->i_index]);

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

static void sub_call(C8 *c8, C8_PROGRAM_COUNTER pc)
{
    c8->pc = pc;

    C8_LOG("Set PC to address %x\n", pc);
};

static void sub_return(C8 *c8) {};

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
    C8_INSTR decoded = (C8_INSTR)((instruction >> 12) & 0xFF);
    C8_LOG("Decoded instruction type: %d\n", decoded);

    C8_INSTRUCTION_DATA d = {
        .type = decoded,
    };

    switch (d.type)
    {
    case JUMP:
    {
        C8_PROGRAM_COUNTER pc = instruction & 0xFFF;

        d.params.pc = pc;

        break;
    }

    case VX_SET:
    {
        C8_VX vx = instruction >> 8 & 0xF;
        uint8_t value = instruction & 0xFF;

        d.params.vx = vx;
        d.params.vx_value = value;

        break;
    }

    case VX_ADD:
    {
        C8_VX vx = instruction >> 8 & 0xF;
        uint8_t value = instruction & 0xFF;

        d.params.vx = vx;
        d.params.vx_value = value;

        break;
    }

    case I_SET:
    {
        uint16_t value = instruction & 0xFFF;

        d.params.i_value = value;

        break;
    }

    case DRAW:
    {
        uint8_t x_reg = instruction >> 8 & 0xF;
        uint8_t y_reg = instruction >> 4 & 0xF;
        uint8_t height = instruction & 0xF;

        d.params.x_reg = x_reg;
        d.params.y_reg = y_reg;
        d.params.draw_height = height;

        break;
    }

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
    case CLEAR_SCREEN:
        C8_LOG("Recognized the clear screen instruction\n");

        clear_screen(c8);

        break;

    case JUMP:
        C8_LOG("Recognized the jump instruction\n");

        jump(c8, data.params.pc);

        break;

    case VX_SET:
        C8_LOG("Recognized the VX SET instruction\n");

        set_vx(c8, data.params.vx, data.params.vx_value);

        break;

    case VX_ADD:
        C8_LOG("Recognized the VX ADD instruction\n");

        add_vx(c8, data.params.vx, data.params.vx_value);

        break;

    case I_SET:
        C8_LOG("Recognized the I SET instruction\n");

        set_i(c8, data.params.i_value);

        break;

    case DRAW:
        C8_LOG("Recognized the DRAW instruction\n");

        draw(c8, data.params.x_reg, data.params.y_reg, data.params.draw_height);

        break;

    default:
        break;
    }
}