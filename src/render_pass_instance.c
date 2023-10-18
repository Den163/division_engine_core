#include "division_engine_core/platform_internal/platform_render_pass_instance.h"
#include <division_engine_core/render_pass_instance.h>
#include <stdint.h>

void division_engine_render_pass_instance_draw(
    DivisionContext* ctx,
    const DivisionRenderPassInstance* render_pass_instances,
    uint32_t render_pass_instance_count
)
{
    division_engine_internal_platform_render_pass_instance_draw(
        ctx, render_pass_instances, render_pass_instance_count
    );
}
