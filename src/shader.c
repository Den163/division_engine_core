#include "rendering/shader.h"
#include "rendering/platform_internal/platfrom_shader.h"

#include <stdbool.h>
#include <stdlib.h>

bool division_engine_internal_shader_system_context_alloc(DivisionContext* ctx, const DivisionSettings* settings)
{
    ctx->shader_context = malloc(sizeof(DivisionShaderSystemContext));
    ctx->shader_context->shader_count = 0;

    division_unordered_id_table_alloc(&ctx->shader_context->id_table, 10);

    return division_engine_internal_platform_shader_system_context_alloc(ctx, settings);
}

void division_engine_internal_shader_system_context_free(DivisionContext* ctx)
{
    division_engine_internal_platform_shader_system_context_free(ctx);

    division_unordered_id_table_free(&ctx->shader_context->id_table);

    free(ctx->shader_context);
}

bool division_engine_shader_program_alloc(
    DivisionContext* ctx,
    const DivisionShaderSettings* settings,
    int32_t source_count,
    uint32_t* out_shader_program_id)
{
    return division_engine_internal_platform_shader_program_alloc(ctx, settings, source_count, out_shader_program_id);
}

void division_engine_shader_program_free(DivisionContext* ctx, uint32_t shader_program_id)
{
    division_engine_internal_platform_shader_program_free(ctx, shader_program_id);
}
