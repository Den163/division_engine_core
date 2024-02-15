#pragma once

#include <stdbool.h>

#include "types/color.h"
#include "types/division_lifecycle.h"
#include "types/settings.h"
#include "types/state.h"

#include <division_engine_core_export.h>

#define DIVISION_INTERNAL_ERROR (-8)
#define DIVISION_THROW_INTERNAL_ERROR(ctx, message)                                      \
    ctx->lifecycle.error_callback(ctx, DIVISION_INTERNAL_ERROR, message)

struct DivisionRendererSystemContext;
struct DivisionShaderSystemContext;
struct DivisionVertexBufferSystemContext;
struct DivisionUniformBufferSystemContext;
struct DivisionRenderPassSystemContext;
struct DivisionTextureSystemContext;
struct DivisionInputSystemContext;
struct DivisionFontSystemContext;

typedef struct DivisionContext
{
    DivisionLifecycle lifecycle;
    DivisionState state;

    struct DivisionRendererSystemContext* renderer_context;
    struct DivisionShaderSystemContext* shader_context;
    struct DivisionVertexBufferSystemContext* vertex_buffer_context;
    struct DivisionUniformBufferSystemContext* uniform_buffer_context;
    struct DivisionTextureSystemContext* texture_context;
    struct DivisionRenderPassSystemContext* render_pass_context;
    struct DivisionInputSystemContext* input_context;
    struct DivisionFontSystemContext* font_context;

    void* user_data;
} DivisionContext;

#ifdef __cplusplus
extern "C"
{
#endif

    DIVISION_EXPORT bool division_engine_context_initialize(
        const DivisionSettings* settings, DivisionContext* output_context
    );

    DIVISION_EXPORT void division_engine_context_register_lifecycle(
        DivisionContext* context, const DivisionLifecycle* lifecycle
    );

    DIVISION_EXPORT void division_engine_context_finalize(DivisionContext* ctx);

#ifdef __cplusplus
}
#endif