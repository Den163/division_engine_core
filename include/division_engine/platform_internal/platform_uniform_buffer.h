#pragma once

#include "division_engine_core_export.h"
#include "division_engine/context.h"
#include "division_engine/uniform_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

DIVISION_EXPORT bool division_engine_internal_platform_uniform_buffer_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings);
DIVISION_EXPORT void division_engine_internal_platform_uniform_buffer_context_free(DivisionContext* ctx);


DIVISION_EXPORT void division_engine_internal_platform_uniform_buffer_alloc(
    DivisionContext* ctx, DivisionUniformBuffer buffer);

DIVISION_EXPORT void* division_engine_internal_platform_uniform_buffer_borrow_data_pointer(
    DivisionContext* ctx, int32_t buffer);
DIVISION_EXPORT void division_engine_internal_platform_uniform_buffer_return_data_pointer(
    DivisionContext* ctx, int32_t buffer, void* data_pointer);

#ifdef __cplusplus
}
#endif
