#pragma once

#include <stdbool.h>
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

typedef struct DivisionVertexAttributeSettings
{
    DivisionShaderVariableType type;
    int32_t location;
} DivisionVertexAttributeSettings;

typedef struct DivisionVertexAttribute
{
    int32_t location;
    int32_t offset;
    int32_t base_size;
    int32_t component_count;
    DivisionShaderVariableType type;
} DivisionVertexAttribute;

typedef struct DivisionVertexBuffer
{
    DivisionVertexAttribute* per_vertex_attributes;
    int32_t per_vertex_attribute_count;
    size_t per_vertex_data_size;

    DivisionVertexAttribute* per_instance_attributes;
    int32_t per_instance_attribute_count;
    size_t per_instance_data_size;

    int32_t vertex_count;
    int32_t index_count;
    int32_t instance_count;

    DivisionRenderTopology topology;
} DivisionVertexBuffer;

typedef struct DivisionVertexBufferSystemContext
{
    DivisionUnorderedIdTable id_table;
    DivisionVertexBuffer* buffers;
    struct DivisionVertexBufferInternalPlatform_* buffers_impl;
    size_t buffers_count;
} DivisionVertexBufferSystemContext;

typedef struct DivisionVertexBufferSettings
{
    const DivisionVertexAttributeSettings* per_vertex_attributes;
    const DivisionVertexAttributeSettings* per_instance_attributes;
    int32_t per_vertex_attribute_count;
    int32_t per_instance_attribute_count;
    int32_t vertex_count;
    int32_t index_count;
    int32_t instance_count;
    DivisionRenderTopology topology;
} DivisionVertexBufferSettings;

typedef struct DivisionVertexBufferBorrowedData
{
    void* vertex_data_ptr;
    void* index_data_ptr;
    void* instance_data_ptr;

    uint32_t vertex_count;
    uint32_t index_count;
    uint32_t instance_count;
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
        const DivisionVertexBufferSettings* vertex_buffer_settings,
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
    
#ifdef __cplusplus
}
#endif