#include "division_engine/platform_internal/platform_render_pass.h"
#include "osx_render_pass.h"

#include "division_engine/renderer.h"

#include "osx_vertex_buffer.h"
#include "osx_window_context.h"

bool division_engine_internal_platform_render_pass_context_alloc(DivisionContext* ctx, const DivisionSettings* settings)
{
    ctx->render_pass_context->render_passes_impl = NULL;

    return true;
}

void division_engine_internal_platform_render_pass_context_free(DivisionContext* ctx)
{
    DivisionOSXViewDelegate* view_delegate = ctx->renderer_context->window_data->app_delegate->viewDelegate;

    for (int i = 0; i < ctx->render_pass_context->render_pass_count; i++)
    {
        DivisionRenderPassInternalPlatform_* pass = &ctx->render_pass_context->render_passes_impl[i];
        pass->mtl_pipeline_state = nil;
    }
    free(ctx->render_pass_context->render_passes_impl);
}

bool division_engine_internal_platform_render_pass_alloc(DivisionContext* ctx, DivisionRenderPass* render_pass)
{
    DivisionRenderPassSystemContext* render_pass_ctx = ctx->render_pass_context;
    render_pass_ctx->render_passes_impl = realloc(
        render_pass_ctx->render_passes_impl,
        sizeof(DivisionRenderPassInternalPlatform_[render_pass_ctx->render_pass_count])
    );

    DivisionMetalShaderProgram* shader_program = &ctx->shader_context->shader_programs[render_pass->shader_program];
    DivisionVertexBufferInternalPlatform_* vert_buff =
        &ctx->vertex_buffer_context->buffers_impl[render_pass->vertex_buffer];
    DivisionOSXViewDelegate* view_delegate = ctx->renderer_context->window_data->app_delegate->viewDelegate;

    id <MTLRenderPipelineState> pipeline_state = [view_delegate
        createRenderPipelineStateForShaderProgram:shader_program
                                 vertexDescriptor:vert_buff->mtl_vertex_descriptor];

    render_pass_ctx->render_passes_impl[render_pass_ctx->render_pass_count - 1] = (DivisionRenderPassInternalPlatform_)
        {
            .mtl_pipeline_state = pipeline_state
        };

    return pipeline_state != NULL;
}
