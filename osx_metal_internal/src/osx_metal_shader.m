#include "division_engine/platform_internal/platfrom_shader.h"

#include <MetalKit/MetalKit.h>
#include "division_engine/renderer.h"
#include "osx_window_context.h"

bool division_engine_internal_platform_shader_system_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings)
{
    ctx->shader_context->shaders_impl = NULL;

    return true;
}

void division_engine_internal_platform_shader_system_context_free(DivisionContext* ctx)
{
    DivisionShaderSystemContext* shader_context = ctx->shader_context;
    DivisionOSXWindowContext* window_context = ctx->renderer_context->window_data;

    for (int32_t i; i < shader_context->shader_count; i++)
    {
        DivisionMetalShaderProgram* shader_program = &shader_context->shaders_impl[i];
        shader_program->vertex_function = nil;
        shader_program->fragment_function = nil;
    }
    free(shader_context->shaders_impl);
}

bool division_engine_internal_platform_shader_program_create(
    DivisionContext* ctx, const DivisionShaderSettings* settings, int32_t source_count)
{
    DivisionOSXWindowContext* window_ctx = ctx->renderer_context->window_data;
    DivisionShaderSystemContext* shader_ctx = ctx->shader_context;
    int32_t shader_program_count = shader_ctx->shader_count;

    DivisionMetalShaderProgram shader_program = {
        .vertex_function = nil,
        .fragment_function = nil,
    };
    if ([window_ctx->app_delegate->viewDelegate
        createShaderProgramWithSettings:settings
                            sourceCount:source_count
                             outProgram:&shader_program])
    {
        int32_t shaderIdx = shader_program_count - 1;

        shader_ctx->shaders_impl = realloc(
            shader_ctx->shaders_impl, sizeof(DivisionMetalShaderProgram[shader_program_count]));
        shader_ctx->shaders_impl[shaderIdx] = shader_program;

        return true;
    }

    return false;
}