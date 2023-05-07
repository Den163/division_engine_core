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

    return division_engine_internal_platform_uniform_buffer_context_alloc(ctx, settings);
}

void division_engine_internal_uniform_buffer_context_free(DivisionContext* ctx)
{
    division_engine_internal_platform_uniform_buffer_context_free(ctx);

    free(ctx->uniform_buffer_context->uniform_buffers);
    free(ctx->uniform_buffer_context);
}

int32_t division_engine_uniform_buffer_alloc(DivisionContext* ctx, DivisionUniformBuffer buffer)
{
    DivisionUniformBufferSystemContext* uniform_buffer_ctx = ctx->uniform_buffer_context;

    int32_t new_count = uniform_buffer_ctx->uniform_buffer_count + 1;
    uniform_buffer_ctx->uniform_buffers =
        realloc(uniform_buffer_ctx->uniform_buffers, sizeof(DivisionUniformBuffer) * new_count);
    uniform_buffer_ctx->uniform_buffers[new_count - 1] = buffer;

    uniform_buffer_ctx->uniform_buffer_count = new_count;

    division_engine_internal_platform_uniform_buffer_alloc(ctx, buffer);

    return new_count - 1;
}

void* division_engine_uniform_buffer_borrow_data_pointer(DivisionContext* ctx, int32_t buffer)
{
    return division_engine_internal_platform_uniform_buffer_borrow_data_pointer(ctx, buffer);
}

void division_engine_uniform_buffer_return_data_pointer(DivisionContext* ctx, int32_t buffer, void* data_pointer)
{
    division_engine_internal_platform_uniform_buffer_return_data_pointer(ctx, buffer, data_pointer);
}
