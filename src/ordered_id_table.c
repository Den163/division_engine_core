#include "division_engine_core/data_structures/ordered_id_table.h"

#include <assert.h>
#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

void division_ordered_id_table_alloc(DivisionOrderedIdTable* id_table, size_t capacity)
{
    division_unordered_id_table_alloc(&id_table->unordered_id_table, capacity);
    id_table->orders = malloc(sizeof(uint32_t[capacity]));
    id_table->orders_count = 0;
    id_table->orders_capacity = capacity;
}

void division_ordered_id_table_free(DivisionOrderedIdTable* id_table)
{
    free(id_table->orders);
    division_unordered_id_table_free(&id_table->unordered_id_table);

    id_table->orders = NULL;
    id_table->orders_count = id_table->orders_capacity = 0;
}

uint32_t division_ordered_id_table_new_id(DivisionOrderedIdTable* id_table)
{
    uint32_t id = division_unordered_id_table_new_id(&id_table->unordered_id_table);
    if (id_table->orders_count == id_table->orders_capacity)
    {
        size_t new_capacity = id_table->orders_capacity * 2;
        id_table->orders = realloc(id_table->orders, sizeof(uint32_t[new_capacity]));
        id_table->orders_capacity = new_capacity;
    }

    uint32_t new_index = id_table->orders_count;
    id_table->orders[new_index] = id;

    id_table->orders_count++;

    return id;
}

void division_ordered_id_table_remove_id(DivisionOrderedIdTable* id_table, uint32_t id)
{
    uint32_t order_idx;
    if (!division_ordered_id_table_find_id_order(id_table, id, &order_idx))
    {
        return;
    }

    division_unordered_id_table_remove_id(&id_table->unordered_id_table, id);
    uint32_t* order_ptr = id_table->orders + order_idx;
    memmove(order_ptr, order_ptr + 1, id_table->orders_count - 1 - order_idx);

    id_table->orders_count--;
}

bool division_ordered_id_table_find_id_order(
    DivisionOrderedIdTable* id_table, uint32_t id, uint32_t* out_order_index)
{
    for (int i = 0; i < id_table->orders_count; i++)
    {
        uint32_t order_id = id_table->orders[i];
        if (order_id != id) continue;

        *out_order_index = i;
        return true;
    }

    return false;
}

bool division_ordered_id_table_contains(
    const DivisionOrderedIdTable* id_table, uint32_t id
)
{
    return division_unordered_id_table_contains(&id_table->unordered_id_table, id);
}
