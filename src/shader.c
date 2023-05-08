#include "division_engine/shader.h"
#include "division_engine/platform_internal/platfrom_shader.h"

#include <stdbool.h>
#include <stdlib.h>

bool division_engine_internal_shader_system_context_alloc(DivisionContext* ctx, const DivisionSettings* settings)
{
    ctx->shader_context = malloc(sizeof(DivisionShaderSystemContext));
    ctx->shader_context->shader_count = 0;

    return division_engine_internal_platform_shader_system_context_alloc(ctx, settings);
}

void division_engine_internal_shader_system_context_free(DivisionContext* ctx)
{
    division_engine_internal_platform_shader_system_context_free(ctx);

    free(ctx->shader_context);
}

int32_t division_engine_shader_program_create(
    DivisionContext* ctx, const DivisionShaderSettings* settings, int32_t source_count)
{
    DivisionShaderSystemContext* shader_ctx = ctx->shader_context;
    shader_ctx->shader_count += 1;

    if (division_engine_internal_platform_shader_program_create(ctx, settings, source_count))
    {
        return shader_ctx->shader_count - 1;
    }
    else
    {
        shader_ctx->shader_count -= 1;
        return -1;
    }
}