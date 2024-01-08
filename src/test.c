#include "test.h"
#include "chip8.h"
#include "instructions.h"

#include <stdbool.h>

chip8_t* test_setup(uint16_t opcode)
{
    chip8_t* chip = calloc(1, sizeof(chip8_t));

    const uint32_t entry_point = 0x200;

    const uint8_t font[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80, // F
    };

    // reset entire machine
    memset(chip, 0, sizeof(chip8_t));

    // load font
    memcpy(&chip->memory[0], font, sizeof(font));


    // defaults
    chip->instruction = calloc(1, sizeof(instruction_t));
    chip->pc        = entry_point;
    chip->stack_ptr = &chip->stack[0];
    return chip;

}



// 00E0 - CLS
bool test_00E0_CLS()
{
    // setup
    chip8_t* chip = test_setup(0x00E0);
    run_instruction(chip);
    int sum = 0;
    for (int i = 0; i < WIDTH * HEIGHT; i++)
    {
        sum += chip->display[i];
    }
    return sum == 0;
}

// 00EE - RET
bool test_00EE_RET()
{
    chip8_t* chip = test_setup(0x00E0);
    int test_pc = chip->pc;
    
    run_instruction(chip);
    
    return test_pc == *chip->stack_ptr;
}

// 1nnn - JP addr
bool test_1nnn_JP_addr()
{
    chip8_t* chip = test_setup(0x1234);
    run_instruction(chip);
    return chip->instruction->kk == 0x34;
}

// 6xkk - LD Vx, byte
bool test_6xkk_LD_Vx_kk()
{
    chip8_t* chip = test_setup(0x6234);
    run_instruction(chip);
    return chip->v[2] == 0x34;
}

// 7xkk - ADD Vx, byte
// Set Vx = Vx + kk.
bool test_7xkk_ADD_Vx_kk()
{
    chip8_t* chip = test_setup(0x7234);
    int Vx = chip->v[2];
    int kk = chip->instruction->kk;
    run_instruction(chip);
    return Vx + kk == chip->v[2];
}

// Annn - LD I, addr
// Set I = nnn.
bool test_Annn_LD_I_nnn()
{
    chip8_t* chip = test_setup(0xA234);
    run_instruction(chip);
    return chip->i == chip->instruction->nnn;
}

// Dxyn - DRW Vx, Vy, nibble
// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
bool test_Dxyn_DRW_Vx_Vy_n()
{
    return true;
}


bool test_all()
{


printf("test_00E0_CLS: %s\n", test_00E0_CLS() ? "pass" : "FAIL");

printf("test_00EE_RET: %s\n", test_00EE_RET() ? "pass" : "FAIL");

printf("test_1nnn_JP_addr: %s\n", test_1nnn_JP_addr() ? "pass" : "FAIL");

printf("test_6xkk_LD_Vx_kk: %s\n", test_6xkk_LD_Vx_kk() ? "pass" : "FAIL");

printf("test_7xkk_ADD_Vx_kk: %s\n", test_7xkk_ADD_Vx_kk() ? "pass" : "FAIL");

printf("test_Annn_LD_I_nnn: %s\n", test_Annn_LD_I_nnn() ? "pass" : "FAIL");

printf("test_Dxyn_DRW_Vx_Vy_n: %s\n", test_Dxyn_DRW_Vx_Vy_n() ? "pass" : "FAIL");

    return true;
}






