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

#define DIVISION_UNORDERED_ID_TABLE_DATA_WITH_TYPE_GROW( \
    data_type, id_table_ptr, data_out_ptr, elements_capacity_ptr, out_new_id \
) \
    division_unordered_id_table_data_grow( \
        id_table_ptr, (void**) data_out_ptr, sizeof(data_type), elements_capacity_ptr, out_new_id)


#ifdef __cplusplus
extern "C" {
#endif

DIVISION_EXPORT void division_unordered_id_table_alloc(DivisionUnorderedIdTable* table, size_t capacity);
DIVISION_EXPORT void division_unordered_id_table_free(DivisionUnorderedIdTable* table);

DIVISION_EXPORT bool division_unordered_id_table_contains(const DivisionUnorderedIdTable* table, uint32_t id);

DIVISION_EXPORT uint32_t division_unordered_id_table_new_id(DivisionUnorderedIdTable* table);
DIVISION_EXPORT void division_unordered_id_table_remove_id(DivisionUnorderedIdTable* table, uint32_t id);

DIVISION_EXPORT bool division_unordered_id_table_data_grow(
    DivisionUnorderedIdTable* id_table, 
    void** data, 
    size_t data_per_element_bytes,
    size_t* elements_capacity, 
    uint32_t* out_new_id
);

#ifdef __cplusplus
}
#endif