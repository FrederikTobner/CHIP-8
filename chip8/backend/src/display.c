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
#include "../../base/src/logger.h"
#include "../../io/src/path_utils.h"
#include "backend_pre_compiled_header.h"

static int display_set_window_icon(SDL_Window *, char const *);

void display_render(display_t * display) {
    // The color of the pixels of the texture (ARGB) - the display is black on white
    uint32_t pixels[GRAPHICS_SYSTEM_HEIGHT][GRAPHICS_SYSTEM_WIDTH];
    for (size_t y = 0; y < GRAPHICS_SYSTEM_HEIGHT; y++) {
        for (size_t x = 0; x < GRAPHICS_SYSTEM_WIDTH; x++) {
            pixels[y][x] = display->graphicsSystem[x][y] ? 0xFF000000u : 0xFFFFFFFFu;
        }
    }
    SDL_UpdateTexture(display->texture, NULL, pixels, sizeof(pixels[0]));
    SDL_RenderClear(display->renderer);
    // The texture is scaled to the size of the window
    SDL_RenderCopy(display->renderer, display->texture, NULL, NULL);
    SDL_RenderPresent(display->renderer);
    display->dirty = false;
}

int display_init(display_t * display) {
    if (SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return -1;
    } else {
        log_debug("SDL initialized");
        // Set texture filtering to nearest neighbour, so the pixels of the emulator stay sharp when they are scaled
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0")) {
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
            display->renderer = SDL_CreateRenderer(display->window, -1, 0);
            if (!display->renderer) {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                return -1;
            }
            display->texture =
                SDL_CreateTexture(display->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                  GRAPHICS_SYSTEM_WIDTH, GRAPHICS_SYSTEM_HEIGHT);
            if (!display->texture) {
                printf("Texture could not be created! SDL Error: %s\n", SDL_GetError());
                return -1;
            }
            // The empty screen is shown right away
            display->dirty = true;
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
    // Destroy texture, window and renderer
    SDL_DestroyTexture(display->texture);
    SDL_DestroyRenderer(display->renderer);
    SDL_DestroyWindow(display->window);
    display->window = NULL;
    display->renderer = NULL;
    display->texture = NULL;
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
    path_utils_concatenate_folder(iconPathBuffer, "assets", 240);
    strcat(iconPathBuffer, iconName);
    // Creating a SDL scurface from the bitmap
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
