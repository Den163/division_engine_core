#include "division_engine_core/platform_internal/platform_render_pass.h"

bool division_engine_internal_platform_render_pass_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings)
{
    return true;
}

void division_engine_internal_platform_render_pass_context_free(DivisionContext* ctx)
{
}

bool division_engine_internal_platform_render_pass_realloc(DivisionContext* ctx, size_t new_size)
{
    return true;
}

bool division_engine_internal_platform_render_pass_impl_init_element(DivisionContext* ctx, uint32_t render_pass_id)
{
    return true;
}

void division_engine_internal_platform_render_pass_free(DivisionContext* ctx, uint32_t buffer_id)
{

}

