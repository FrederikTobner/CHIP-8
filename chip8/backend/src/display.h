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
 * @file display.h
 * @brief Declarations regarding the display of the emulator
 */

#ifndef CHIP8_DISPLAY_H_
#define CHIP8_DISPLAY_H_

// This file is included in the test-suite that is written in c++ using the google-test framework
#ifdef __cplusplus
extern "C" {
#endif

#include <SDL.h>
#include <stdbool.h>

/// The graphics system of the chip-8 has a height of 32 pixels
#define GRAPHICS_SYSTEM_HEIGHT (32)

/// The graphics system of the chip-8 has a width of 64 pixels
#define GRAPHICS_SYSTEM_WIDTH  (64)

/// The scale factor from the emulator display to the real display
#define SCALE_FACTOR           (20)

/// @brief Models the display of the emulator using SDL
typedef struct {
    /// The window where the display of the emulator is displayed
    SDL_Window * window;
    /// The renderer that is used to render the display of the emulator in the window
    SDL_Renderer * renderer;
    /// The texture that holds the pixels of the graphics system, it is scaled to the size of the window when rendered
    SDL_Texture * texture;
    /// Determines if the graphics system was changed since the display was rendered the last time
    bool dirty;
    /// The underlying graphics system of the CHIP-8
    uint8_t graphicsSystem[GRAPHICS_SYSTEM_WIDTH][GRAPHICS_SYSTEM_HEIGHT];
} display_t;

/// @brief Renders the display and marks it as up to date
/// @param display The display that is rendered
void display_render(display_t * display);

/// @brief Initializes the display
/// @param window The window where the display of the emulator is emulated
/// @param renderer The renderer that is used to create image from
/// @return 0 if everything went well, -1 if an error occured
int display_init(display_t * display);

/// @brief Quits SDL
/// @param window The window that is closed
/// @param renderer The renderer used to render the display of the emulator
void display_quit(display_t * display);

#ifdef __cplusplus
}
#endif

#endif
