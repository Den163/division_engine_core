#include "division_engine_core/platform_internal/platform_render_pass.h"
#include "osx_render_pass.h"

#include "division_engine_core/renderer.h"

#include "osx_vertex_buffer.h"
#include "osx_window_context.h"

bool division_engine_internal_platform_render_pass_context_alloc(DivisionContext* ctx, const DivisionSettings* settings)
{
    ctx->render_pass_context->render_passes_impl = NULL;

    return true;
}

void division_engine_internal_platform_render_pass_context_free(DivisionContext* ctx)
{
    for (int i = 0; i < ctx->render_pass_context->render_pass_count; i++)
    {
        DivisionRenderPassInternalPlatform_* pass = &ctx->render_pass_context->render_passes_impl[i];
        pass->mtl_pipeline_state = nil;
    }
    free(ctx->render_pass_context->render_passes_impl);
}

void division_engine_internal_platform_render_pass_free(DivisionContext* ctx, uint32_t pass_id)
{
    DivisionRenderPassInternalPlatform_* render_pass_impl = &ctx->render_pass_context->render_passes_impl[pass_id];
    render_pass_impl->mtl_pipeline_state = nil;
}

bool division_engine_internal_platform_render_pass_realloc(DivisionContext* ctx, size_t new_size)
{
    DivisionRenderPassSystemContext* render_pass_ctx = ctx->render_pass_context;
    render_pass_ctx->render_passes_impl = realloc(
        render_pass_ctx->render_passes_impl, sizeof(DivisionRenderPassInternalPlatform_[new_size]));

    return render_pass_ctx->render_passes_impl != NULL;
}

bool division_engine_internal_platform_render_pass_impl_init_element(DivisionContext* ctx, uint32_t render_pass_id)
{
    DivisionRenderPassSystemContext* render_pass_ctx = ctx->render_pass_context;
    DivisionRenderPass* render_pass = &ctx->render_pass_context->render_passes[render_pass_id];
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

    render_pass_ctx->render_passes_impl[render_pass_id] = (DivisionRenderPassInternalPlatform_)
    {
        .mtl_pipeline_state = pipeline_state
    };

    return true;
}
