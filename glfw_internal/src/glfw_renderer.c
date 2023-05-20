#define GLFW_INCLUDE_NONE
#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "division_engine_core/render_pass.h"
#include "division_engine_core/renderer.h"
#include "division_engine_core/texture.h"
#include "division_engine_core/uniform_buffer.h"
#include "division_engine_core/vertex_buffer.h"

#include "glfw_shader.h"
#include "glfw_texture.h"
#include "glfw_uniform_buffer.h"
#include "glfw_vertex_buffer.h"

#include "division_engine_core/platform_internal/platform_renderer.h"

static inline void renderer_draw(DivisionContext* ctx);
static inline void
bind_uniform_buffer(DivisionUniformBufferSystemContext* ctx, uint32_t buffer_id);

typedef struct DivisionWindowContextPlatformInternal_* DivisionWindowContextPlatformInternalPtr_;

bool division_engine_internal_platform_renderer_alloc(DivisionContext* ctx, const DivisionSettings* settings)
{
    glfwSetErrorCallback(settings->error_callback);

    if (!glfwInit())
    {
        settings->error_callback(0, "Failed to init GLFW");
        return false;
    }

    GLFWwindow* window = glfwCreateWindow(
        (int) settings->window_width,
        (int) settings->window_height,
        settings->window_title, NULL, NULL
    );

    if (!window)
    {
        settings->error_callback(0, "Can't create a new GLFW window");
        return false;
    }

    glfwMakeContextCurrent(window);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        settings->error_callback(0, "Failed to load GLAD");
        return false;
    }

    ctx->renderer_context->window_data = (DivisionWindowContextPlatformInternalPtr_) window;

    return true;
}

void division_engine_internal_platform_renderer_run_loop(DivisionContext* ctx, const DivisionSettings* settings)
{
    settings->init_callback(ctx);

    DivisionRendererSystemContext* renderer_context = ctx->renderer_context;
    GLFWwindow* window = (GLFWwindow*) renderer_context->window_data;
    double last_frame_time, current_time, delta_time;

    last_frame_time = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        current_time = glfwGetTime();
        delta_time = current_time - last_frame_time;

        if (delta_time >= 1 / 60.f)
        {
            delta_time = current_time - last_frame_time;
            last_frame_time = current_time;

            ctx->state.delta_time = delta_time;
            settings->update_callback(ctx);
            renderer_draw(ctx);
            glfwSwapBuffers(window);
        }

        glfwPollEvents();
    }
}

void division_engine_internal_platform_renderer_free(DivisionContext* ctx)
{
    glfwDestroyWindow((GLFWwindow*) ctx->renderer_context->window_data);
    glfwTerminate();
}

void renderer_draw(DivisionContext* ctx)
{
    DivisionRendererSystemContext* renderer_ctx = ctx->renderer_context;
    DivisionVertexBufferSystemContext* vert_buff_ctx = ctx->vertex_buffer_context;
    DivisionUniformBufferSystemContext* uniform_buff_ctx = ctx->uniform_buffer_context;
    DivisionRenderPassSystemContext* render_pass_ctx = ctx->render_pass_context;
    DivisionShaderSystemContext* shader_ctx = ctx->shader_context;
    DivisionTextureSystemContext* tex_ctx = ctx->texture_context;

    glClearBufferfv(GL_COLOR, 0, (const GLfloat*) &renderer_ctx->clear_color);
    for (int32_t i = 0; i < render_pass_ctx->id_table.orders_count; i++)
    {
        DivisionRenderPass pass = render_pass_ctx->render_passes[render_pass_ctx->id_table.orders[i]];
        DivisionVertexBufferInternalPlatform_ vb_internal = vert_buff_ctx->buffers_impl[pass.vertex_buffer];
        DivisionShaderInternal_ shader_internal = shader_ctx->shaders_impl[pass.shader_program];

        glBindVertexArray(vb_internal.gl_vao);
        glBindBuffer(GL_ARRAY_BUFFER, vb_internal.gl_vbo);
        glUseProgram(shader_internal.gl_shader_program);

        for (int uniform_idx = 0; uniform_idx < pass.uniform_vertex_buffer_count; uniform_idx++)
        {
            bind_uniform_buffer(uniform_buff_ctx, pass.uniform_vertex_buffers[uniform_idx]);
        }

        for (int uniform_idx = 0; uniform_idx < pass.uniform_fragment_buffer_count; uniform_idx++)
        {
            bind_uniform_buffer(uniform_buff_ctx, pass.uniform_fragment_buffers[uniform_idx]);
        }

        for (int frag_tex_idx = 0; frag_tex_idx < pass.fragment_texture_count; frag_tex_idx++)
        {
            DivisionIdWithBinding tex_bind = pass.fragment_textures[frag_tex_idx];
            DivisionTextureImpl_* tex_impl = &tex_ctx->textures_impl[tex_bind.id];

            glBindTextureUnit(tex_bind.shader_location, tex_impl->gl_texture);
        }

        if (pass.instance_count > 0)
        {
            glDrawArraysInstanced(
                vb_internal.gl_topology,
                (GLint) pass.first_vertex,
                (GLint) pass.vertex_count,
                (GLint) pass.instance_count
            );
        }
        else
        {
            glDrawArrays(vb_internal.gl_topology, (GLint) pass.first_vertex, (GLint) pass.vertex_count);
        }
    }
}

void bind_uniform_buffer(DivisionUniformBufferSystemContext* ctx, uint32_t buffer_id)
{
    GLuint gl_uniform_buff = ctx->uniform_buffers_impl[buffer_id].gl_buffer;
    DivisionUniformBufferDescriptor* uniform_buffer = &ctx->uniform_buffers[buffer_id];

    glBindBufferBase(GL_UNIFORM_BUFFER, uniform_buffer->binding, gl_uniform_buff);
}
