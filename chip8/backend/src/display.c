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

/**
 * @file display.c
 * @brief Definitions regarding the display of the emulator
 */

#include "display.h"
#include "../../../build/chip8/chip8_config.h"
#include "pre_compiled_header.h"
#include "../../io/src/path_utils.h"

static int display_set_window_icon(SDL_Window *, char const *);

void display_render(display_t display) {
    // Setting renderer color
    SDL_SetRenderDrawColor(display.renderer, 0xFF, 0xFF, 0xFF, 0xFF); // white color
    uint8_t currentColor = 0xFF;
    SDL_RenderClear(display.renderer);
    for (size_t width = 0; width < GRAPHICS_SYSTEM_WIDTH; width++) {
        for (size_t height = 0; height < GRAPHICS_SYSTEM_HEIGHT; height++) {
            // Render black filled quad (Currently the display uses 20x20 pixels to simulate a single pixel of the
            // graphics system)
            SDL_Rect fillRect = {width * SCALE_FACTOR, height * SCALE_FACTOR, (width + 1) * SCALE_FACTOR,
                                 (height + 1) * SCALE_FACTOR};
            // Check if current color is white and pixel is set before changing renderer color
            if (currentColor && display.graphicsSystem[width][height]) {
                SDL_SetRenderDrawColor(display.renderer, 0x00, 0x00, 0x00, 0xFF); // black color
                currentColor = 0x00;
            }
            // Check if current color is black and pixel is not set before changing renderer color
            else if (!currentColor && !display.graphicsSystem[width][height]) {
                SDL_SetRenderDrawColor(display.renderer, 0xFF, 0xFF, 0xFF, 0xFF); // white color
                currentColor = 0xFF;
            }
            SDL_RenderFillRect(display.renderer, &fillRect);
        }
    }
    // Update screen
    SDL_RenderPresent(display.renderer);
    SDL_UpdateWindowSurface(display.window);
}

int display_init(display_t * display) {
    if (SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return -1;
    } else {
        // Set texture filtering to linear
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
            printf("Warning: Linear texture filtering not enabled!");
        }
        // Create window
        display->window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                           GRAPHICS_SYSTEM_WIDTH * SCALE_FACTOR, GRAPHICS_SYSTEM_HEIGHT * SCALE_FACTOR,
                                           SDL_WINDOW_SHOWN);
        if (!display->window) {
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            return -1;
        } else {
            // Create renderer for window
            display->renderer = SDL_CreateRenderer(display->window, -1, SDL_RENDERER_ACCELERATED);
            if (!display->renderer) {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                return -1;
            }
            SDL_SetRenderDrawColor(display->renderer, 0xFF, 0xFF, 0xFF, 0xFF); // white
            // Initialize graphics system
            for (size_t width = 0; width < GRAPHICS_SYSTEM_WIDTH; width++) {
                for (size_t height = 0; height < GRAPHICS_SYSTEM_HEIGHT; height++) {
                    display->graphicsSystem[width][height] = 0u;
                }
            }
            return display_set_window_icon(display->window, "chip8_window_icon.bmp");
        }
    }
    return 0;
}

void display_quit(display_t * display) {
    // Destroy window and renderer
    SDL_DestroyRenderer(display->renderer);
    SDL_DestroyWindow(display->window);
    display->window = NULL;
    display->renderer = NULL;
    SDL_Quit();
}

/// @brief Sets the icon of a window
/// @param window The window where the icon is set
/// @param iconPath Path to the icon
/// @return 0 if the icon was successfully appled to the window, -1 if an error occured
static int display_set_window_icon(SDL_Window * window, char const * iconName) {

    char iconPathBuffer[240];
    if (path_utils_get_executable_path(iconPathBuffer, 240)) {
        printf("Failed to determine path of the emulator executable\n");
        return -1;
    }
    path_utils_remove_file_layer(iconPathBuffer, 2);
    path_utils_concatenate_folder(iconPathBuffer, "icons", 240);
    strcat(iconPathBuffer, iconName);
    SDL_Surface * window_icon_scurface = SDL_LoadBMP(iconPathBuffer);
    if (!window_icon_scurface) {
        printf("Failed to load window icon %s\n", SDL_GetError());
        return -1;
    }
    // Setting the icon of the window
    SDL_SetWindowIcon(window, window_icon_scurface);
    SDL_FreeSurface(window_icon_scurface);
    return 0;
}
