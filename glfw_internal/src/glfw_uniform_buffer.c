#include "division_engine/platform_internal/platform_uniform_buffer.h"

#include <stdlib.h>
#include "glfw_uniform_buffer.h"

static inline GLuint get_gl_uniform_buffer(const DivisionContext* ctx, int32_t division_buffer)
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

void division_engine_internal_platform_uniform_buffer_alloc(
    DivisionContext* ctx, DivisionUniformBuffer buffer)
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
    uniform_buffer_ctx->uniform_buffers_impl[uniform_buffer_ctx->uniform_buffer_count - 1] =
        (DivisionUniformBufferInternal_) { .gl_buffer = gl_buff };
}

void* division_engine_internal_platform_uniform_buffer_borrow_data_pointer(
    DivisionContext* ctx, int32_t buffer)
{
    return glMapNamedBuffer(get_gl_uniform_buffer(ctx, buffer), GL_READ_WRITE);
}

void division_engine_internal_platform_uniform_buffer_return_data_pointer(
    DivisionContext* ctx, int32_t buffer, void* data_pointer)
{
    glUnmapNamedBuffer(get_gl_uniform_buffer(ctx, buffer));
}
