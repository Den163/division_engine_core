#include "division_engine/render_pass.h"

#include <stdlib.h>
#include <memory.h>

#include "division_engine/platform_internal/platform_render_pass.h"

bool division_engine_internal_render_pass_context_alloc(DivisionContext* ctx, const DivisionSettings* settings)
{
    ctx->render_pass_context = malloc(sizeof(DivisionRenderPassSystemContext));
    *ctx->render_pass_context = (DivisionRenderPassSystemContext) {
        .render_passes = NULL,
        .render_pass_count = 0
    };

    return division_engine_internal_platform_render_pass_context_alloc(ctx, settings);
}

void division_engine_internal_render_pass_context_free(DivisionContext* ctx)
{
    division_engine_internal_platform_render_pass_context_free(ctx);

    DivisionRenderPassSystemContext* render_pass_ctx = ctx->render_pass_context;
    for (int i = 0; i < render_pass_ctx->render_pass_count; i++)
    {
        free(render_pass_ctx->render_passes[i].uniform_buffers);
    }
    free(render_pass_ctx->render_passes);
    free(render_pass_ctx);
}

int32_t division_engine_render_pass_alloc(DivisionContext* ctx, DivisionRenderPass render_pass)
{
    DivisionRenderPassSystemContext * pass_ctx = ctx->render_pass_context;

    DivisionRenderPass render_pass_copy = render_pass;
    size_t uniform_buffers_size = sizeof(int32_t[render_pass.uniform_buffer_count]);
    render_pass_copy.uniform_buffers = malloc(uniform_buffers_size);
    render_pass_copy.uniform_buffer_count = render_pass.uniform_buffer_count;
    memcpy(render_pass_copy.uniform_buffers, render_pass.uniform_buffers, uniform_buffers_size);

    int32_t render_pass_count = pass_ctx->render_pass_count;
    int32_t new_render_pass_count = render_pass_count + 1;
    pass_ctx->render_passes = realloc(pass_ctx->render_passes, sizeof(DivisionRenderPass) * new_render_pass_count);
    pass_ctx->render_passes[render_pass_count] = render_pass_copy;
    pass_ctx->render_pass_count++;

    return division_engine_internal_platform_render_pass_alloc(ctx, &render_pass)
        ? pass_ctx->render_pass_count - 1
        : -1;
}
