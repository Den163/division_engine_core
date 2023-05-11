#include "rendering/platform_internal/platform_uniform_buffer.h"

#include <stdlib.h>
#include "glfw_uniform_buffer.h"

static inline GLuint get_gl_uniform_buffer(const DivisionContext* ctx, uint32_t division_buffer)
{
    return ctx->uniform_buffer_context->uniform_buffers_impl[division_buffer].gl_buffer;
}

bool division_engine_internal_platform_uniform_buffer_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings)
{
    ctx->uniform_buffer_context->uniform_buffers_impl = NULL;
    return true;
}

void division_engine_internal_platform_uniform_buffer_context_free(DivisionContext* ctx)
{
    free(ctx->uniform_buffer_context->uniform_buffers_impl);
}

bool division_engine_internal_platform_uniform_buffer_alloc(
    DivisionContext* ctx, DivisionUniformBuffer buffer, uint32_t buffer_id)
{
    GLuint gl_buff;
    glGenBuffers(1, &gl_buff);
    glBindBuffer(GL_UNIFORM_BUFFER, gl_buff);
    glNamedBufferData(gl_buff, (GLsizeiptr) buffer.data_bytes, NULL, GL_DYNAMIC_COPY);

    DivisionUniformBufferSystemContext* uniform_buffer_ctx = ctx->uniform_buffer_context;
    uniform_buffer_ctx->uniform_buffers_impl = realloc(
        uniform_buffer_ctx->uniform_buffers_impl,
        sizeof(DivisionUniformBufferInternal_[uniform_buffer_ctx->uniform_buffer_count])
    );

    if (uniform_buffer_ctx == NULL)
    {
        ctx->error_callback(DIVISION_INTERNAL_ERROR, "Failed to realloc Uniform Buffer Implementation array");
        return false;
    }

    uniform_buffer_ctx->uniform_buffers_impl[buffer_id] =
        (DivisionUniformBufferInternal_) { .gl_buffer = gl_buff };

    return true;
}

void division_engine_internal_platform_uniform_buffer_free(DivisionContext* ctx, uint32_t buffer_id)
{
    glDeleteBuffers(1, &ctx->uniform_buffer_context->uniform_buffers_impl[buffer_id].gl_buffer);
}

void* division_engine_internal_platform_uniform_buffer_borrow_data_pointer(
    DivisionContext* ctx, uint32_t buffer_id)
{
    return glMapNamedBuffer(get_gl_uniform_buffer(ctx, buffer_id), GL_READ_WRITE);
}

void division_engine_internal_platform_uniform_buffer_return_data_pointer(
    DivisionContext* ctx, uint32_t buffer_id, void* data_pointer)
{
    glUnmapNamedBuffer(get_gl_uniform_buffer(ctx, buffer_id));
}
