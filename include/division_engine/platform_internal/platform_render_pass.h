#pragma once

#include "division_engine/context.h"
#include "division_engine/render_pass.h"
#include "division_engine_core_export.h"

#ifdef __cplusplus
extern "C" {
#endif

DIVISION_EXPORT bool division_engine_internal_platform_render_pass_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings);

DIVISION_EXPORT void division_engine_internal_platform_render_pass_context_free(DivisionContext* ctx);

DIVISION_EXPORT bool division_engine_internal_platform_render_pass_alloc(
    DivisionContext* ctx, DivisionRenderPass* render_pass);

#ifdef __cplusplus
}
#endif