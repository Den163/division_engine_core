#include "rendering/platform_internal/platform_render_pass.h"

bool division_engine_internal_platform_render_pass_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings)
{
    return true;
}

void division_engine_internal_platform_render_pass_context_free(DivisionContext* ctx)
{
}

bool division_engine_internal_platform_render_pass_alloc(
    DivisionContext* ctx, DivisionRenderPass* render_pass, uint32_t buffer_id)
{
    return true;
}

void division_engine_internal_platform_render_pass_free(DivisionContext* ctx, uint32_t buffer_id)
{

}

