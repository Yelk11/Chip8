#include "chip8.h"
#include "instructions.h"

#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

// Screen dimension constants
const int WIDTH = 64;
const int HEIGHT = 32;

sdl_t* init_sdl(config_t* config)
{
    sdl_t* sdl = calloc(1, sizeof(sdl_t));
    sdl->config = config;
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Failed to init SDL: %s\n", SDL_GetError());
        return false;
    }
    sdl->window = SDL_CreateWindow(
        "Chip8 Emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH * sdl->config->scale_factor,
        HEIGHT * sdl->config->scale_factor,
        0);
    if (!sdl->window) {
        SDL_Log("Failed to create a window: %s\n", SDL_GetError());
        return false;
    }
    
    sdl->renderer = SDL_CreateRenderer(sdl->window, -1, SDL_RENDERER_ACCELERATED);
    if (!sdl->renderer) {
        SDL_Log("Failed to create a renderer: %s\n", SDL_GetError());
        return false;
    }
    return sdl;
}

void draw(sdl_t *sdl, chip8_t* chip)
{
    SDL_SetRenderDrawColor(sdl->renderer, 0, 0, 0, 0);
    SDL_RenderClear(sdl->renderer);
    SDL_SetRenderDrawColor(sdl->renderer, 255, 0, 0, 255);

    SDL_Rect rect = {.x = 0, .y = 0, .w = sdl->config->scale_factor, .h = sdl->config->scale_factor};

    for (uint32_t i = 0; i < sizeof chip->display; i++)
    {
        rect.x = (i % WIDTH) * sdl->config->scale_factor;
        rect.y = (i / WIDTH) * sdl->config->scale_factor;

        if (chip->display[i]) 
        {
            // if pixel is on draw fg colour
            SDL_SetRenderDrawColor(sdl->renderer, 0, 0xFF, 0, 0);
            SDL_RenderFillRect(sdl->renderer, &rect);
        } 
        else 
        {
            // if pixel is off draw bg colour
            SDL_SetRenderDrawColor(sdl->renderer, 0x00, 0xFF, 0x0, 0x0);
            SDL_RenderFillRect(sdl->renderer, &rect);
        }
    }
    
    
    SDL_RenderPresent(sdl->renderer);
    
}

void close_sdl(sdl_t *sdl)
{
    //Destroy window
    SDL_DestroyWindow( sdl->window );

    //Quit SDL subsystems
    SDL_Quit();
}

chip8_t* init_chip(char* rom_name)
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

    // load rom
    FILE *rom = fopen(rom_name, "rb");
    if (!rom) {
        SDL_Log("Failed to load rom file %s\n", rom_name);
        return false;
    }

    // get rom size
    fseek(rom, 0, SEEK_END);
    const size_t rom_size = ftell(rom);
    const size_t max_size = sizeof chip->memory - entry_point;
    rewind(rom);

    if (rom_size > max_size) {
        SDL_Log("Rom file %s is too big! Rom size: %zu, max allowed: %zu.\n", rom_name, rom_size, max_size);
        return false;
    }

    if (fread(&chip->memory[entry_point], rom_size, 1, rom) != 1) {
        SDL_Log("Failed to load rom file into vm's ram\n");
        return false;
    };

    fclose(rom);

    // defaults
    chip->instruction = calloc(1, sizeof(instruction_t));
    chip->pc        = entry_point;
    chip->rom_name  = rom_name;
    chip->stack_ptr = &chip->stack[0];
    return chip;
}



int main( int argc, char* args[] )
{
    // configure
    config_t* config = calloc(1, sizeof(config_t));
    config->scale_factor = 5;
    
    // initialize the window
    sdl_t* sdl = init_sdl(config);
    
    // init ship and load rom
    chip8_t* chip = init_chip("roms/IBM Logo.ch8");
    
    SDL_Event e; 
    bool quit = false;

    while( quit == false )
    { 
        while( SDL_PollEvent( &e ) )
        { 
            if( e.type == SDL_QUIT ) 
            {
                quit = true; 
            }
        }
        // run next instruction
        // run_instruction(chip);
        // draw screen
        
        memset(&chip->display[0], true, sizeof(chip->display));
        draw(sdl, chip);
    }
    return 0;
}



