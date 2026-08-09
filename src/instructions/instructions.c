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

    C8_LOG("Jumped to memory address %02hX\n", pc);
}

static void set_vx(C8 *c8, C8_VX vx, uint8_t value)
{
    c8->v_regs[vx] = value;

    C8_LOG("Set register %02hhX to value %d\n", vx, value);
};

static void add_vx(C8 *c8, C8_VX vx, uint8_t value)
{
    c8->v_regs[vx] += value;

    C8_LOG("Added value %d to register %02hhX\n", value, vx);
};

static void set_i(C8 *c8, uint16_t value)
{
    c8->i_index = value;

    C8_LOG("Set Index register to value %02hX\n", value);
};

static void draw(C8 *c8, uint8_t x_reg, uint8_t y_reg, uint8_t height)
{

    uint8_t x = c8->v_regs[x_reg];
    uint8_t y = c8->v_regs[y_reg];

    C8_LOG("Executing draw. X: %d; Y: %d; Height: %d\n", x, y, height);

    uint8_t *sprite = &(c8->ram[c8->i_index]);

    bool off = false;
    for (int i = 0; i < height; ++i)
    {
        // TODO: Fix magic number
        for (int j = 0; j < 8; ++j)
        {
            bool bit = (*(sprite + i) >> (7 - j)) & 1;
            // TODO: Can detect collisions here
            c8->display[y + i][x + j] ^= bit;

            if (!c8->display[y + i][x + j])
            {
                off = true;
            }
        }
    }

    if (off)
    {
        c8->v_regs[C8_REG_VF] = 1;
    }
    else
    {
        c8->v_regs[C8_REG_VF] = 0;
    }
}

static void sub_call(C8 *c8, C8_PROGRAM_COUNTER pc)
{

    // Push current pc location to the stack
    // before updating the pc to point at the new location
    c8->stack[(c8->sp)++] = c8->pc;

    c8->pc = pc;

    C8_LOG("Subroutine call: set PC to address %02hX\n", pc);
};

static void sub_ret(C8 *c8)
{

    c8->pc = c8->stack[--(c8->sp)];

    C8_LOG("Subroutine return: set PC to address %02hX\n", c8->pc);
};

static void skip_if_vx(C8 *c8, C8_VX reg, uint8_t value)
{

    if (c8->v_regs[reg] == value)
    {
        c8->pc += 2;

        C8_LOG("Skip if VX condition matched\n");
    }
};

static void skip_if_not_vx(C8 *c8, C8_VX reg, uint8_t value)
{

    if (c8->v_regs[reg] != value)
    {
        c8->pc += 2;

        C8_LOG("Skip if not VX condition matched\n");
    }
};

static void skip_if_vx_vy(C8 *c8, C8_VX reg_x, C8_VX reg_y)
{
    if (c8->v_regs[reg_x] == c8->v_regs[reg_y])
    {
        c8->pc += 2;

        C8_LOG("Skip if VX == VY condition matched\n");
    }
};

static void skip_if_not_vx_vy(C8 *c8, C8_VX reg_x, C8_VX reg_y)
{
    if (c8->v_regs[reg_x] != c8->v_regs[reg_y])
    {
        c8->pc += 2;

        C8_LOG("Skip if VX != VY condition matched\n");
    }
};

static void set(C8 *c8, C8_VX reg_x, C8_VX reg_y)
{ //
    c8->v_regs[reg_x] = c8->v_regs[reg_y];

    C8_LOG("Set register %02hhX to value of register %02hhX \n", reg_x, reg_y);
};

static void b_or(C8 *c8, C8_VX reg_x, C8_VX reg_y)
{
    //
    c8->v_regs[reg_x] = c8->v_regs[reg_x] | c8->v_regs[reg_y];

    C8_LOG("Set register %02hhX to binary OR with register %02hhX \n", reg_x, reg_y);
};

static void b_and(C8 *c8, C8_VX reg_x, C8_VX reg_y)
{
    //
    c8->v_regs[reg_x] = c8->v_regs[reg_x] & c8->v_regs[reg_y];

    C8_LOG("Set register %02hhX to binary AND with register %02hhX \n", reg_x, reg_y);
};

static void lxor(C8 *c8, C8_VX reg_x, C8_VX reg_y)
{
    //
    c8->v_regs[reg_x] = c8->v_regs[reg_x] ^ c8->v_regs[reg_y];

    C8_LOG("Set register %02hhX to logical XOR with register %02hhX \n", reg_x, reg_y);
};

