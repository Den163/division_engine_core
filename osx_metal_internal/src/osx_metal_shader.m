#include "division_engine_core/data_structures/unordered_id_table.h"
#include "division_engine_core/types/division_lifecycle.h"

#include "division_engine_core/platform_internal/platfrom_shader.h"

#include "division_engine_core/renderer.h"
#include "osx_window_context.h"
#include <MetalKit/MetalKit.h>
#include <stdbool.h>

static inline bool create_shader_program(
    DivisionContext* ctx,
    const DivisionShaderSourceDescriptor* descriptors,
    int32_t descriptor_count,
    DivisionMetalShaderProgram* out_shader_program
);

bool division_engine_internal_platform_shader_system_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings
)
{
    ctx->shader_context->shaders_impl = NULL;

    return true;
}

void division_engine_internal_platform_shader_system_context_free(DivisionContext* ctx)
{
    DivisionShaderSystemContext* shader_context = ctx->shader_context;

    for (int32_t i = 0; i < shader_context->shader_count; i++)
    {
        DivisionMetalShaderProgram* shader_program = &shader_context->shaders_impl[i];
        shader_program->vertex_function = nil;
        shader_program->fragment_function = nil;
    }
    free(shader_context->shaders_impl);
}

bool division_engine_internal_platform_shader_program_alloc(
    DivisionContext* ctx,
    const DivisionShaderSourceDescriptor* shader_descriptors,
    int32_t shader_descriptor_count,
    uint32_t* out_shader_program_id
)
{
    DivisionShaderSystemContext* shader_ctx = ctx->shader_context;
    size_t shader_program_count = shader_ctx->shader_count;

    DivisionMetalShaderProgram shader_program = {
        .vertex_function = nil,
        .fragment_function = nil,
    };

    if (!create_shader_program(
            ctx, shader_descriptors, shader_descriptor_count, &shader_program
        ))
    {
        return false;
    }

    uint32_t program_id = division_unordered_id_table_new_id(&shader_ctx->id_table);

    if (program_id >= shader_program_count)
    {
        shader_ctx->shaders_impl = realloc(
            shader_ctx->shaders_impl, sizeof(DivisionMetalShaderProgram[program_id + 1])
        );

        if (shader_ctx->shaders_impl == NULL)
        {
            division_unordered_id_table_remove_id(&shader_ctx->id_table, program_id);
            ctx->lifecycle.error_callback(
                ctx,
                DIVISION_INTERNAL_ERROR,
                "Failed to reallocate Shader Implementation array"
            );

            return false;
        }
    }

    shader_ctx->shaders_impl[program_id] = shader_program;

    *out_shader_program_id = program_id;
    return true;
}

bool create_shader_program(
    DivisionContext* ctx,
    const DivisionShaderSourceDescriptor* descriptors,
    int32_t descriptor_count,
    DivisionMetalShaderProgram* out_shader_program
)
{
    @autoreleasepool
    {
        id<MTLDevice> device =
            ctx->renderer_context->window_data->app_delegate->viewDelegate->device;
        NSError* err = nil;

        for (int32_t i = 0; i < descriptor_count; i++)
        {
            const DivisionShaderSourceDescriptor* desc = &descriptors[i];

            id<MTLLibrary> library =
                [device newLibraryWithSource:[NSString stringWithUTF8String:desc->source]
                                     options:nil
                                       error:&err];

            if (err)
            {
                ctx->lifecycle.error_callback(
                    ctx, DIVISION_INTERNAL_ERROR, [[err debugDescription] UTF8String]
                );
                return false;
            }

            if (!library)
            {
                ctx->lifecycle.error_callback(
                    ctx, DIVISION_INTERNAL_ERROR, "Created shader library is null\n"
                );
                return false;
            }

            MTLFunctionDescriptor* mtl_func_desc = [MTLFunctionDescriptor new];
            [mtl_func_desc
                setName:[NSString stringWithUTF8String:desc->entry_point_name]];

            id<MTLFunction> func = [library newFunctionWithDescriptor:mtl_func_desc
                                                                error:&err];
            if (err)
            {
                ctx->lifecycle.error_callback(
                    ctx, DIVISION_INTERNAL_ERROR, [[err debugDescription] UTF8String]
                );
                return false;
            }

            switch (desc->type)
            {
            case DIVISION_SHADER_VERTEX:
                out_shader_program->vertex_function = func;
                break;
            case DIVISION_SHADER_FRAGMENT:
                out_shader_program->fragment_function = func;
                break;
            default:
                fprintf(stderr, "Unknown shader function type `%d`\n", desc->type);
                return false;
            }
        }

        return true;
    }
}

void division_engine_internal_platform_shader_program_free(
    DivisionContext* ctx, uint32_t shader_program_id
)
{
    DivisionMetalShaderProgram* shader =
        &ctx->shader_context->shaders_impl[shader_program_id];
    shader->fragment_function = nil;
    shader->vertex_function = nil;

    division_unordered_id_table_remove_id(
        &ctx->shader_context->id_table, shader_program_id
    );
}
