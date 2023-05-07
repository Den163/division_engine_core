#pragma once

#include <stdint.h>
#include <stddef.h>

#include "context.h"
#include "division_engine_core_export.h"
#include "division_engine/shader.h"

struct DivisionUniformBufferInternal_;

typedef struct DivisionUniformBuffer {
    size_t data_bytes;
    int32_t binding;
    DivisionShaderType  shaderType;
} DivisionUniformBuffer;

typedef struct DivisionUniformBufferSystemContext {
    DivisionUniformBuffer* uniform_buffers;
    struct DivisionUniformBufferInternal_* uniform_buffers_impl;
    int32_t uniform_buffer_count;
} DivisionUniformBufferSystemContext;

bool division_engine_internal_uniform_buffer_context_alloc(DivisionContext* ctx, const DivisionSettings* settings);
void division_engine_internal_uniform_buffer_context_free(DivisionContext* ctx);

#ifdef __cplusplus
extern "C" {
#endif

DIVISION_EXPORT int32_t division_engine_uniform_buffer_alloc(DivisionContext* ctx, DivisionUniformBuffer buffer);
DIVISION_EXPORT void* division_engine_uniform_buffer_borrow_data_pointer(DivisionContext* ctx, int32_t buffer);
DIVISION_EXPORT void
division_engine_uniform_buffer_return_data_pointer(DivisionContext* ctx, int32_t buffer, void* data_pointer);

#ifdef __cplusplus
}
#endif