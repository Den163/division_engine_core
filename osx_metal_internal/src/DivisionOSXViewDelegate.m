#include "DivisionOSXViewDelegate.h"

#include <Foundation/Foundation.h>

#include "division_engine/renderer.h"
#include "division_engine/render_pass.h"
#include "division_engine/uniform_buffer.h"
#include "osx_render_pass.h"
#include "osx_vertex_buffer.h"
#include "osx_uniform_buffer.h"

#define MTL_VERTEX_DATA_BUFFER_INDEX 0

static inline MTLVertexFormat division_attribute_type_to_mtl_format(DivisionShaderVariableType attrType);

static char* readFromFile(const char* path);

@implementation DivisionOSXViewDelegate
- (instancetype)initWithContext:(DivisionContext*)aContext
                       settings:(const DivisionSettings*)aSettings
                         device:(id)aDevice
{
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

- (id <MTLBuffer>)createBufferWithSize:(size_t)data_bytes
{
    return [device newBufferWithLength:data_bytes options:MTLResourceStorageModeManaged];
}

- (MTLVertexDescriptor*)createVertexDescriptorForBuffer:(const DivisionVertexBuffer*)vertexBuffer
{
    MTLVertexDescriptor* descriptor = [MTLVertexDescriptor new];
    MTLVertexAttributeDescriptorArray* attrDescArray = [descriptor attributes];
    for (int i = 0; i < vertexBuffer->attribute_count; i++)
    {
        const DivisionVertexAttribute* attr = &vertexBuffer->attributes[i];
        MTLVertexAttributeDescriptor* attrDesc = [attrDescArray objectAtIndexedSubscript:attr->location];
        [attrDesc setOffset:attr->offset];
        [attrDesc setBufferIndex:MTL_VERTEX_DATA_BUFFER_INDEX];
        [attrDesc setFormat:division_attribute_type_to_mtl_format(attr->type)];
    }

    [[[descriptor layouts] objectAtIndexedSubscript:MTL_VERTEX_DATA_BUFFER_INDEX]
                  setStride:vertexBuffer->per_vertex_data_size];

    return descriptor;
}

- (id <MTLRenderPipelineState>)createRenderPipelineStateForShaderProgram:(const DivisionMetalShaderProgram*)program
                                                        vertexDescriptor:(MTLVertexDescriptor*)desc
{
    MTLRenderPipelineDescriptor* pipeline_descriptor = [MTLRenderPipelineDescriptor new];
    if (program->vertex_function != NULL)
    {
        [pipeline_descriptor setVertexFunction:program->vertex_function];
        [pipeline_descriptor setVertexDescriptor:desc];
    }

    if (program->fragment_function != NULL)
    {
        [pipeline_descriptor setFragmentFunction:program->fragment_function];
    }

    [[[pipeline_descriptor colorAttachments] objectAtIndexedSubscript:0]
                           setPixelFormat:MTLPixelFormatBGRA8Unorm_sRGB];

    NSError* err = nil;
    id <MTLRenderPipelineState> renderPipelineState = [device
        newRenderPipelineStateWithDescriptor:pipeline_descriptor
                                       error:&err
    ];

    if (err)
    {
        fprintf(stderr, "%s\n", [[err debugDescription] UTF8String]);
        return nil;
    }

    if (!renderPipelineState)
    {
        fprintf(stderr, "Render pipeline state is null\n");
        return nil;
    }

    return renderPipelineState;
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

            char* shaderSrc = readFromFile(shader.file_path);


            id <MTLLibrary> library = [device
                newLibraryWithSource:[NSString stringWithUTF8String:shaderSrc] options:nil error:&err];
            free(shaderSrc);

            if (err)
            {
                fprintf(stderr, "%s\n", [[err debugDescription] UTF8String]);
                return false;
            }

            if (!library)
            {
                fprintf(stderr, "Created shader library is null\n");
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
                     device:(id)aDevice
{
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

        for (int32_t i = 0; i < render_pass_ctx->render_pass_count; i++)
        {
            DivisionRenderPass* pass = &render_pass_ctx->render_passes[i];
            DivisionRenderPassInternalPlatform_* pass_impl = &render_pass_ctx->render_passes_impl[i];

            id <MTLRenderPipelineState> pipelineState = pass_impl->mtl_pipeline_state;
            id <MTLBuffer> vertDataMtlBuffer = vert_buff_ctx->buffers_impl[pass->vertex_buffer].mtl_buffer;

            [renderEnc setRenderPipelineState:pipelineState];

            [renderEnc setVertexBuffer:vertDataMtlBuffer offset:0 atIndex:MTL_VERTEX_DATA_BUFFER_INDEX];

            for (int ubIdx = 0; ubIdx < pass->uniform_buffer_count; ubIdx++)
            {
                int32_t buff_id = pass->uniform_buffers[ubIdx];
                id <MTLBuffer> uniformMtlBuffer = uniform_buff_ctx->uniform_buffers_impl[buff_id].mtl_buffer;
                DivisionUniformBuffer buffDesc = uniform_buff_ctx->uniform_buffers[buff_id];
                switch (buffDesc.shaderType)
                {
                    case DIVISION_SHADER_VERTEX:
                        [renderEnc setVertexBuffer:uniformMtlBuffer offset:0 atIndex:buffDesc.binding];
                        break;
                    case DIVISION_SHADER_FRAGMENT:
                        [renderEnc setFragmentBuffer:uniformMtlBuffer offset:0 atIndex:buffDesc.binding];
                        break;
                    default:
                        fprintf(stderr, "Unknown shader type in the pass");
                        break;
                }
            }

            [renderEnc
                drawPrimitives:MTLPrimitiveTypeTriangle
                   vertexStart:pass->first_vertex
                   vertexCount:pass->vertex_count
            ];
        }

        [renderEnc endEncoding];
        [cmdBuffer presentDrawable: [view currentDrawable]];
        [cmdBuffer commit];
    }
}

- (void)mtkView:(nonnull MTKView*)view drawableSizeWillChange:(CGSize)size {
}
@end

char* readFromFile(const char* path)
{
    FILE* srcFile = fopen(path, "rt");
    if (!srcFile)
    {
        fprintf(stderr, "Cannot open file: %s\n Current dir: %s\n", path, getenv("PWD"));
        return NULL;
    }

    fseek(srcFile, 0, SEEK_END);
    size_t fileSize = ftell(srcFile);
    fseek(srcFile, 0, SEEK_SET);

    char* shaderSrc = malloc(sizeof(char) * fileSize);
    size_t readSize = fread(shaderSrc, sizeof(char), fileSize, srcFile);
    fclose(srcFile);

    if (readSize != fileSize)
    {
        fprintf(stderr, "Error while reading the file by path: %s\n", path);
        return NULL;
    }

    return shaderSrc;
}

MTLVertexFormat division_attribute_type_to_mtl_format(DivisionShaderVariableType attrType)
{
    switch (attrType)
    {
        case DIVISION_FLOAT:
            return MTLVertexFormatFloat;
        case DIVISION_INTEGER:
            return MTLVertexFormatInt;
        case DIVISION_FVEC2:
            return MTLVertexFormatFloat2;
        case DIVISION_FVEC3:
            return MTLVertexFormatFloat3;
        case DIVISION_FVEC4:
            return MTLVertexFormatFloat4;
        case DIVISION_DOUBLE:
        case DIVISION_FMAT4X4:
        default:
            fprintf(stderr, "Unsupported attribute format");
            return 0;
    }
}
