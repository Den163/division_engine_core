#include "glfw_vertex_buffer.h"
#include "division_engine_core/context.h"
#include "division_engine_core/platform_internal/platform_vertex_buffer.h"
#include "division_engine_core/utility.h"
#include "division_engine_core/vertex_buffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct GlAttrTraits_
{
    GLenum type;
    int32_t divide_by_components;
} GlAttrTraits_;

static inline GlAttrTraits_ get_gl_attr_traits(
    DivisionContext* ctx, DivisionShaderVariableType attributeType
);
static inline GLenum topology_to_gl_type(DivisionContext* ctx, DivisionRenderTopology t);
static inline void enable_gl_attributes(
    DivisionContext* ctx,
    const DivisionVertexAttribute* attributes,
    const DivisionVertexAttributeSettings* attribute_settings,
    int32_t attribute_count,
    size_t attributes_offset,
    size_t attributes_data_size,
    bool enable_divisor
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
    DivisionVertexBufferSystemContext* vertex_buffer_ctx = ctx->vertex_buffer_context;

    free(vertex_buffer_ctx->buffers_impl);
}

GlAttrTraits_ get_gl_attr_traits(
    DivisionContext* ctx, DivisionShaderVariableType attributeType
)
{
    switch (attributeType)
    {
    case DIVISION_DOUBLE:
        return (GlAttrTraits_){GL_DOUBLE, 1};
    case DIVISION_INTEGER:
        return (GlAttrTraits_){GL_INT, 1};
    case DIVISION_FLOAT:
    case DIVISION_FVEC2:
    case DIVISION_FVEC3:
    case DIVISION_FVEC4:
        return (GlAttrTraits_){GL_FLOAT, 1};
    case DIVISION_FMAT4X4:
        return (GlAttrTraits_){GL_FLOAT, 4};
    default: {
        DIVISION_THROW_INTERNAL_ERROR(ctx, "Unknown attribute type");
        return (GlAttrTraits_){0, 0};
    }
    }
}

GLenum topology_to_gl_type(DivisionContext* ctx, DivisionRenderTopology t)
{
    switch (t)
    {
    case DIVISION_TOPOLOGY_TRIANGLES:
        return GL_TRIANGLES;
    case DIVISION_TOPOLOGY_LINES:
        return GL_LINES;
    case DIVISION_TOPOLOGY_POINTS:
        return GL_POINTS;
    default:
        DIVISION_THROW_INTERNAL_ERROR(ctx, "Unknown type of topology");
        exit(EXIT_FAILURE);
    }
}

bool division_engine_internal_platform_vertex_buffer_realloc(
    DivisionContext* ctx, size_t new_size
)
{
    DivisionVertexBufferSystemContext* vertex_ctx = ctx->vertex_buffer_context;
    vertex_ctx->buffers_impl = realloc(
        vertex_ctx->buffers_impl, sizeof(DivisionVertexBufferInternalPlatform_[new_size])
    );

    return vertex_ctx->buffers_impl != NULL;
}

bool division_engine_internal_platform_vertex_buffer_impl_init_element(
    DivisionContext* ctx, uint32_t buffer_id
)
{
    DivisionVertexBufferSystemContext* vertex_ctx = ctx->vertex_buffer_context;
    const DivisionVertexBuffer* vb = &vertex_ctx->buffers[buffer_id];
    const DivisionVertexBufferSettings* vb_settings = &vb->settings;
    const DivisionVertexBufferSize* vb_size = &vb_settings->size;

    DivisionVertexBufferInternalPlatform_ vertex_buffer_impl = {
        .gl_topology = topology_to_gl_type(ctx, vb->settings.topology),
    };

    glGenVertexArrays(1, &vertex_buffer_impl.gl_vao);
    glBindVertexArray(vertex_buffer_impl.gl_vao);

    glGenBuffers(1, &vertex_buffer_impl.gl_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_impl.gl_index_buffer);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        (GLsizei)sizeof(uint32_t[vb_size->index_count]),
        NULL,
        GL_DYNAMIC_DRAW
    );

    glGenBuffers(1, &vertex_buffer_impl.gl_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_impl.gl_vbo);

    size_t per_vertex_data_size = vb->per_vertex_data_size;
    size_t per_instance_data_size = vb->per_instance_data_size;
    size_t vertex_count = vb_size->vertex_count;
    size_t instance_count = vb_size->instance_count;

    enable_gl_attributes(
        ctx,
        vb->per_vertex_attributes,
        vb_settings->per_vertex_attributes,
        vb_settings->per_vertex_attribute_count,
        0,
        vb->per_vertex_data_size,
        false
    );

    if (vb_settings->per_instance_attribute_count > 0)
    {
        enable_gl_attributes(
            ctx,
            vb->per_instance_attributes,
            vb_settings->per_instance_attributes,
            vb_settings->per_instance_attribute_count,
            vb->per_vertex_data_size * vertex_count,
            vb->per_instance_data_size,
            true
        );
    }

    glBufferData(
        GL_ARRAY_BUFFER,
        (GLsizei)(division_engine_vertex_buffer_vertices_bytes(vb) +
                  division_engine_vertex_buffer_instances_bytes(vb)),
        NULL,
        GL_DYNAMIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    vertex_ctx->buffers_impl[buffer_id] = vertex_buffer_impl;

    return true;
}

