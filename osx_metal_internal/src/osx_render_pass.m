#include "division_engine_core/platform_internal/platform_render_pass.h"
#include "osx_render_pass.h"

#include "division_engine_core/renderer.h"
#include "division_engine_core/utility.h"

#include "osx_vertex_buffer.h"
#include "osx_window_context.h"

static inline bool try_get_mtl_blend_arg(
    DivisionContext* ctx, DivisionAlphaBlend blend_arg, MTLBlendFactor* out_mtl_blend_arg);

static inline bool try_get_mtl_blend_op(
    DivisionContext* ctx, DivisionAlphaBlendOperation blend_op, MTLBlendOperation* out_mtl_blend_op);

static inline MTLColorWriteMask division_to_mtl_color_mask(DivisionColorMask color_mask);

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
    DivisionRenderPassInternalPlatform_* render_pass_impl = &render_pass_ctx->render_passes_impl[render_pass_id];
    DivisionMetalShaderProgram* shader_program = &ctx->shader_context->shaders_impl[render_pass->shader_program];
    DivisionVertexBufferInternalPlatform_* vert_buff =
        &ctx->vertex_buffer_context->buffers_impl[render_pass->vertex_buffer];
    DivisionOSXViewDelegate* view_delegate = ctx->renderer_context->window_data->app_delegate->viewDelegate;
    DivisionAlphaBlendingOptions* blend_options = &render_pass->alpha_blending_options;

    MTLRenderPipelineDescriptor* pipeline_descriptor = [MTLRenderPipelineDescriptor new];
    if (shader_program->vertex_function != NULL)
    {
        [pipeline_descriptor setVertexFunction:shader_program->vertex_function];
        [pipeline_descriptor setVertexDescriptor:vert_buff->mtl_vertex_descriptor];
    }

    if (shader_program->fragment_function != NULL)
    {
        [pipeline_descriptor setFragmentFunction:shader_program->fragment_function];
    }

    MTLRenderPipelineColorAttachmentDescriptor* color_attach_desc = [
        [pipeline_descriptor colorAttachments] objectAtIndexedSubscript:0
    ];

    [color_attach_desc setPixelFormat:MTLPixelFormatBGRA8Unorm_sRGB];
    [color_attach_desc setWriteMask: division_to_mtl_color_mask(render_pass->color_mask)];

    if (division_utility_mask_has_flag(render_pass->capabilities_mask, DIVISION_RENDER_PASS_CAPABILITY_ALPHA_BLEND))
    {
        MTLBlendOperation mtl_blend_op;
        MTLBlendFactor mtl_src;
        MTLBlendFactor mtl_dst;
        MTLColorWriteMask mtl_color_mask;
        if (!try_get_mtl_blend_arg(ctx, blend_options->src, &mtl_src) ||
            !try_get_mtl_blend_arg(ctx, blend_options->dst, &mtl_dst) ||
            !try_get_mtl_blend_op(ctx, blend_options->operation, &mtl_blend_op))
        {
            return false;
        }

        [color_attach_desc setBlendingEnabled: true];
        [color_attach_desc setSourceRGBBlendFactor: mtl_src];
        [color_attach_desc setSourceAlphaBlendFactor: mtl_src];
        [color_attach_desc setDestinationRGBBlendFactor: mtl_dst];
        [color_attach_desc setDestinationAlphaBlendFactor: mtl_dst];
        [color_attach_desc setRgbBlendOperation: mtl_blend_op];
        [color_attach_desc setAlphaBlendOperation: mtl_blend_op];
    }

    NSError* err = nil;
    id <MTLRenderPipelineState> pipeline_state = [view_delegate->device
        newRenderPipelineStateWithDescriptor:pipeline_descriptor
                                       error:&err
    ];

    if (err)
    {
        ctx->error_callback(DIVISION_INTERNAL_ERROR, [[err debugDescription] UTF8String]);
        return false;
    }

    if (!pipeline_state)
    {
        ctx->error_callback(DIVISION_INTERNAL_ERROR, "Render pipeline state is null");
        return false;
    }

    render_pass_impl->mtl_pipeline_state = pipeline_state;

    return true;
}

