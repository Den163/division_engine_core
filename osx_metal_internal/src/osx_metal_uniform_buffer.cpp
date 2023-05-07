#include "division_engine/platform_internal/platform_uniform_buffer.h"

#include "osx_uniform_buffer.h"
#include "osx_window_context.h"
#include "division_engine/renderer.h"
#include <stdlib.h>

bool division_engine_internal_platform_uniform_buffer_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings)
{
    ctx->uniform_buffer_context->uniform_buffers_impl = nullptr;
    return true;
}

void division_engine_internal_platform_uniform_buffer_context_free(DivisionContext* ctx)
{
    DivisionUniformBufferSystemContext* uniform_buffer_ctx = ctx->uniform_buffer_context;
    DivisionUniformBufferInternal_* uniform_buffers_impl = uniform_buffer_ctx->uniform_buffers_impl;

    for (size_t i = 0; i < uniform_buffer_ctx->uniform_buffer_count; i++)
    {
        uniform_buffers_impl[i].mtl_buffer->release();
    }

    free(uniform_buffers_impl);
}

void division_engine_internal_platform_uniform_buffer_alloc(DivisionContext* ctx, DivisionUniformBuffer buffer)
{
    auto* window_ctx = static_cast<DivisionOSXWindowContext*>(ctx->renderer_context->window_data);
    MTL::Buffer* mtl_buffer = window_ctx->app_delegate->viewDelegate->createBuffer(buffer.data_bytes);

    DivisionUniformBufferSystemContext* uniform_buffer_ctx = ctx->uniform_buffer_context;
    uniform_buffer_ctx->uniform_buffers_impl = static_cast<DivisionUniformBufferInternal_*>(realloc(
        uniform_buffer_ctx->uniform_buffers_impl,
        sizeof(DivisionUniformBufferInternal_) * uniform_buffer_ctx->uniform_buffer_count
    ));

    DivisionUniformBufferInternal_* internal_buffer =
        &uniform_buffer_ctx->uniform_buffers_impl[uniform_buffer_ctx->uniform_buffer_count - 1];

    internal_buffer->mtl_buffer = mtl_buffer;
}

void* division_engine_internal_platform_uniform_buffer_borrow_data_pointer(DivisionContext* ctx, int32_t buffer)
{
    return ctx->uniform_buffer_context->uniform_buffers_impl[buffer].mtl_buffer->contents();
}

void division_engine_internal_platform_uniform_buffer_return_data_pointer(
    DivisionContext* ctx, int32_t buffer, void* data_pointer)
{
    DivisionUniformBufferSystemContext* uniform_buffer_ctx = ctx->uniform_buffer_context;
    MTL::Buffer* mtl_buffer = uniform_buffer_ctx->uniform_buffers_impl[buffer].mtl_buffer;
    mtl_buffer->didModifyRange(NS::Range::Make(0, mtl_buffer->length()));
}
