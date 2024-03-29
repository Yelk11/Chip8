#include "instructions.h"
#include "chip8.h"

#include <stdint.h>


void next(chip8_t* chip)
{
    chip->instruction->opcode  = chip->memory[chip->pc];
    chip->instruction->opcode <<= 8;
    chip->instruction->opcode  |= chip->memory[chip->pc + 1];
    chip->pc += 2;
    chip->instruction->nnn  = chip->instruction->opcode  & 0x0FFF;
    chip->instruction->kk   = chip->instruction->opcode  & 0x00FF;
    chip->instruction->n    = chip->instruction->opcode  & 0x000F;
    chip->instruction->x    = (chip->instruction->opcode >> 8)  & 0x0F;
    chip->instruction->y    = (chip->instruction->opcode >> 4) & 0x0F;

    // #ifdef DEBUG
    //     printf("Opcode: %.4X\n", chip->instruction->opcode);
    //     printf("nnn: %.4X\n", chip->instruction->nnn);
    //     printf("kk: %.4X\n", chip->instruction->kk);
    //     printf("n: %.4X\n", chip->instruction->n);
    //     printf("x: %.4X\n", chip->instruction->x);
    //     printf("y: %.4X\n", chip->instruction->y);
    //     printf("I: %.4X\n", chip->i);
    //     printf("pc: %.4X\n\n", chip->pc);
    // #endif
}


