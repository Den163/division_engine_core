#include "division_engine/platform_internal/platfrom_shader.h"

#include <MetalKit/MetalKit.h>
#include "division_engine/renderer.h"
#include "osx_window_context.h"

bool division_engine_internal_platform_shader_system_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings)
{
    DivisionShaderSystemContext* shader_context = malloc(sizeof(DivisionShaderSystemContext));
    shader_context->shader_programs = NULL;
    shader_context->shader_program_count = 0;

    ctx->shader_context = shader_context;

    return true;
}

void division_engine_internal_platform_shader_system_context_free(DivisionContext* ctx)
{
    DivisionShaderSystemContext* shader_context = ctx->shader_context;
    DivisionOSXWindowContext* window_context = ctx->renderer_context->window_data;
    DivisionOSXViewDelegate* view_delegate = window_context->app_delegate->viewDelegate;

    for (int32_t i; i < shader_context->shader_program_count; i++)
    {
        DivisionMetalShaderProgram* shader_program = &shader_context->shader_programs[i];
        shader_program->vertex_function = nil;
        shader_program->fragment_function = nil;
    }
    free(shader_context->shader_programs);
    free(shader_context);
}

int32_t division_engine_internal_platform_shader_program_create(
    DivisionContext* ctx, const DivisionShaderSettings* settings, int32_t source_count)
{
    DivisionOSXWindowContext* window_ctx = ctx->renderer_context->window_data;
    DivisionShaderSystemContext* shader_ctx = ctx->shader_context;
    int32_t pipeline_state_count = shader_ctx->shader_program_count;

    DivisionMetalShaderProgram shader_program = {
        .vertex_function = nil,
        .fragment_function = nil,
    };
    if ([window_ctx->app_delegate->viewDelegate
        createShaderProgramWithSettings:settings
                            sourceCount:source_count
                             outProgram:&shader_program])
    {
        int32_t new_count = pipeline_state_count + 1;
        shader_ctx->shader_programs = realloc(
            shader_ctx->shader_programs, sizeof(DivisionMetalShaderProgram[new_count]));
        shader_ctx->shader_programs[pipeline_state_count] = shader_program;
        shader_ctx->shader_program_count = new_count;

        return pipeline_state_count;
    }

    return -1;
}

void division_engine_internal_platform_shader_program_free(DivisionContext* ctx, int32_t program_id)
{
    // TODO: Not implemented
}