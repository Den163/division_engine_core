#pragma once

#include <stdint.h>
#include <stddef.h>

#include "division_engine_core_export.h"
#include "context.h"
#include "division_id_table/ordered_id_table.h"

typedef struct DivisionRenderPass {
    size_t vertex_count;
    size_t first_vertex;
    uint32_t* uniform_buffers;
    int32_t uniform_buffer_count;
    uint32_t vertex_buffer;
    uint32_t shader_program;
} DivisionRenderPass;

typedef struct DivisionRenderPassSystemContext {
    DivisionOrderedIdTable id_table;
    DivisionRenderPass* render_passes;
    struct DivisionRenderPassInternalPlatform_* render_passes_impl;
    int32_t render_pass_count;
} DivisionRenderPassSystemContext;

bool division_engine_internal_render_pass_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings);

void division_engine_internal_render_pass_context_free(DivisionContext* ctx);

#ifdef __cplusplus
extern "C" {
#endif

DIVISION_EXPORT bool division_engine_render_pass_alloc(
    DivisionContext* ctx, DivisionRenderPass render_pass, uint32_t* out_render_pass_id);

DIVISION_EXPORT void division_engine_render_pass_free(DivisionContext* ctx, uint32_t render_pass_id);

#ifdef __cplusplus
}
#endif