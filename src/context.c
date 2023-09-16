#include "division_engine_core/context.h"

#include "division_engine_core/render_pass.h"
#include "division_engine_core/renderer.h"
#include "division_engine_core/shader.h"
#include "division_engine_core/texture.h"
#include "division_engine_core/uniform_buffer.h"
#include "division_engine_core/vertex_buffer.h"
#include <stdlib.h>

bool division_engine_context_alloc(
    const DivisionSettings* settings, DivisionContext** output_context
)
{
    DivisionContext* ctx = (DivisionContext*)malloc(sizeof(DivisionContext));
    ctx->error_callback = settings->error_callback;
    ctx->state.delta_time = 0;
    *output_context = ctx;

    if (!division_engine_internal_renderer_context_alloc(ctx, settings))
        return false;
    if (!division_engine_internal_shader_system_context_alloc(ctx, settings))
        return false;
    if (!division_engine_internal_vertex_buffer_context_alloc(ctx, settings))
        return false;
    if (!division_engine_internal_uniform_buffer_context_alloc(ctx, settings))
        return false;
    if (!division_engine_internal_texture_context_alloc(ctx, settings))
        return false;
    if (!division_engine_internal_render_pass_context_alloc(ctx, settings))
        return false;

    return true;
}

void division_engine_context_free(DivisionContext* ctx)
{
    division_engine_internal_render_pass_context_free(ctx);
    division_engine_internal_texture_context_free(ctx);
    division_engine_internal_uniform_buffer_context_free(ctx);
    division_engine_internal_vertex_buffer_context_free(ctx);
    division_engine_internal_shader_system_context_free(ctx);
    division_engine_internal_renderer_context_free(ctx);
    free(ctx);
}
