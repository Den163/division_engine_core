#pragma once

#include "types/color.h"
#include "types/render_pass_instance.h"

#include <stddef.h>
#include <stdint.h>

#include <division_engine_core/context.h>

#ifdef __cplusplus
extern "C"
{
#endif

    DIVISION_EXPORT void division_engine_render_pass_instance_draw(
        DivisionContext* ctx,
        const DivisionColor* clear_color,
        const DivisionRenderPassInstance* render_pass_instances,
        uint32_t render_pass_instance_count
    );

#ifdef __cplusplus
}
#endif