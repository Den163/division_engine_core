#pragma once

#include <stddef.h>

#include "division_engine_core/context.h"
#include "division_engine_core/settings.h"
#include "division_engine_core/vertex_buffer.h"

#include <division_engine_core_export.h>

#ifdef __cplusplus
extern "C" {
#endif

DIVISION_EXPORT bool division_engine_internal_platform_vertex_buffer_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings);

DIVISION_EXPORT void division_engine_internal_platform_vertex_buffer_context_free(DivisionContext* ctx);

DIVISION_EXPORT void division_engine_internal_platform_vertex_buffer_free(DivisionContext* ctx, uint32_t buffer_id);

DIVISION_EXPORT bool division_engine_internal_platform_vertex_buffer_realloc(DivisionContext* ctx, size_t new_size);
DIVISION_EXPORT bool division_engine_internal_platform_vertex_buffer_impl_init_element(
    DivisionContext* ctx, uint32_t buffer_id);

DIVISION_EXPORT bool division_engine_internal_platform_vertex_buffer_borrow_data_pointer(
    DivisionContext* ctx, uint32_t buffer_id, DivisionVertexBufferBorrowedData* out_borrow_data);

DIVISION_EXPORT void division_engine_internal_platform_vertex_buffer_return_data_pointer(
    DivisionContext* ctx, uint32_t buffer_id, DivisionVertexBufferBorrowedData* out_borrow_data);

#ifdef __cplusplus
}
#endif