#pragma once

#include <stddef.h>
#include <stdint.h>

#include "context.h"
#include "division_engine_core/data_structures/ordered_id_table.h"
#include "division_engine_core_export.h"

typedef struct DivisionIdWithBinding
{
    uint32_t id;
    uint32_t shader_location;
} DivisionIdWithBinding;

typedef enum DivisionColorMask
{
    DIVISION_COLOR_MASK_NONE = 0,
    DIVISION_COLOR_MASK_R = 1 << 0,
    DIVISION_COLOR_MASK_G = 1 << 1,
    DIVISION_COLOR_MASK_B = 1 << 2,
    DIVISION_COLOR_MASK_A = 1 << 3,
    DIVISION_COLOR_MASK_RGB =
        DIVISION_COLOR_MASK_R | DIVISION_COLOR_MASK_G | DIVISION_COLOR_MASK_B,
    DIVISION_COLOR_MASK_RGBA = DIVISION_COLOR_MASK_RGB | DIVISION_COLOR_MASK_A,
} DivisionColorMask;

typedef enum DivisionRenderPassCapabilityMask
{
    DIVISION_RENDER_PASS_CAPABILITY_NONE = 0,
    DIVISION_RENDER_PASS_CAPABILITY_ALPHA_BLEND = 1 << 0,
    DIVISION_RENDER_PASS_CAPABILITY_INSTANCED_RENDERING = 1 << 1
} DivisionRenderPassCapabilityMask;

typedef enum DivisionAlphaBlend
{
    DIVISION_ALPHA_BLEND_ZERO = 0,
    DIVISION_ALPHA_BLEND_ONE = 1,
    DIVISION_ALPHA_BLEND_SRC_COLOR = 2,
    DIVISION_ALPHA_BLEND_SRC_ALPHA = 3,
    DIVISION_ALPHA_BLEND_SRC_ALPHA_SATURATE = 4,
    DIVISION_ALPHA_BLEND_DST_COLOR = 5,
    DIVISION_ALPHA_BLEND_DST_ALPHA = 6,
    DIVISION_ALPHA_BLEND_CONSTANT_COLOR = 7,
    DIVISION_ALPHA_BLEND_CONSTANT_ALPHA = 8,
    DIVISION_ALPHA_BLEND_ONE_MINUS_SRC_COLOR = 9,
    DIVISION_ALPHA_BLEND_ONE_MINUS_SRC_ALPHA = 10,
    DIVISION_ALPHA_BLEND_ONE_MINUS_DST_COLOR = 11,
    DIVISION_ALPHA_BLEND_ONE_MINUS_DST_ALPHA = 12,
    DIVISION_ALPHA_BLEND_ONE_MINUS_CONSTANT_COLOR = 13,
    DIVISION_ALPHA_BLEND_ONE_MINUS_CONSTANT_ALPHA = 14,
} DivisionAlphaBlend;

typedef enum DivisionAlphaBlendOperation
{
    DIVISION_ALPHA_BLEND_OP_ADD = 1,
    DIVISION_ALPHA_BLEND_OP_SUBTRACT = 2,
    DIVISION_ALPHA_BLEND_OP_REVERSE_SUBTRACT = 3,
    DIVISION_ALPHA_BLEND_OP_MIN = 4,
    DIVISION_ALPHA_BLEND_OP_MAX = 5,
} DivisionAlphaBlendOperation;

typedef struct DivisionAlphaBlendingOptions
{
    DivisionAlphaBlend src;
    DivisionAlphaBlend dst;
    DivisionAlphaBlendOperation operation;
    float constant_blend_color[4];
} DivisionAlphaBlendingOptions;

typedef struct DivisionRenderPass
{
    DivisionAlphaBlendingOptions alpha_blending_options;

    size_t first_vertex;
    size_t vertex_count;
    size_t index_count;
    size_t instance_count;
    DivisionIdWithBinding* uniform_vertex_buffers;
    int32_t uniform_vertex_buffer_count;
    DivisionIdWithBinding* uniform_fragment_buffers;
    int32_t uniform_fragment_buffer_count;
    DivisionIdWithBinding* fragment_textures;
    int32_t fragment_texture_count;
    uint32_t vertex_buffer;
    uint32_t shader_program;
    DivisionRenderPassCapabilityMask capabilities_mask;
    DivisionColorMask color_mask;
} DivisionRenderPass;

typedef struct DivisionRenderPassSystemContext
{
    DivisionOrderedIdTable id_table;
    DivisionRenderPass* render_passes;
    struct DivisionRenderPassInternalPlatform_* render_passes_impl;
    int32_t render_pass_count;
} DivisionRenderPassSystemContext;

#define DIVISION_GET_RENDER_PASS(ctx, render_pass_id)                                    \
    (&ctx->render_pass_context->render_passes[render_pass_id])

bool division_engine_internal_render_pass_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings
);

void division_engine_internal_render_pass_context_free(DivisionContext* ctx);

#ifdef __cplusplus
extern "C"
{
#endif

    DIVISION_EXPORT bool division_engine_render_pass_alloc(
        DivisionContext* ctx, DivisionRenderPass render_pass, uint32_t* out_render_pass_id
    );

    DIVISION_EXPORT DivisionRenderPass* division_engine_render_pass_borrow(
        DivisionContext* ctx, uint32_t render_pass_id
    );

    DIVISION_EXPORT void division_engine_render_pass_return(
        DivisionContext* ctx, uint32_t render_pass_id, DivisionRenderPass* render_pass_ptr
    );

    DIVISION_EXPORT void division_engine_render_pass_free(
        DivisionContext* ctx, uint32_t render_pass_id
    );

#ifdef __cplusplus
}
#endif