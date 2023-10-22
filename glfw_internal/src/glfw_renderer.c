#define GLFW_INCLUDE_NONE
#define GLAD_GL_IMPLEMENTATION

#include "GLFW/glfw3.h"
#include "glad/gl.h"

#include "division_engine_core/context.h"
#include "division_engine_core/renderer.h"

#include <stdint.h>

static inline void check_window_resizing(
    DivisionRendererSystemContext* renderer_context, GLFWwindow* window
);

typedef struct DivisionWindowContextPlatformInternal_*
    DivisionWindowContextPlatformInternalPtr_;

static void gl_debug_message_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* user_data
)
{
    if ((source != GL_DEBUG_SOURCE_SHADER_COMPILER) & (type == GL_DEBUG_TYPE_ERROR))
    {
        DivisionContext* ctx = (DivisionContext*)user_data;
        ctx->lifecycle.error_callback(ctx, id, message);
    }
}

bool division_engine_internal_platform_renderer_alloc(
    DivisionContext* ctx, const DivisionSettings* settings
)
{
    if (!glfwInit())
    {
        DIVISION_THROW_INTERNAL_ERROR(ctx, "Failed to init glfw");
        return false;
    }

    GLFWwindow* window = glfwCreateWindow(
        (int)settings->window_width,
        (int)settings->window_height,
        settings->window_title,
        NULL,
        NULL
    );

    if (!window)
    {
        DIVISION_THROW_INTERNAL_ERROR(ctx, "Can't create a new GLFW window");
        return false;
    }

    glfwMakeContextCurrent(window);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        DIVISION_THROW_INTERNAL_ERROR(ctx, "Failed to load GLAD");
        return false;
    }

    DivisionRendererSystemContext* renderer_context = ctx->renderer_context;
    glfwGetFramebufferSize(
        window,
        &renderer_context->frame_buffer_width,
        &renderer_context->frame_buffer_height
    );
    glViewport(
        0, 0, renderer_context->frame_buffer_width, renderer_context->frame_buffer_height
    );

    renderer_context->window_data = (DivisionWindowContextPlatformInternalPtr_)window;

    return true;
}

void division_engine_internal_platform_renderer_free(DivisionContext* ctx)
{
    glfwDestroyWindow((GLFWwindow*)ctx->renderer_context->window_data);
    glfwTerminate();
}

void division_engine_internal_platform_renderer_run_loop(DivisionContext* ctx)
{
#if DIVISION_OPENGL_DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(gl_debug_message_callback, ctx);
#endif

    ctx->lifecycle.init_callback(ctx);

    DivisionRendererSystemContext* renderer_context = ctx->renderer_context;
    GLFWwindow* window = (GLFWwindow*)renderer_context->window_data;
    double last_frame_time, current_time, delta_time;

    last_frame_time = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        current_time = glfwGetTime();
        delta_time = current_time - last_frame_time;

        if (delta_time >= 1 / 60.f)
        {
            check_window_resizing(renderer_context, window);

            delta_time = current_time - last_frame_time;
            last_frame_time = current_time;

            ctx->state.delta_time = delta_time;

            ctx->lifecycle.draw_callback(ctx);
            glfwSwapBuffers(window);
        }

        glfwPollEvents();
    }

    ctx->lifecycle.free_callback(ctx);
}

void check_window_resizing(
    DivisionRendererSystemContext* renderer_context, GLFWwindow* window
)
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    if ((width != renderer_context->frame_buffer_width) |
        (height != renderer_context->frame_buffer_height))
    {
        glViewport(0, 0, width, height);
        renderer_context->frame_buffer_width = width;
        renderer_context->frame_buffer_height = height;
    }
}
