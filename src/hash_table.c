#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

#include "division_engine_core/data_structures/hash_table.h"

#include <assert.h>
#include <stdlib.h>

#define DIVISION_MAP_HASH_TO_IDX(hash, size) (hash % (size))

void division_hash_table_alloc(DivisionHashTable* table, size_t capacity)
{
    table->buckets = malloc(sizeof(uint32_t[capacity]));
    for (int i = 0; i < capacity; i++)
    {
        table->buckets[i] = DIVISION_HASH_TABLE_EMPTY_BUCKET_HASH;
    }
    table->buckets_capacity = capacity;
    table->buckets_size = 0;
    table->load_factor_limit = DIVISION_HASH_TABLE_STD_LOAD_FACTOR_LIMIT;
}

void division_hash_table_free(DivisionHashTable* table)
{
    free(table->buckets);
    table->buckets = NULL;
    table->buckets_capacity = table->buckets_size = 0;
    table->load_factor_limit = 0;
}

bool division_hash_table_find(DivisionHashTable* table, uint32_t hash, size_t* out_bucket_index)
{
    size_t buckets_capacity = table->buckets_capacity;
    size_t mapped_hash = DIVISION_MAP_HASH_TO_IDX(hash, buckets_capacity);

#define DIVISION_CHECK_HASH_IN_ITERATION__(i) \
    uint32_t value = table->buckets[i]; \
    bool value_eq_hash = value == hash;       \
    bool value_eq_empty = value == DIVISION_HASH_TABLE_EMPTY_BUCKET_HASH; \
    if (value_eq_hash | value_eq_empty)       \
    {                                         \
        *out_bucket_index = i;                \
        return value_eq_hash;                 \
    }

    for (size_t i = mapped_hash; i < buckets_capacity; i++)
    {
        DIVISION_CHECK_HASH_IN_ITERATION__(i)
    }

    for (size_t i = 0; i < mapped_hash; i++)
    {
        DIVISION_CHECK_HASH_IN_ITERATION__(i)
    }

    return false;
}

bool division_hash_table_insert(DivisionHashTable* table, uint32_t hash, size_t* out_bucket_index)
{
    float load_factor = (float) table->buckets_size / (float) table->buckets_capacity;
    if (load_factor >= table->load_factor_limit)
    {
        division_hash_table_increase_capacity(table, table->buckets_capacity * 2);
    }

    size_t buckets_capacity = table->buckets_capacity;
    size_t mapped_hash = DIVISION_MAP_HASH_TO_IDX(hash, buckets_capacity);

#define DIVISION_CHECK_INSERTION_IN_ITERATION__(i) \
    uint32_t value = table->buckets[i];           \
    if ((value == DIVISION_HASH_TABLE_EMPTY_BUCKET_HASH) | (value == DIVISION_HASH_TABLE_DELETED_BUCKET_HASH)) \
    {                                              \
        table->buckets[i] = hash;                  \
        table->buckets_size++;                     \
        *out_bucket_index = i;                     \
        return true;                               \
    }

    for (size_t i = mapped_hash; i < buckets_capacity; i++)
    {
        DIVISION_CHECK_INSERTION_IN_ITERATION__(i)
    }

    for (size_t i = 0; i < mapped_hash; i++)
    {
        DIVISION_CHECK_INSERTION_IN_ITERATION__(i)
    }

    return false;
}

void division_hash_table_remove(DivisionHashTable* table, uint32_t hash)
{
    size_t buckets_capacity = table->buckets_capacity;
    size_t mapped_hash = DIVISION_MAP_HASH_TO_IDX(hash, buckets_capacity);

#define DIVISION_CHECK_REMOVE_IN_ITERATION__(i)  \
    uint32_t value = table->buckets[i];         \
    bool value_eq_hash = value == hash;         \
    bool value_eq_empty = value == DIVISION_HASH_TABLE_EMPTY_BUCKET_HASH; \
    if (value_eq_hash | value_eq_empty)         \
    {                                           \
        table->buckets[i] = DIVISION_HASH_TABLE_DELETED_BUCKET_HASH * value_eq_hash + value * value_eq_empty; \
        table->buckets_size = (table->buckets_size - 1) * value_eq_hash + table->buckets_size * value_eq_empty; \
        return;                                 \
    }

    for (size_t i = mapped_hash; i < buckets_capacity; i++)
    {
        DIVISION_CHECK_REMOVE_IN_ITERATION__(i)
    }

    for (size_t i = 0; i < mapped_hash; i++)
    {
        DIVISION_CHECK_REMOVE_IN_ITERATION__(i)
    }
}

void division_hash_table_increase_capacity(DivisionHashTable* table, size_t new_capacity)
{
    if (new_capacity <= table->buckets_capacity) return;

    uint32_t* old_buckets = table->buckets;
    uint32_t old_buckets_capacity = table->buckets_capacity;
    size_t old_buckets_size = table->buckets_size;

    table->buckets = malloc(sizeof(uint32_t[new_capacity]));
    for (int i = 0; i < new_capacity; i++)
    {
        table->buckets[i] = DIVISION_HASH_TABLE_EMPTY_BUCKET_HASH;
    }
    table->buckets_capacity = new_capacity;
    table->buckets_size = 0;

    size_t out_ignore_idx;
    size_t bucket_counter = 0;
    for (int i = 0; (i < old_buckets_capacity) & (bucket_counter < old_buckets_size); i++)
    {
        uint32_t value = old_buckets[i];
        if ((value != DIVISION_HASH_TABLE_EMPTY_BUCKET_HASH) & (value != DIVISION_HASH_TABLE_DELETED_BUCKET_HASH))
        {
            division_hash_table_insert(table, value, &out_ignore_idx);
            bucket_counter++;
        }
    }

    free(old_buckets);
    assert(bucket_counter == old_buckets_size);
}
#pragma clang diagnostic pop