/****************************************************************************
 * Copyright (C) 2023 by Frederik Tobner                                    *
 *                                                                          *
 * This file is part of CHIP-8.                                             *
 *                                                                          *
 * Permission to use, copy, modify, and distribute this software and its    *
 * documentation under the terms of the GNU General Public License is       *
 * hereby granted.                                                          *
 * No representations are made about the suitability of this software for   *
 * any purpose.                                                             *
 * It is provided "as is" without express or implied warranty.              *
 * See the <https://www.gnu.org/licenses/gpl-3.0.html/>GNU General Public   *
 * License for more details.                                                *
 ****************************************************************************/

#include "display.h"

#include "pre_compiled_header.h"

void display_render(display_t display) {
    // Clear screen
    SDL_SetRenderDrawColor(display.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(display.renderer);
    for (size_t i = 0; i < GRAPHICS_SYSTEM_WIDTH; i++) {
        for (size_t j = 0; j < GRAPHICS_SYSTEM_HEIGHT; j++) {
            // Render black filled quad (Currently the display uses 10x10 pixels to simulate a single pixel of the
            // graphics system)
            SDL_Rect fillRect = {i * SCALE_FACTOR, j * SCALE_FACTOR, (i + 1) * SCALE_FACTOR, (j + 1) * SCALE_FACTOR};
            if (display.graphicsSystem[i][j])
                SDL_SetRenderDrawColor(display.renderer, 0x00, 0x00, 0x00, 0xFF); // black color
            else
                SDL_SetRenderDrawColor(display.renderer, 0xFF, 0xFF, 0xFF, 0xFF); // white color
            SDL_RenderFillRect(display.renderer, &fillRect);
        }
    }
    // Update screen
    SDL_RenderPresent(display.renderer);
}

int display_init(display_t * display) {
    for (size_t i = 0; i < GRAPHICS_SYSTEM_WIDTH; i++)
        for (size_t j = 0; j < GRAPHICS_SYSTEM_HEIGHT; j++)
            display->graphicsSystem[i][j] = 0u;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return -1;
    } else {
        // Set texture filtering to linear
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
            printf("Warning: Linear texture filtering not enabled!");

        // Create window
        display->window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, GRAPHICS_SYSTEM_WIDTH * SCALE_FACTOR, GRAPHICS_SYSTEM_HEIGHT * SCALE_FACTOR, SDL_WINDOW_SHOWN);
        if (display->window == NULL) {
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            return -1;
        } else {
            // Create renderer for window
            display->renderer = SDL_CreateRenderer(display->window, -1, SDL_RENDERER_ACCELERATED);
            if (display->renderer == NULL) {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                return -1;
            } else
                SDL_SetRenderDrawColor(display->renderer, 0xFF, 0xFF, 0xFF, 0xFF); // white
        }
    }
    return 0;
}

void display_quit(display_t * display) {
    // Destroy window
    SDL_DestroyRenderer(display->renderer);
    SDL_DestroyWindow(display->window);
    display->window = NULL;
    display->renderer = NULL;
    SDL_Quit();
}