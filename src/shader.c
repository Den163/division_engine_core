#include "division_engine/shader.h"
#include "division_engine/platform_internal/platfrom_shader.h"

#include <stdbool.h>

bool division_engine_internal_shader_system_context_alloc(DivisionContext* ctx, const DivisionSettings* settings)
{
    return division_engine_internal_platform_shader_system_context_alloc(ctx, settings);
}

void division_engine_internal_shader_system_context_free(DivisionContext* ctx)
{
    division_engine_internal_platform_shader_system_context_free(ctx);
}

int32_t division_engine_shader_program_create(
    DivisionContext* ctx, const DivisionShaderSettings* settings, int32_t source_count)
{
    return division_engine_internal_platform_shader_program_create(ctx, settings, source_count);
}

void division_engine_shader_program_free(DivisionContext* ctx, int32_t program_id)
{
    division_engine_internal_platform_shader_program_free(ctx, program_id);
}
