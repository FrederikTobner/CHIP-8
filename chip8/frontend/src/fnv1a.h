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
 * @file fnv1a.h
 * @brief Declarations regarding the fowler-noll-vo hashing function
 */

#ifndef CHIP8_FNV1A_H_
#define CHIP8_FNV1A_H_

// This file is included in the test-suite that is written in c++ using the google-test framework
#ifdef __cplusplus
extern "C" {
#endif

#include "frontend_pre_compiled_header.h"

// Fowler-Noll-Vo hashing function https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function

/// @brief Hashes the data using fowler noll vo
/// @param data The data that is hashed
/// @param length The length of the data that is hashed
/// @return an unsigned 32-bit
uint32_t fnv1a_hash_data(uint8_t const * data, size_t length);

#ifdef __cplusplus
}
#endif

#endif