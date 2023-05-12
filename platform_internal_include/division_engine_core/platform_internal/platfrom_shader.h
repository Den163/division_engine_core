#pragma once

#include "division_engine_core/context.h"
#include "division_engine_core/shader.h"

#include <division_engine_core_export.h>

#ifdef __cplusplus
extern "C" {
#endif

DIVISION_EXPORT bool division_engine_internal_platform_shader_system_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings);
DIVISION_EXPORT void division_engine_internal_platform_shader_system_context_free(DivisionContext* ctx);


DIVISION_EXPORT bool division_engine_internal_platform_shader_program_alloc(
    DivisionContext* ctx,
    const DivisionShaderSettings* settings,
    int32_t source_count,
    uint32_t* out_shader_program_id
);

DIVISION_EXPORT void division_engine_internal_platform_shader_program_free(
    DivisionContext* ctx, uint32_t shader_program_id);

#ifdef __cplusplus
}
#endif
