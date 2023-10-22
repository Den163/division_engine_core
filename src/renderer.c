#include "division_engine_core/renderer.h"
#include "division_engine_core/platform_internal/platform_renderer.h"

#include <stdlib.h>

bool division_engine_renderer_system_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings
)
{
    ctx->renderer_context = malloc(sizeof(DivisionRendererSystemContext));

    return division_engine_internal_platform_renderer_alloc(ctx, settings);
}

void division_engine_renderer_run_loop(DivisionContext* ctx)
{
    division_engine_internal_platform_renderer_run_loop(ctx);
}

void division_engine_renderer_system_context_free(DivisionContext* ctx)
{
    division_engine_internal_platform_renderer_free(ctx);
    free(ctx->renderer_context);
}