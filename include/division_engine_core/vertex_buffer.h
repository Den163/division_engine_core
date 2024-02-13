#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "context.h"
#include "shader.h"

#include <division_engine_core_export.h>

typedef enum DivisionRenderTopology
{
    DIVISION_TOPOLOGY_TRIANGLES = 1,
    DIVISION_TOPOLOGY_POINTS = 2,
    DIVISION_TOPOLOGY_LINES = 3
} DivisionRenderTopology;

typedef struct DivisionVertexBufferSize
{
    uint32_t vertex_count;
    uint32_t index_count;
    uint32_t instance_count;
} DivisionVertexBufferSize;

typedef struct DivisionVertexAttributeSettings
{
    DivisionShaderVariableType type;
    int32_t location;
} DivisionVertexAttributeSettings;

typedef struct DivisionVertexBufferSettings
{
    DivisionVertexBufferSize size;
    DivisionVertexAttributeSettings* per_vertex_attributes;
    DivisionVertexAttributeSettings* per_instance_attributes;
    int32_t per_vertex_attribute_count;
    int32_t per_instance_attribute_count;
    DivisionRenderTopology topology;
} DivisionVertexBufferSettings;

typedef struct DivisionVertexBufferConstSettings 
{
    DivisionVertexBufferSize size;
    const DivisionVertexAttributeSettings* per_vertex_attributes;
    const DivisionVertexAttributeSettings* per_instance_attributes;
    int32_t per_vertex_attribute_count;
    int32_t per_instance_attribute_count;
    DivisionRenderTopology topology;
} DivisionVertexBufferConstSettings;

typedef struct DivisionVertexAttribute
{
    int32_t offset;
    int32_t base_size;
    int32_t component_count;
} DivisionVertexAttribute;

typedef struct DivisionVertexBuffer
{
    DivisionVertexBufferSettings settings;

    DivisionVertexAttribute* per_vertex_attributes;
    DivisionVertexAttribute* per_instance_attributes;

    size_t per_vertex_data_size;
    size_t per_instance_data_size;
} DivisionVertexBuffer;

typedef struct DivisionVertexBufferSystemContext
{
    DivisionUnorderedIdTable id_table;
    DivisionVertexBuffer* buffers;
    struct DivisionVertexBufferInternalPlatform_* buffers_impl;
    size_t buffers_count;
} DivisionVertexBufferSystemContext;

typedef struct DivisionVertexBufferBorrowedData
{
    DivisionVertexBufferSize size;

    void* vertex_data_ptr;
    void* index_data_ptr;
    void* instance_data_ptr;
} DivisionVertexBufferBorrowedData;

bool division_engine_vertex_buffer_system_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings
);
void division_engine_vertex_buffer_system_context_free(DivisionContext* ctx);

#ifdef __cplusplus
extern "C"
{
#endif

    /*
     *  Vertex specification:
     *  1. Non-instanced rendering buffer. Attributes are interleaved, consecutive in the
     * order they are received
     *  2. Instanced rendering buffer. First goes vertex data,
     *  after - interleaved instance data attributes in the order they are received
     */
    DIVISION_EXPORT bool division_engine_vertex_buffer_alloc(
        DivisionContext* ctx,
        const DivisionVertexBufferConstSettings* vertex_buffer_settings,
        uint32_t* out_vertex_buffer_id
    );

    DIVISION_EXPORT void division_engine_vertex_buffer_free(
        DivisionContext* ctx, uint32_t vertex_buffer_id
    );

    DIVISION_EXPORT bool division_engine_vertex_buffer_borrow_data(
        DivisionContext* ctx,
        uint32_t vertex_buffer,
        DivisionVertexBufferBorrowedData* out_borrow_data
    );

    DIVISION_EXPORT void division_engine_vertex_buffer_return_data(
        DivisionContext* ctx,
        uint32_t vertex_buffer,
        DivisionVertexBufferBorrowedData* borrow_data
    );

    DIVISION_EXPORT bool division_engine_vertex_buffer_resize(
        DivisionContext* ctx, uint32_t vertex_buffer, DivisionVertexBufferSize new_size
    );

#ifdef __cplusplus
}
#endif

static inline size_t division_engine_vertex_buffer_vertices_bytes(
    const DivisionVertexBuffer* vertex_buffer
)
{
    return vertex_buffer->settings.size.vertex_count *
           vertex_buffer->per_vertex_data_size;
}

static inline size_t division_engine_vertex_buffer_indices_bytes(
    const DivisionVertexBuffer* vertex_buffer
)
{
    return vertex_buffer->settings.size.index_count * sizeof(uint32_t);
}

static inline size_t division_engine_vertex_buffer_instances_bytes(
    const DivisionVertexBuffer* vertex_buffer
)
{
    return vertex_buffer->settings.size.instance_count *
           vertex_buffer->per_instance_data_size;
}