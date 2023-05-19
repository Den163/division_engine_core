#pragma once

#include "division_engine_core/context.h"
#include "division_engine_core/render_pass.h"
#include "division_engine_core_export.h"

#ifdef __cplusplus
extern "C" {
#endif

DIVISION_EXPORT bool division_engine_internal_platform_render_pass_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings);

DIVISION_EXPORT void division_engine_internal_platform_render_pass_context_free(DivisionContext* ctx);

DIVISION_EXPORT bool division_engine_internal_platform_render_pass_realloc(DivisionContext* ctx, size_t new_size);
DIVISION_EXPORT bool division_engine_internal_platform_render_pass_impl_init_element(
    DivisionContext* ctx, uint32_t render_pass_id);

DIVISION_EXPORT void division_engine_internal_platform_render_pass_free(DivisionContext* ctx, uint32_t pass_id);

#ifdef __cplusplus
}
#endif