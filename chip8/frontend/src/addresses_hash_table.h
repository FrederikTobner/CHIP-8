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
 * @file addresses_hash_table.h
 * @brief Declarations regarding the hashtable that stores multiple memory adresses under their label
 */

#ifndef CHIP8_ADDRESSES_HASH_TABLE_H_
#define CHIP8_ADDRESSES_HASH_TABLE_H_

#include "dynamic_address_array.h"
#include "frontend_pre_compiled_header.h"

// Type Definition of an Entry structure in a hashtable (linear probing)
typedef struct {
    /// Key of the entry 🔑
    char const * key;
    /// Addresses that are stored in the entry
    dynamic_address_array_t * array;
} addresses_hash_table_entry_t;

// Type definition of a HashTable structure using linear probing for hash collisions
typedef struct {
    /// The amount or allocated entries
    size_t allocated;
    /// The amount of used entries
    size_t used;
    /// Pointer to the entries
    addresses_hash_table_entry_t ** entries;
} addresses_hash_table_t;

/// Used to mark a bucket that has stored an entry that has been removed
#define ADDRESSES_ENTRY_TOMBSTONE (addresses_hash_table_entry_t *)(0xFFFFFFFFFFFFFFFFUL)

/// @brief Initializes a new hashtable
/// @param table The hashtable that is initialized
/// @return 0 if it could be initialized, -1 if not
int addresses_table_init_table(addresses_hash_table_t * table);

/// @brief Creates and initializes a new hashtable
/// @return The hashtable that was created
addresses_hash_table_t * addresses_table_new();

/// @brief Creates a new addresses table entry
/// @param key The key of the entry
/// @param array Pointer to the array of addresses
/// @return The newly created entry
addresses_hash_table_entry_t * addresses_table_entry_new(char const * key, dynamic_address_array_t * array);

/// @brief Frees the memory occupied by a hashtable (does not free the memory occupied by the entries)
/// @param table The table that is freed
void addresses_table_destory(addresses_hash_table_t ** table);

/// @brief  Frees the memory used by the hashtable
/// @param table The hashtable where the entries are freed
void addresses_table_free_entries(addresses_hash_table_t * table);

/// @brief Adds the memory location to the table
/// @param address The address that is added to the table
/// @param key The label that is referenced
/// @param table The table where the address is added
/// @return 0 if the address was added, -1 if the address couldn't be added
int addresses_table_add(uint16_t address, char const * key, addresses_hash_table_t * table);

/// @brief Removes a entry from the hashtable
/// @param entry The entry that is removed from the hashtable
/// @param table The table where the entry is removed
/// @return The entry that was removed or null if no entry was removed
addresses_hash_table_entry_t * addresses_table_remove_entry(addresses_hash_table_entry_t * entry,
                                                            addresses_hash_table_t * table);

/// @brief Looks up a entry in the hashtable
/// @param key The key that is used to look up the entry
/// @param table The table where the entry is looked up
/// @return The entry that corresponds to the key or NULL if no entry corresponfing to the key was found
addresses_hash_table_entry_t * addresses_table_look_up_entry(char const * key, addresses_hash_table_t * table);

/// @brief Inserts a new entry into the hashtable
/// @param entry The entry that is inserted
/// @param table The table where the entry is inserted
/// @return 0 if the entry was added, -1 if the entry couldn't be added
int addresses_table_insert_entry(addresses_hash_table_entry_t * node, addresses_hash_table_t * table);

#endif
