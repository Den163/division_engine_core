#include <MetalKit/MetalKit.h>

#include "rendering/platform_internal/platform_vertex_buffer.h"
#include "rendering/renderer.h"
#include "osx_window_context.h"
#include "osx_vertex_buffer.h"

static inline MTLPrimitiveType division_topology_to_mtl_type(DivisionRenderTopology topology);

bool division_engine_internal_platform_vertex_buffer_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings)
{
    ctx->vertex_buffer_context->buffers_impl = NULL;
    return true;
}

void division_engine_internal_platform_vertex_buffer_context_free(DivisionContext* ctx)
{
    DivisionVertexBufferSystemContext* vert_buffer_ctx = ctx->vertex_buffer_context;

    for (int i = 0; i < vert_buffer_ctx->buffers_count; i++)
    {
        DivisionVertexBufferInternalPlatform_* osx_vert_buffer = &vert_buffer_ctx->buffers_impl[i];
        osx_vert_buffer->mtl_buffer = nil;
        osx_vert_buffer->mtl_vertex_descriptor = nil;
    }

    free(vert_buffer_ctx->buffers_impl);
}

bool division_engine_internal_platform_vertex_buffer_alloc(
    DivisionContext* ctx, uint32_t buffer_id, DivisionVertexBuffer* vertex_buffer)
{
    DivisionVertexBufferSystemContext* vert_buffer_ctx = ctx->vertex_buffer_context;
    DivisionOSXWindowContext* window_context = ctx->renderer_context->window_data;

    if (buffer_id >= vert_buffer_ctx->buffers_count)
    {
        vert_buffer_ctx->buffers_impl = realloc(
            vert_buffer_ctx->buffers_impl,
            sizeof(DivisionVertexBufferInternalPlatform_[buffer_id + 1])
        );
    }


    if (vert_buffer_ctx->buffers_impl == NULL)
    {
        ctx->error_callback(DIVISION_INTERNAL_ERROR, "Failed to reallocate Vertex Buffer Implementation array");
        return false;
    }

    DivisionVertexBufferInternalPlatform_* impl_buffer = &vert_buffer_ctx->buffers_impl[buffer_id];

    DivisionOSXViewDelegate* view_delegate = window_context->app_delegate->viewDelegate;
    id <MTLBuffer> buffer = [view_delegate
        createBufferWithSize:vertex_buffer->per_vertex_data_size * vertex_buffer->vertex_count];

    if (buffer == nil)
    {
        ctx->error_callback(DIVISION_INTERNAL_ERROR, "Failed to create MTLBuffer");
        return false;
    }

    MTLVertexDescriptor* vertex_descriptor = [view_delegate createVertexDescriptorForBuffer:vertex_buffer];

    if (vertex_descriptor == nil)
    {
        ctx->error_callback(DIVISION_INTERNAL_ERROR, "Failed to create MTLVertexDescriptor");
        return false;
    }

    impl_buffer->mtl_buffer = buffer;
    impl_buffer->mtl_vertex_descriptor = vertex_descriptor;
    impl_buffer->mtl_primitive_type = division_topology_to_mtl_type(vertex_buffer->topology);

    return true;
}

void* division_engine_internal_platform_vertex_buffer_borrow_data_pointer(DivisionContext* ctx, uint32_t vertex_buffer)
{
    return [ctx->vertex_buffer_context->buffers_impl[vertex_buffer].mtl_buffer contents];
}

void division_engine_internal_platform_vertex_buffer_return_data_pointer(
    DivisionContext* ctx, uint32_t vertex_buffer, void* data_pointer)
{
    id<MTLBuffer> mtl_buffer = ctx->vertex_buffer_context->buffers_impl[vertex_buffer].mtl_buffer;
    [mtl_buffer didModifyRange:NSMakeRange(0, [mtl_buffer length])];
}

void division_engine_internal_platform_vertex_buffer_free(DivisionContext* ctx, uint32_t buffer_id)
{
    DivisionVertexBufferInternalPlatform_* vertex_buffer = &ctx->vertex_buffer_context->buffers_impl[buffer_id];
    vertex_buffer->mtl_buffer = nil;
    vertex_buffer->mtl_vertex_descriptor = nil;
}

MTLPrimitiveType division_topology_to_mtl_type(DivisionRenderTopology topology)
{
    switch (topology)
    {
        case DIVISION_TOPOLOGY_TRIANGLES:
            return MTLPrimitiveTypeTriangle;
        case DIVISION_TOPOLOGY_POINTS:
            return MTLPrimitiveTypePoint;
        case DIVISION_TOPOLOGY_LINES:
            return MTLPrimitiveTypeLine;
        default:
            fprintf(stderr, "Unknown topology");
            return 0;
    };
}