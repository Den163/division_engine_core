#include "division_engine_core/vertex_buffer.h"
#include "division_engine_core/context.h"
#include "division_engine_core/platform_internal/platform_vertex_buffer.h"
#include "division_engine_core/types/vertex_buffer.h"
#include "division_engine_core/utility.h"

#include <memory.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct AttrTraits_
{
    int32_t base_size;
    int32_t component_count;
} AttrTraits_;

static inline AttrTraits_ division_attribute_get_traits(
    DivisionContext* ctx, DivisionShaderVariableType attributeType
);
static inline void free_buffer_data_and_handle_error(
    DivisionContext* ctx, DivisionVertexBuffer* buffer, uint32_t buffer_id
);

static inline bool alloc_vert_attrs_(
    DivisionContext* ctx,
    const DivisionVertexAttributeSettings* input_attributes,
    int32_t attr_count,
    DivisionVertexAttribute** output_attributes,
    DivisionVertexAttributeSettings** output_settings,
    size_t* output_all_attributes_data_size
);

bool division_engine_vertex_buffer_system_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings
)
{
    ctx->vertex_buffer_context = malloc(sizeof(DivisionVertexBufferSystemContext));
    *ctx->vertex_buffer_context = (DivisionVertexBufferSystemContext){
        .buffers = NULL,
        .buffers_impl = NULL,
        .buffers_count = 0,
    };

    division_unordered_id_table_alloc(&ctx->vertex_buffer_context->id_table, 10);

    return division_engine_internal_platform_vertex_buffer_context_alloc(ctx, settings);
}

void division_engine_vertex_buffer_system_context_free(DivisionContext* ctx)
{
    division_engine_internal_platform_vertex_buffer_context_free(ctx);

    DivisionVertexBufferSystemContext* vertex_buffer_ctx = ctx->vertex_buffer_context;
    division_unordered_id_table_free(&vertex_buffer_ctx->id_table);

    for (int i = 0; i < vertex_buffer_ctx->buffers_count; i++)
    {
        DivisionVertexBuffer* buff = &vertex_buffer_ctx->buffers[i];
        free(buff->per_vertex_attributes);
        free(buff->per_instance_attributes);
        free(buff->settings.per_vertex_attributes);
        free(buff->settings.per_instance_attributes);
    }

    free(vertex_buffer_ctx->buffers);
    free(vertex_buffer_ctx);
}

bool division_engine_vertex_buffer_alloc(
    DivisionContext* ctx,
    const DivisionVertexBufferConstSettings* vertex_buffer_settings,
    uint32_t* out_vertex_buffer_id
)
{
    DivisionVertexBufferSystemContext* vertex_ctx = ctx->vertex_buffer_context;

    uint32_t vertex_buffer_id = division_unordered_id_table_new_id(&vertex_ctx->id_table);

    DivisionVertexBuffer vertex_buffer = {
        .per_vertex_attributes = NULL,
        .per_instance_attributes = NULL,
        .settings =
            {
                .size = vertex_buffer_settings->size,
                .topology = vertex_buffer_settings->topology,
                .per_vertex_attribute_count =
                    vertex_buffer_settings->per_vertex_attribute_count,
                .per_instance_attribute_count =
                    vertex_buffer_settings->per_instance_attribute_count,
            },
    };
    vertex_buffer.settings.per_vertex_attributes = NULL;
    vertex_buffer.settings.per_instance_attributes = NULL;

    if (!alloc_vert_attrs_(
            ctx,
            vertex_buffer_settings->per_vertex_attributes,
            vertex_buffer_settings->per_vertex_attribute_count,
            &vertex_buffer.per_vertex_attributes,
            &vertex_buffer.settings.per_vertex_attributes,
            &vertex_buffer.per_vertex_data_size
        ))
    {
        free_buffer_data_and_handle_error(ctx, &vertex_buffer, vertex_buffer_id);
        return false;
    }

    if (!alloc_vert_attrs_(
            ctx,
            vertex_buffer_settings->per_instance_attributes,
            vertex_buffer_settings->per_instance_attribute_count,
            &vertex_buffer.per_instance_attributes,
            &vertex_buffer.settings.per_instance_attributes,
            &vertex_buffer.per_instance_data_size
        ))
    {
        free_buffer_data_and_handle_error(ctx, &vertex_buffer, vertex_buffer_id);
        return false;
    }

    if (vertex_buffer_id >= vertex_ctx->buffers_count)
    {
        uint32_t new_buffers_count = vertex_buffer_id + 1;
        vertex_ctx->buffers_count = new_buffers_count;
        vertex_ctx->buffers =
            realloc(vertex_ctx->buffers, sizeof(DivisionVertexBuffer[new_buffers_count]));

        if (vertex_ctx->buffers == NULL ||
            !division_engine_internal_platform_vertex_buffer_realloc(
                ctx, new_buffers_count
            ))
        {
            free_buffer_data_and_handle_error(ctx, &vertex_buffer, vertex_buffer_id);
            return false;
        }
    }

    *out_vertex_buffer_id = vertex_buffer_id;
    vertex_ctx->buffers[vertex_buffer_id] = vertex_buffer;
    return division_engine_internal_platform_vertex_buffer_impl_init_element(
        ctx, vertex_buffer_id
    );
}

