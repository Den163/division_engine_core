#include "division_engine_core/render_pass_descriptor.h"

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#include "division_engine_core/context.h"
#include "division_engine_core/data_structures/unordered_id_table.h"
#include "division_engine_core/platform_internal/platform_render_pass_descriptor.h"

static inline void handle_render_pass_alloc_error(
    DivisionContext* ctx,
    uint32_t render_pass_id,
    DivisionRenderPassDescriptor* render_pass_copy
);

bool division_engine_render_pass_system_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings
)
{
    ctx->render_pass_context = malloc(sizeof(DivisionRenderPassSystemContext));
    *ctx->render_pass_context =
        (DivisionRenderPassSystemContext){.render_pass_descriptors = NULL, .render_pass_count = 0};

    division_unordered_id_table_alloc(&ctx->render_pass_context->id_table, 10);

    return division_engine_internal_platform_render_pass_context_alloc(ctx, settings);
}

void division_engine_render_pass_system_context_free(DivisionContext* ctx)
{
    division_engine_internal_platform_render_pass_context_free(ctx);
    division_unordered_id_table_free(&ctx->render_pass_context->id_table);

    DivisionRenderPassSystemContext* render_pass_ctx = ctx->render_pass_context;
    for (int i = 0; i < render_pass_ctx->render_pass_count; i++)
    {
        DivisionRenderPassDescriptor pass = render_pass_ctx->render_pass_descriptors[i];
    }
    free(render_pass_ctx->render_pass_descriptors);
    free(render_pass_ctx);
}

bool division_engine_render_pass_descriptor_alloc(
    DivisionContext* ctx,
    const DivisionRenderPassDescriptor* render_pass,
    uint32_t* out_render_pass_id
)
{
    DivisionRenderPassSystemContext* pass_ctx = ctx->render_pass_context;
    uint32_t render_pass_id = division_unordered_id_table_new_id(&pass_ctx->id_table);

    DivisionRenderPassDescriptor render_pass_copy = *render_pass;

    int32_t render_pass_count = pass_ctx->render_pass_count;
    int32_t new_render_pass_count = render_pass_count + 1;
    if (render_pass_id >= pass_ctx->render_pass_count)
    {
        pass_ctx->render_pass_descriptors = realloc(
            pass_ctx->render_pass_descriptors,
            sizeof(DivisionRenderPassDescriptor) * new_render_pass_count
        );
        if (pass_ctx->render_pass_descriptors == NULL ||
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
    pass_ctx->render_pass_descriptors[render_pass_count] = render_pass_copy;

    return division_engine_internal_platform_render_pass_impl_init_element(
        ctx, render_pass_id
    );
}

void handle_render_pass_alloc_error(
    DivisionContext* ctx,
    uint32_t render_pass_id,
    DivisionRenderPassDescriptor* render_pass_copy
)
{
    DivisionRenderPassSystemContext* pass_ctx = ctx->render_pass_context;
    division_unordered_id_table_remove_id(&pass_ctx->id_table, render_pass_id);

    ctx->lifecycle.error_callback(
        ctx, DIVISION_INTERNAL_ERROR, "Failed to realloc Render pass array"
    );
}

DivisionRenderPassDescriptor* division_engine_render_pass_descriptor_borrow(
    DivisionContext* ctx, uint32_t render_pass_id
)
{
    return DIVISION_GET_RENDER_PASS_DESCRIPTOR(ctx, render_pass_id);
}

void division_engine_render_pass_descriptor_return(
    DivisionContext* ctx,
    uint32_t render_pass_id,
    DivisionRenderPassDescriptor* render_pass_ptr
)
{
}

void division_engine_render_pass_descriptor_free(
    DivisionContext* ctx, uint32_t render_pass_id
)
{
    division_engine_internal_platform_render_pass_free(ctx, render_pass_id);

    DivisionRenderPassSystemContext* render_pass_ctx = ctx->render_pass_context;
    division_unordered_id_table_remove_id(&render_pass_ctx->id_table, render_pass_id);

    DivisionRenderPassDescriptor* render_pass =
        &render_pass_ctx->render_pass_descriptors[render_pass_id];
}
