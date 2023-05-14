#pragma once

#include <stdlib.h>
#include <stdbool.h>

#include "context.h"
#include "shader.h"

#include <division_engine_core_export.h>

typedef enum {
    DIVISION_TOPOLOGY_TRIANGLES = 0,
    DIVISION_TOPOLOGY_POINTS = 1,
    DIVISION_TOPOLOGY_LINES = 2
} DivisionRenderTopology;

typedef struct DivisionVertexAttributeSettings {
    DivisionShaderVariableType type;
    int32_t location;
} DivisionVertexAttributeSettings;

typedef struct DivisionVertexAttribute {
    int32_t location;
    int32_t offset;
    int32_t base_size;
    int32_t component_count;
    DivisionShaderVariableType type;
} DivisionVertexAttribute;

typedef struct DivisionVertexBuffer {
    struct VertexAttributeInternalPlatform_* per_vertex_attributes_impl;
    DivisionVertexAttribute* per_vertex_attributes;
    int32_t per_vertex_attribute_count;
    size_t per_vertex_data_size;

    struct VertexAttributeInternalPlatform_* per_instance_attributes_impl;
    DivisionVertexAttribute* per_instance_attributes;
    int32_t per_instance_attribute_count;
    size_t per_instance_data_size;

    int32_t vertex_count;
    int32_t instance_count;

    DivisionRenderTopology topology;
} DivisionVertexBuffer;

typedef struct DivisionVertexBufferSystemContext {
    DivisionUnorderedIdTable id_table;
    DivisionVertexBuffer* buffers;
    struct DivisionVertexBufferInternalPlatform_* buffers_impl;
    size_t buffers_count;
} DivisionVertexBufferSystemContext;

typedef struct DivisionVertexBufferSettings {
    const DivisionVertexAttributeSettings* per_vertex_attributes;
    const DivisionVertexAttributeSettings* per_instance_attributes;
    int32_t per_vertex_attribute_count;
    int32_t per_instance_attribute_count;
    int32_t vertex_count;
    int32_t instance_count;
    DivisionRenderTopology topology;
} DivisionVertexBufferSettings;


bool division_engine_internal_vertex_buffer_context_alloc(DivisionContext* ctx, const DivisionSettings* settings);
void division_engine_internal_vertex_buffer_context_free(DivisionContext* ctx);

#ifdef __cplusplus
extern "C" {
#endif


DIVISION_EXPORT bool division_engine_vertex_buffer_alloc(
    DivisionContext* ctx, const DivisionVertexBufferSettings* vertex_buffer_settings, uint32_t* out_vertex_buffer_id);

DIVISION_EXPORT void division_engine_vertex_buffer_free(DivisionContext* ctx, uint32_t vertex_buffer_id);

DIVISION_EXPORT void* division_engine_vertex_buffer_borrow_per_vertex_data_pointer(
    DivisionContext* ctx, uint32_t vertex_buffer);
DIVISION_EXPORT void division_engine_vertex_buffer_return_per_vertex_data_pointer(
    DivisionContext* ctx, uint32_t vertex_buffer, void* data_pointer);

DIVISION_EXPORT void* division_engine_vertex_buffer_borrow_per_instance_data_pointer(
    DivisionContext* ctx, uint32_t vertex_buffer);
DIVISION_EXPORT void division_engine_vertex_buffer_return_per_instance_data_pointer(
    DivisionContext* ctx, uint32_t vertex_buffer, void* data_pointer);

#ifdef __cplusplus
}
#endif