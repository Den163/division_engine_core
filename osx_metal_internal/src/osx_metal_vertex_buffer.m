#include <MetalKit/MetalKit.h>

#include "division_engine_core/context.h"
#include "division_engine_core/platform_internal/platform_vertex_buffer.h"
#include "division_engine_core/renderer.h"
#include "division_engine_core/utility.h"
#include "division_engine_core/vertex_buffer.h"
#include "osx_vertex_buffer.h"
#include "osx_window_context.h"

#include <memory.h>
#include <string.h>

typedef struct
{
    MTLVertexFormat vertex_format;
    int32_t divide_by_components;
} DivisionToMslAttrTraits_;

static inline MTLPrimitiveType division_topology_to_mtl_type(
    DivisionContext* ctx, DivisionRenderTopology topology
);

static inline MTLVertexDescriptor* create_vertex_descriptor(
    DivisionContext* ctx,
    const DivisionVertexBuffer* vertex_buffer,
    const DivisionVertexBufferInternalPlatform_* vertex_buffer_impl
);

static inline void define_msl_vertex_attributes(
    DivisionContext* ctx,
    const DivisionVertexAttribute* attributes,
    const DivisionVertexAttributeSettings* attribute_settings,
    int32_t attribute_count,
    MTLVertexAttributeDescriptorArray* attr_desc_arr,
    size_t attributes_offset,
    int32_t buffer_index
);

static inline DivisionToMslAttrTraits_ get_vert_attr_msl_traits_(
    DivisionContext* ctx, DivisionShaderVariableType attrType
);

bool division_engine_internal_platform_vertex_buffer_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings
)
{
    ctx->vertex_buffer_context->buffers_impl = NULL;
    return true;
}

void division_engine_internal_platform_vertex_buffer_context_free(DivisionContext* ctx)
{
    DivisionVertexBufferSystemContext* vert_buffer_ctx = ctx->vertex_buffer_context;

    for (int i = 0; i < vert_buffer_ctx->buffers_count; i++)
    {
        DivisionVertexBufferInternalPlatform_* osx_vert_buffer =
            &vert_buffer_ctx->buffers_impl[i];
        osx_vert_buffer->mtl_index_buffer = nil;
        osx_vert_buffer->mtl_vertex_buffer = nil;
        osx_vert_buffer->mtl_vertex_descriptor = nil;
    }

    free(vert_buffer_ctx->buffers_impl);
}

bool division_engine_internal_platform_vertex_buffer_borrow_data_pointer(
    DivisionContext* ctx,
    uint32_t buffer_id,
    DivisionVertexBufferBorrowedData* out_borrow_data
)
{
    DivisionVertexBufferSystemContext* vertex_buffer_ctx = ctx->vertex_buffer_context;
    const DivisionVertexBuffer* vertex_buffer = &vertex_buffer_ctx->buffers[buffer_id];
    const DivisionVertexBufferInternalPlatform_* impl_buffer =
        &vertex_buffer_ctx->buffers_impl[buffer_id];
    void* ptr = [impl_buffer->mtl_vertex_buffer contents];

    out_borrow_data->index_data_ptr = [impl_buffer->mtl_index_buffer contents];
    out_borrow_data->vertex_data_ptr = ptr;
    out_borrow_data->instance_data_ptr =
        ptr + division_engine_vertex_buffer_vertices_bytes(vertex_buffer);

    return true;
}

void division_engine_internal_platform_vertex_buffer_return_data(
    DivisionContext* ctx,
    uint32_t buffer_id,
    DivisionVertexBufferBorrowedData* data_pointer
)
{
    const DivisionVertexBufferInternalPlatform_* impl_buffer =
        &ctx->vertex_buffer_context->buffers_impl[buffer_id];
    id<MTLBuffer> mtl_vert_buffer = impl_buffer->mtl_vertex_buffer;
    id<MTLBuffer> mtl_idx_buffer = impl_buffer->mtl_index_buffer;

    [mtl_vert_buffer didModifyRange:NSMakeRange(0, [mtl_vert_buffer length])];
    [mtl_idx_buffer didModifyRange:NSMakeRange(0, [mtl_idx_buffer length])];
}

void division_engine_internal_platform_vertex_buffer_free(
    DivisionContext* ctx, uint32_t buffer_id
)
{
    DivisionVertexBufferInternalPlatform_* vertex_buffer =
        &ctx->vertex_buffer_context->buffers_impl[buffer_id];
    vertex_buffer->mtl_vertex_buffer = nil;
    vertex_buffer->mtl_index_buffer = nil;
    vertex_buffer->mtl_vertex_descriptor = nil;
}

bool division_engine_internal_platform_vertex_buffer_realloc(
    DivisionContext* ctx, size_t new_size
)
{
    DivisionVertexBufferSystemContext* vert_buffer_ctx = ctx->vertex_buffer_context;
    vert_buffer_ctx->buffers_impl = realloc(
        vert_buffer_ctx->buffers_impl,
        sizeof(DivisionVertexBufferInternalPlatform_[new_size])
    );
    return vert_buffer_ctx->buffers_impl != NULL;
}

