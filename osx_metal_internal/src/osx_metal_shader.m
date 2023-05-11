#include "rendering/platform_internal/platfrom_shader.h"

#include <MetalKit/MetalKit.h>
#include "rendering/renderer.h"
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

    for (int32_t i = 0; i < shader_context->shader_count; i++)
    {
        DivisionMetalShaderProgram* shader_program = &shader_context->shaders_impl[i];
        shader_program->vertex_function = nil;
        shader_program->fragment_function = nil;
    }
    free(shader_context->shaders_impl);
}

bool division_engine_internal_platform_shader_program_alloc(
    DivisionContext* ctx, const DivisionShaderSettings* settings, int32_t source_count, uint32_t* out_shader_program_id)
{
    DivisionOSXWindowContext* window_ctx = ctx->renderer_context->window_data;
    DivisionShaderSystemContext* shader_ctx = ctx->shader_context;
    size_t shader_program_count = shader_ctx->shader_count;

    DivisionMetalShaderProgram shader_program = {
        .vertex_function = nil,
        .fragment_function = nil,
    };
    if ([window_ctx->app_delegate->viewDelegate
            createShaderProgramWithSettings:settings sourceCount:source_count outProgram:&shader_program] == false)
    {
        ctx->error_callback(DIVISION_INTERNAL_ERROR, "Failed to create shader program");
        return false;
    }

    uint32_t program_id = division_unordered_id_table_insert(&shader_ctx->id_table);

    if (program_id >= shader_program_count)
    {
        shader_ctx->shaders_impl = realloc(
            shader_ctx->shaders_impl,
            sizeof(DivisionMetalShaderProgram[program_id + 1])
        );

        if (shader_ctx->shaders_impl == NULL)
        {
            division_unordered_id_table_remove(&shader_ctx->id_table, program_id);
            ctx->error_callback(DIVISION_INTERNAL_ERROR, "Failed to reallocate Shader Implementation array");

            return false;
        }
    }

    shader_ctx->shaders_impl[program_id] = shader_program;

    *out_shader_program_id = program_id;
    return true;
}

void division_engine_internal_platform_shader_program_free(DivisionContext* ctx, uint32_t shader_program_id)
{
    DivisionMetalShaderProgram* shader = &ctx->shader_context->shaders_impl[shader_program_id];
    shader->fragment_function = nil;
    shader->vertex_function = nil;

    division_unordered_id_table_remove(&ctx->shader_context->id_table, shader_program_id);
}
