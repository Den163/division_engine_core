#include "division_engine_core/data_structures/unordered_id_table.h"

#include <assert.h>
#include <memory.h>
#include <stdlib.h>

void division_unordered_id_table_alloc(DivisionUnorderedIdTable *table, size_t capacity)
{
    assert(capacity > 0);

    table->max_id = capacity - 1;
    table->free_ids = malloc(sizeof(uint32_t[capacity]));
    table->free_ids_count = capacity;
    table->free_ids_capacity = capacity;

    for (int i = 0; i < capacity; i++)
    {
        table->free_ids[i] = i;
    }
}

void division_unordered_id_table_free(DivisionUnorderedIdTable *table)
{
    free(table->free_ids);

    table->free_ids = NULL;
    table->free_ids_count = table->free_ids_capacity = table->max_id = 0;
}

uint32_t division_unordered_id_table_new_id(DivisionUnorderedIdTable *table)
{
    if (table->free_ids_count > 0)
    {
        table->free_ids_count--;

        uint32_t id = table->free_ids[0];
        memmove(
            table->free_ids, table->free_ids + 1, sizeof(uint32_t[table->free_ids_count])
        );
        return id;
    }

    return ++table->max_id;
}

void division_unordered_id_table_remove_id(DivisionUnorderedIdTable *table, uint32_t id)
{
    assert(id <= table->max_id && division_unordered_id_table_contains(table, id));

    if (table->max_id == id & id != 0)
    {
        table->max_id--;
    }
    else
    {
        if (table->free_ids_count == table->free_ids_capacity)
        {
            table->free_ids_capacity *= 2;
            table->free_ids =
                realloc(table->free_ids, sizeof(uint32_t[table->free_ids_capacity]));
        }

        table->free_ids[table->free_ids_count++] = id;
    }
}

bool division_unordered_id_table_contains(
    const DivisionUnorderedIdTable *table, uint32_t id
)
{
    if (id > table->max_id)
        return false;

    for (size_t i = 0; i < table->free_ids_count; i++)
    {
        if (table->free_ids[i] == id)
        {
            return false;
        }
    }

    return true;
}

bool division_unordered_id_table_data_grow(
    DivisionUnorderedIdTable* id_table, 
    void** data, 
    size_t data_bytes,
    size_t* elements_capacity, 
    uint32_t* out_new_id
)
{
    uint32_t new_id = division_unordered_id_table_new_id(id_table);
    size_t new_capacity = *elements_capacity + 1;
    
    *out_new_id = new_id;

    if (new_id >= *elements_capacity)
    {
        
        void* new_data = realloc(*data, data_bytes * new_capacity);
        if (new_data == NULL)
        {
            return false;
        }

        *data = new_data;
        *elements_capacity = new_capacity;
    }

    return true;
}
