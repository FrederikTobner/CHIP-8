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

#include "pre_compiled_header.h"

/// @brief A dynamic value array
/// @details A size of a dynamic array can varry, meaning it can shrink or grow.
typedef struct {
    /// @brief The capacity of the dynamic value array
    uint32_t capacity;
    /// @brief The amount of values stored in the array
    uint32_t count;
    /// @brief A pointer to the beginning of the values stored in the array
    uint16_t * addresses;
} dynamic_address_array_t;

/// @brief Dealocates a dynamic array
/// @param array The array that is freed
void dynamic_address_array_free(dynamic_address_array_t * array);

/// @brief Initializes a dynamic array to the size zero
/// @param array The array that is inititialized
void dynamic_address_array_init(dynamic_address_array_t * array);

void dynamic_address_array_remove(dynamic_address_array_t * array, size_t index);

/// @brief Adds a value to the dynamic array
/// @param array The array where the value is added
/// @param value The value that is added to the array
void dynamic_address_array_write(dynamic_address_array_t * array, uint16_t value);

#endif