static void add(C8 *c8, C8_VX reg_x, C8_VX reg_y)
{
    if ((c8->v_regs[reg_x] + c8->v_regs[reg_y]) > 255)
    {
        c8->v_regs[C8_REG_VF] = 1;
    }
    else
    {
        c8->v_regs[C8_REG_VF] = 0;
    }

    uint8_t result = c8->v_regs[reg_x] + c8->v_regs[reg_y];

    c8->v_regs[reg_x] = result;
};

static void sub_y_from_x(C8 *c8, C8_VX reg_x, C8_VX reg_y)
{
    uint8_t x_value = c8->v_regs[reg_x];
    uint8_t y_value = c8->v_regs[reg_y];

    c8->v_regs[reg_x] = x_value - y_value;

    if (x_value >= y_value)
    {
        c8->v_regs[C8_REG_VF] = 1;
    }
    else
    {
        c8->v_regs[C8_REG_VF] = 0;
    }
};

static void sub_x_from_y(C8 *c8, C8_VX reg_x, C8_VX reg_y)
{
    uint8_t x_value = c8->v_regs[reg_x];
    uint8_t y_value = c8->v_regs[reg_y];

    c8->v_regs[reg_x] = y_value - x_value;

    if (y_value >= x_value)
    {
        c8->v_regs[C8_REG_VF] = 1;
    }
    else
    {
        c8->v_regs[C8_REG_VF] = 0;
    }
};

// TODO: Make this configurable to work with reg_y for compatibility
// https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#8xy6-and-8xye-shift
static void shift_right(C8 *c8, C8_VX reg_x, C8_VX reg_y)
{
    uint8_t val = c8->v_regs[reg_x] & 1;

    c8->v_regs[reg_x] = c8->v_regs[reg_x] >> 1;

    if (val)
    {
        c8->v_regs[C8_REG_VF] = 1;
    }
    else
    {
        c8->v_regs[C8_REG_VF] = 0;
    }
};

