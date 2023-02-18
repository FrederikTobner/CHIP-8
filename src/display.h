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

#include "chip8.h"

#include "../external/SDL/include/SDL.h"

/// @brief Renders the display of the passed chip8 struct
/// @param chip8
/// @param renderer
void display_render(chip8_t chip8, SDL_Renderer * renderer);

#endif
