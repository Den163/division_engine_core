#pragma once

#include "division_engine_core/color.h"
#include <stddef.h>
#include <stdint.h>
#include <division_engine_core/context.h>

typedef struct DivisionIdWithBinding
{
    uint32_t id;
    uint32_t shader_location;
} DivisionIdWithBinding;

typedef enum DivisionRenderPassInstanceCapabilityMask
{
    DIVISION_RENDER_PASS_INSTANCE_CAPABILITY_NONE = 0,
    DIVISION_RENDER_PASS_INSTANCE_CAPABILITY_INSTANCED_RENDERING = 1,
} DivisionRenderPassInstanceCapabilityMask;

typedef struct DivisionRenderPassInstance
{
    uint32_t first_vertex;
    uint32_t first_instance;
    uint32_t vertex_count;
    uint32_t index_count;
    uint32_t instance_count;
    DivisionIdWithBinding* uniform_vertex_buffers;
    DivisionIdWithBinding* uniform_fragment_buffers;
    DivisionIdWithBinding* fragment_textures;

    int32_t uniform_vertex_buffer_count;
    int32_t uniform_fragment_buffer_count;
    int32_t fragment_texture_count;
    uint32_t render_pass_descriptor_id;
    DivisionRenderPassInstanceCapabilityMask capabilities_mask;
} DivisionRenderPassInstance;

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