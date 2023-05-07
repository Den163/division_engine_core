#include "division_engine/vertex_buffer.h"
#include "division_engine/platform_internal/platform_vertex_buffer.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct AttrTraits_
{
    int32_t base_size;
    int32_t component_count;
} AttrTraits_;

static inline AttrTraits_ division_attribute_get_traits(DivisionShaderVariableType attributeType);

static inline void gather_attributes_info(
    DivisionVertexAttributeSettings* attrs,
    int32_t attr_count,
    DivisionVertexAttribute* attributes,
    int32_t* per_vertex_data_size);

bool division_engine_internal_vertex_buffer_context_alloc(DivisionContext* ctx, const DivisionSettings* settings)
{
    ctx->vertex_buffer_context = malloc(sizeof(DivisionVertexBufferSystemContext));
    *ctx->vertex_buffer_context = (DivisionVertexBufferSystemContext) {
        .buffers = NULL,
        .buffers_impl = NULL,
        .buffers_count = 0
    };

    return division_engine_internal_platform_vertex_buffer_context_alloc(ctx, settings);
}

void division_engine_internal_vertex_buffer_context_free(DivisionContext* ctx)
{
    division_engine_internal_platform_vertex_buffer_context_free(ctx);

    DivisionVertexBufferSystemContext* vertex_buffer_ctx = ctx->vertex_buffer_context;

    for (int i = 0; i < vertex_buffer_ctx->buffers_count; i++)
    {
        free(vertex_buffer_ctx->buffers[i].attributes);
    }

    free(vertex_buffer_ctx->buffers);
    free(vertex_buffer_ctx);
}

int32_t division_engine_vertex_buffer_alloc(
    DivisionContext* ctx,
    DivisionVertexAttributeSettings* attrs,
    int32_t attr_count,
    int32_t vertex_count,
    DivisionRenderTopology render_topology
                                           )
{
    DivisionVertexBufferSystemContext* vertex_ctx = ctx->vertex_buffer_context;

    DivisionVertexBuffer vertex_buffer = {
        .attributes = malloc(sizeof(DivisionVertexAttribute[attr_count])),
        .attribute_count = attr_count,
    };

    int32_t per_vertex_data_size;
    gather_attributes_info(
        attrs, attr_count, vertex_buffer.attributes, &per_vertex_data_size);

    vertex_buffer.vertex_count = vertex_count;
    vertex_buffer.per_vertex_data_size = per_vertex_data_size;
    vertex_buffer.topology = render_topology;

    int32_t buffers_count = vertex_ctx->buffers_count;
    int32_t new_buffers_count = buffers_count + 1;
    vertex_ctx->buffers = realloc(vertex_ctx->buffers, sizeof(DivisionVertexBuffer[new_buffers_count]));

    vertex_ctx->buffers[buffers_count] = vertex_buffer;
    vertex_ctx->buffers_count++;

    division_engine_internal_platform_vertex_buffer_alloc(ctx);

    return vertex_ctx->buffers_count - 1;
}

void gather_attributes_info(
    DivisionVertexAttributeSettings* attrs,
    int32_t attr_count,
    DivisionVertexAttribute* attributes,
    int32_t* per_vertex_data_size
                           )
{
    *per_vertex_data_size = 0;

    for (int32_t i = 0; i < attr_count; i++)
    {
        DivisionVertexAttributeSettings at = attrs[i];
        AttrTraits_ attr_traits = division_attribute_get_traits(at.type);

        int32_t attr_size = attr_traits.base_size * attr_traits.component_count;
        int32_t offset = *per_vertex_data_size;
        *per_vertex_data_size += attr_size;

        attributes[i] = (DivisionVertexAttribute) {
            .location = at.location,
            .offset = offset,
            .base_size = attr_traits.base_size,
            .component_count = attr_traits.component_count,
            .type = at.type
        };
    }
}

AttrTraits_ division_attribute_get_traits(DivisionShaderVariableType attributeType)
{
    switch (attributeType)
    {
        case DIVISION_FLOAT:
            return (AttrTraits_) {4, 1};
        case DIVISION_DOUBLE:
            return (AttrTraits_) {8, 1};
        case DIVISION_INTEGER:
            return (AttrTraits_) {4, 1};
        case DIVISION_FVEC2:
            return (AttrTraits_) {4, 2};
        case DIVISION_FVEC3:
            return (AttrTraits_) {4, 3};
        case DIVISION_FVEC4:
            return (AttrTraits_) {4, 4};
        case DIVISION_FMAT4X4:
            return (AttrTraits_) {4, 16};
        default:
        {
            fprintf(stderr, "Unknown attribute type");
        }
    }
}

void* division_engine_vertex_buffer_borrow_data_pointer(DivisionContext* ctx, int32_t vertex_buffer)
{
    return division_engine_internal_platform_vertex_buffer_borrow_data_pointer(ctx, vertex_buffer);
}

void division_engine_vertex_buffer_return_data_pointer(DivisionContext* ctx, int32_t vertex_buffer, void* data_pointer)
{
    division_engine_internal_platform_vertex_buffer_return_data_pointer(ctx, vertex_buffer, data_pointer);
}