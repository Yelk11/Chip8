#ifndef CHIP8_H
#define CHIP8_H



#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>


#define  WIDTH 64
#define  HEIGHT 32


typedef struct {
    uint16_t opcode; // 16 bit opcode
    uint8_t nnn;    // nnn or addr - A 12-bit value, the lowest 12 bits of the instruction
    uint8_t n;      // n or nibble - A 4-bit value, the lowest 4 bits of the instruction
    uint8_t x;      // x - A 4-bit value, the lower 4 bits of the high byte of the instruction
    uint8_t y;      // y - A 4-bit value, the upper 4 bits of the low byte of the instruction
    uint8_t kk;     // kk or byte - An 8-bit value, the lowest 8 bits of the instruction
} instruction_t;

typedef struct
{
    uint8_t scale_factor;
} config_t;

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    config_t* config;
} sdl_t;



typedef struct
{
    uint8_t memory[4096];
    bool display[32 * 64];
    uint16_t stack[12];
    uint16_t* stack_ptr;
    uint8_t v[16];
    uint16_t i;
    uint16_t pc;
    uint8_t delay_timer;
    uint8_t sound_timer;
    bool keypad[16];
    const char *rom_name;
    instruction_t* instruction;
    bool redraw;
} chip8_t;

void PrintEvent(const SDL_Event *event, sdl_t *sdl);

sdl_t *init_sdl(config_t *config);

void draw(sdl_t *sdl, chip8_t* chip);

void close_sdl(sdl_t *sdl);

#endif