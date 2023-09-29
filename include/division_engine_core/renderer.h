#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "color.h"
#include "context.h"
#include "state.h"

typedef struct DivisionRendererSystemContext
{
    DivisionColor clear_color;
    int32_t frame_buffer_width;
    int32_t frame_buffer_height;

    struct DivisionWindowContextPlatformInternal_* window_data;
} DivisionRendererSystemContext;

bool division_engine_renderer_system_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings
);
void division_engine_renderer_system_context_free(DivisionContext* ctx);

#ifdef __cplusplus
extern "C"
{
#endif

    DIVISION_EXPORT void division_engine_renderer_run_loop(DivisionContext* ctx);

#ifdef __cplusplus
}
#endif