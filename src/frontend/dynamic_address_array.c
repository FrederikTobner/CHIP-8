#include "dynamic_address_array.h"

#include "pre_compiled_header.h"

/// Growth factor of a dynamic value array
#define ARRAY_GROWTH_FACTOR                 (1.5)

/// Makro that dealocates an existing dynamic array
#define FREE_ARRAY(type, pointer, oldCount) (dynamic_address_array_reallocate(pointer, sizeof(type) * (oldCount), 0))

/// Makro that determines the increase in capacity for a dynamic array (initalizes capacity at 8)
#define GROW_CAPACITY(capacity)             ((capacity) < 8u ? 8u : (capacity)*ARRAY_GROWTH_FACTOR)

/// Makro that increases the size of a dynamic Array
#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    ((type *)dynamic_address_array_reallocate(pointer, sizeof(type) * (oldCount), sizeof(type) * (newCount)))

static void * dynamic_address_array_reallocate(void *, size_t, size_t);

void dynamic_address_array_free(dynamic_address_array_t * array) {
    FREE_ARRAY(uint16_t, array->addresses, array->capacity);
    dynamic_value_array_init(array);
}

void dynamic_address_array_init(dynamic_address_array_t * array) {
    array->addresses = NULL;
    array->count = array->capacity = 0u;
}

void dynamic_address_array_remove(dynamic_address_array_t * array, size_t index) {
    if (index >= array->count) {
        return;
    }
    memcpy(array->addresses + index, array->addresses + index + 1, array->count - (index + 1));
    array->count--;
}

void dynamic_address_array_write(dynamic_address_array_t * array, uint16_t value) {
    if (array->capacity < array->count + 1u) {
        uint32_t oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldCapacity);
        uint16_t * grownArray;
        grownArray = GROW_ARRAY(uint16_t, array->addresses, oldCapacity, array->capacity);
        array->addresses = grownArray;
    }
    array->addresses[array->count] = value;
    array->count++;
}


static void * dynamic_address_array_reallocate(void * pointer, size_t oldSize, size_t newSize) {
    if (!newSize) {
        free(pointer);
        return NULL;
    }

    void * result = realloc(pointer, newSize);
    if (!result) {
        fprintf(stderr, "Failed too reallocate memory");
        exit(70);
    }
    return result;
}