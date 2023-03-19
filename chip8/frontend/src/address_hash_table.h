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
 * @file address_hash_table.h
 * @brief Declarations regarding the hashtable that stored memory adresses under their label
 */

#ifndef CHIP8_ADDRESS_HASH_TABLE_H_
#define CHIP8_ADDRESS_HASH_TABLE_H_

#include "frontend_pre_compiled_header.h"

#include "../../base/src/table_code_gen.h"

// Type Definition of an Entry structure in a hashtable (linear probing)
GENERATE_TABLE_ENTRY_TYPE(uint16_t);

GENERATE_TABLE_TYPE(uint16_t);

GENERATE_TABLE_PROTOTYPES(uint16_t);

#endif