bool division_engine_internal_platform_vertex_buffer_impl_init_element(
    DivisionContext* ctx, uint32_t buffer_id
)
{
    DivisionOSXWindowContext* window_context = ctx->renderer_context->window_data;
    id<MTLDevice> device = window_context->app_delegate->viewDelegate->device;

    DivisionVertexBufferSystemContext* vert_buffer_ctx = ctx->vertex_buffer_context;
    const DivisionVertexBuffer* vertex_buffer = &vert_buffer_ctx->buffers[buffer_id];
    DivisionVertexBufferSize buffer_size = vertex_buffer->settings.size;
    DivisionVertexBufferInternalPlatform_* impl_buffer =
        &vert_buffer_ctx->buffers_impl[buffer_id];

    size_t idx_buffer_size = sizeof(uint32_t[buffer_size.index_count]);
    size_t vert_buffer_size =
        vertex_buffer->per_vertex_data_size * buffer_size.vertex_count +
        vertex_buffer->per_instance_data_size * buffer_size.instance_count;

    id<MTLBuffer> vert_buffer =
        [device newBufferWithLength:vert_buffer_size
                            options:MTLResourceStorageModeManaged];
    id<MTLBuffer> idx_buffer = [device newBufferWithLength:idx_buffer_size
                                                   options:MTLResourceStorageModeManaged];

    if (vert_buffer == nil || idx_buffer == nil)
    {
        DIVISION_THROW_INTERNAL_ERROR(ctx, "Failed to create MTLBuffer");
        return false;
    }

    MTLVertexDescriptor* vertex_descriptor =
        create_vertex_descriptor(ctx, vertex_buffer, NULL);

    if (vertex_descriptor == nil)
    {
        DIVISION_THROW_INTERNAL_ERROR(ctx, "Failed to create MTLVertexDescriptor");
        return false;
    }

    impl_buffer->mtl_vertex_buffer = vert_buffer;
    impl_buffer->mtl_index_buffer = idx_buffer;
    impl_buffer->mtl_vertex_descriptor = vertex_descriptor;
    impl_buffer->mtl_primitive_type =
        division_topology_to_mtl_type(ctx, vertex_buffer->settings.topology);

    return true;
}

DIVISION_EXPORT void division_engine_internal_platform_vertex_buffer_copy_data(
    DivisionContext* ctx, uint32_t src_buffer_id, uint32_t dst_buffer_id
)
{
    DivisionVertexBufferSystemContext* vb_ctx = ctx->vertex_buffer_context;
    const DivisionVertexBuffer* src_buff = &vb_ctx->buffers[src_buffer_id];
    const DivisionVertexBuffer* dst_buff = &vb_ctx->buffers[dst_buffer_id];
    const DivisionVertexBufferInternalPlatform_* src_buffer_impl =
        &vb_ctx->buffers_impl[src_buffer_id];
    DivisionVertexBufferInternalPlatform_* dst_buffer_impl =
        &vb_ctx->buffers_impl[dst_buffer_id];

    const void* src_vertex_ptr = [src_buffer_impl->mtl_vertex_buffer contents];
    void* dst_vertex_ptr = [dst_buffer_impl->mtl_vertex_buffer contents];
    size_t src_vert_bytes = division_engine_vertex_buffer_vertices_bytes(src_buff);
    size_t dst_vert_bytes = division_engine_vertex_buffer_vertices_bytes(dst_buff);

    memcpy(dst_vertex_ptr, src_vertex_ptr, DIVISION_MIN(src_vert_bytes, dst_vert_bytes));

    const void* src_instance_ptr = src_vertex_ptr + src_vert_bytes;
    void* dst_instance_ptr = dst_vertex_ptr + dst_vert_bytes;
    memcpy(
        dst_instance_ptr,
        src_instance_ptr,
        DIVISION_MIN(
            division_engine_vertex_buffer_instances_bytes(src_buff),
            division_engine_vertex_buffer_instances_bytes(dst_buff)
        )
    );

    memcpy(
        [dst_buffer_impl->mtl_index_buffer contents],
        [src_buffer_impl->mtl_index_buffer contents],
        DIVISION_MIN(
            division_engine_vertex_buffer_indices_bytes(src_buff),
            division_engine_vertex_buffer_indices_bytes(dst_buff)
        )
    );
}

DIVISION_EXPORT void division_engine_internal_platform_vertex_buffer_swap_data(
    DivisionContext* ctx, uint32_t src_id, uint32_t dst_id
)
{
    DivisionVertexBufferSystemContext* vb_ctx = ctx->vertex_buffer_context;
    DIVISION_SWAP(
        DivisionVertexBufferInternalPlatform_,
        vb_ctx->buffers_impl[src_id],
        vb_ctx->buffers_impl[dst_id]
    );
}

