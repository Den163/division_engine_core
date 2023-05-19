#include "division_engine_core/platform_internal/platform_vertex_buffer.h"
#include "division_engine_core/render_pass.h"
#include "division_engine_core/vertex_buffer.h"
#include "glfw_vertex_buffer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct GlAttrTraits_ {
    GLenum type;
    int32_t divide_by_components;
} GlAttrTraits_;

static inline GlAttrTraits_ get_gl_attr_traits(DivisionShaderVariableType attributeType);
static inline GLenum topology_to_gl_type(DivisionRenderTopology t);
static inline void enable_gl_attributes(
    const DivisionVertexAttribute* attributes,
    int32_t attribute_count,
    size_t attributes_offset,
    size_t attributes_data_size,
    bool enable_divisor
);

bool division_engine_internal_platform_vertex_buffer_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings)
{
    return true;
}

void division_engine_internal_platform_vertex_buffer_context_free(DivisionContext* ctx)
{
    DivisionVertexBufferSystemContext* vertex_buffer_ctx = ctx->vertex_buffer_context;

    free(vertex_buffer_ctx->buffers_impl);
}

GlAttrTraits_ get_gl_attr_traits(DivisionShaderVariableType attributeType)
{
    switch (attributeType)
    {
        case DIVISION_DOUBLE: return (GlAttrTraits_) { GL_DOUBLE , 1 };
        case DIVISION_INTEGER: return (GlAttrTraits_) { GL_INT, 1 };
        case DIVISION_FLOAT:
        case DIVISION_FVEC2:
        case DIVISION_FVEC3:
        case DIVISION_FVEC4:
            return (GlAttrTraits_) { GL_FLOAT, 1 };
        case DIVISION_FMAT4X4:
            return (GlAttrTraits_) { GL_FLOAT, 4 };
        default:
        {
            fprintf(stderr, "Unknown attribute type");
        }
    }
}

GLenum topology_to_gl_type(DivisionRenderTopology t)
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
        {
            fprintf(stderr, "Unknown type of topology");
            exit(EXIT_FAILURE);
        }
    }
}

bool division_engine_internal_platform_vertex_buffer_realloc(DivisionContext* ctx, size_t new_size)
{
    DivisionVertexBufferSystemContext* vertex_ctx = ctx->vertex_buffer_context;
    vertex_ctx->buffers_impl = realloc(
        vertex_ctx->buffers_impl,
        sizeof(DivisionVertexBufferInternalPlatform_[new_size])
    );

    return vertex_ctx->buffers_impl != NULL;
}

bool division_engine_internal_platform_vertex_buffer_impl_init_element(DivisionContext* ctx, uint32_t buffer_id)
{
    DivisionVertexBufferSystemContext* vertex_ctx = ctx->vertex_buffer_context;
    DivisionVertexBuffer* vertex_buffer = &vertex_ctx->buffers[buffer_id];

    DivisionVertexBufferInternalPlatform_ vertex_buffer_impl = {
        .gl_topology = topology_to_gl_type(vertex_buffer->topology)
    };

    glGenVertexArrays(1, &vertex_buffer_impl.gl_vao);
    glBindVertexArray(vertex_buffer_impl.gl_vao);
    glGenBuffers(1, &vertex_buffer_impl.gl_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_impl.gl_vbo);

    size_t per_vertex_data_size = vertex_buffer->per_vertex_data_size;
    size_t per_instance_data_size = vertex_buffer->per_instance_data_size;
    size_t vertex_count = vertex_buffer->vertex_count;
    size_t instance_count = vertex_buffer->instance_count;

    enable_gl_attributes(
        vertex_buffer->per_vertex_attributes,
        vertex_buffer->per_vertex_attribute_count,
        0,
        vertex_buffer->per_vertex_data_size,
        false
    );

    if (vertex_buffer->per_instance_attribute_count > 0)
    {
        enable_gl_attributes(
            vertex_buffer->per_instance_attributes,
            vertex_buffer->per_instance_attribute_count,
            vertex_buffer->per_vertex_data_size * vertex_count,
            vertex_buffer->per_instance_data_size,
            true
        );
    }

    glBufferData(
        GL_ARRAY_BUFFER,
        (GLsizei) (per_vertex_data_size * vertex_count + per_instance_data_size * instance_count),
        NULL,
        GL_DYNAMIC_DRAW
    );

    vertex_ctx->buffers_impl[buffer_id] = vertex_buffer_impl;

    return true;
}

void division_engine_internal_platform_vertex_buffer_free(DivisionContext* ctx, uint32_t buffer_id)
{
    DivisionVertexBufferInternalPlatform_ buffer_impl = ctx->vertex_buffer_context->buffers_impl[buffer_id];
    glDeleteBuffers(1, &buffer_impl.gl_vbo);
    glDeleteVertexArrays(1, &buffer_impl.gl_vao);
}

void* division_engine_internal_platform_vertex_buffer_borrow_data_pointer(
    DivisionContext* ctx, uint32_t buffer_id)
{
    DivisionVertexBufferInternalPlatform_* vb = &ctx->vertex_buffer_context->buffers_impl[buffer_id];
    glBindBuffer(GL_ARRAY_BUFFER, vb->gl_vbo);
    return glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
}

void division_engine_internal_platform_vertex_buffer_return_data_pointer(
    DivisionContext* ctx, uint32_t buffer_id, void* data_pointer)
{
    DivisionVertexBufferInternalPlatform_* vb = &ctx->vertex_buffer_context->buffers_impl[buffer_id];
    glBindBuffer(GL_ARRAY_BUFFER, vb->gl_vbo);
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void enable_gl_attributes(
    const DivisionVertexAttribute* attributes,
    int32_t attribute_count,
    size_t attributes_offset,
    size_t attributes_data_size,
    bool enable_divisor
)
{
    for (int32_t i = 0; i < attribute_count; i++)
    {
        const DivisionVertexAttribute* at = &attributes[i];
        GlAttrTraits_ gl_attr_traits = get_gl_attr_traits(at->type);
        int gl_comp_count = at->component_count / gl_attr_traits.divide_by_components;
        size_t gl_comp_size = gl_comp_count * at->base_size;
        size_t attr_base_offset = attributes_offset + at->offset;

        for (int comp_idx = 0; comp_idx < gl_attr_traits.divide_by_components; comp_idx++ )
        {
            GLuint gl_location = at->location + comp_idx;

            glEnableVertexAttribArray(gl_location);
            glVertexAttribPointer(
                gl_location,
                gl_comp_count,
                gl_attr_traits.type,
                GL_FALSE,
                (GLsizei) (attributes_data_size),
                (void*) (attr_base_offset + comp_idx * gl_comp_size)
            );

            if (enable_divisor)
            {
                glVertexAttribDivisor(gl_location, 1);
            }
        }
    }
}