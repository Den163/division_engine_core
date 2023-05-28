#include "division_engine_core/platform_internal/platform_render_pass.h"
#include "division_engine_core/utility.h"

#include "glfw_render_pass.h"

#include <stdlib.h>

static inline bool try_get_gl_blend_arg(
    DivisionContext* ctx, DivisionAlphaBlend blend_arg, GLenum* out_gl_blend_arg);
static inline bool try_get_gl_blend_eq(
    DivisionContext* ctx, DivisionAlphaBlendOperation blend_op, GLenum* out_gl_eq);

bool division_engine_internal_platform_render_pass_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings)
{
    ctx->render_pass_context->render_passes_impl = NULL;

    return true;
}

void division_engine_internal_platform_render_pass_context_free(DivisionContext* ctx)
{
    DivisionRenderPassSystemContext* pass_ctx = ctx->render_pass_context;
    for (int i = 0; i < pass_ctx->render_pass_count; i++)
    {
        division_engine_internal_platform_render_pass_free(ctx, i);
    }
    free(pass_ctx->render_passes_impl);
}

bool division_engine_internal_platform_render_pass_realloc(DivisionContext* ctx, size_t new_size)
{
    DivisionRenderPassSystemContext* pass_ctx = ctx->render_pass_context;
    pass_ctx->render_passes_impl = realloc(
        pass_ctx->render_passes_impl,
        sizeof(DivisionRenderPassInternalPlatform_[new_size])
    );

    return pass_ctx->render_passes_impl != NULL;
}

bool division_engine_internal_platform_render_pass_impl_init_element(DivisionContext* ctx, uint32_t render_pass_id)
{
    const size_t cap_capacity = 10;

    DivisionRenderPassSystemContext* pass_ctx = ctx->render_pass_context;
    const DivisionRenderPass* pass = &pass_ctx->render_passes[render_pass_id];
    DivisionRenderPassInternalPlatform_* pass_impl = &pass_ctx->render_passes_impl[render_pass_id];

    if (division_utility_mask_has_flag(pass->capabilities_mask, DIVISION_RENDER_PASS_CAPABILITY_ALPHA_BLEND))
    {
        const DivisionAlphaBlendingOptions* blend_options = &pass->alpha_blending_options;

        if (!try_get_gl_blend_arg(ctx, blend_options->src, &pass_impl->gl_blend_src) ||
            !try_get_gl_blend_arg(ctx, blend_options->dst, &pass_impl->gl_blend_dst) ||
            !try_get_gl_blend_eq(ctx, blend_options->operation, &pass_impl->gl_blend_equation))
        {
            return false;
        }
    }
    else 
    {
        pass_impl->gl_blend_src = 0;
        pass_impl->gl_blend_dst = 0;
        pass_impl->gl_blend_equation = 0;
    }

    return true;
}

void division_engine_internal_platform_render_pass_free(DivisionContext* ctx, uint32_t render_pass_id)
{
}

bool try_get_gl_blend_arg(DivisionContext* ctx, DivisionAlphaBlend blend_arg, GLenum* out_gl_blend_arg)
{
    switch (blend_arg)
    {
        case DIVISION_ALPHA_BLEND_ZERO:
            *out_gl_blend_arg = GL_ZERO;
            return true;
        case DIVISION_ALPHA_BLEND_ONE:
            *out_gl_blend_arg = GL_ONE;
            return true;
        case DIVISION_ALPHA_BLEND_SRC_COLOR:
            *out_gl_blend_arg = GL_SRC_COLOR;
            return true;
        case DIVISION_ALPHA_BLEND_SRC_ALPHA:
            *out_gl_blend_arg = GL_SRC_ALPHA;
            return true;
        case DIVISION_ALPHA_BLEND_SRC_ALPHA_SATURATE:
            *out_gl_blend_arg = GL_SRC_ALPHA_SATURATE;
            return true;
        case DIVISION_ALPHA_BLEND_DST_COLOR:
            *out_gl_blend_arg = GL_DST_COLOR;
            return true;
        case DIVISION_ALPHA_BLEND_DST_ALPHA:
            *out_gl_blend_arg = GL_DST_ALPHA;
            return true;
        case DIVISION_ALPHA_BLEND_ONE_MINUS_SRC_COLOR:
            *out_gl_blend_arg = GL_ONE_MINUS_SRC_COLOR;
            return true;
        case DIVISION_ALPHA_BLEND_ONE_MINUS_SRC_ALPHA:
            *out_gl_blend_arg = GL_ONE_MINUS_SRC_ALPHA;
            return true;
        case DIVISION_ALPHA_BLEND_ONE_MINUS_DST_COLOR:
            *out_gl_blend_arg = GL_ONE_MINUS_DST_COLOR;
            return true;
        case DIVISION_ALPHA_BLEND_ONE_MINUS_DST_ALPHA:
            *out_gl_blend_arg = GL_ONE_MINUS_DST_ALPHA;
            return true;
        case DIVISION_ALPHA_BLEND_CONSTANT_COLOR:
            *out_gl_blend_arg = GL_CONSTANT_COLOR;
            return true;
        case DIVISION_ALPHA_BLEND_CONSTANT_ALPHA:
            *out_gl_blend_arg = GL_CONSTANT_ALPHA;
            return true;
        case DIVISION_ALPHA_BLEND_ONE_MINUS_CONSTANT_COLOR:
            *out_gl_blend_arg = GL_ONE_MINUS_CONSTANT_COLOR;
            return true;
        case DIVISION_ALPHA_BLEND_ONE_MINUS_CONSTANT_ALPHA:
            *out_gl_blend_arg = GL_ONE_MINUS_CONSTANT_ALPHA;
            return true;
        default:
            ctx->error_callback(DIVISION_INTERNAL_ERROR, "Unknown Blend arg to GL mapping");
            return false;
    }
}
bool try_get_gl_blend_eq(DivisionContext* ctx, DivisionAlphaBlendOperation blend_op, GLenum* out_gl_eq)
{
    switch (blend_op)
    {
        case DIVISION_ALPHA_BLEND_OP_ADD:
            *out_gl_eq = GL_FUNC_ADD;
            return true;
        case DIVISION_ALPHA_BLEND_OP_SUBTRACT:
            *out_gl_eq = GL_FUNC_SUBTRACT;
            return true;
        case DIVISION_ALPHA_BLEND_OP_REVERSE_SUBTRACT:
            *out_gl_eq = GL_FUNC_REVERSE_SUBTRACT;
            return true;
        case DIVISION_ALPHA_BLEND_OP_MIN:
            *out_gl_eq = GL_MIN;
            return true;
        case DIVISION_ALPHA_BLEND_OP_MAX:
            *out_gl_eq = GL_MAX;
            return true;
        default:
            ctx->error_callback(DIVISION_INTERNAL_ERROR, "Unknown BlendOperation to GL mapping");
            return false;
    }
}
