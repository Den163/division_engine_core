#include "division_engine_core/vertex_buffer.h"
#include "division_engine_core/platform_internal/platform_vertex_buffer.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct AttrTraits_
{
    int32_t base_size;
    int32_t component_count;
} AttrTraits_;

static inline AttrTraits_ division_attribute_get_traits(DivisionShaderVariableType attributeType);
static inline void
free_buffer_data_and_handle_error(DivisionContext* ctx, DivisionVertexBuffer* buffer, uint32_t buffer_id);

static inline bool alloc_vert_attrs_(
    const DivisionVertexAttributeSettings* input_attributes,
    int32_t attr_count,
    DivisionVertexAttribute** output_attributes,
    size_t* output_all_attributes_data_size);

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
        free(vertex_buffer_ctx->buffers[i].per_vertex_attributes);
    }

    free(vertex_buffer_ctx->buffers);
    free(vertex_buffer_ctx);
}

bool division_engine_vertex_buffer_alloc(
    DivisionContext* ctx, const DivisionVertexBufferSettings* vertex_buffer_settings, uint32_t* out_vertex_buffer_id)
{
    DivisionVertexBufferSystemContext* vertex_ctx = ctx->vertex_buffer_context;

    uint32_t vertex_buffer_id = division_unordered_id_table_insert(&vertex_ctx->id_table);

    DivisionVertexBuffer vertex_buffer = {
        .per_vertex_attributes = NULL,
        .per_vertex_attribute_count = vertex_buffer_settings->per_vertex_attribute_count,
        .per_instance_attributes = NULL,
        .per_instance_attribute_count = vertex_buffer_settings->per_instance_attribute_count,
        .vertex_count = vertex_buffer_settings->vertex_count,
        .instance_count = vertex_buffer_settings->instance_count,
        .topology = vertex_buffer_settings->topology
    };

    if (!alloc_vert_attrs_(
        vertex_buffer_settings->per_vertex_attributes, vertex_buffer_settings->per_vertex_attribute_count,
        &vertex_buffer.per_vertex_attributes, &vertex_buffer.per_vertex_data_size
    ))
    {
        free_buffer_data_and_handle_error(ctx, &vertex_buffer, vertex_buffer_id);
        return false;
    }

    if (!alloc_vert_attrs_(
        vertex_buffer_settings->per_instance_attributes, vertex_buffer_settings->per_instance_attribute_count,
        &vertex_buffer.per_instance_attributes, &vertex_buffer.per_instance_data_size
    ))
    {
        free_buffer_data_and_handle_error(ctx, &vertex_buffer, vertex_buffer_id);
        return false;
    }

    if (!division_engine_internal_platform_vertex_buffer_alloc(ctx, vertex_buffer_id, &vertex_buffer))
    {
        free_buffer_data_and_handle_error(ctx, &vertex_buffer, vertex_buffer_id);
        return false;
    }

    if (vertex_buffer_id >= vertex_ctx->buffers_count) {
        vertex_ctx->buffers_count = vertex_buffer_id + 1;
        vertex_ctx->buffers = realloc(vertex_ctx->buffers, sizeof(DivisionVertexBuffer[vertex_ctx->buffers_count]));

        if (vertex_ctx->buffers == NULL)
        {
            free_buffer_data_and_handle_error(ctx, &vertex_buffer, vertex_buffer_id);
            return false;
        }
    }

    vertex_ctx->buffers[vertex_buffer_id] = vertex_buffer;

    *out_vertex_buffer_id = vertex_buffer_id;
    return true;
}

bool alloc_vert_attrs_(
    const DivisionVertexAttributeSettings* input_attributes,
    int32_t attr_count,
    DivisionVertexAttribute** output_attributes,
    size_t* output_all_attributes_data_size
                      )
{
    *output_attributes = NULL;
    *output_all_attributes_data_size = 0;

    if (attr_count <= 0) return true;

    size_t all_attr_data_size = 0;

    DivisionVertexAttribute* attrs = malloc(sizeof(DivisionVertexAttribute[attr_count]));
    if (attrs == NULL) return false;

    for (int32_t i = 0; i < attr_count; i++)
    {
        DivisionVertexAttributeSettings at = input_attributes[i];
        AttrTraits_ attr_traits = division_attribute_get_traits(at.type);

        int32_t attr_size = attr_traits.base_size * attr_traits.component_count;
        int32_t offset = (int32_t) all_attr_data_size;
        all_attr_data_size += attr_size;

        attrs[i] = (DivisionVertexAttribute) {
            .location = at.location,
            .offset = offset,
            .base_size = attr_traits.base_size,
            .component_count = attr_traits.component_count,
            .type = at.type
        };
    }

    *output_attributes = attrs;
    *output_all_attributes_data_size = all_attr_data_size;
    return true;
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

void free_buffer_data_and_handle_error(DivisionContext* ctx, DivisionVertexBuffer* buffer, uint32_t buffer_id)
{
    division_unordered_id_table_remove(&ctx->vertex_buffer_context->id_table, buffer_id);
    free(buffer->per_vertex_attributes);
    free(buffer->per_instance_attributes);
    ctx->error_callback(DIVISION_INTERNAL_ERROR, "Division error: Failed to alloc a Vertex Buffer");
}

void division_engine_vertex_buffer_free(DivisionContext* ctx, uint32_t vertex_buffer_id)
{
    division_engine_internal_platform_vertex_buffer_free(ctx, vertex_buffer_id);
    DivisionVertexBuffer* vertex_buffer = &ctx->vertex_buffer_context->buffers[vertex_buffer_id];
    for (int i = 0; i < vertex_buffer->per_vertex_attribute_count; i++)
    {
        free(vertex_buffer->per_vertex_attributes);
        free(vertex_buffer->per_instance_attributes);
        vertex_buffer->per_vertex_attributes = NULL;
    }

    division_unordered_id_table_remove(&ctx->vertex_buffer_context->id_table, vertex_buffer_id);
}

void* division_engine_vertex_buffer_borrow_per_vertex_data_pointer(DivisionContext* ctx, uint32_t vertex_buffer)
{
    return division_engine_internal_platform_vertex_buffer_borrow_per_vertex_data_pointer(ctx, vertex_buffer);
}

void division_engine_vertex_buffer_return_per_vertex_data_pointer(DivisionContext* ctx, uint32_t vertex_buffer, void* data_pointer)
{
    division_engine_internal_platform_vertex_buffer_return_per_vertex_data_pointer(ctx, vertex_buffer, data_pointer);
}

void* division_engine_vertex_buffer_borrow_per_instance_data_pointer(DivisionContext* ctx, uint32_t vertex_buffer)
{
    return division_engine_internal_platform_vertex_buffer_borrow_per_instance_data_pointer(ctx, vertex_buffer);
}

void division_engine_vertex_buffer_return_per_instance_data_pointer(
    DivisionContext* ctx, uint32_t vertex_buffer, void* data_pointer)
{
    division_engine_internal_platform_vertex_buffer_return_per_instance_data_pointer(ctx, vertex_buffer, data_pointer);
}