MTLPrimitiveType division_topology_to_mtl_type(
    DivisionContext* ctx, DivisionRenderTopology topology
)
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
        DIVISION_THROW_INTERNAL_ERROR(ctx, "Unknown topology");
        return 0;
    }
}

MTLVertexDescriptor* create_vertex_descriptor(
    DivisionContext* ctx,
    const DivisionVertexBuffer* vertex_buffer,
    const DivisionVertexBufferInternalPlatform_* vertex_buffer_impl
)
{
    MTLVertexDescriptor* descriptor = [MTLVertexDescriptor new];
    MTLVertexAttributeDescriptorArray* attrDescArray = [descriptor attributes];

    define_msl_vertex_attributes(
        ctx,
        vertex_buffer->per_vertex_attributes,
        vertex_buffer->settings.per_vertex_attributes,
        vertex_buffer->settings.per_vertex_attribute_count,
        attrDescArray,
        0,
        DIVISION_MTL_VERTEX_DATA_BUFFER_INDEX
    );

    MTLVertexBufferLayoutDescriptor* perVertexLayoutDesc = [[descriptor layouts]
        objectAtIndexedSubscript:DIVISION_MTL_VERTEX_DATA_BUFFER_INDEX];

    [perVertexLayoutDesc setStride:vertex_buffer->per_vertex_data_size];
    [perVertexLayoutDesc setStepFunction:MTLVertexStepFunctionPerVertex];

    if (vertex_buffer->settings.per_instance_attribute_count > 0)
    {
        define_msl_vertex_attributes(
            ctx,
            vertex_buffer->per_instance_attributes,
            vertex_buffer->settings.per_instance_attributes,
            vertex_buffer->settings.per_instance_attribute_count,
            attrDescArray,
            division_engine_vertex_buffer_vertices_bytes(vertex_buffer),
            DIVISION_MTL_VERTEX_DATA_INSTANCE_ARRAY_INDEX
        );

        MTLVertexBufferLayoutDescriptor* perInstanceLayoutDesc = [[descriptor layouts]
            objectAtIndexedSubscript:DIVISION_MTL_VERTEX_DATA_INSTANCE_ARRAY_INDEX];

        [perInstanceLayoutDesc setStride:vertex_buffer->per_instance_data_size];
        [perInstanceLayoutDesc setStepFunction:MTLVertexStepFunctionPerInstance];
    }

    return descriptor;
}

void define_msl_vertex_attributes(
    DivisionContext* ctx,
    const DivisionVertexAttribute* attributes,
    const DivisionVertexAttributeSettings* attribute_settings,
    int32_t attribute_count,
    MTLVertexAttributeDescriptorArray* attr_desc_arr,
    size_t attributes_offset,
    int32_t buffer_index
)
{
    for (int i = 0; i < attribute_count; i++)
    {
        const DivisionVertexAttribute* attr = &attributes[i];
        const DivisionVertexAttributeSettings* attr_setting = &attribute_settings[i];
        DivisionToMslAttrTraits_ traits =
            get_vert_attr_msl_traits_(ctx, attr_setting->type);
        size_t gl_attr_comp_count = (attr->component_count / traits.divide_by_components);
        size_t comp_size = attr->base_size * gl_attr_comp_count;
        size_t offset = attributes_offset + attr->offset;

        for (int comp_idx = 0; comp_idx < gl_attr_comp_count; comp_idx++)
        {
            MTLVertexAttributeDescriptor* attrDesc = [attr_desc_arr
                objectAtIndexedSubscript:attr_setting->location + comp_idx];
            [attrDesc setOffset:offset + comp_idx * comp_size];
            [attrDesc setBufferIndex:buffer_index];
            [attrDesc setFormat:traits.vertex_format];
        }
    }
}

DivisionToMslAttrTraits_ get_vert_attr_msl_traits_(
    DivisionContext* ctx, DivisionShaderVariableType attrType
)
{
    switch (attrType)
    {
    case DIVISION_FLOAT:
        return (DivisionToMslAttrTraits_){MTLVertexFormatFloat, 1};
    case DIVISION_INTEGER:
        return (DivisionToMslAttrTraits_){MTLVertexFormatInt, 1};
    case DIVISION_FVEC2:
        return (DivisionToMslAttrTraits_){MTLVertexFormatFloat2, 1};
    case DIVISION_FVEC3:
        return (DivisionToMslAttrTraits_){MTLVertexFormatFloat3, 1};
    case DIVISION_FVEC4:
        return (DivisionToMslAttrTraits_){MTLVertexFormatFloat4, 1};
    case DIVISION_FMAT4X4:
        return (DivisionToMslAttrTraits_){MTLVertexFormatFloat4, 4};
    case DIVISION_DOUBLE:
    default:
        DIVISION_THROW_INTERNAL_ERROR(ctx, "Unsupported attribute type");
        return (DivisionToMslAttrTraits_){0, 0};
    }
}
