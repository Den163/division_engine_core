#include "division_engine_core/platform_internal/platform_uniform_buffer.h"

#include "glfw_uniform_buffer.h"
#include <stdlib.h>

static inline GLuint get_gl_uniform_buffer(
    const DivisionContext* ctx, uint32_t division_buffer
)
{
    return ctx->uniform_buffer_context->uniform_buffers_impl[division_buffer].gl_buffer;
}

bool division_engine_internal_platform_uniform_buffer_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings
)
{
    ctx->uniform_buffer_context->uniform_buffers_impl = NULL;
    return true;
}

void division_engine_internal_platform_uniform_buffer_context_free(DivisionContext* ctx)
{
    free(ctx->uniform_buffer_context->uniform_buffers_impl);
}

bool division_engine_internal_platform_uniform_buffer_realloc(
    DivisionContext* ctx, size_t new_size
)
{
    DivisionUniformBufferSystemContext* uniform_buffer_ctx = ctx->uniform_buffer_context;
    uniform_buffer_ctx->uniform_buffers_impl = realloc(
        uniform_buffer_ctx->uniform_buffers_impl,
        sizeof(DivisionUniformBufferInternal_[new_size])
    );
    return uniform_buffer_ctx->uniform_buffers_impl != NULL;
}

bool division_engine_internal_platform_uniform_buffer_impl_init_element(
    DivisionContext* ctx, uint32_t buffer_id
)
{
    DivisionUniformBufferSystemContext* uniform_buffer_ctx = ctx->uniform_buffer_context;
    DivisionUniformBufferDescriptor* buffer =
        &uniform_buffer_ctx->uniform_buffers[buffer_id];

    GLuint gl_buff;
    glGenBuffers(1, &gl_buff);
    glBindBuffer(GL_UNIFORM_BUFFER, gl_buff);
    glNamedBufferData(gl_buff, (GLsizeiptr)buffer->data_bytes, NULL, GL_DYNAMIC_COPY);

    uniform_buffer_ctx->uniform_buffers_impl[buffer_id] =
        (DivisionUniformBufferInternal_){.gl_buffer = gl_buff};

    return true;
}

void division_engine_internal_platform_uniform_buffer_free(
    DivisionContext* ctx, uint32_t buffer_id
)
{
    DivisionUniformBufferInternal_* buff =
        &ctx->uniform_buffer_context->uniform_buffers_impl[buffer_id];
    glDeleteBuffers(1, &buff->gl_buffer);
    buff->gl_buffer = 0;
}

void* division_engine_internal_platform_uniform_buffer_borrow_data_pointer(
    DivisionContext* ctx, uint32_t buffer_id
)
{
    return glMapNamedBuffer(get_gl_uniform_buffer(ctx, buffer_id), GL_READ_WRITE);
}

void division_engine_internal_platform_uniform_buffer_return_data_pointer(
    DivisionContext* ctx, uint32_t buffer_id, void* data_pointer
)
{
    glUnmapNamedBuffer(get_gl_uniform_buffer(ctx, buffer_id));
}
