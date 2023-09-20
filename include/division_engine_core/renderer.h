#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "color.h"
#include "context.h"
#include "state.h"

typedef struct DivisionRendererSystemContext
{
    DivisionColor clear_color;
    struct DivisionWindowContextPlatformInternal_* window_data;
} DivisionRendererSystemContext;

bool division_engine_internal_renderer_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings
);
void division_engine_internal_renderer_context_free(DivisionContext* ctx);

#ifdef __cplusplus
extern "C"
{
#endif

    DIVISION_EXPORT void division_engine_renderer_run_loop(DivisionContext* ctx);

#ifdef __cplusplus
}
#endif