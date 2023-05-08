#include "division_engine/platform_internal/platform_uniform_buffer.h"

#include "osx_uniform_buffer.h"
#include "osx_window_context.h"
#include "division_engine/renderer.h"

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

void division_engine_internal_platform_uniform_buffer_alloc(DivisionContext* ctx, DivisionUniformBuffer buffer)
{
    DivisionOSXWindowContext* window_ctx = ctx->renderer_context->window_data;
    id<MTLBuffer> mtl_buffer = [window_ctx->app_delegate->viewDelegate createBufferWithSize:buffer.data_bytes];

    DivisionUniformBufferSystemContext* uniform_buffer_ctx = ctx->uniform_buffer_context;
    uniform_buffer_ctx->uniform_buffers_impl = realloc(
        uniform_buffer_ctx->uniform_buffers_impl,
        sizeof(DivisionUniformBufferInternal_) * uniform_buffer_ctx->uniform_buffer_count
    );

    DivisionUniformBufferInternal_* internal_buffer =
        &uniform_buffer_ctx->uniform_buffers_impl[uniform_buffer_ctx->uniform_buffer_count - 1];

    internal_buffer->mtl_buffer = mtl_buffer;
}

void* division_engine_internal_platform_uniform_buffer_borrow_data_pointer(DivisionContext* ctx, int32_t buffer)
{
    return [ctx->uniform_buffer_context->uniform_buffers_impl[buffer].mtl_buffer contents];
}

void division_engine_internal_platform_uniform_buffer_return_data_pointer(
    DivisionContext* ctx, int32_t buffer, void* data_pointer)
{
    DivisionUniformBufferSystemContext* uniform_buffer_ctx = ctx->uniform_buffer_context;
    id<MTLBuffer> mtl_buffer = uniform_buffer_ctx->uniform_buffers_impl[buffer].mtl_buffer;
    [mtl_buffer didModifyRange:NSMakeRange(0, [mtl_buffer length])];
}
