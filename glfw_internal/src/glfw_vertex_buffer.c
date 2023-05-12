#include "division_engine_core/platform_internal/platform_vertex_buffer.h"
#include "division_engine_core/render_pass.h"
#include "division_engine_core/vertex_buffer.h"
#include "glfw_vertex_buffer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct VertexAttributeInternalPlatform_ {
    GLenum gl_type;
} VertexAttributeInternalPlatform_;

static inline GLenum division_attribute_to_gl_type(DivisionShaderVariableType attributeType);
static inline GLenum topology_to_gl_type(DivisionRenderTopology t);

bool division_engine_internal_platform_vertex_buffer_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings)
{
    return true;
}

void division_engine_internal_platform_vertex_buffer_context_free(DivisionContext* ctx)
{
    DivisionVertexBufferSystemContext* vertex_buffer_ctx = ctx->vertex_buffer_context;

    for (int i = 0; i < vertex_buffer_ctx->buffers_count; i++)
    {
        DivisionVertexBuffer* buffer = &vertex_buffer_ctx->buffers[i];

        free(buffer->attributes_impl);
    }

    free(vertex_buffer_ctx->buffers_impl);
}

bool division_engine_internal_platform_vertex_buffer_alloc(
    DivisionContext* ctx, uint32_t buffer_id, DivisionVertexBuffer* vertex_buffer)
{
    GLuint gl_buffer;
    glGenBuffers(1, &gl_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, gl_buffer);

    struct DivisionVertexBufferSystemContext* vertex_ctx = ctx->vertex_buffer_context;
    int attr_count = vertex_buffer->attribute_count;

    vertex_buffer->attributes_impl = malloc(sizeof(VertexAttributeInternalPlatform_[attr_count]));
    if (vertex_buffer->attributes_impl == NULL)
    {
        ctx->error_callback(DIVISION_INTERNAL_ERROR, "Failed to alloc Vertex Attribute Implementation array");
        return false;
    }

    if (buffer_id >= vertex_ctx->buffers_count)
    {
        vertex_ctx->buffers_impl = realloc(
            vertex_ctx->buffers_impl,
            sizeof(DivisionVertexBufferInternalPlatform_[buffer_id + 1])
        );

        if (vertex_ctx->buffers_impl == NULL)
        {
            ctx->error_callback(DIVISION_INTERNAL_ERROR, "Failed to realloc Vertex Buffer Implementation array");
            free(vertex_buffer->attributes_impl);
            return false;
        }
    }

    vertex_ctx->buffers_impl[buffer_id] = (DivisionVertexBufferInternalPlatform_) {
        .gl_buffer = gl_buffer,
        .gl_topology = topology_to_gl_type(vertex_buffer->topology)
    };

    int per_vertex_data_size = (int) vertex_buffer->per_vertex_data_size;

    for (int32_t i = 0; i < attr_count; i++)
    {
        DivisionVertexAttribute* at = &vertex_buffer->attributes[i];
        VertexAttributeInternalPlatform_ at_impl = {
            .gl_type = division_attribute_to_gl_type(vertex_buffer->attributes[i].type)
        };
        vertex_buffer->attributes_impl[i] = at_impl;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wint-to-pointer-cast"
        void* offset = (void*) at->offset;
#pragma clang diagnostic pop

        glVertexAttribPointer(
            at->location,
            at->component_count,
            at_impl.gl_type,
            GL_FALSE,
            (int) vertex_buffer->per_vertex_data_size,
            offset
        );
        glEnableVertexAttribArray(at->location);
    }

    glBufferData(
        GL_ARRAY_BUFFER,
        (GLsizei) (per_vertex_data_size * vertex_buffer->vertex_count),
        NULL,
        GL_DYNAMIC_DRAW
    );

    return true;
}

GLenum division_attribute_to_gl_type(DivisionShaderVariableType attributeType)
{
    switch (attributeType)
    {
        case DIVISION_DOUBLE:
            return GL_DOUBLE;
        case DIVISION_INTEGER:
            return GL_INT;
        case DIVISION_FLOAT:
        case DIVISION_FVEC2:
        case DIVISION_FVEC3:
        case DIVISION_FVEC4:
        case DIVISION_FMAT4X4:
            return GL_FLOAT;
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

void division_engine_internal_platform_vertex_buffer_free(DivisionContext* ctx, uint32_t buffer_id)
{
    glDeleteBuffers(1, &ctx->vertex_buffer_context->buffers_impl[buffer_id].gl_buffer);
}

void* division_engine_internal_platform_vertex_buffer_borrow_data_pointer(
    DivisionContext* ctx, uint32_t buffer_id)
{
    DivisionVertexBufferInternalPlatform_* vb = &ctx->vertex_buffer_context->buffers_impl[buffer_id];
    glBindBuffer(GL_ARRAY_BUFFER, vb->gl_buffer);
    return glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
}

void division_engine_internal_platform_vertex_buffer_return_data_pointer(
    DivisionContext* ctx, uint32_t buffer_id, void* data_pointer)
{
    DivisionVertexBufferInternalPlatform_* vb = &ctx->vertex_buffer_context->buffers_impl[buffer_id];
    glBindBuffer(GL_ARRAY_BUFFER, vb->gl_buffer);
    glUnmapBuffer(GL_ARRAY_BUFFER);
}
