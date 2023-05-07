#include "division_engine/platform_internal/platfrom_shader.h"

#include <MetalKit/MetalKit.hpp>
#include <NSUtils.h>
#include "division_engine/renderer.h"
#include "division_engine/shader.h"
#include "osx_window_context.h"
#include "osx_shader_context.h"

bool division_engine_internal_platform_shader_system_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings)
{
    auto* shader_context = static_cast<DivisionShaderSystemContext*>(malloc(sizeof(DivisionShaderSystemContext)));
    shader_context->shader_programs = nullptr;
    shader_context->shader_program_count = 0;

    ctx->shader_context = shader_context;

    return true;
}

void division_engine_internal_platform_shader_system_context_free(DivisionContext* ctx)
{
    DivisionShaderSystemContext* shader_context = ctx->shader_context;
    auto* window_context = static_cast<DivisionOSXWindowContext*>(ctx->renderer_context->window_data);
    DivisionOSXViewDelegate* view_delegate = window_context->app_delegate->viewDelegate;

    for (int32_t i; i < shader_context->shader_program_count; i++)
    {
        view_delegate->deleteShaderProgram(&shader_context->shader_programs[i]);
    }
    free(shader_context->shader_programs);
    free(shader_context);
}

int32_t division_engine_internal_platform_shader_program_create(
    DivisionContext* ctx, const DivisionShaderSettings* settings, int32_t source_count)
{
    auto* window_ctx = static_cast<DivisionOSXWindowContext*>(ctx->renderer_context->window_data);
    auto* shader_ctx = ctx->shader_context;
    auto pipeline_state_count = shader_ctx->shader_program_count;

    DivisionMetalShaderProgram shader_program = {
        .vertex_function = NULL,
        .fragment_function = NULL,
    };
    if (window_ctx->app_delegate->viewDelegate->createShaderProgram(settings, source_count, &shader_program))
    {
        size_t new_count = pipeline_state_count + 1;
        shader_ctx->shader_programs = static_cast<DivisionMetalShaderProgram*>(realloc(
            shader_ctx->shader_programs,
            sizeof(DivisionMetalShaderProgram[new_count])
        ));
        shader_ctx->shader_programs[pipeline_state_count] = shader_program;
        shader_ctx->shader_program_count = new_count;

        return static_cast<int32_t>(pipeline_state_count);
    }

    return -1;
}

void division_engine_internal_platform_shader_program_free(DivisionContext* ctx, int32_t program_id)
{
    // TODO: Not implemented
}