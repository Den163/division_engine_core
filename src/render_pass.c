#include "division_engine_core/render_pass.h"

#include <stdlib.h>
#include <memory.h>

#include "division_engine_core/platform_internal/platform_render_pass.h"

bool division_engine_internal_render_pass_context_alloc(DivisionContext* ctx, const DivisionSettings* settings)
{
    ctx->render_pass_context = malloc(sizeof(DivisionRenderPassSystemContext));
    *ctx->render_pass_context = (DivisionRenderPassSystemContext) {
        .render_passes = NULL,
        .render_pass_count = 0
    };

    division_ordered_id_table_alloc(&ctx->render_pass_context->id_table, 10);

    return division_engine_internal_platform_render_pass_context_alloc(ctx, settings);
}

void division_engine_internal_render_pass_context_free(DivisionContext* ctx)
{
    division_engine_internal_platform_render_pass_context_free(ctx);
    division_ordered_id_table_free(&ctx->render_pass_context->id_table);

    DivisionRenderPassSystemContext* render_pass_ctx = ctx->render_pass_context;
    for (int i = 0; i < render_pass_ctx->render_pass_count; i++)
    {
        DivisionRenderPass pass = render_pass_ctx->render_passes[i];
        free(pass.uniform_vertex_buffers);
        free(pass.uniform_fragment_buffers);
    }
    free(render_pass_ctx->render_passes);
    free(render_pass_ctx);
}

bool division_engine_render_pass_alloc(
    DivisionContext* ctx, DivisionRenderPass render_pass, uint32_t* out_render_pass_id)
{
    DivisionRenderPassSystemContext* pass_ctx = ctx->render_pass_context;
    uint32_t render_pass_id = division_ordered_id_table_insert(&pass_ctx->id_table);

    if (!division_engine_internal_platform_render_pass_alloc(ctx, &render_pass, render_pass_id))
    {
        division_ordered_id_table_remove(&pass_ctx->id_table, render_pass_id);
        return false;
    }

    DivisionRenderPass render_pass_copy = render_pass;
    size_t uniform_vert_buffs_size = sizeof(int32_t[render_pass.uniform_vertex_buffer_count]);
    render_pass_copy.uniform_vertex_buffers = malloc(uniform_vert_buffs_size);
    size_t uniform_frag_buffs_size = sizeof(int32_t[render_pass.uniform_fragment_buffer_count]);
    render_pass_copy.uniform_fragment_buffers = malloc(uniform_frag_buffs_size);
    if (render_pass_copy.uniform_fragment_buffers == NULL ||
        render_pass_copy.uniform_vertex_buffers == NULL)
    {
        division_ordered_id_table_remove(&pass_ctx->id_table, render_pass_id);
        free(render_pass_copy.uniform_vertex_buffers);
        free(render_pass_copy.uniform_fragment_buffers);
        ctx->error_callback(DIVISION_INTERNAL_ERROR, "Failed to allocate Render pass fragment buffers array");
        return false;
    }

    render_pass_copy.uniform_vertex_buffer_count = render_pass.uniform_vertex_buffer_count;
    render_pass_copy.uniform_fragment_buffer_count = render_pass.uniform_fragment_buffer_count;

    memcpy(render_pass_copy.uniform_vertex_buffers, render_pass.uniform_vertex_buffers, uniform_vert_buffs_size);
    memcpy(render_pass_copy.uniform_fragment_buffers, render_pass.uniform_fragment_buffers, uniform_frag_buffs_size);

    int32_t render_pass_count = pass_ctx->render_pass_count;
    int32_t new_render_pass_count = render_pass_count + 1;
    if (render_pass_id >= pass_ctx->render_pass_count)
    {
        pass_ctx->render_passes = realloc(pass_ctx->render_passes, sizeof(DivisionRenderPass) * new_render_pass_count);
        if (pass_ctx->render_passes == NULL)
        {
            division_ordered_id_table_remove(&pass_ctx->id_table, render_pass_id);
            free(render_pass_copy.uniform_vertex_buffers);
            free(render_pass_copy.uniform_fragment_buffers);
            ctx->error_callback(DIVISION_INTERNAL_ERROR, "Failed to realloc Render pass array");
            return false;
        }

        pass_ctx->render_pass_count++;
    }

    pass_ctx->render_passes[render_pass_count] = render_pass_copy;

    *out_render_pass_id = render_pass_id;
    return true;
}

void division_engine_render_pass_free(DivisionContext* ctx, uint32_t render_pass_id)
{
    division_engine_internal_platform_render_pass_free(ctx, render_pass_id);

    DivisionRenderPassSystemContext* render_pass_ctx = ctx->render_pass_context;
    division_ordered_id_table_remove(&render_pass_ctx->id_table, render_pass_id);

    DivisionRenderPass* render_pass = &render_pass_ctx->render_passes[render_pass_id];
    free(render_pass->uniform_vertex_buffers);
    free(render_pass->uniform_fragment_buffers);
    render_pass->uniform_vertex_buffers = NULL;
}