void division_engine_internal_platform_vertex_buffer_free(
    DivisionContext* ctx, uint32_t buffer_id
)
{
    DivisionVertexBufferInternalPlatform_ buffer_impl =
        ctx->vertex_buffer_context->buffers_impl[buffer_id];
    glDeleteBuffers(1, &buffer_impl.gl_vbo);
    glDeleteBuffers(1, &buffer_impl.gl_index_buffer);
    glDeleteVertexArrays(1, &buffer_impl.gl_vao);

    buffer_impl.gl_vbo = 0;
    buffer_impl.gl_vao = 0;
    buffer_impl.gl_index_buffer = 0;
}

bool division_engine_internal_platform_vertex_buffer_borrow_data_pointer(
    DivisionContext* ctx,
    uint32_t buffer_id,
    DivisionVertexBufferBorrowedData* out_borrow_data
)
{
    DivisionVertexBufferSystemContext* vertex_buffer_ctx = ctx->vertex_buffer_context;
    DivisionVertexBufferInternalPlatform_* vb =
        &vertex_buffer_ctx->buffers_impl[buffer_id];
    const DivisionVertexBuffer* vertex_buffer = &vertex_buffer_ctx->buffers[buffer_id];

    glBindBuffer(GL_ARRAY_BUFFER, vb->gl_vbo);
    void* vbo_ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vb->gl_index_buffer);
    void* idx_ptr = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);

    out_borrow_data->vertex_data_ptr = vbo_ptr;
    out_borrow_data->instance_data_ptr =
        vbo_ptr + division_engine_vertex_buffer_vertices_bytes(vertex_buffer);

    out_borrow_data->index_data_ptr = idx_ptr;

    return true;
}

void division_engine_internal_platform_vertex_buffer_return_data_pointer(
    DivisionContext* ctx,
    uint32_t buffer_id,
    DivisionVertexBufferBorrowedData* out_borrow_data
)
{
    DivisionVertexBufferInternalPlatform_* vb =
        &ctx->vertex_buffer_context->buffers_impl[buffer_id];
    glBindBuffer(GL_ARRAY_BUFFER, vb->gl_vbo);
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vb->gl_index_buffer);
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void division_engine_internal_platform_vertex_buffer_copy_data(
    DivisionContext* ctx, uint32_t src_buffer, uint32_t dst_buffer
)
{
    DivisionVertexBufferSystemContext* vb_context = ctx->vertex_buffer_context;
    const DivisionVertexBuffer* src_vb = &vb_context->buffers[src_buffer];
    const DivisionVertexBufferInternalPlatform_* src_vb_impl =
        &vb_context->buffers_impl[src_buffer];
    DivisionVertexBuffer* dst_vb = &vb_context->buffers[dst_buffer];
    DivisionVertexBufferInternalPlatform_* dst_vb_impl =
        &vb_context->buffers_impl[dst_buffer];

    const size_t src_vertices_bytes =
        division_engine_vertex_buffer_vertices_bytes(src_vb);
    const size_t dst_vertices_bytes =
        division_engine_vertex_buffer_vertices_bytes(dst_vb);

    glCopyNamedBufferSubData(
        src_vb_impl->gl_vbo,
        dst_vb_impl->gl_vbo,
        0,
        0,
        DIVISION_MIN(src_vertices_bytes, dst_vertices_bytes)
    );

    glCopyNamedBufferSubData(
        src_vb_impl->gl_vbo,
        dst_vb_impl->gl_vbo,
        src_vertices_bytes,
        dst_vertices_bytes,
        DIVISION_MIN(
            division_engine_vertex_buffer_instances_bytes(src_vb),
            division_engine_vertex_buffer_instances_bytes(dst_vb)
        )
    );

    glCopyNamedBufferSubData(
        src_vb_impl->gl_index_buffer,
        dst_vb_impl->gl_index_buffer,
        0,
        0,
        DIVISION_MIN(
            division_engine_vertex_buffer_indices_bytes(src_vb),
            division_engine_vertex_buffer_indices_bytes(dst_vb)
        )
    );
}

void division_engine_internal_platform_vertex_buffer_swap_data(
    DivisionContext* ctx, uint32_t src_id, uint32_t dst_id
)
{
    DivisionVertexBufferSystemContext* vb_context = ctx->vertex_buffer_context;
    DIVISION_SWAP(
        DivisionVertexBufferInternalPlatform_,
        vb_context->buffers_impl[src_id],
        vb_context->buffers_impl[dst_id]
    );
}

void enable_gl_attributes(
    DivisionContext* ctx,
    const DivisionVertexAttribute* attributes,
    const DivisionVertexAttributeSettings* attribute_settings,
    int32_t attribute_count,
    size_t attributes_offset,
    size_t attributes_data_size,
    bool enable_divisor
)
{
    for (int32_t i = 0; i < attribute_count; i++)
    {
        const DivisionVertexAttribute* at = &attributes[i];
        const DivisionVertexAttributeSettings* setting = &attribute_settings[i];
        GlAttrTraits_ gl_attr_traits = get_gl_attr_traits(ctx, setting->type);
        int gl_comp_count = at->component_count / gl_attr_traits.divide_by_components;
        size_t gl_comp_size = gl_comp_count * at->base_size;
        size_t attr_base_offset = attributes_offset + at->offset;

        for (int comp_idx = 0; comp_idx < gl_attr_traits.divide_by_components; comp_idx++)
        {
            GLuint gl_location = setting->location + comp_idx;

            glEnableVertexAttribArray(gl_location);
            glVertexAttribPointer(
                gl_location,
                gl_comp_count,
                gl_attr_traits.type,
                GL_FALSE,
                (GLsizei)(attributes_data_size),
                (void*)(attr_base_offset + comp_idx * gl_comp_size)
            );

            if (enable_divisor)
            {
                glVertexAttribDivisor(gl_location, 1);
            }
        }
    }
}