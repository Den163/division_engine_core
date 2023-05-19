#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "division_engine_core_export.h"

const uint32_t DIVISION_HASH_TABLE_EMPTY_BUCKET_HASH = UINT32_MAX;
const uint32_t DIVISION_HASH_TABLE_DELETED_BUCKET_HASH = DIVISION_HASH_TABLE_EMPTY_BUCKET_HASH - 1;
const float DIVISION_HASH_TABLE_STD_LOAD_FACTOR_LIMIT = 0.9f;

/*
 * A hash table with linear open addressing collision resolving
 */
typedef struct DivisionHashTable
{
    uint32_t* buckets;
    size_t buckets_size;
    size_t buckets_capacity;
    float load_factor_limit;
} DivisionHashTable;

#ifdef __cplusplus
extern "C" {
#endif

DIVISION_EXPORT void division_hash_table_alloc(DivisionHashTable* table, size_t capacity);
DIVISION_EXPORT void division_hash_table_free(DivisionHashTable* table);

DIVISION_EXPORT bool division_hash_table_find(DivisionHashTable* table, uint32_t hash, size_t* out_bucket_index);
DIVISION_EXPORT bool division_hash_table_insert(DivisionHashTable* table, uint32_t hash, size_t* out_bucket_index);
DIVISION_EXPORT void division_hash_table_remove(DivisionHashTable* table, uint32_t hash);
DIVISION_EXPORT void division_hash_table_increase_capacity(DivisionHashTable* table, size_t new_capacity);

#ifdef __cplusplus
}
#endif