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
 * @file dynamic_address_array.h
 * @brief Declarations regarding the dynamic address array
 */

#ifndef CHIP8_DYNAMIC_ADDRESS_ARRAY_H_
#define CHIP8_DYNAMIC_ADDRESS_ARRAY_H_

#include "frontend_pre_compiled_header.h"

#include "../../base/src/dynamic_array_code_gen.h"

GENERATE_DYNAMIC_ARRAY_TYPE(uint16_t)

GENERATE_DYNAMIC_ARRAY_PROTOTYPES(uint16_t)

#endif