// TODO: Make this configurable to work with reg_y for compatibility
// https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#8xy6-and-8xye-shift
static void shift_left(C8 *c8, C8_VX reg_x, C8_VX reg_y)
{
    uint8_t val = c8->v_regs[reg_x] & 1;

    c8->v_regs[reg_x] = c8->v_regs[reg_x] << 1;

    if (val)
    {
        c8->v_regs[C8_REG_VF] = 1;
    }
    else
    {
        c8->v_regs[C8_REG_VF] = 0;
    }
};

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
    C8_OP_HN high_nibble = (C8_OP_HN)((instruction >> 12) & 0xF);

    C8_INSTRUCTION_DATA d = {};

    switch (high_nibble)
    {
    case C8_OP_HN_0:
    {
        C8_OP_HN_0_LB low_byte = (C8_OP_HN_0_LB)(instruction & 0xFF);

        switch (low_byte)
        {
        case C8_OP_HN_0_LB_E0:
            d.type = C8_I_CLEAR_SCREEN;

            break;

        case C8_OP_HN_0_LB_EE:
            d.type = C8_I_SUB_RET;

            break;

        default:
            d.type = C8_I_GARBAGE;

            break;
        }

        break;
    }

    case C8_OP_HN_1:
    {
        d.type = C8_I_JUMP;

        C8_PROGRAM_COUNTER pc = instruction & 0xFFF;

        d.params.nnn = pc;

        break;
    }

    case C8_OP_HN_2:
    {
        d.type = C8_I_SUB_CALL;
        C8_PROGRAM_COUNTER pc = instruction & 0xFFF;

        d.params.nnn = pc;

        break;
    }

    case C8_OP_HN_3:
    {
        d.type = C8_I_SKIP_IF_VX;
        C8_VX reg = (C8_VX)((instruction >> 8) & 0xF);
        uint8_t value = instruction & 0xFF;

        d.params.vx = reg;
        d.params.nn = value;

        break;
    }

    case C8_OP_HN_4:
    {
        d.type = C8_I_SKIP_IF_NOT_VX;
        C8_VX reg = (C8_VX)((instruction >> 8) & 0xF);
        uint8_t value = instruction & 0xFF;

        d.params.vx = reg;
        d.params.nn = value;

        break;
    }

    case C8_OP_HN_5:
    {
        C8_OP_HN_5_LN low_nibble = (C8_OP_HN_5_LN)(instruction & 0xF);

        switch (low_nibble)
        {
        case C8_OP_HN_5_LN_0:
        {
            d.type = C8_I_SKIP_IF_VX_VY;
            C8_VX reg_x = (C8_VX)((instruction >> 8) & 0xF);
            C8_VX reg_y = (C8_VX)((instruction >> 4) & 0xF);

            d.params.vx = reg_x;
            d.params.vy = reg_y;

            break;
        }

        default:
            d.type = C8_I_GARBAGE;

            break;
        }

        break;
    }

    case C8_OP_HN_6:
    {
        d.type = C8_I_VX_SET;
        C8_VX vx = (C8_VX)(instruction >> 8 & 0xF);
        uint8_t value = instruction & 0xFF;

        d.params.vx = vx;
        d.params.nn = value;

        break;
    }

    case C8_OP_HN_7:
    {
        d.type = C8_I_VX_ADD;
        C8_VX vx = (C8_VX)(instruction >> 8 & 0xF);
        uint8_t value = instruction & 0xFF;

        d.params.vx = vx;
        d.params.nn = value;

        break;
    }

    case C8_OP_HN_8:
    {
        C8_OP_HN_8_LN low_nibble = (C8_OP_HN_8_LN)(instruction & 0xF);

        switch (low_nibble)
        {

        case C8_OP_HN_8_LN_0:
            d.type = C8_I_SET;

            d.params.vx = (C8_VX)((instruction >> 8) & 0xF);
            d.params.vy = (C8_VX)((instruction >> 4) & 0xF);

            break;

        case C8_OP_HN_8_LN_1:
            d.type = C8_I_B_OR;

            d.params.vx = (C8_VX)((instruction >> 8) & 0xF);
            d.params.vy = (C8_VX)((instruction >> 4) & 0xF);

            break;

        case C8_OP_HN_8_LN_2:
            d.type = C8_I_B_AND;

            d.params.vx = (C8_VX)((instruction >> 8) & 0xF);
            d.params.vy = (C8_VX)((instruction >> 4) & 0xF);

            break;

        case C8_OP_HN_8_LN_3:
            d.type = C8_I_XOR;

            d.params.vx = (C8_VX)((instruction >> 8) & 0xF);
            d.params.vy = (C8_VX)((instruction >> 4) & 0xF);

            break;

        case C8_OP_HN_8_LN_4:
            d.type = C8_I_ADD;

            d.params.vx = (C8_VX)((instruction >> 8) & 0xF);
            d.params.vy = (C8_VX)((instruction >> 4) & 0xF);

            break;

        case C8_OP_HN_8_LN_5:
            d.type = C8_I_SUBTRACT_Y_X;

            d.params.vx = (C8_VX)((instruction >> 8) & 0xF);
            d.params.vy = (C8_VX)((instruction >> 4) & 0xF);

            break;

        case C8_OP_HN_8_LN_7:
            d.type = C8_I_SUBTRACT_X_Y;

            d.params.vx = (C8_VX)((instruction >> 8) & 0xF);
            d.params.vy = (C8_VX)((instruction >> 4) & 0xF);

            break;

        case C8_OP_HN_8_LN_6:
            d.type = C8_I_SHIFT_R;

            d.params.vx = (C8_VX)((instruction >> 8) & 0xF);
            d.params.vy = (C8_VX)((instruction >> 4) & 0xF);

            break;

        case C8_OP_HN_8_LN_E:
            d.type = C8_I_SHIFT_L;

            d.params.vx = (C8_VX)((instruction >> 8) & 0xF);
            d.params.vy = (C8_VX)((instruction >> 4) & 0xF);

            break;

        default:
            d.type = C8_I_GARBAGE;

            break;
        }

        break;
    }

    case C8_OP_HN_9:
    {
        C8_OP_HN_9_LN low_nibble = (C8_OP_HN_9_LN)(instruction & 0xF);

        switch (low_nibble)
        {
        case C8_OP_HN_9_LN_0:
        {
            d.type = C8_I_SKIP_IF_NOT_VX_VY;
            C8_VX reg_x = (C8_VX)((instruction >> 8) & 0xF);
            C8_VX reg_y = (C8_VX)((instruction >> 4) & 0xF);

            d.params.vx = reg_x;
            d.params.vy = reg_y;

            break;
        }

        default:
            d.type = C8_I_GARBAGE;

            break;
        }

        break;
    }

    case C8_OP_HN_A:
    {
        d.type = C8_I_SET_I;
        uint16_t value = instruction & 0xFFF;

        d.params.nnn = value;

        break;
    }

    case C8_OP_HN_D:
    {
        d.type = C8_I_DRAW;
        C8_VX x_reg = (C8_VX)(instruction >> 8 & 0xF);
        C8_VX y_reg = (C8_VX)(instruction >> 4 & 0xF);
        uint8_t height = instruction & 0xF;

        d.params.vx = x_reg;
        d.params.vy = y_reg;
        d.params.n = height;

        break;
    }

    default:
        d.type = C8_I_GARBAGE;

        break;
    }

    C8_LOG("Decoded instruction type: %02hhX\n", d.type);

    return d;
}

