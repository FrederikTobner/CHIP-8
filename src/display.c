#include "display.h"

#include "pre_compiled_header.h"

#define SCALE_FACTOR (10)

void display_render(chip8_t chip8, SDL_Renderer * renderer)
{
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
    for (size_t i = 0; i < GRAPHICS_SYSTEM_WIDTH; i++)
    {
        for (size_t j = 0; j < GRAPHICS_SYSTEM_HEIGHT; j++)
        {
            // Render black filled quad (Currently the display uses 10x10 pixels to simulate a single pixel of the graphics system)
            SDL_Rect fillRect = {i * SCALE_FACTOR, j * SCALE_FACTOR, (i + 1) * SCALE_FACTOR, (j + 1) * SCALE_FACTOR};
            if (chip8.graphicsSystem[i][j])
                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF); // black color
            else
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); // white color
            SDL_RenderFillRect(renderer, &fillRect);
        }
    }
    // Update screen
    SDL_RenderPresent(renderer);
}