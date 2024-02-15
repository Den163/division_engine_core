#pragma once

#include <stddef.h>
#include <stdint.h>

#include "context.h"
#include "types/render_pass_descriptor.h"

#include "data_structures/ordered_id_table.h"
#include "data_structures/unordered_id_table.h"

#include <division_engine_core_export.h>

typedef struct DivisionRenderPassSystemContext
{
    DivisionUnorderedIdTable id_table;
    DivisionRenderPassDescriptor* render_pass_descriptors;
    struct DivisionRenderPassInternalPlatform_* render_passes_descriptors_impl;
    int32_t render_pass_count;
} DivisionRenderPassSystemContext;

#define DIVISION_GET_RENDER_PASS_DESCRIPTOR(ctx, render_pass_id) \
    (&ctx->render_pass_context->render_pass_descriptors[render_pass_id])

bool division_engine_render_pass_system_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings
);

void division_engine_render_pass_system_context_free(DivisionContext* ctx);

#ifdef __cplusplus
extern "C"
{
#endif

    DIVISION_EXPORT bool division_engine_render_pass_descriptor_alloc(
        DivisionContext* ctx,
        const DivisionRenderPassDescriptor* render_pass_descriptor,
        uint32_t* out_render_pass_descriptor_id
    );

    // TODO: delete
    DIVISION_EXPORT DivisionRenderPassDescriptor* division_engine_render_pass_descriptor_borrow(
        DivisionContext* ctx, uint32_t render_pass_id
    );

    // TODO: delete
    DIVISION_EXPORT void division_engine_render_pass_descriptor_return(
        DivisionContext* ctx,
        uint32_t render_pass_id,
        DivisionRenderPassDescriptor* render_pass_ptr
    );

    DIVISION_EXPORT void division_engine_render_pass_descriptor_free(
        DivisionContext* ctx, uint32_t render_pass_id
    );

#ifdef __cplusplus
}
#endif