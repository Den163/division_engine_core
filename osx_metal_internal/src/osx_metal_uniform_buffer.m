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

void* division_engine_internal_platform_uniform_buffer_borrow_data_pointer(DivisionContext* ctx, uint32_t buffer_id)
{
    return [ctx->uniform_buffer_context->uniform_buffers_impl[buffer_id].mtl_buffer contents];
}

void division_engine_internal_platform_uniform_buffer_return_data_pointer(
    DivisionContext* ctx, uint32_t buffer_id, void* data_pointer)
{
    DivisionUniformBufferSystemContext* uniform_buffer_ctx = ctx->uniform_buffer_context;
    id <MTLBuffer> mtl_buffer = uniform_buffer_ctx->uniform_buffers_impl[buffer_id].mtl_buffer;
    [mtl_buffer didModifyRange:NSMakeRange(0, [mtl_buffer length])];
}

void division_engine_internal_platform_uniform_buffer_free(DivisionContext* ctx, uint32_t buffer_id)
{
    ctx->uniform_buffer_context->uniform_buffers_impl[buffer_id].mtl_buffer = nil;
}

bool division_engine_internal_platform_uniform_buffer_realloc(DivisionContext* ctx, size_t new_size)
{
    DivisionUniformBufferInternal_** uniform_buffers_impl = &ctx->uniform_buffer_context->uniform_buffers_impl;

    *uniform_buffers_impl = realloc(*uniform_buffers_impl, sizeof(DivisionUniformBufferInternal_[new_size]));
    return *uniform_buffers_impl != NULL;
}

bool division_engine_internal_platform_uniform_buffer_impl_init_element(
    DivisionContext* ctx, uint32_t buffer_id)
{
    DivisionOSXWindowContext* window_ctx = ctx->renderer_context->window_data;
    id <MTLDevice> device = window_ctx->app_delegate->viewDelegate->device;
    DivisionUniformBufferSystemContext* uniform_buffer_ctx = ctx->uniform_buffer_context;

    DivisionUniformBufferDescriptor* uniform_buffer = &uniform_buffer_ctx->uniform_buffers[buffer_id];
    DivisionUniformBufferInternal_* internal_buffer = &uniform_buffer_ctx->uniform_buffers_impl[buffer_id];

    id <MTLBuffer> mtl_buffer = [device newBufferWithLength:uniform_buffer->data_bytes
                                                    options:MTLResourceStorageModeManaged];

    if (mtl_buffer == nil)
    {
        ctx->error_callback(DIVISION_INTERNAL_ERROR, "Failed to create MTLBuffer");
        return false;
    }

    internal_buffer->mtl_buffer = mtl_buffer;

    return true;
}