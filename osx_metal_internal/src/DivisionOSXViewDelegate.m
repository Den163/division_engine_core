#include "DivisionOSXViewDelegate.h"

#include "division_engine_core/renderer.h"
#include "division_engine_core/render_pass.h"
#include "division_engine_core/texture.h"
#include "division_engine_core/uniform_buffer.h"
#include "division_engine_core/io_utility.h"
#include "osx_render_pass.h"
#include "osx_texture.h"
#include "osx_vertex_buffer.h"
#include "osx_uniform_buffer.h"

#define MTL_VERTEX_DATA_BUFFER_INDEX 29
#define MTL_VERTEX_DATA_INSTANCE_ARRAY_INDEX 30

typedef struct DivisionToMslAttrTraits_
{
    MTLVertexFormat vertex_format;
    int32_t divide_by_components;
} DivisionToMslAttrTraits_;

static inline void define_msl_vertex_attributes(
    const DivisionVertexAttribute* attributes, int32_t attribute_count,
    MTLVertexAttributeDescriptorArray* attr_desc_arr, size_t attributes_offset, int32_t buffer_index);

static inline DivisionToMslAttrTraits_ get_vert_attr_msl_traits_(DivisionShaderVariableType attrType);

@implementation DivisionOSXViewDelegate
- (instancetype)initWithContext:(DivisionContext*)aContext
                       settings:(const DivisionSettings*)aSettings
                         device:(id)aDevice {
    self = [super init];
    if (self)
    {
        context = aContext;
        settings = aSettings;
        device = aDevice;
        commandQueue = [device newCommandQueue];
    }

    return self;
}

- (id <MTLBuffer>)createBufferWithSize:(size_t)data_bytes {
    return [device newBufferWithLength:data_bytes options:MTLResourceStorageModeManaged];
}

- (MTLVertexDescriptor*)createVertexDescriptorForBuffer:(const DivisionVertexBuffer*)vertexBuffer {
    MTLVertexDescriptor* descriptor = [MTLVertexDescriptor new];
    MTLVertexAttributeDescriptorArray* attrDescArray = [descriptor attributes];

    define_msl_vertex_attributes(
        vertexBuffer->per_vertex_attributes,
        vertexBuffer->per_vertex_attribute_count,
        attrDescArray,
        0,
        MTL_VERTEX_DATA_BUFFER_INDEX
    );

    MTLVertexBufferLayoutDescriptor* perVertexLayoutDesc =
        [[descriptor layouts] objectAtIndexedSubscript:MTL_VERTEX_DATA_BUFFER_INDEX];

    [perVertexLayoutDesc setStride:vertexBuffer->per_vertex_data_size];
    [perVertexLayoutDesc setStepFunction:MTLVertexStepFunctionPerVertex];

    if (vertexBuffer->per_instance_attribute_count > 0)
    {
        define_msl_vertex_attributes(
            vertexBuffer->per_instance_attributes,
            vertexBuffer->per_instance_attribute_count,
            attrDescArray,
            vertexBuffer->per_vertex_data_size * vertexBuffer->vertex_count,
            MTL_VERTEX_DATA_INSTANCE_ARRAY_INDEX
        );

        MTLVertexBufferLayoutDescriptor* perInstanceLayoutDesc =
            [[descriptor layouts] objectAtIndexedSubscript:MTL_VERTEX_DATA_INSTANCE_ARRAY_INDEX];

        [perInstanceLayoutDesc setStride:vertexBuffer->per_instance_data_size];
        [perInstanceLayoutDesc setStepFunction:MTLVertexStepFunctionPerInstance];
    }

    return descriptor;
}

