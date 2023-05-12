#include "division_engine_core/platform_internal/platform_uniform_buffer.h"

#include "osx_uniform_buffer.h"
#include "osx_window_context.h"
#include "division_engine_core/renderer.h"

bool division_engine_internal_platform_uniform_buffer_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings)
{
    ctx->uniform_buffer_context->uniform_buffers_impl = NULL;
    return true;
}

void division_engine_internal_platform_uniform_buffer_context_free(DivisionContext* ctx)
{
    DivisionUniformBufferSystemContext* uniform_buffer_ctx = ctx->uniform_buffer_context;
    DivisionUniformBufferInternal_* uniform_buffers_impl = uniform_buffer_ctx->uniform_buffers_impl;

    for (size_t i = 0; i < uniform_buffer_ctx->uniform_buffer_count; i++)
    {
        DivisionUniformBufferInternal_* buffer_impl = &uniform_buffers_impl[i];
        buffer_impl->mtl_buffer = nil;
    }

    free(uniform_buffers_impl);
}

bool division_engine_internal_platform_uniform_buffer_alloc(
    DivisionContext* ctx, DivisionUniformBuffer buffer, uint32_t buffer_id)
{
    DivisionOSXWindowContext* window_ctx = ctx->renderer_context->window_data;
    id<MTLBuffer> mtl_buffer = [window_ctx->app_delegate->viewDelegate createBufferWithSize:buffer.data_bytes];
    if (mtl_buffer == nil)
    {
        ctx->error_callback(DIVISION_INTERNAL_ERROR, "Failed to create MTLBuffer");
        return false;
    }

    DivisionUniformBufferSystemContext* uniform_buffer_ctx = ctx->uniform_buffer_context;

    if (buffer_id >= uniform_buffer_ctx->uniform_buffer_count)
    {
        uniform_buffer_ctx->uniform_buffers_impl = realloc(
            uniform_buffer_ctx->uniform_buffers_impl,
            sizeof(DivisionUniformBufferInternal_[buffer_id + 1])
        );
    }

    if (uniform_buffer_ctx->uniform_buffers_impl == NULL)
    {
        ctx->error_callback(DIVISION_INTERNAL_ERROR, "Failed to reallocate Uniform Buffer Implementation array");
        return false;
    }

    DivisionUniformBufferInternal_* internal_buffer = &uniform_buffer_ctx->uniform_buffers_impl[buffer_id];
    internal_buffer->mtl_buffer = mtl_buffer;

    return true;
}

void* division_engine_internal_platform_uniform_buffer_borrow_data_pointer(DivisionContext* ctx, uint32_t buffer_id)
{
    return [ctx->uniform_buffer_context->uniform_buffers_impl[buffer_id].mtl_buffer contents];
}

void division_engine_internal_platform_uniform_buffer_return_data_pointer(
    DivisionContext* ctx, uint32_t buffer_id, void* data_pointer)
{
    DivisionUniformBufferSystemContext* uniform_buffer_ctx = ctx->uniform_buffer_context;
    id<MTLBuffer> mtl_buffer = uniform_buffer_ctx->uniform_buffers_impl[buffer_id].mtl_buffer;
    [mtl_buffer didModifyRange:NSMakeRange(0, [mtl_buffer length])];
}

void division_engine_internal_platform_uniform_buffer_free(DivisionContext* ctx, uint32_t buffer_id)
{
    ctx->uniform_buffer_context->uniform_buffers_impl[buffer_id].mtl_buffer = nil;
}
