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

#include "address_hash_table.h"

#include "fnv1a.h"
#include "memory.h"
#include "table.h"

/// Used to mark a bucket that has stored an entry that has been removed
#define ADDRESS_ENTRY_TOMBSTONE (address_hash_table_entry_t *)(0xFFFFFFFFFFFFFFFFUL)

static int address_table_grow_table(address_hash_table_t *);

address_hash_table_t * address_table_new() {
    address_hash_table_t * table = new (address_hash_table_t);
    if (!table) {
        return NULL;
    }
    address_table_init_table(table);
    return table;
}

address_hash_table_entry_t * address_table_entry_new(uint16_t address, char const * key) {
    address_hash_table_entry_t * entry = new (address_hash_table_entry_t);
    if (!entry) {
        return NULL;
    }
    entry->opcodeAddress = address;
    entry->key = key;
    return entry;
}

void address_table_destory(address_hash_table_t ** table) {
    if (!*table) {
        return;
    }
    address_table_free_entries(*table);
    free(*table);
    *table = NULL;
}

int address_table_init_table(address_hash_table_t * table) {
    if (!table) {
        return -1;
    }
    table->used = 0;
    table->allocated = TABLE_INIT_SIZE;
    table->entries = malloc(table->allocated * sizeof(address_hash_table_entry_t));
    if (!table->entries) {
        return -1;
    }
    for (size_t i = 0; i < table->allocated; i++) {
        table->entries[i] = NULL;
    }
    return 0;
}

void address_table_free_entries(address_hash_table_t * table) {
    if (!table->entries) {
        return;
    }
    for (size_t i = 0; i < table->allocated; i++) {
        if (!table->entries[i] || table->entries[i] == ADDRESS_ENTRY_TOMBSTONE) {
            break;
        }
        if (table->entries[i]->key) {
            free((char *)table->entries[i]->key);
        }
    }
    free(table->entries);
    table->allocated = table->used = 0;
    table->entries = NULL;
}

int address_table_insert_entry(address_hash_table_entry_t * node, address_hash_table_t * table) {
    if (!node || !table) {
        return -1;
    }
    if (table->used >= ((double)table->allocated) * TABLE_GROWTH_TRIGGER_VALUE) {
        if (address_table_grow_table(table)) {
            return -1;
        }
    }
    uint32_t index, try;
    index = fnv1a_hash_data((uint8_t *)node->key, strlen(node->key));
    for (size_t i = 0; i < table->allocated; i++) {
        // When we reach the end of the hashTable we continue from the beginning
        try = (i + index) & (table->allocated - 1);
        if (!table->entries[try] || table->entries[try] == ADDRESS_ENTRY_TOMBSTONE) {
            table->entries[try] = node;
            table->used++;
            return 0;
        }
    }
    return -1;
}

address_hash_table_entry_t * address_table_remove_entry(address_hash_table_entry_t * node,
                                                        address_hash_table_t * table) {
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
            address_hash_table_entry_t * tempNode = table->entries[try];
            table->entries[try] = ADDRESS_ENTRY_TOMBSTONE;
            table->used--;
            return tempNode;
        }
    }
    return NULL;
}

address_hash_table_entry_t * address_table_look_up_entry(char const * key, address_hash_table_t * table) {
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
static int address_table_grow_table(address_hash_table_t * table) {
    address_hash_table_entry_t ** newEntries =
        CHECKED_MALLOC_USING_TYPE(table->allocated * GROWTH_FACTOR, *table->entries);
    if (!newEntries) {
        return -1;
    }
    for (size_t i = 0; i < table->allocated * GROWTH_FACTOR; i++) {
        newEntries[i] = NULL;
    }
    address_hash_table_entry_t ** oldEntries = table->entries;
    table->entries = newEntries;
    table->used = 0;
    table->allocated *= GROWTH_FACTOR;
    for (size_t j = 0; j < table->allocated / GROWTH_FACTOR; j++) {
        address_table_insert_entry(oldEntries[j], table);
    }
    free(oldEntries);
    return 0;
}