bool try_get_mtl_blend_arg(DivisionContext* ctx, DivisionAlphaBlend blend_arg, MTLBlendFactor* out_mtl_blend_arg)
{
    switch (blend_arg)
    {
        case DIVISION_ALPHA_BLEND_ZERO:
            *out_mtl_blend_arg = MTLBlendFactorZero;
            return true;
        case DIVISION_ALPHA_BLEND_ONE:
            *out_mtl_blend_arg = MTLBlendFactorOne;
            return true;
        case DIVISION_ALPHA_BLEND_SRC_COLOR:
            *out_mtl_blend_arg = MTLBlendFactorSourceColor;
            return true;
        case DIVISION_ALPHA_BLEND_SRC_ALPHA:
            *out_mtl_blend_arg = MTLBlendFactorSourceAlpha;
            return true;
        case DIVISION_ALPHA_BLEND_SRC_ALPHA_SATURATE:
            *out_mtl_blend_arg = MTLBlendFactorSourceAlphaSaturated;
            return true;
        case DIVISION_ALPHA_BLEND_DST_COLOR:
            *out_mtl_blend_arg = MTLBlendFactorDestinationColor;
            return true;
        case DIVISION_ALPHA_BLEND_DST_ALPHA:
            *out_mtl_blend_arg = MTLBlendFactorDestinationAlpha;
            return true;
        case DIVISION_ALPHA_BLEND_ONE_MINUS_SRC_COLOR:
            *out_mtl_blend_arg = MTLBlendFactorOneMinusSourceColor;
            return true;
        case DIVISION_ALPHA_BLEND_ONE_MINUS_SRC_ALPHA:
            *out_mtl_blend_arg = MTLBlendFactorOneMinusSourceAlpha;
            return true;
        case DIVISION_ALPHA_BLEND_ONE_MINUS_DST_COLOR:
            *out_mtl_blend_arg = MTLBlendFactorOneMinusDestinationColor;
            return true;
        case DIVISION_ALPHA_BLEND_ONE_MINUS_DST_ALPHA:
            *out_mtl_blend_arg = MTLBlendFactorOneMinusDestinationAlpha;
            return true;
        case DIVISION_ALPHA_BLEND_CONSTANT_COLOR:
            *out_mtl_blend_arg = MTLBlendFactorBlendColor;
            return true;
        case DIVISION_ALPHA_BLEND_CONSTANT_ALPHA:
            *out_mtl_blend_arg = MTLBlendFactorBlendAlpha;
            return true;
        case DIVISION_ALPHA_BLEND_ONE_MINUS_CONSTANT_COLOR:
            *out_mtl_blend_arg = MTLBlendFactorOneMinusBlendColor;
            return true;
        case DIVISION_ALPHA_BLEND_ONE_MINUS_CONSTANT_ALPHA:
            *out_mtl_blend_arg = MTLBlendFactorOneMinusBlendAlpha;
            return true;
        default:
            ctx->error_callback(DIVISION_INTERNAL_ERROR, "Unknown Blend to MTLBlendFactor mapping");
            return false;
    }
}

bool try_get_mtl_blend_op(
    DivisionContext* ctx, DivisionAlphaBlendOperation blend_op, MTLBlendOperation* out_mtl_blend_op)
{
    switch (blend_op)
    {
        case DIVISION_ALPHA_BLEND_OP_ADD:
            *out_mtl_blend_op = MTLBlendOperationAdd;
            return true;
        case DIVISION_ALPHA_BLEND_OP_SUBTRACT:
            *out_mtl_blend_op = MTLBlendOperationSubtract;
            return true;
        case DIVISION_ALPHA_BLEND_OP_REVERSE_SUBTRACT:
            *out_mtl_blend_op = MTLBlendOperationReverseSubtract;
            return true;
        case DIVISION_ALPHA_BLEND_OP_MIN:
            *out_mtl_blend_op = MTLBlendOperationMin;
            return true;
        case DIVISION_ALPHA_BLEND_OP_MAX:
            *out_mtl_blend_op = MTLBlendOperationMax;
            return true;
        default:
            ctx->error_callback(DIVISION_INTERNAL_ERROR, "Unknown BlendOperation to MTLBlendOperation mapping");
            return false;
    }
}

MTLColorWriteMask division_to_mtl_color_mask(DivisionColorMask color_mask)
{
    MTLColorWriteMask result_mask = MTLColorWriteMaskNone;
    result_mask |= (MTLColorWriteMaskRed * division_utility_mask_has_flag(color_mask, DIVISION_COLOR_MASK_R));
    result_mask |= (MTLColorWriteMaskGreen * division_utility_mask_has_flag(color_mask, DIVISION_COLOR_MASK_G));
    result_mask |= (MTLColorWriteMaskBlue * division_utility_mask_has_flag(color_mask, DIVISION_COLOR_MASK_B));
    result_mask |= (MTLColorWriteMaskAlpha * division_utility_mask_has_flag(color_mask, DIVISION_COLOR_MASK_A));

    return result_mask;
}
