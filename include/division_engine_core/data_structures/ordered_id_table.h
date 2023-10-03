#pragma once

#include "division_engine_core_export.h"

#include "unordered_id_table.h"
#include <stddef.h>
#include <stdint.h>

typedef struct DivisionOrderedIdTable
{
    DivisionUnorderedIdTable unordered_id_table;
    uint32_t* orders;
    size_t orders_count;
    size_t orders_capacity;
} DivisionOrderedIdTable;

#ifdef __cplusplus
extern "C"
{
#endif

    DIVISION_EXPORT void division_ordered_id_table_alloc(
        DivisionOrderedIdTable* id_table, size_t capacity
    );
    DIVISION_EXPORT void division_ordered_id_table_free(DivisionOrderedIdTable* id_table);

    DIVISION_EXPORT bool division_ordered_id_table_contains(
        const DivisionOrderedIdTable* id_table, uint32_t id
    );

    DIVISION_EXPORT uint32_t
    division_ordered_id_table_new_id(DivisionOrderedIdTable* id_table);
    DIVISION_EXPORT void division_ordered_id_table_remove_id(
        DivisionOrderedIdTable* id_table, uint32_t id
    );
    DIVISION_EXPORT bool division_ordered_id_table_find_id_order(
        DivisionOrderedIdTable* id_table, uint32_t id, uint32_t* out_order_index
    );

#ifdef __cplusplus
}
#endif