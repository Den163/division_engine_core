#pragma once

#include <stddef.h>
#include <stdint.h>

#include "context.h"
#include "division_engine_core/data_structures/unordered_id_table.h"
#include "division_engine_core/shader.h"
#include "division_engine_core_export.h"

struct DivisionUniformBufferInternal_;

typedef struct DivisionUniformBufferDescriptor
{
    size_t data_bytes;
} DivisionUniformBufferDescriptor;

typedef struct DivisionUniformBufferSystemContext
{
    DivisionUnorderedIdTable id_table;
    DivisionUniformBufferDescriptor* uniform_buffers;
    struct DivisionUniformBufferInternal_* uniform_buffers_impl;
    size_t uniform_buffer_count;
} DivisionUniformBufferSystemContext;

bool division_engine_uniform_buffer_system_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings
);

void division_engine_uniform_buffer_system_context_free(DivisionContext* ctx);

#ifdef __cplusplus
extern "C"
{
#endif

    DIVISION_EXPORT bool division_engine_uniform_buffer_alloc(
        DivisionContext* ctx,
        DivisionUniformBufferDescriptor buffer,
        uint32_t* out_buffer_id
    );

    DIVISION_EXPORT void division_engine_uniform_buffer_free(
        DivisionContext* ctx, uint32_t buffer_id
    );

    DIVISION_EXPORT void* division_engine_uniform_buffer_borrow_data_pointer(
        DivisionContext* ctx, uint32_t buffer_id
    );
    DIVISION_EXPORT void division_engine_uniform_buffer_return_data_pointer(
        DivisionContext* ctx, uint32_t buffer_id, void* data_pointer
    );

#ifdef __cplusplus
}
#endif