- (bool)createShaderProgramWithSettings:(const DivisionShaderSettings*)shaderSettings
                            sourceCount:(int32_t)sourceCount
                             outProgram:(DivisionMetalShaderProgram*)out_program {
    @autoreleasepool
    {
        NSError* err = nil;

        for (int32_t i = 0; i < sourceCount; i++)
        {
            DivisionShaderSettings shader = shaderSettings[i];

            void* shader_src;
            size_t shader_size;
            if (!division_io_read_all_bytes_from_file(shader.file_path, &shader_src, &shader_size))
            {
                context->error_callback(DIVISION_INTERNAL_ERROR, "Failed to read from file");
                return false;
            }

            id <MTLLibrary> library = [device
                newLibraryWithSource:[NSString stringWithUTF8String:shader_src] options:nil error:&err];
            free(shader_src);

            if (err)
            {
                context->error_callback(DIVISION_INTERNAL_ERROR, [[err debugDescription] UTF8String]);
                return false;
            }

            if (!library)
            {
                context->error_callback(DIVISION_INTERNAL_ERROR, "Created shader library is null\n");
                return false;
            }

            id <MTLFunction> func = [library newFunctionWithName:[NSString stringWithUTF8String:shader.entry_point_name]];

            switch (shader.type)
            {
                case DIVISION_SHADER_VERTEX:
                    out_program->vertex_function = func;
                    break;
                case DIVISION_SHADER_FRAGMENT:
                    out_program->fragment_function = func;
                    break;
                default:
                    fprintf(stderr, "Unknown shader function type `%d`\n", shader.type);
                    return false;
            }
        }

        return true;
    }
}

+ (instancetype)withContext:(DivisionContext*)aContext
                   settings:(const DivisionSettings*)aSettings
                     device:(id)aDevice {
    return [[self alloc] initWithContext:aContext settings:aSettings device:aDevice];
}


- (void)drawInMTKView:(nonnull MTKView*)view {
    settings->update_callback(context);

    @autoreleasepool
    {
        DivisionColor clearColor = context->renderer_context->clear_color;
        [view setClearColor:MTLClearColorMake(clearColor.r, clearColor.g, clearColor.b, clearColor.a)];

        id <MTLCommandBuffer> cmdBuffer = [commandQueue commandBuffer];
        MTLRenderPassDescriptor* renderPassDesc = [view currentRenderPassDescriptor];
        id <MTLRenderCommandEncoder> renderEnc = [cmdBuffer renderCommandEncoderWithDescriptor:renderPassDesc];

        DivisionRenderPassSystemContext* render_pass_ctx = context->render_pass_context;
        DivisionVertexBufferSystemContext* vert_buff_ctx = context->vertex_buffer_context;
        DivisionUniformBufferSystemContext* uniform_buff_ctx = context->uniform_buffer_context;
        DivisionTextureSystemContext* tex_ctx = context->texture_context;

        uint32_t* render_pass_ids = render_pass_ctx->id_table.orders;
        for (int32_t i = 0; i < render_pass_ctx->id_table.orders_count; i++)
        {
            DivisionRenderPass* pass = &render_pass_ctx->render_passes[render_pass_ids[i]];

            float* blend_color = pass->alpha_blending_options.constant_blend_color;
            DivisionRenderPassInternalPlatform_* pass_impl = &render_pass_ctx->render_passes_impl[i];
            DivisionVertexBufferInternalPlatform_ vert_buffer_impl = vert_buff_ctx->buffers_impl[pass->vertex_buffer];

            id <MTLRenderPipelineState> pipelineState = pass_impl->mtl_pipeline_state;
            id <MTLBuffer> vertDataMtlBuffer = vert_buffer_impl.mtl_vertex_buffer;

            [renderEnc setRenderPipelineState:pipelineState];
            [renderEnc setVertexBuffer:vertDataMtlBuffer offset:0 atIndex:MTL_VERTEX_DATA_BUFFER_INDEX];

            for (int ubIdx = 0; ubIdx < pass->uniform_vertex_buffer_count; ubIdx++)
            {
                uint32_t buff_id = pass->uniform_vertex_buffers[ubIdx];
                id <MTLBuffer> uniformBuff = uniform_buff_ctx->uniform_buffers_impl[buff_id].mtl_buffer;
                DivisionUniformBufferDescriptor buffDesc = uniform_buff_ctx->uniform_buffers[buff_id];
                [renderEnc setVertexBuffer:uniformBuff offset:0 atIndex:buffDesc.binding];
            }

            for (int ubIdx = 0; ubIdx < pass->uniform_fragment_buffer_count; ubIdx++)
            {
                uint32_t buff_id = pass->uniform_fragment_buffers[ubIdx];
                id <MTLBuffer> uniformBuff = uniform_buff_ctx->uniform_buffers_impl[buff_id].mtl_buffer;
                DivisionUniformBufferDescriptor buffDesc = uniform_buff_ctx->uniform_buffers[buff_id];
                [renderEnc setFragmentBuffer:uniformBuff offset:0 atIndex:buffDesc.binding];
            }

            for (int texIdx = 0; texIdx < pass->fragment_texture_count; texIdx++)
            {
                const DivisionIdWithBinding* texture_binding = &pass->fragment_textures[texIdx];
                const DivisionTextureImpl_* tex_impl = &tex_ctx->textures_impl[texture_binding->id];

                [renderEnc setFragmentTexture:tex_impl->mtl_texture atIndex:texture_binding->shader_location];
            }

            [renderEnc
                setBlendColorRed: blend_color[0] green: blend_color[1] blue: blend_color[2] alpha: blend_color[3]];

            if (pass->instance_count > 0)
            {
                [renderEnc setVertexBuffer:vertDataMtlBuffer
                                    offset:0
                                   atIndex:MTL_VERTEX_DATA_INSTANCE_ARRAY_INDEX];

                [renderEnc drawPrimitives:vert_buffer_impl.mtl_primitive_type
                              vertexStart:pass->first_vertex
                              vertexCount:pass->vertex_count
                            instanceCount:pass->instance_count
                ];
            }
            else
            {
                [renderEnc drawPrimitives:vert_buffer_impl.mtl_primitive_type
                              vertexStart:pass->first_vertex
                              vertexCount:pass->vertex_count
                ];
            }

        }

        [renderEnc endEncoding];
        [cmdBuffer presentDrawable:[view currentDrawable]];
        [cmdBuffer commit];
    }
}

