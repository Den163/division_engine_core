#include <MetalKit/MetalKit.h>

#include "division_engine_core/platform_internal/platform_vertex_buffer.h"
#include "division_engine_core/renderer.h"
#include "osx_window_context.h"
#include "osx_vertex_buffer.h"

typedef struct
{
    MTLVertexFormat vertex_format;
    int32_t divide_by_components;
} DivisionToMslAttrTraits_;

static inline MTLPrimitiveType division_topology_to_mtl_type(DivisionRenderTopology topology);

static inline MTLVertexDescriptor* create_vertex_descriptor(
    const DivisionVertexBuffer* vertex_buffer, const DivisionVertexBufferInternalPlatform_* vertex_buffer_impl);

static inline void define_msl_vertex_attributes(
    const DivisionVertexAttribute* attributes, int32_t attribute_count,
    MTLVertexAttributeDescriptorArray* attr_desc_arr, size_t attributes_offset, int32_t buffer_index);

static inline DivisionToMslAttrTraits_ get_vert_attr_msl_traits_(DivisionShaderVariableType attrType);

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
        osx_vert_buffer->mtl_index_buffer = nil;
        osx_vert_buffer->mtl_vertex_buffer = nil;
        osx_vert_buffer->mtl_vertex_descriptor = nil;
    }

    free(vert_buffer_ctx->buffers_impl);
}

bool division_engine_internal_platform_vertex_buffer_borrow_data_pointer(
    DivisionContext* ctx,
    uint32_t buffer_id,
    DivisionVertexBufferBorrowedData* out_borrow_data)
{
    DivisionVertexBufferSystemContext* vertex_buffer_ctx = ctx->vertex_buffer_context;
    const DivisionVertexBuffer* vertex_buffer = &vertex_buffer_ctx->buffers[buffer_id];
    const DivisionVertexBufferInternalPlatform_* impl_buffer = &vertex_buffer_ctx->buffers_impl[buffer_id];
    void* ptr = [impl_buffer->mtl_vertex_buffer contents];

    out_borrow_data->index_data_ptr = [impl_buffer->mtl_index_buffer contents];
    out_borrow_data->vertex_data_ptr = ptr;
    out_borrow_data->instance_data_ptr = ptr + vertex_buffer->vertex_count * vertex_buffer->per_vertex_data_size;

    return true;
}

void division_engine_internal_platform_vertex_buffer_return_data_pointer(
    DivisionContext* ctx, uint32_t buffer_id, DivisionVertexBufferBorrowedData* data_pointer)
{
    const DivisionVertexBufferInternalPlatform_* impl_buffer = &ctx->vertex_buffer_context->buffers_impl[buffer_id];
    id <MTLBuffer> mtl_vert_buffer = impl_buffer->mtl_vertex_buffer;
    id <MTLBuffer> mtl_idx_buffer = impl_buffer->mtl_index_buffer;

    [mtl_vert_buffer didModifyRange:NSMakeRange(0, [mtl_vert_buffer length])];
    [mtl_idx_buffer didModifyRange:NSMakeRange(0, [mtl_idx_buffer length])];
}

void division_engine_internal_platform_vertex_buffer_free(DivisionContext* ctx, uint32_t buffer_id)
{
    DivisionVertexBufferInternalPlatform_* vertex_buffer = &ctx->vertex_buffer_context->buffers_impl[buffer_id];
    vertex_buffer->mtl_vertex_buffer = nil;
    vertex_buffer->mtl_index_buffer = nil;
    vertex_buffer->mtl_vertex_descriptor = nil;
}

bool division_engine_internal_platform_vertex_buffer_realloc(DivisionContext* ctx, size_t new_size)
{
    DivisionVertexBufferSystemContext* vert_buffer_ctx = ctx->vertex_buffer_context;
    vert_buffer_ctx->buffers_impl = realloc(
        vert_buffer_ctx->buffers_impl,
        sizeof(DivisionVertexBufferInternalPlatform_[new_size])
    );
    return vert_buffer_ctx->buffers_impl != NULL;
}

bool division_engine_internal_platform_vertex_buffer_impl_init_element(DivisionContext* ctx, uint32_t buffer_id)
{
    DivisionOSXWindowContext* window_context = ctx->renderer_context->window_data;
    id <MTLDevice> device = window_context->app_delegate->viewDelegate->device;
    DivisionVertexBufferSystemContext* vert_buffer_ctx = ctx->vertex_buffer_context;
    const DivisionVertexBuffer* vertex_buffer = &vert_buffer_ctx->buffers[buffer_id];
    DivisionVertexBufferInternalPlatform_* impl_buffer = &vert_buffer_ctx->buffers_impl[buffer_id];

    size_t idx_buffer_size = sizeof(uint32_t[vertex_buffer->index_count]);
    size_t vert_buffer_size = vertex_buffer->per_vertex_data_size * vertex_buffer->vertex_count +
                              vertex_buffer->per_instance_data_size * vertex_buffer->instance_count;

    id <MTLBuffer> vert_buffer = [device newBufferWithLength:vert_buffer_size options:MTLResourceStorageModeManaged];
    id <MTLBuffer> idx_buffer = [device newBufferWithLength:idx_buffer_size options:MTLResourceStorageModeManaged];

    if (vert_buffer == nil || idx_buffer == nil)
    {
        ctx->error_callback(DIVISION_INTERNAL_ERROR, "Failed to create MTLBuffer");
        return false;
    }

    MTLVertexDescriptor* vertex_descriptor = create_vertex_descriptor(vertex_buffer, NULL);

    if (vertex_descriptor == nil)
    {
        ctx->error_callback(DIVISION_INTERNAL_ERROR, "Failed to create MTLVertexDescriptor");
        return false;
    }

    impl_buffer->mtl_vertex_buffer = vert_buffer;
    impl_buffer->mtl_index_buffer = idx_buffer;
    impl_buffer->mtl_vertex_descriptor = vertex_descriptor;
    impl_buffer->mtl_primitive_type = division_topology_to_mtl_type(vertex_buffer->topology);

    return true;
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
    }
}

