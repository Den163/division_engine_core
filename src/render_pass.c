#include "division_engine_core/render_pass.h"

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#include "division_engine_core/context.h"
#include "division_engine_core/data_structures/ordered_id_table.h"
#include "division_engine_core/platform_internal/platform_render_pass.h"

static inline void handle_render_pass_alloc_error(
    DivisionContext* ctx, uint32_t render_pass_id, DivisionRenderPass* render_pass_copy
);

bool division_engine_render_pass_system_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings
)
{
    ctx->render_pass_context = malloc(sizeof(DivisionRenderPassSystemContext));
    *ctx->render_pass_context =
        (DivisionRenderPassSystemContext){.render_passes = NULL, .render_pass_count = 0};

    division_ordered_id_table_alloc(&ctx->render_pass_context->id_table, 10);

    return division_engine_internal_platform_render_pass_context_alloc(ctx, settings);
}

void division_engine_render_pass_system_context_free(DivisionContext* ctx)
{
    division_engine_internal_platform_render_pass_context_free(ctx);
    division_ordered_id_table_free(&ctx->render_pass_context->id_table);

    DivisionRenderPassSystemContext* render_pass_ctx = ctx->render_pass_context;
    for (int i = 0; i < render_pass_ctx->render_pass_count; i++)
    {
        DivisionRenderPass pass = render_pass_ctx->render_passes[i];
        free(pass.uniform_vertex_buffers);
        free(pass.uniform_fragment_buffers);
        free(pass.fragment_textures);
    }
    free(render_pass_ctx->render_passes);
    free(render_pass_ctx);
}

bool division_engine_render_pass_alloc(
    DivisionContext* ctx, DivisionRenderPass render_pass, uint32_t* out_render_pass_id
)
{
    DivisionRenderPassSystemContext* pass_ctx = ctx->render_pass_context;
    uint32_t render_pass_id = division_ordered_id_table_insert(&pass_ctx->id_table);

    DivisionRenderPass render_pass_copy = render_pass;

    if (render_pass.uniform_vertex_buffer_count > 0)
    {
        size_t uniform_vert_buffs_bytes =
            sizeof(DivisionIdWithBinding[render_pass.uniform_vertex_buffer_count]);
        render_pass_copy.uniform_vertex_buffers = malloc(uniform_vert_buffs_bytes);
        memcpy(
            render_pass_copy.uniform_vertex_buffers,
            render_pass.uniform_vertex_buffers,
            uniform_vert_buffs_bytes
        );
    }
    else
    {
        render_pass_copy.uniform_vertex_buffers = NULL;
    }

    if (render_pass.uniform_fragment_buffer_count > 0)
    {
        size_t uniform_frag_buffs_bytes =
            sizeof(DivisionIdWithBinding[render_pass.uniform_fragment_buffer_count]);
        render_pass_copy.uniform_fragment_buffers = malloc(uniform_frag_buffs_bytes);
        memcpy(
            render_pass_copy.uniform_fragment_buffers,
            render_pass.uniform_fragment_buffers,
            uniform_frag_buffs_bytes
        );
    }
    else
    {
        render_pass_copy.uniform_fragment_buffers = NULL;
    }

    if (render_pass.fragment_texture_count > 0)
    {
        size_t fragment_textures_bytes =
            sizeof(DivisionIdWithBinding[render_pass.fragment_texture_count]);
        render_pass_copy.fragment_textures = malloc(fragment_textures_bytes);
        memcpy(
            render_pass_copy.fragment_textures,
            render_pass.fragment_textures,
            fragment_textures_bytes
        );
    }
    else
    {
        render_pass_copy.fragment_textures = NULL;
    }

    if ((render_pass_copy.uniform_fragment_buffer_count > 0 &&
         render_pass_copy.uniform_fragment_buffers == NULL) ||
        (render_pass_copy.uniform_vertex_buffer_count > 0 &&
         render_pass_copy.uniform_vertex_buffers == NULL) ||
        (render_pass_copy.fragment_texture_count > 0 &&
         render_pass_copy.fragment_textures == NULL))
    {
        handle_render_pass_alloc_error(ctx, render_pass_id, &render_pass_copy);
        return false;
    }

    int32_t render_pass_count = pass_ctx->render_pass_count;
    int32_t new_render_pass_count = render_pass_count + 1;
    if (render_pass_id >= pass_ctx->render_pass_count)
    {
        pass_ctx->render_passes = realloc(
            pass_ctx->render_passes, sizeof(DivisionRenderPass) * new_render_pass_count
        );
        if (pass_ctx->render_passes == NULL ||
            !division_engine_internal_platform_render_pass_realloc(
                ctx, new_render_pass_count
            ))
        {
            handle_render_pass_alloc_error(ctx, render_pass_id, &render_pass_copy);
            return false;
        }

        pass_ctx->render_pass_count++;
    }

    *out_render_pass_id = render_pass_id;
    pass_ctx->render_passes[render_pass_count] = render_pass_copy;

    return division_engine_internal_platform_render_pass_impl_init_element(
        ctx, render_pass_id
    );
}

void handle_render_pass_alloc_error(
    DivisionContext* ctx, uint32_t render_pass_id, DivisionRenderPass* render_pass_copy
)
{
    DivisionRenderPassSystemContext* pass_ctx = ctx->render_pass_context;
    division_ordered_id_table_remove(&pass_ctx->id_table, render_pass_id);
    free(render_pass_copy->uniform_vertex_buffers);
    free(render_pass_copy->uniform_fragment_buffers);
    free(render_pass_copy->fragment_textures);

    ctx->lifecycle.error_callback(
        ctx, DIVISION_INTERNAL_ERROR, "Failed to realloc Render pass array"
    );
}

DivisionRenderPass* division_engine_render_pass_borrow(
    DivisionContext* ctx, uint32_t render_pass_id
)
{
    return DIVISION_GET_RENDER_PASS(ctx, render_pass_id);
}

void division_engine_render_pass_return(
    DivisionContext* ctx, uint32_t render_pass_id, DivisionRenderPass* render_pass_ptr
)
{
}

void division_engine_render_pass_free(DivisionContext* ctx, uint32_t render_pass_id)
{
    division_engine_internal_platform_render_pass_free(ctx, render_pass_id);

    DivisionRenderPassSystemContext* render_pass_ctx = ctx->render_pass_context;
    division_ordered_id_table_remove(&render_pass_ctx->id_table, render_pass_id);

    DivisionRenderPass* render_pass = &render_pass_ctx->render_passes[render_pass_id];
    free(render_pass->uniform_vertex_buffers);
    free(render_pass->uniform_fragment_buffers);
    free(render_pass->fragment_textures);

    render_pass->uniform_vertex_buffers = NULL;
    render_pass->uniform_fragment_buffers = NULL;
    render_pass->fragment_textures = NULL;
}
