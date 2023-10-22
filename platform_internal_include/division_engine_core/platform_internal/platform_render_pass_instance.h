#pragma once

#include "division_engine_core/context.h"
#include "division_engine_core/render_pass_instance.h"
#include "division_engine_core_export.h"

#ifdef __cplusplus
extern "C"
{
#endif

DIVISION_EXPORT void division_engine_internal_platform_render_pass_instance_draw(
    DivisionContext* ctx,
    const DivisionColor* clear_color,
    const DivisionRenderPassInstance* render_pass_instances,
    uint32_t render_pass_instance_count
);

#ifdef __cplusplus
}
#endif