void free_buffer_data_and_handle_error(
    DivisionContext* ctx, DivisionVertexBuffer* buffer, uint32_t buffer_id
)
{
    DivisionVertexBufferSettings* settings = &buffer->settings;

    division_unordered_id_table_remove_id(
        &ctx->vertex_buffer_context->id_table, buffer_id
    );

    free(buffer->per_vertex_attributes);
    free(buffer->per_instance_attributes);
    free(settings->per_vertex_attributes);
    free(settings->per_instance_attributes);

    buffer->per_vertex_attributes = buffer->per_instance_attributes = NULL;
    settings->per_vertex_attributes = settings->per_instance_attributes = NULL;

    ctx->lifecycle.error_callback(
        ctx, DIVISION_INTERNAL_ERROR, "Division error: Failed to alloc a Vertex Buffer"
    );
}

void division_engine_vertex_buffer_free(DivisionContext* ctx, uint32_t vertex_buffer_id)
{
    division_engine_internal_platform_vertex_buffer_free(ctx, vertex_buffer_id);
    DivisionVertexBuffer* vb = &ctx->vertex_buffer_context->buffers[vertex_buffer_id];
    DivisionVertexBufferSettings* settings = &vb->settings;

    for (int i = 0; i < vb->settings.per_vertex_attribute_count; i++)
    {
        free(vb->per_vertex_attributes);
        free(vb->per_instance_attributes);
        free(settings->per_vertex_attributes);
        free(settings->per_instance_attributes);

        vb->per_vertex_attributes = vb->per_instance_attributes = NULL;
        settings->per_vertex_attributes = settings->per_instance_attributes = NULL;
    }

    division_unordered_id_table_remove_id(
        &ctx->vertex_buffer_context->id_table, vertex_buffer_id
    );
}

bool division_engine_vertex_buffer_borrow_data(
    DivisionContext* ctx,
    uint32_t vertex_buffer,
    DivisionVertexBufferBorrowedData* out_borrow_data
)
{
    const DivisionVertexBuffer* buff =
        &ctx->vertex_buffer_context->buffers[vertex_buffer];

    out_borrow_data->size = buff->settings.size;

    return division_engine_internal_platform_vertex_buffer_borrow_data_pointer(
        ctx, vertex_buffer, out_borrow_data
    );
}

void division_engine_vertex_buffer_return_data(
    DivisionContext* ctx,
    uint32_t vertex_buffer,
    DivisionVertexBufferBorrowedData* borrow_data
)
{
    division_engine_internal_platform_vertex_buffer_return_data_pointer(
        ctx, vertex_buffer, borrow_data
    );
}