void execute_instruction(C8 *c8, C8_INSTRUCTION_DATA data)
{
    C8_LOG("Executing instruction\n");

    switch (data.type)
    {
    case C8_I_GARBAGE:
        C8_LOG("Garbage instruction\n");

        break;

    case C8_I_CLEAR_SCREEN:
        C8_LOG("Recognized the CLEAR instruction\n");

        clear_screen(c8);

        break;

    case C8_I_JUMP:
        C8_LOG("Recognized the JUMP instruction\n");

        jump(c8, (C8_PROGRAM_COUNTER)data.params.nnn);

        break;

    case C8_I_VX_SET:
        C8_LOG("Recognized the VX_SET instruction\n");

        set_vx(c8, data.params.vx, data.params.nn);

        break;

    case C8_I_VX_ADD:
        C8_LOG("Recognized the VX_ADD instruction\n");

        add_vx(c8, data.params.vx, data.params.nn);

        break;

    case C8_I_SET_I:
        C8_LOG("Recognized the SET_I instruction\n");

        set_i(c8, data.params.nnn);

        break;

    case C8_I_DRAW:
        C8_LOG("Recognized the DRAW instruction\n");

        draw(c8, data.params.vx, data.params.vy, data.params.n);

        break;

    case C8_I_SUB_CALL:
        C8_LOG("Recognized the SUB_CALL instruction\n");

        sub_call(c8, data.params.nnn);

        break;

    case C8_I_SUB_RET:
        C8_LOG("Recognized the SUB_RET instruction\n");

        sub_ret(c8);

        break;

    case C8_I_SKIP_IF_VX:
        C8_LOG("Recognized the SKIP_IF_VX instruction\n");

        skip_if_vx(c8, data.params.vx, data.params.nn);

        break;

    case C8_I_SKIP_IF_NOT_VX:
        C8_LOG("Recognized the SKIP_IF_NOT_VX instruction\n");

        skip_if_not_vx(c8, data.params.vx, data.params.nn);

        break;

    case C8_I_SKIP_IF_VX_VY:
        C8_LOG("Recognized the SKIP_IF_VX_VY instruction\n");

        skip_if_vx_vy(c8, data.params.vx, data.params.vy);

        break;

    case C8_I_SKIP_IF_NOT_VX_VY:
        C8_LOG("Recognized the SKIP_IF_NOT_VX_VY instruction\n");

        skip_if_not_vx_vy(c8, data.params.vx, data.params.vy);

        break;

    case C8_I_SET:
        C8_LOG("Recognized the SET instruction\n");

        set(c8, data.params.vx, data.params.vy);

        break;

    case C8_I_B_OR:
        C8_LOG("Recognized the B OR instruction\n");

        b_or(c8, data.params.vx, data.params.vy);

        break;

    case C8_I_B_AND:
        C8_LOG("Recognized the B AND instruction\n");

        b_and(c8, data.params.vx, data.params.vy);

        break;

    case C8_I_XOR:
        C8_LOG("Recognized the XOR instruction\n");

        lxor(c8, data.params.vx, data.params.vy);

        break;

    case C8_I_ADD:
        C8_LOG("Recognized the ADD instruction\n");

        add(c8, data.params.vx, data.params.vy);

        break;

    case C8_I_SUBTRACT_Y_X:
        C8_LOG("Recognized the C8_I_SUBTRACT_Y_X instruction\n");

        sub_y_from_x(c8, data.params.vx, data.params.vy);

        break;

    case C8_I_SUBTRACT_X_Y:
        C8_LOG("Recognized the C8_I_SUBTRACT_X_Y instruction\n");

        sub_x_from_y(c8, data.params.vx, data.params.vy);

        break;

    case C8_I_SHIFT_R:
        C8_LOG("Recognized the C8_I_SHIFT_R instruction\n");

        shift_right(c8, data.params.vx, data.params.vy);

        break;

    case C8_I_SHIFT_L:
        C8_LOG("Recognized the C8_I_SHIFT_L instruction\n");

        shift_left(c8, data.params.vx, data.params.vy);

        break;

    default:
        C8_LOG("Unrecognized instruction type (this should never happen?)\n");

        break;
    }
}