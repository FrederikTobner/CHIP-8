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
 * @file address_hash_table.c
 * @brief Definitions regarding the hashtable that stored memory adresses under their label
 */

#include "addresses_hash_table.h"

#include "pre_compiled_header.h"

#include "fnv1a.h"
#include "table.h"

/// Used to mark a bucket that has stored an entry that has been removed
#define TOMBSTONE                          (addresses_hash_table_entry_t *)(0xFFFFFFFFFFFFFFFFUL)

static int address_table_grow_table(addresses_hash_table_t *);

addresses_hash_table_t * addresses_table_new() {
    addresses_hash_table_t * table = new(addresses_hash_table_t);
    if (!table) {
        return NULL;
    }
    address_table_init_table(table);
    return table;
}

void addresses_table_destory(addresses_hash_table_t ** table) {
    if (!*table) {
        return;
    }
    address_table_free_entries(*table);
    free(*table);
    *table = NULL;
}

int addresses_table_init_table(addresses_hash_table_t * table) {
    if (!table) {
        return -1;
    }
    table->used = 0;
    table->allocated = TABLE_INIT_SIZE;
    table->entries = malloc(table->allocated * sizeof(addresses_hash_table_entry_t));
    if (!table->entries) {
        return -1;
    }
    for (size_t i = 0; i < table->allocated; i++) {
        table->entries[i] = NULL;
    }
    return 0;
}

void addresses_table_free_entries(addresses_hash_table_t * table) {
    if (!table->entries) {
        return;
    }
    for (size_t i = 0; i < table->allocated; i++) {
        if (table->entries[i]->key) {
            free((char *)table->entries[i]->key);
        }
    }
    free(table->entries);
    table->allocated = table->used = 0;
    table->entries = NULL;
}

int addresses_table_add(uint16_t address, char const * label, addresses_hash_table_t * table) {
    // Lookup label
    addresses_hash_table_entry_t * entry;
    if (entry = addresses_table_look_up_entry(label, table)) {
        dynamic_address_array_write(entry->array, address);
    }
    if (!label || !table) {
        return -1;
    }
    if (table->used >= ((double)table->allocated) * TABLE_GROWTH_TRIGGER_VALUE) {
        if (addresses_table_grow_table(table)) {
            return -1;
        }
    }
    uint32_t index, try;
    index = fnv1a_hash_data((uint8_t *)label, strlen(label));
    for (size_t i = 0; i < table->allocated; i++) {
        // When we reach the end of the hashTable we continue from the beginning
        try = (i + index) & (table->allocated - 1);
        if ((!table->entries[try] || table->entries[try] == TOMBSTONE)) {
            entry = new(addresses_hash_table_entry_t);
            dynamic_address_array_init(table->entries[try]->array);
            dynamic_address_array_write(table->entries[try]->array, address);
            table->used++;
            return 0;
        }
    }
    return -1;
}

addresses_hash_table_entry_t * addresses_table_remove_entry(addresses_hash_table_entry_t * node,
                                                        addresses_hash_table_t * table) {
    if (!table) {
        return NULL;
    }
    uint32_t index = fnv1a_hash_data((uint8_t *)node->key, strlen(node->key));
    for (uint32_t i = 0; i < table->allocated; i++) {
        uint32_t try = (i + index) & (table->allocated - 1);
        // When we reach the end of the hashTable we continue from the beginning
        if (!table->entries[try]) {
            return false;
        }
        if (!strncmp(table->entries[try]->key, node->key, MAX_KEY_LENGTH)) {
            addresses_hash_table_entry_t * tempNode = table->entries[try];
            table->entries[try] = TOMBSTONE;
            table->used--;
            return tempNode;
        }
    }
    return NULL;
}

addresses_hash_table_entry_t * addresses_table_look_up_entry(char const * key, addresses_hash_table_t * table) {
    if (!table) {
        return NULL;
    }
    uint32_t index = fnv1a_hash_data((uint8_t *)key, strlen(key));
    for (uint32_t i = 0; i < table->allocated; i++) {
        uint32_t try = (i + index) & (table->allocated - 1);
        if (!table->entries[try]) {
            return NULL;
        }
        if (!strncmp(table->entries[try]->key, key, MAX_KEY_LENGTH)) {
            return table->entries[try]; // Entry found
        }
    }
    return NULL;
}

/// @brief Grows the size of a linear probing hashtable
/// @param table The table that is grown
/// @return 0 if successfull, -1 if not
static int addresses_table_grow_table(addresses_hash_table_t * table) {
    addresses_hash_table_entry_t ** newEntries =
        CHECKED_MALLOC_USING_TYPE(table->allocated * GROWTH_FACTOR, *table->entries);
    if (!newEntries) {
        return -1;
    }
    for (size_t i = 0; i < table->allocated * GROWTH_FACTOR; i++) {
        newEntries[i] = NULL;
    }
    addresses_hash_table_entry_t ** oldEntries = table->entries;
    table->entries = newEntries;
    table->used = 0;
    table->allocated *= GROWTH_FACTOR;
    for (size_t j = 0; j < table->allocated / GROWTH_FACTOR; j++) {
        addresses_table_insert_entry(oldEntries[j], table);
    }
    free(oldEntries);
    return 0;
}