#define GLFW_INCLUDE_NONE
#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "division_engine/render_pass.h"
#include "division_engine/renderer.h"
#include "division_engine/uniform_buffer.h"
#include "division_engine/vertex_buffer.h"

#include "glfw_uniform_buffer.h"
#include "glfw_vertex_buffer.h"

#include "division_engine/platform_internal/platform_renderer.h"

static inline void renderer_draw(DivisionContext* ctx);

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

    ctx->renderer_context->window_data = window;

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
    int32_t pass_count = render_pass_ctx->render_pass_count;

    glClearBufferfv(GL_COLOR, 0, (const GLfloat*) &renderer_ctx->clear_color);
    for (int32_t i = 0; i < pass_count; i++)
    {
        DivisionRenderPass pass = render_pass_ctx->render_passes[i];
        DivisionVertexBufferInternalPlatform_ vb_internal = vert_buff_ctx->buffers_impl[pass.vertex_buffer];

        glBindBuffer(GL_ARRAY_BUFFER, vb_internal.gl_buffer);
        glUseProgram(pass.shader_program);

        for (int32_t uniform_idx = 0; uniform_idx < pass.uniform_buffer_count; uniform_idx++)
        {
            int32_t uniform_buffer_id = pass.uniform_buffers[uniform_idx];
            GLuint gl_uniform_buff = uniform_buff_ctx->uniform_buffers_impl[uniform_buffer_id].gl_buffer;
            DivisionUniformBuffer* uniform_buffer = &uniform_buff_ctx->uniform_buffers[uniform_buffer_id];


            glBindBufferBase(GL_UNIFORM_BUFFER, uniform_buffer->binding, gl_uniform_buff);
        }

        glDrawArrays(vb_internal.gl_topology, pass.first_vertex, pass.vertex_count);
    }
}