#pragma once

#include <stdbool.h>

#include "color.h"
#include "settings.h"
#include "state.h"

#include <division_engine_core_export.h>

#define DIVISION_INTERNAL_ERROR (-8)

struct DivisionRendererSystemContext;
struct DivisionShaderSystemContext;
struct DivisionVertexBufferSystemContext;
struct DivisionUniformBufferSystemContext;
struct DivisionRenderPassSystemContext;

typedef struct DivisionContext {
    DivisionState state;

    DivisionErrorFunc error_callback;
    struct DivisionRendererSystemContext* renderer_context;
    struct DivisionShaderSystemContext* shader_context;
    struct DivisionVertexBufferSystemContext* vertex_buffer_context;
    struct DivisionUniformBufferSystemContext* uniform_buffer_context;
    struct DivisionRenderPassSystemContext* render_pass_context;

    void* user_data;
} DivisionContext;

#ifdef __cplusplus
extern "C" {
#endif

DIVISION_EXPORT bool division_engine_context_alloc(const DivisionSettings* settings, DivisionContext** output_context);
DIVISION_EXPORT void division_engine_context_free(DivisionContext* ctx);

#ifdef __cplusplus
}
#endif