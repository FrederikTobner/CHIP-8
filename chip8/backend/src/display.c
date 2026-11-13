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

#include "backend/display.h"
#include "backend_pre_compiled_header.h"
#include "base/logger.h"
#include "window_icon.h"

static void display_set_window_icon(SDL_Window *);

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
        fprintf(stderr, "SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return -1;
    } else {
        log_debug("SDL initialized");
        // Set texture filtering to nearest neighbour, so the pixels of the emulator stay sharp when they are scaled
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0")) {
            fprintf(stderr, "Warning: Linear texture filtering not enabled!\n");
        }
        // Create window
        display->window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                           GRAPHICS_SYSTEM_WIDTH * SCALE_FACTOR, GRAPHICS_SYSTEM_HEIGHT * SCALE_FACTOR,
                                           SDL_WINDOW_SHOWN);
        if (!display->window) {
            fprintf(stderr, "Window could not be created! SDL Error: %s\n", SDL_GetError());
            return -1;
        } else {
            // Create renderer for window
            display->renderer = SDL_CreateRenderer(display->window, -1, 0);
            if (!display->renderer) {
                fprintf(stderr, "Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                return -1;
            }
            display->texture =
                SDL_CreateTexture(display->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                  GRAPHICS_SYSTEM_WIDTH, GRAPHICS_SYSTEM_HEIGHT);
            if (!display->texture) {
                fprintf(stderr, "Texture could not be created! SDL Error: %s\n", SDL_GetError());
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
            // The icon is only cosmetic - a window without it is still fine
            display_set_window_icon(display->window);
            return 0;
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

/// @brief Sets the icon of a window to the icon that is embedded in the library
/// @details Failing to set the icon is not fatal - a warning is printed and the window keeps its default icon
/// @param window The window where the icon is set
static void display_set_window_icon(SDL_Window * window) {
    // The icon is a bitmap that is stored in the memory of the program (see window_icon.h)
    SDL_RWops * source = SDL_RWFromConstMem(WINDOW_ICON_DATA, (int)sizeof(WINDOW_ICON_DATA));
    // The source is closed by SDL_LoadBMP_RW (even if loading the bitmap fails)
    SDL_Surface * icon = source ? SDL_LoadBMP_RW(source, 1) : NULL;
    if (!icon) {
        fprintf(stderr, "Warning: Failed to load the window icon: %s\n", SDL_GetError());
        return;
    }
    SDL_SetWindowIcon(window, icon);
    SDL_FreeSurface(icon);
}
