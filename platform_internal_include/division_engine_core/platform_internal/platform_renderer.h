#pragma once

#include "division_engine_core/context.h"

#include <division_engine_core_export.h>

#ifdef __cplusplus
extern "C"
{
#endif

    DIVISION_EXPORT bool division_engine_internal_platform_renderer_alloc(
        DivisionContext* ctx, const DivisionSettings* settings
    );
    DIVISION_EXPORT void division_engine_internal_platform_renderer_free(
        DivisionContext* ctx
    );

    DIVISION_EXPORT void division_engine_internal_platform_renderer_run_loop(
        DivisionContext* ctx
    );

#ifdef __cplusplus
}
#endif