MTLVertexDescriptor* create_vertex_descriptor(
    const DivisionVertexBuffer* vertex_buffer, const DivisionVertexBufferInternalPlatform_* vertex_buffer_impl)
{
    MTLVertexDescriptor* descriptor = [MTLVertexDescriptor new];
    MTLVertexAttributeDescriptorArray* attrDescArray = [descriptor attributes];

    define_msl_vertex_attributes(
        vertex_buffer->per_vertex_attributes,
        vertex_buffer->per_vertex_attribute_count,
        attrDescArray,
        0,
        DIVISION_MTL_VERTEX_DATA_BUFFER_INDEX
    );

    MTLVertexBufferLayoutDescriptor* perVertexLayoutDesc =
        [[descriptor layouts] objectAtIndexedSubscript:DIVISION_MTL_VERTEX_DATA_BUFFER_INDEX];

    [perVertexLayoutDesc setStride:vertex_buffer->per_vertex_data_size];
    [perVertexLayoutDesc setStepFunction:MTLVertexStepFunctionPerVertex];

    if (vertex_buffer->per_instance_attribute_count > 0)
    {
        define_msl_vertex_attributes(
            vertex_buffer->per_instance_attributes,
            vertex_buffer->per_instance_attribute_count,
            attrDescArray,
            vertex_buffer->per_vertex_data_size * vertex_buffer->vertex_count,
            DIVISION_MTL_VERTEX_DATA_INSTANCE_ARRAY_INDEX
        );

        MTLVertexBufferLayoutDescriptor* perInstanceLayoutDesc =
            [[descriptor layouts] objectAtIndexedSubscript:DIVISION_MTL_VERTEX_DATA_INSTANCE_ARRAY_INDEX];

        [perInstanceLayoutDesc setStride:vertex_buffer->per_instance_data_size];
        [perInstanceLayoutDesc setStepFunction:MTLVertexStepFunctionPerInstance];
    }

    return descriptor;
}

void define_msl_vertex_attributes(
    const DivisionVertexAttribute* attributes, int32_t attribute_count,
    MTLVertexAttributeDescriptorArray* attr_desc_arr, size_t attributes_offset, int32_t buffer_index)
{
    for (int i = 0; i < attribute_count; i++)
    {
        const DivisionVertexAttribute* attr = &attributes[i];
        DivisionToMslAttrTraits_ traits = get_vert_attr_msl_traits_(attr->type);
        size_t gl_attr_comp_count = (attr->component_count / traits.divide_by_components);
        size_t comp_size = attr->base_size * gl_attr_comp_count;
        size_t offset = attributes_offset + attr->offset;

        for (int comp_idx = 0; comp_idx < gl_attr_comp_count; comp_idx++)
        {
            MTLVertexAttributeDescriptor* attrDesc = [attr_desc_arr objectAtIndexedSubscript:attr->location + comp_idx];
            [attrDesc setOffset:offset + comp_idx * comp_size];
            [attrDesc setBufferIndex:buffer_index];
            [attrDesc setFormat:traits.vertex_format];
        }
    }
}

DivisionToMslAttrTraits_ get_vert_attr_msl_traits_(DivisionShaderVariableType attrType)
{
    switch (attrType)
    {
        case DIVISION_FLOAT:
            return (DivisionToMslAttrTraits_) {MTLVertexFormatFloat, 1};
        case DIVISION_INTEGER:
            return (DivisionToMslAttrTraits_) {MTLVertexFormatInt, 1};
        case DIVISION_FVEC2:
            return (DivisionToMslAttrTraits_) {MTLVertexFormatFloat2, 1};
        case DIVISION_FVEC3:
            return (DivisionToMslAttrTraits_) {MTLVertexFormatFloat3, 1};
        case DIVISION_FVEC4:
            return (DivisionToMslAttrTraits_) {MTLVertexFormatFloat4, 1};
        case DIVISION_FMAT4X4:
            return (DivisionToMslAttrTraits_) {MTLVertexFormatFloat4, 4};
        case DIVISION_DOUBLE:
        default:
            fprintf(stderr, "Unsupported attribute format");
            exit(1);
    }
}