void run_instruction(chip8_t* chip)
{
    next(chip);
    
    switch((chip->instruction->opcode & 0xF000) >> 12)
    {
        case 0x0:
        {
            /*
                00E0 - CLS
                Clear the display.
            */
            if(chip->instruction->opcode == 0x00E0)
            {
                #ifdef DEBUG
                    printf("00E0 - CLS: Clear the display.\n");
                    printf("Opcode: %.4X\n", chip->instruction->opcode);
                #endif
                memset(&chip->display[0], false, sizeof(chip->display));
                chip->redraw = true;
            }
            /*
                00EE - RET
                Return from a subroutine.
                The interpreter sets the program counter to the address at the top of the stack, 
                    then subtracts 1 from the stack pointer.

            */
            else if (chip->instruction->opcode == 0x00EE)
            {
                chip->pc = *chip->stack_ptr;
                chip->stack_ptr--;
                #ifdef DEBUG
                    printf("00EE - RET: Return from a subroutine.\n");
                    printf("Opcode: %.4X\n", chip->instruction->opcode);
                    printf("pc: %.4X\n", chip->pc);
                    printf("nnn: %.4X\n", *chip->stack_ptr);
                #endif
                break;
            }
            /*
                0nnn - SYS addr
                Jump to a machine code routine at nnn.
                This instruction is only used on the old computers on which Chip-8 was originally implemented.
                     is ignored by modern interpreters.
            */
            else
            {
                
                // chip->pc = chip->instruction->nnn;
            }
            break;
        }


        // 1nnn - JP addr
        // Jump to location nnn.
        
        // The interpreter sets the program counter to nnn.
        case 0x1:
        {
            chip->pc = chip->instruction->nnn;
            #ifdef DEBUG
                printf("1nnn - JP addr: Jump to location nnn(%X).", chip->instruction->nnn);
                printf("Opcode: %.4X\n", chip->instruction->opcode);
                printf("pc: %.4X\n", chip->pc);
                printf("nnn: %.4X\n", chip->instruction->nnn);
            #endif
            break;
        }
            


        // 2nnn - CALL addr
        // Call subroutine at nnn.

        // The interpreter increments the stack pointer, then puts the current PC on the top of the stack. The PC is then set to nnn.


        // 3xkk - SE Vx, byte
        // Skip next instruction if Vx = kk.

        // The interpreter compares register Vx to kk, and if they are equal, increments the program counter by 2.


        // 4xkk - SNE Vx, byte
        // Skip next instruction if Vx != kk.

        // The interpreter compares register Vx to kk, and if they are not equal, increments the program counter by 2.


        // 5xy0 - SE Vx, Vy
        // Skip next instruction if Vx = Vy.

        // The interpreter compares register Vx to register Vy, and if they are equal, increments the program counter by 2.


        // 6xkk - LD Vx, byte
        // Set Vx = kk.

        // The interpreter puts the value kk into register Vx.
        case 0x6:
        {
            chip->v[chip->instruction->x] = chip->instruction->kk;
            #ifdef DEBUG
                printf("6xkk - LD Vx, byte: Set Vx(%X) = kk(%X).\n", chip->v[chip->instruction->x], chip->instruction->kk);
                printf("Opcode: %.4X\n", chip->instruction->opcode);
                printf("V[x]: %.4X\n", chip->v[chip->instruction->x]);
                printf("x: %.4X\n", chip->instruction->x);
                printf("kk: %.4X\n", chip->instruction->kk);
            #endif
            break;
        }
            

        // 7xkk - ADD Vx, byte
        // Set Vx = Vx + kk.

        // Adds the value kk to the value of register Vx, then stores the result in Vx. 
        case 0x7:
        {
            #ifdef DEBUG
                printf("7xkk - ADD Vx, byte\n");
                printf("Opcode: %.4X\n", chip->instruction->opcode);
                printf("V[x]: %.4X\n", chip->v[chip->instruction->x]);
            #endif
            chip->v[chip->instruction->x] = chip->v[chip->instruction->x] + chip->instruction->kk;
            #ifdef DEBUG
                printf("V[x] after addition: %.4X\n", chip->v[chip->instruction->x]);
            #endif
            break;
        }

        // 8xy0 - LD Vx, Vy
        // Set Vx = Vy.

        // Stores the value of register Vy in register Vx.


        // 8xy1 - OR Vx, Vy
        // Set Vx = Vx OR Vy.

        // Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx. A bitwise OR compares the corrseponding bits from two values, and if either bit is 1, then the same bit in the result is also 1. Otherwise, it is 0. 


        // 8xy2 - AND Vx, Vy
        // Set Vx = Vx AND Vy.

        // Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx. A bitwise AND compares the corrseponding bits from two values, and if both bits are 1, then the same bit in the result is also 1. Otherwise, it is 0. 


        // 8xy3 - XOR Vx, Vy
        // Set Vx = Vx XOR Vy.

        // Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx. An exclusive OR compares the corrseponding bits from two values, and if the bits are not both the same, then the corresponding bit in the result is set to 1. Otherwise, it is 0. 


        // 8xy4 - ADD Vx, Vy
        // Set Vx = Vx + Vy, set VF = carry.

        // The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.


        // 8xy5 - SUB Vx, Vy
        // Set Vx = Vx - Vy, set VF = NOT borrow.

        // If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.


        // 8xy6 - SHR Vx {, Vy}
        // Set Vx = Vx SHR 1.

        // If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.


        // 8xy7 - SUBN Vx, Vy
        // Set Vx = Vy - Vx, set VF = NOT borrow.

        // If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.


        // 8xyE - SHL Vx {, Vy}
        // Set Vx = Vx SHL 1.

        // If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.


        // 9xy0 - SNE Vx, Vy
        // Skip next instruction if Vx != Vy.

        // The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.


        // Annn - LD I, addr
        // Set I = nnn.

        // The value of register I is set to nnn.
        case 0xA: // TODO: nnn is only kk?
        {
            /*
            Annn - LD I, addr: Set I = nnn(66).
                Opcode: A266
                i: 0066
                nnn: 0066
            */
            chip->i = chip->instruction->nnn;
            #ifdef DEBUG
                printf("Annn - LD I, addr: Set I = nnn(%X).\n", chip->instruction->nnn);
                printf("Opcode: %.4X\n", chip->instruction->opcode);
                printf("i: %.4X\n", chip->i);
                printf("nnn: %.4X\n", chip->instruction->nnn);
            #endif
        }

        // Bnnn - JP V0, addr
        // Jump to location nnn + V0.

        // The program counter is set to nnn plus the value of V0.


        // Cxkk - RND Vx, byte
        // Set Vx = random byte AND kk.

        // The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk. The results are stored in Vx. See instruction 8xy2 for more information on AND.


        // Dxyn - DRW Vx, Vy, nibble
        // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.

        // The interpreter reads n bytes from memory, starting at the address stored in I. These bytes are then displayed as sprites on screen at coordinates (Vx, Vy). Sprites are XORed onto the existing screen. If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0. If the sprite is positioned so part of it is outside the coordinates of the display, it wraps around to the opposite side of the screen. See instruction 8xy3 for more information on XOR, and section 2.4, Display, for more information on the Chip-8 screen and sprites.
        case 0xD:
        {
            #ifdef DEBUG
                printf("Dxyn - DRW Vx(%X), Vy(%X), nibble(%X)\n", chip->v[chip->instruction->x], chip->v[chip->instruction->y], chip->instruction->n);
            #endif
            uint8_t x = chip->v[chip->instruction->x] % WIDTH;
            uint8_t y = chip->v[chip->instruction->y] % HEIGHT;

            const uint8_t og_x = x;

            chip->v[0x0F] = 0;
            // loop over sprite rows (N in total)
            for (uint8_t i = 0; i < chip->instruction->n; i++) {
                const uint8_t sprite_data = chip->memory[chip->i + i]; // next byte/row of sprite data

                x = og_x; // reset x for the next row to draw

                for (int8_t j = 7; j >= 0; j--) {
                    const bool sprite_bit = (sprite_data & (1 << j));
                    bool *pixel           = &chip->display[y * WIDTH + x];

                    // if sprite pixel is on and display pixel is on, set carry flag
                    if (sprite_bit && *pixel) chip->v[0x0F] = 1;

                    // xor display pixel with sprite pixel to set it on/off
                    *pixel ^= sprite_bit;

                    // stop drawing if we hit right screen edge
                    if (++x >= WIDTH) break;
                }

                // stop drawing the entire sprite if we hit bottom screen edge
                if (++y >= HEIGHT) break;
            }
            chip->redraw = true; // will update the screen on next tick
            break;
        }
            

        // Ex9E - SKP Vx
        // Skip next instruction if key with the value of Vx is pressed.

        // Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, PC is increased by 2.


        // ExA1 - SKNP Vx
        // Skip next instruction if key with the value of Vx is not pressed.

        // Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.


        // Fx07 - LD Vx, DT
        // Set Vx = delay timer value.

        // The value of DT is placed into Vx.


        // Fx0A - LD Vx, K
        // Wait for a key press, store the value of the key in Vx.

        // All execution stops until a key is pressed, then the value of that key is stored in Vx.


        // Fx15 - LD DT, Vx
        // Set delay timer = Vx.

        // DT is set equal to the value of Vx.


        // Fx18 - LD ST, Vx
        // Set sound timer = Vx.

        // ST is set equal to the value of Vx.


        // Fx1E - ADD I, Vx
        // Set I = I + Vx.

        // The values of I and Vx are added, and the results are stored in I.


        // Fx29 - LD F, Vx
        // Set I = location of sprite for digit Vx.

        // The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx. See section 2.4, Display, for more information on the Chip-8 hexadecimal font.


        // Fx33 - LD B, Vx
        // Store BCD representation of Vx in memory locations I, I+1, and I+2.

        // The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.


        // Fx55 - LD [I], Vx
        // Store registers V0 through Vx in memory starting at location I.

        // The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.


        // Fx65 - LD Vx, [I]
        // Read registers V0 through Vx from memory starting at location I.

        // The interpreter reads values from memory starting at location I into registers V0 through Vx.

        default:
            printf("OPCODE %X not implemented\n", chip->instruction->opcode);
    }
}