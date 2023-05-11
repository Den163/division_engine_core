#include "rendering/platform_internal/platform_render_pass.h"
#include "osx_render_pass.h"

#include "rendering/renderer.h"

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

bool division_engine_internal_platform_render_pass_alloc(
    DivisionContext* ctx, DivisionRenderPass* render_pass, uint32_t render_pass_id)
{
    DivisionRenderPassSystemContext* render_pass_ctx = ctx->render_pass_context;

    DivisionMetalShaderProgram* shader_program = &ctx->shader_context->shaders_impl[render_pass->shader_program];
    DivisionVertexBufferInternalPlatform_* vert_buff =
        &ctx->vertex_buffer_context->buffers_impl[render_pass->vertex_buffer];
    DivisionOSXViewDelegate* view_delegate = ctx->renderer_context->window_data->app_delegate->viewDelegate;

    id <MTLRenderPipelineState> pipeline_state = [view_delegate
        createRenderPipelineStateForShaderProgram:shader_program
                                 vertexDescriptor:vert_buff->mtl_vertex_descriptor];

    if (pipeline_state == nil)
    {
        ctx->error_callback(DIVISION_INTERNAL_ERROR, "Failed to create Metal Pipeline state");
        return false;
    }

    if (render_pass_id >= render_pass_ctx->render_pass_count)
    {
        render_pass_ctx->render_passes_impl = realloc(
            render_pass_ctx->render_passes_impl, sizeof(DivisionRenderPassInternalPlatform_[render_pass_id + 1]));

        if (render_pass_ctx->render_passes_impl == NULL)
        {
            ctx->error_callback(DIVISION_INTERNAL_ERROR, "Failed to realloc Render pass platform array");
            return false;
        }
    }

    render_pass_ctx->render_passes_impl[render_pass_id] = (DivisionRenderPassInternalPlatform_)
    {
        .mtl_pipeline_state = pipeline_state
    };

    return true;
}

void division_engine_internal_platform_render_pass_free(DivisionContext* ctx, uint32_t buffer_id)
{
    DivisionRenderPassInternalPlatform_* render_pass_impl = &ctx->render_pass_context->render_passes_impl[buffer_id];
    render_pass_impl->mtl_pipeline_state = nil;
}
