#include "division_engine/uniform_buffer.h"
#include "division_engine/platform_internal/platform_uniform_buffer.h"

#include <stdlib.h>

bool division_engine_internal_uniform_buffer_context_alloc(DivisionContext* ctx, const DivisionSettings* settings)
{
    ctx->uniform_buffer_context = malloc(sizeof(DivisionUniformBufferSystemContext));
    *ctx->uniform_buffer_context = (DivisionUniformBufferSystemContext) {
        .uniform_buffers = NULL,
        .uniform_buffer_count = 0
    };

    division_unordered_id_table_alloc(&ctx->uniform_buffer_context->id_table, 10);

    return division_engine_internal_platform_uniform_buffer_context_alloc(ctx, settings);
}

void division_engine_internal_uniform_buffer_context_free(DivisionContext* ctx)
{
    division_engine_internal_platform_uniform_buffer_context_free(ctx);

    division_unordered_id_table_free(&ctx->uniform_buffer_context->id_table);

    free(ctx->uniform_buffer_context->uniform_buffers);
    free(ctx->uniform_buffer_context);
}

bool division_engine_uniform_buffer_alloc(DivisionContext* ctx, DivisionUniformBuffer buffer, uint32_t* out_buffer_id)
{
    DivisionUniformBufferSystemContext* uniform_buffer_ctx = ctx->uniform_buffer_context;
    const uint32_t buff_id = division_unordered_id_table_insert(&uniform_buffer_ctx->id_table);
    if (buff_id >= uniform_buffer_ctx->uniform_buffer_count)
    {
        uniform_buffer_ctx->uniform_buffer_count = buff_id + 1;
        uniform_buffer_ctx->uniform_buffers = realloc(
            uniform_buffer_ctx->uniform_buffers,
            sizeof(DivisionUniformBuffer[uniform_buffer_ctx->uniform_buffer_count])
        );

        if (uniform_buffer_ctx->uniform_buffers == NULL)
        {
            ctx->error_callback(DIVISION_INTERNAL_ERROR, "Failed to reallocate Uniform Buffers array");
            division_unordered_id_table_remove(&uniform_buffer_ctx->id_table, buff_id);
            return false;
        }
    }

    uniform_buffer_ctx->uniform_buffers[buff_id] = buffer;

    *out_buffer_id = buff_id;
    if (division_engine_internal_platform_uniform_buffer_alloc(ctx, buffer, buff_id) == false)
    {
        division_unordered_id_table_remove(&uniform_buffer_ctx->id_table, buff_id);
        return false;
    }

    return true;
}

void* division_engine_uniform_buffer_borrow_data_pointer(DivisionContext* ctx, uint32_t buffer)
{
    return division_engine_internal_platform_uniform_buffer_borrow_data_pointer(ctx, buffer);
}

void division_engine_uniform_buffer_return_data_pointer(DivisionContext* ctx, uint32_t buffer, void* data_pointer)
{
    division_engine_internal_platform_uniform_buffer_return_data_pointer(ctx, buffer, data_pointer);
}

void division_engine_uniform_buffer_free(DivisionContext* ctx, uint32_t buffer_id)
{
    division_engine_internal_platform_uniform_buffer_free(ctx, buffer_id);
    division_unordered_id_table_remove(&ctx->uniform_buffer_context->id_table, buffer_id);
}