bool division_engine_vertex_buffer_resize(
    DivisionContext* ctx, uint32_t vertex_buffer_id, DivisionVertexBufferSize new_size
)
{
    DivisionVertexBufferSystemContext* vb_ctx = ctx->vertex_buffer_context;
    const DivisionVertexBuffer* src_buffer = &vb_ctx->buffers[vertex_buffer_id];
    DivisionVertexBufferSettings new_settings = src_buffer->settings;
    new_settings.size = new_size;

    uint32_t new_buffer_id;
    if (!division_engine_vertex_buffer_alloc(
            ctx, (DivisionVertexBufferConstSettings*) &new_settings, &new_buffer_id
        ))
    {
        DIVISION_THROW_INTERNAL_ERROR(ctx, "Failed to resize vertex buffer");
        return false;
    }

    division_engine_internal_platform_vertex_buffer_copy_data(
        ctx, vertex_buffer_id, new_buffer_id
    );

    DIVISION_SWAP(
        DivisionVertexBuffer,
        vb_ctx->buffers[vertex_buffer_id],
        vb_ctx->buffers[new_buffer_id]
    );
    division_engine_internal_platform_vertex_buffer_swap_data(
        ctx, vertex_buffer_id, new_buffer_id
    );
    division_engine_vertex_buffer_free(ctx, new_buffer_id);

    return true;
}

bool alloc_vert_attrs_(
    DivisionContext* ctx,
    const DivisionVertexAttributeSettings* input_attribute_settings,
    int32_t attr_count,
    DivisionVertexAttribute** output_attributes,
    DivisionVertexAttributeSettings** output_settings,
    size_t* output_all_attributes_data_size
)
{
    *output_attributes = NULL;
    *output_settings = NULL;
    *output_all_attributes_data_size = 0;

    if (attr_count <= 0)
    {
        return true;
    }

    size_t all_attr_data_size = 0;

    DivisionVertexAttribute* attrs = malloc(sizeof(DivisionVertexAttribute[attr_count]));

    const size_t attr_bytes = sizeof(DivisionVertexAttributeSettings[attr_count]);
    DivisionVertexAttributeSettings* attr_settings = malloc(attr_bytes);
    memcpy(attr_settings, input_attribute_settings, attr_bytes);

    if (attrs == NULL || attr_settings == NULL)
    {
        free(attrs);
        free(attr_settings);

        return false;
    }

    for (int32_t i = 0; i < attr_count; i++)
    {
        DivisionVertexAttributeSettings at = input_attribute_settings[i];
        AttrTraits_ attr_traits = division_attribute_get_traits(ctx, at.type);

        int32_t attr_size = attr_traits.base_size * attr_traits.component_count;
        int32_t offset = (int32_t)all_attr_data_size;
        all_attr_data_size += attr_size;

        attrs[i] = (DivisionVertexAttribute){
            .offset = offset,
            .base_size = attr_traits.base_size,
            .component_count = attr_traits.component_count,
        };
        attr_settings[i] =
            (DivisionVertexAttributeSettings){.location = at.location, .type = at.type};
    }

    *output_attributes = attrs;
    *output_settings = attr_settings;
    *output_all_attributes_data_size = all_attr_data_size;
    return true;
}

AttrTraits_ division_attribute_get_traits(
    DivisionContext* ctx, DivisionShaderVariableType attributeType
)
{
    switch (attributeType)
    {
    case DIVISION_FLOAT:
        return (AttrTraits_){4, 1};
    case DIVISION_DOUBLE:
        return (AttrTraits_){8, 1};
    case DIVISION_INTEGER:
        return (AttrTraits_){4, 1};
    case DIVISION_FVEC2:
        return (AttrTraits_){4, 2};
    case DIVISION_FVEC3:
        return (AttrTraits_){4, 3};
    case DIVISION_FVEC4:
        return (AttrTraits_){4, 4};
    case DIVISION_FMAT4X4:
        return (AttrTraits_){4, 16};
    default: {
        DIVISION_THROW_INTERNAL_ERROR(ctx, "Unknown attribute type");
    }
    }
}