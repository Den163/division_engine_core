#pragma once

#include "unordered_id_table.h"
#include <stdint.h>
#include <stddef.h>

typedef struct DivisionOrderedIdTable
{
    DivisionUnorderedIdTable unordered_id_table;
    uint32_t* orders;
    uint32_t* id_to_order;
    size_t orders_count;
    size_t orders_capacity;
    size_t id_to_order_capacity;
} DivisionOrderedIdTable;

#ifdef __cplusplus
extern "C" {
#endif

void division_ordered_id_table_alloc(DivisionOrderedIdTable* id_table, size_t capacity);
void division_ordered_id_table_free(DivisionOrderedIdTable* id_table);

bool division_ordered_id_table_contains(const DivisionOrderedIdTable* id_table, uint32_t id);

uint32_t division_ordered_id_table_insert(DivisionOrderedIdTable* id_table);
void division_ordered_id_table_remove(DivisionOrderedIdTable* id_table, uint32_t id);

#ifdef __cplusplus
}
#endif