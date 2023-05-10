#pragma once

#include "division_engine_core_export.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * Data structure, that creates new ids
 */
typedef struct DivisionUnorderedIdTable
{
    uint32_t max_id;
    uint32_t* free_ids;
    size_t free_ids_count;
    size_t free_ids_capacity;
} DivisionUnorderedIdTable;

#ifdef __cplusplus
extern "C" {
#endif

DIVISION_EXPORT void division_unordered_id_table_alloc(DivisionUnorderedIdTable* table, size_t capacity);
DIVISION_EXPORT void division_unordered_id_table_free(DivisionUnorderedIdTable* table);

DIVISION_EXPORT bool division_unordered_id_table_contains(const DivisionUnorderedIdTable* table, uint32_t id);

DIVISION_EXPORT uint32_t division_unordered_id_table_insert(DivisionUnorderedIdTable* table);
DIVISION_EXPORT void division_unordered_id_table_remove(DivisionUnorderedIdTable* table, uint32_t id);

#ifdef __cplusplus
}
#endif