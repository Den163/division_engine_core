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

    division_unordered_id_table_alloc(&ctx->vertex_buffer_context->id_table, 10);

    return division_engine_internal_platform_vertex_buffer_context_alloc(ctx, settings);
}

void division_engine_internal_vertex_buffer_context_free(DivisionContext* ctx)
{
    division_engine_internal_platform_vertex_buffer_context_free(ctx);

    DivisionVertexBufferSystemContext* vertex_buffer_ctx = ctx->vertex_buffer_context;
    division_unordered_id_table_free(&vertex_buffer_ctx->id_table);

    for (int i = 0; i < vertex_buffer_ctx->buffers_count; i++)
    {
        free(vertex_buffer_ctx->buffers[i].attributes);
    }

    free(vertex_buffer_ctx->buffers);
    free(vertex_buffer_ctx);
}

bool division_engine_vertex_buffer_alloc(
    DivisionContext* ctx,
    DivisionVertexAttributeSettings* attrs,
    int32_t attr_count,
    int32_t vertex_count,
    DivisionRenderTopology render_topology,
    uint32_t* out_vertex_buffer_id)
{
    DivisionVertexBufferSystemContext* vertex_ctx = ctx->vertex_buffer_context;

    uint32_t vertex_buffer_id = division_unordered_id_table_insert(&vertex_ctx->id_table);
    DivisionVertexBuffer vertex_buffer = {
        .attributes = malloc(sizeof(DivisionVertexAttribute[attr_count])),
        .attribute_count = attr_count,
    };

    if (vertex_buffer.attributes == NULL)
    {
        division_unordered_id_table_remove(&vertex_ctx->id_table, vertex_buffer_id);
        ctx->error_callback(DIVISION_INTERNAL_ERROR, "Failed to reallocate Attributes array");
        return false;
    }

    int32_t per_vertex_data_size;
    gather_attributes_info(
        attrs, attr_count, vertex_buffer.attributes, &per_vertex_data_size);

    vertex_buffer.vertex_count = vertex_count;
    vertex_buffer.per_vertex_data_size = per_vertex_data_size;
    vertex_buffer.topology = render_topology;

    if (!division_engine_internal_platform_vertex_buffer_alloc(ctx, vertex_buffer_id, &vertex_buffer))
    {
        division_unordered_id_table_remove(&vertex_ctx->id_table, vertex_buffer_id);
        free(vertex_buffer.attributes);
        return false;
    }

    if (vertex_buffer_id >= vertex_ctx->buffers_count) {
        vertex_ctx->buffers_count = vertex_buffer_id + 1;
        vertex_ctx->buffers = realloc(vertex_ctx->buffers, sizeof(DivisionVertexBuffer[vertex_ctx->buffers_count]));

        if (vertex_ctx->buffers == NULL)
        {
            division_unordered_id_table_remove(&vertex_ctx->id_table, vertex_buffer_id);
            ctx->error_callback(DIVISION_INTERNAL_ERROR, "Failed to reallocate Vertex Buffers array");
            return false;
        }
    }

    vertex_ctx->buffers[vertex_buffer_id] = vertex_buffer;

    *out_vertex_buffer_id = vertex_buffer_id;
    return true;
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

void division_engine_vertex_buffer_free(DivisionContext* ctx, uint32_t vertex_buffer_id)
{
    division_engine_internal_platform_vertex_buffer_free(ctx, vertex_buffer_id);
    DivisionVertexBuffer* vertex_buffer = &ctx->vertex_buffer_context->buffers[vertex_buffer_id];
    for (int i = 0; i < vertex_buffer->attribute_count; i++)
    {
        free(vertex_buffer->attributes);
        vertex_buffer->attributes = NULL;
    }

    division_unordered_id_table_remove(&ctx->vertex_buffer_context->id_table, vertex_buffer_id);
}

void* division_engine_vertex_buffer_borrow_data_pointer(DivisionContext* ctx, uint32_t vertex_buffer)
{
    return division_engine_internal_platform_vertex_buffer_borrow_data_pointer(ctx, vertex_buffer);
}

void division_engine_vertex_buffer_return_data_pointer(DivisionContext* ctx, uint32_t vertex_buffer, void* data_pointer)
{
    division_engine_internal_platform_vertex_buffer_return_data_pointer(ctx, vertex_buffer, data_pointer);
}
