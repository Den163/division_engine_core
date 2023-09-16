#include "division_engine_core/data_structures/ordered_id_table.h"

#include <memory.h>
#include <stdlib.h>

void division_ordered_id_table_alloc(DivisionOrderedIdTable* id_table, size_t capacity)
{
    division_unordered_id_table_alloc(&id_table->unordered_id_table, capacity);
    id_table->orders = malloc(sizeof(uint32_t[capacity]));
    id_table->id_to_order = malloc(sizeof(uint32_t[capacity]));
    id_table->orders_count = 0;
    id_table->orders_capacity = capacity;
    id_table->id_to_order_capacity = capacity;
}

void division_ordered_id_table_free(DivisionOrderedIdTable* id_table)
{
    free(id_table->id_to_order);
    free(id_table->orders);
    division_unordered_id_table_free(&id_table->unordered_id_table);

    id_table->orders = NULL;
    id_table->id_to_order = NULL;
    id_table->orders_count = id_table->orders_capacity = id_table->id_to_order_capacity =
        0;
}

uint32_t division_ordered_id_table_insert(DivisionOrderedIdTable* id_table)
{
    uint32_t id = division_unordered_id_table_insert(&id_table->unordered_id_table);
    if (id_table->orders_count == id_table->orders_capacity)
    {
        size_t new_capacity = id_table->orders_capacity * 2;
        id_table->orders = realloc(id_table->orders, sizeof(uint32_t[new_capacity]));
        id_table->orders_capacity = new_capacity;
    }

    if (id_table->unordered_id_table.max_id >= id_table->id_to_order_capacity)
    {
        size_t new_capacity = id_table->unordered_id_table.max_id * 2;
        id_table->id_to_order = realloc(id_table->orders, sizeof(uint32_t[new_capacity]));
        id_table->id_to_order_capacity = new_capacity;
    }

    uint32_t new_index = id_table->orders_count;
    id_table->orders[new_index] = id;
    id_table->id_to_order[id] = new_index;

    id_table->orders_count++;

    return id;
}

void division_ordered_id_table_remove(DivisionOrderedIdTable* id_table, uint32_t id)
{
    division_unordered_id_table_remove(&id_table->unordered_id_table, id);

    uint32_t order_idx = id_table->id_to_order[id];
    uint32_t* order_ptr = id_table->orders + order_idx;
    memmove(order_ptr, order_ptr + 1, id_table->orders_count - 1 - order_idx);

    id_table->orders_count--;
}

bool division_ordered_id_table_contains(
    const DivisionOrderedIdTable* id_table, uint32_t id
)
{
    return division_unordered_id_table_contains(&id_table->unordered_id_table, id);
}
