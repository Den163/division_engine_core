#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "context.h"
#include "types/vertex_buffer.h"

#include "data_structures/unordered_id_table.h"

#include <division_engine_core_export.h>

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