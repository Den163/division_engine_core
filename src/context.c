#if __ARM_NEON__
    #define STBI_NEON
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb_image.h>
#include <stb_image_write.h>

#include "division_engine_core/context.h"

#include "division_engine_core/types/division_lifecycle.h"
#include "division_engine_core/font.h"
#include "division_engine_core/input.h"
#include "division_engine_core/render_pass_descriptor.h"
#include "division_engine_core/renderer.h"
#include "division_engine_core/shader.h"
#include "division_engine_core/texture.h"
#include "division_engine_core/uniform_buffer.h"
#include "division_engine_core/vertex_buffer.h"
#include <stdlib.h>

bool division_engine_context_initialize(
    const DivisionSettings* settings, 
    DivisionContext* ctx
)
{
    ctx->state.delta_time = 0;

    if (!division_engine_renderer_system_context_alloc(ctx, settings))
        return false;
    if (!division_engine_shader_system_context_alloc(ctx, settings))
        return false;
    if (!division_engine_vertex_buffer_system_context_alloc(ctx, settings))
        return false;
    if (!division_engine_uniform_buffer_system_context_alloc(ctx, settings))
        return false;
    if (!division_engine_texture_system_context_alloc(ctx, settings))
        return false;
    if (!division_engine_render_pass_system_context_alloc(ctx, settings))
        return false;
    if (!division_engine_input_system_alloc(ctx, settings))
        return false;
    if (!division_engine_font_system_context_alloc(ctx, settings))
        return false;

    return true;
}

DIVISION_EXPORT void division_engine_context_register_lifecycle(
    DivisionContext* context, const DivisionLifecycle* lifecycle
)
{
    context->lifecycle = *lifecycle;
}

void division_engine_context_finalize(DivisionContext* ctx)
{
    division_engine_render_pass_system_context_free(ctx);
    division_engine_texture_system_context_free(ctx);
    division_engine_uniform_buffer_system_context_free(ctx);
    division_engine_vertex_buffer_system_context_free(ctx);
    division_engine_shader_system_context_free(ctx);
    division_engine_renderer_system_context_free(ctx);
    division_engine_input_system_free(ctx);
    division_engine_font_system_context_free(ctx);
}
