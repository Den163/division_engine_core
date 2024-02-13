#pragma once

#include <stddef.h>

#include "division_engine_core/context.h"

#ifdef __cplusplus
extern "C" {
#endif

DIVISION_EXPORT bool division_engine_internal_platform_texture_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings);

DIVISION_EXPORT void division_engine_internal_platform_texture_context_free(DivisionContext* ctx);

DIVISION_EXPORT bool division_engine_internal_platform_texture_realloc(
    DivisionContext* ctx, size_t new_size);
DIVISION_EXPORT bool division_engine_internal_platform_texture_impl_init_new_element(
    DivisionContext* ctx, uint32_t texture_id);

DIVISION_EXPORT void division_engine_internal_platform_texture_set_data(
    DivisionContext* ctx, uint32_t texture_id, const void* data);

DIVISION_EXPORT void division_engine_internal_platform_texture_free(DivisionContext* ctx, uint32_t texture_id);

#ifdef __cplusplus
}
#endif
