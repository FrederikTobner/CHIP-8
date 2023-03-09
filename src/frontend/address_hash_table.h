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

#include "../pre_compiled_header.h"

// Type Definition of an Entry structure in a hashtable (linear probing)
typedef struct
{
    /// Key of the entry 🔑 / The label
    char const * key;
    /// Value that is stored in the entry
    uint16_t opcodeAddress;
} address_hash_table_entry_t;

// Type definition of a HashTable structure using linear probing for hash collisions
typedef struct
{
    /// The amount or allocated entries
    size_t allocated;
    /// The amount of used entries
    size_t used;    
    /// Pointer to the entries
    address_hash_table_entry_t ** entries;
} address_hash_table_t;

address_hash_table_entry_t * address_table_entry_new(uint16_t opcodeAddress, char const * key);

/// @brief Initializes a new hashtable
/// @param table The hashtable that is initialized
/// @return 0 if it could be initialized, -1 if not
int address_table_init_table(address_hash_table_t * table);

/// @brief Creates and initializes a new hashtable
/// @return The hashtable that was created
address_hash_table_t * address_table_new();

/// @brief Frees the memory occupied by a hashtable (does not free the memory occupied by the entries)
/// @param table The table that is freed
void address_table_destory(address_hash_table_t ** table);

/// @brief  Frees the memory used by the hashtable
/// @param table The hashtable where the entries are freed
void address_table_free_entries(address_hash_table_t * table);

/// @brief Inserts a new entry into the hashtable
/// @param entry The entry that is inserted
/// @param table The table where the entry is inserted
/// @return 0 if the entry was added, -1 if the entry couldn't be added
int address_table_insert_entry(address_hash_table_entry_t * entry, address_hash_table_t * table);

/// @brief Removes a entry from the hashtable
/// @param entry The entry that is removed from the hashtable
/// @param table The table where the entry is removed
/// @return The entry that was removed or null if no entry was removed
address_hash_table_entry_t * address_table_remove_entry(address_hash_table_entry_t * entry, address_hash_table_t * table);

/// @brief Looks up a entry in the hashtable
/// @param key The key that is used to look up the entry
/// @param table The table where the entry is looked up
/// @return The entry that corresponds to the key or NULL if no entry corresponfing to the key was found
address_hash_table_entry_t * address_table_look_up_entry(char const * key, address_hash_table_t * table);

#endif