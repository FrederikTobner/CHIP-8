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

#ifndef CHIP8_DISPLAY_H_
#define CHIP8_DISPLAY_H_

#include "../external/SDL/include/SDL.h"

/// The graphics system of the chip-8 has a height of 32 pixels
#define GRAPHICS_SYSTEM_HEIGHT (32)

/// The graphics system of the chip-8 has a width of 64 pixels
#define GRAPHICS_SYSTEM_WIDTH (64)

#define SCALE_FACTOR (10)

typedef struct {
    SDL_Window * window;
    SDL_Renderer * renderer;
    uint8_t graphicsSystem[64][32];
} display_t;

/// @brief Renders the display
/// @param display The display that is rendered
void display_render(display_t display);

/// @brief Initializes the display
/// @param window The window where the display of the emulator is emulated
/// @param renderer The renderer that is used to create image from
/// @return 0 if everything went well, -1 if an error occured
int display_init(display_t * display);

/// @brief Quits SDL
/// @param window The window that is closed
/// @param renderer The renderer used to render the display of the emulator
void display_quit(display_t * display);

#endif