- (void)mtkView:(nonnull MTKView*)view drawableSizeWillChange:(CGSize)size {
}
@end

void define_msl_vertex_attributes(
    const DivisionVertexAttribute* attributes, int32_t attribute_count,
    MTLVertexAttributeDescriptorArray* attr_desc_arr, size_t attributes_offset, int32_t buffer_index)
{
    for (int i = 0; i < attribute_count; i++)
    {
        const DivisionVertexAttribute* attr = &attributes[i];
        DivisionToMslAttrTraits_ traits = get_vert_attr_msl_traits_(attr->type);
        size_t gl_attr_comp_count = (attr->component_count / traits.divide_by_components);
        size_t comp_size = attr->base_size * gl_attr_comp_count;
        size_t offset = attributes_offset + attr->offset;

        for (int comp_idx = 0; comp_idx < gl_attr_comp_count; comp_idx++)
        {
            MTLVertexAttributeDescriptor* attrDesc = [attr_desc_arr objectAtIndexedSubscript:attr->location + comp_idx];
            [attrDesc setOffset:offset + comp_idx * comp_size];
            [attrDesc setBufferIndex:buffer_index];
            [attrDesc setFormat:traits.vertex_format];
        }
    }
}

DivisionToMslAttrTraits_ get_vert_attr_msl_traits_(DivisionShaderVariableType attrType)
{
    switch (attrType)
    {
        case DIVISION_FLOAT:
            return (DivisionToMslAttrTraits_) {MTLVertexFormatFloat, 1};
        case DIVISION_INTEGER:
            return (DivisionToMslAttrTraits_) {MTLVertexFormatInt, 1};
        case DIVISION_FVEC2:
            return (DivisionToMslAttrTraits_) {MTLVertexFormatFloat2, 1};
        case DIVISION_FVEC3:
            return (DivisionToMslAttrTraits_) {MTLVertexFormatFloat3, 1};
        case DIVISION_FVEC4:
            return (DivisionToMslAttrTraits_) {MTLVertexFormatFloat4, 1};
        case DIVISION_FMAT4X4:
            return (DivisionToMslAttrTraits_) {MTLVertexFormatFloat4, 4};
        case DIVISION_DOUBLE:
        default:
            fprintf(stderr, "Unsupported attribute format");
            exit(1);
    }
}
