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
 * @file memory.h
 * @brief Macros used for memory allocation of the project
 */

#ifndef CHIP8_MEMORY_H
#define CHIP8_MEMORY_H

#include <stdlib.h>

/// Size check using a type (used for the checked malloc)
#define SIZE_CHECK_USING_TYPE(n, type)     ((SIZE_MAX / sizeof(type)) >= (n))

/// @brief Checked malloc using the type
/// @details Treats a overflow parameters when malloc was called like a usual allocation error
#define CHECKED_MALLOC_USING_TYPE(n, type) (SIZE_CHECK_USING_TYPE((n), (type)) ? malloc((n) * sizeof(type)) : 0)

/// @brief Checked realloc using the size in byte
/// @details Treats a overflow parameters when malloc was called like a usual allocation error
#define CHECKED_REALLOC_USING_SIZE(p, n, obj_size) \
    SIZE_CHECK_USING_SIZE((n), (obj_size)) ? realloc((p), (n) * (obj_size)) : 0

/// @brief Checked realloc using the type
/// @details Treats a overflow parameters when malloc was called like a usual allocation error
#define CHECKED_REALLOC_USING_TYPE(p, n, type) \
    (SIZE_CHECK_USING_TYPE((n), (type)) ? realloc((p), (n) * sizeof(type)) : 0)

/// @brief Size check using the size in byte
/// @details Treats a overflow parameters when malloc was called like a usual allocation error
#define SIZE_CHECK_USING_SIZE(n, obj_size)     ((SIZE_MAX / (obj_size)) >= (n))

/// @brief Checked malloc using the size in byte
/// @details Treats a overflow parameters when malloc was called like a usual allocation error
#define CHECKED_MALLOC_USING_SIZE(n, obj_size) (SIZE_CHECK_USING_SIZE((n), (obj_size)) ? malloc((n) * (obj_size)) : 0)

/// Allocates the memory for a single value of the specified type on the heap
#define new(type)                              (malloc(sizeof(type)))

#endif