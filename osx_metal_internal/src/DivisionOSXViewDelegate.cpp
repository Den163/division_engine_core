#include "DivisionOSXViewDelegate.h"
#include <cstdio>

#include <NSUtils.h>

#include "division_engine/render_pass.h"
#include "division_engine/renderer.h"
#include "division_engine/uniform_buffer.h"
#include "division_engine/vertex_buffer.h"
#include "osx_render_pass.h"
#include "osx_shader_context.h"
#include "osx_uniform_buffer.h"
#include "osx_vertex_buffer.h"

#define MTL_VERTEX_DATA_BUFFER_INDEX 0

static char* readFromFile(const char* path);
static inline MTL::VertexFormat division_attribute_type_to_mtl_format(DivisionShaderVariableType attrType);

DivisionOSXViewDelegate::DivisionOSXViewDelegate(
    MTL::Device* device, const DivisionSettings* settings, DivisionContext* context)
    : MTK::ViewDelegate(), _device(device), _commandQueue(device->newCommandQueue()),
      settings(settings), context(context)
{}

DivisionOSXViewDelegate::~DivisionOSXViewDelegate()
{
    _commandQueue->release();
}

MTL::Buffer* DivisionOSXViewDelegate::createBuffer(size_t dataSize)
{
    MTL::Buffer* buff = _device->newBuffer(dataSize, MTL::ResourceStorageModeManaged);
    return buff;
}

void DivisionOSXViewDelegate::deleteBuffer(MTL::Buffer* buffer)
{
    buffer->release();
}

void DivisionOSXViewDelegate::drawInMTKView(MTK::View* pView)
{
    settings->update_callback(context);

    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();

    auto clearColor = context->renderer_context->clear_color;
    pView->setClearColor(MTL::ClearColor::Make(clearColor.r, clearColor.g, clearColor.b, clearColor.a));

    MTL::CommandBuffer* cmdBuffer = _commandQueue->commandBuffer();
    MTL::RenderPassDescriptor* renderPassDesc = pView->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder* renderEnc = cmdBuffer->renderCommandEncoder(renderPassDesc);

    DivisionRenderPassSystemContext* render_pass_ctx = context->render_pass_context;
    DivisionVertexBufferSystemContext* vert_buff_ctx = context->vertex_buffer_context;
    DivisionUniformBufferSystemContext* uniform_buff_ctx = context->uniform_buffer_context;

    for (int32_t i = 0; i < render_pass_ctx->render_pass_count; i++)
    {
        DivisionRenderPass* pass = &render_pass_ctx->render_passes[i];
        DivisionRenderPassInternalPlatform_* pass_impl = &render_pass_ctx->render_passes_impl[i];

        MTL::RenderPipelineState* pipelineState = pass_impl->mtl_pipeline_state;
        MTL::Buffer* vertDataMtlBuffer = vert_buff_ctx->buffers_impl[pass->vertex_buffer].mtl_buffer;

        renderEnc->setRenderPipelineState(pipelineState);
        renderEnc->setVertexBuffer(vertDataMtlBuffer, 0, MTL_VERTEX_DATA_BUFFER_INDEX);

        for (int ubIdx = 0; ubIdx < pass->uniform_buffer_count; ubIdx++)
        {
            int32_t buff_id = pass->uniform_buffers[ubIdx];
            MTL::Buffer* uniformMtlBuffer = uniform_buff_ctx->uniform_buffers_impl[buff_id].mtl_buffer;
            DivisionUniformBuffer buffDesc = uniform_buff_ctx->uniform_buffers[buff_id];
            switch (buffDesc.shaderType)
            {
                case DIVISION_SHADER_VERTEX:
                    renderEnc->setVertexBuffer(uniformMtlBuffer, 0, buffDesc.binding);
                    break;
                case DIVISION_SHADER_FRAGMENT:
                    renderEnc->setFragmentBuffer(uniformMtlBuffer, 0, buffDesc.binding);
                    break;
                default:
                    fprintf(stderr, "Unknown shader type in the pass");
                    break;
            }
        }

        renderEnc->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, pass->first_vertex, pass->vertex_count);
    }

    renderEnc->endEncoding();
    cmdBuffer->presentDrawable(pView->currentDrawable());
    cmdBuffer->commit();

    pool->release();
}

void DivisionOSXViewDelegate::drawableSizeWillChange(MTK::View* pView, CGSize size)
{

}

bool DivisionOSXViewDelegate::createShaderProgram(
    const DivisionShaderSettings* shaderSettings, int32_t source_count, DivisionMetalShaderProgram* out_program)
{
    NS::Error* err = nullptr;

    for (int32_t i = 0; i < source_count; i++)
    {
        DivisionShaderSettings shader = shaderSettings[i];

        char* shaderSrc = readFromFile(shader.file_path);
        MTL::Library* library = _device->newLibrary(NSUtils::createUtf8String(shaderSrc), nullptr, &err);
        free(static_cast<void*>(shaderSrc));

        if (err)
        {
            fprintf(stderr, "%s\n", err->debugDescription()->utf8String());
            return false;
        }

        if (!library)
        {
            fprintf(stderr, "Created shader library is null\n");
            return false;
        }

        auto* func = library->newFunction(NSUtils::createUtf8String(shader.entry_point_name));

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
                break;
        }

        library->release();
    }

    return out_program;
}

void DivisionOSXViewDelegate::deleteShaderProgram(DivisionMetalShaderProgram* program)
{
    program->fragment_function->release();
    program->vertex_function->release();
}

MTL::VertexDescriptor* DivisionOSXViewDelegate::createVertexDescriptor(const DivisionVertexBuffer* vertexBuffer)
{
    MTL::VertexDescriptor* descriptor = MTL::VertexDescriptor::alloc()->init();
    MTL::VertexAttributeDescriptorArray* attrDescArray = descriptor->attributes();
    for (int i = 0; i < vertexBuffer->attribute_count; i++)
    {
        const DivisionVertexAttribute* attr = &vertexBuffer->attributes[i];
        MTL::VertexAttributeDescriptor* attrDesc = attrDescArray->object(attr->location);
        attrDesc->setOffset(attr->offset);
        attrDesc->setBufferIndex(MTL_VERTEX_DATA_BUFFER_INDEX);
        attrDesc->setFormat(division_attribute_type_to_mtl_format(attr->type));
    }

    descriptor->layouts()->object(MTL_VERTEX_DATA_BUFFER_INDEX)->setStride(vertexBuffer->per_vertex_data_size);

    return descriptor;
}

void DivisionOSXViewDelegate::deleteVertexDescriptor(MTL::VertexDescriptor* vertexDescriptor)
{
    vertexDescriptor->release();
}

MTL::RenderPipelineState* DivisionOSXViewDelegate::createRenderPipelineState(
    const DivisionMetalShaderProgram* program, MTL::VertexDescriptor* vertexDescriptor)
{
    auto* pipeline_descriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    if (program->vertex_function != NULL)
    {
        pipeline_descriptor->setVertexFunction(program->vertex_function);
        pipeline_descriptor->setVertexDescriptor(vertexDescriptor);
    }

    if (program->fragment_function != NULL)
    {
        pipeline_descriptor->setFragmentFunction(program->fragment_function);
    }

    pipeline_descriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);

    NS::Error* err = nullptr;
    auto* renderPipelineState = _device->newRenderPipelineState(pipeline_descriptor, &err);
    if (err)
    {
        fprintf(stderr, "%s\n", err->debugDescription()->utf8String());
        pipeline_descriptor->release();
        return NULL;
    }

    if (!renderPipelineState)
    {
        fprintf(stderr, "Render pipeline state is null\n");
        pipeline_descriptor->release();
        return NULL;
    }

    pipeline_descriptor->release();

    return renderPipelineState;
}

void DivisionOSXViewDelegate::deleteRenderPipelineState(MTL::RenderPipelineState* pipelineState)
{
    pipelineState->release();
}

char* readFromFile(const char* path)
{
    FILE* srcFile = std::fopen(path, "rt");
    if (!srcFile)
    {
        fprintf(stderr, "Cannot open file: %s\n Current dir: %s\n", path, getenv("PWD"));
        return nullptr;
    }

    std::fseek(srcFile, 0, SEEK_END);
    size_t fileSize = std::ftell(srcFile);
    std::fseek(srcFile, 0, SEEK_SET);

    char* shaderSrc = static_cast<char*>(malloc(sizeof(char) * fileSize));
    size_t readSize = fread(shaderSrc, sizeof(char), fileSize, srcFile);
    std::fclose(srcFile);

    if (readSize != fileSize)
    {
        fprintf(stderr, "Error while reading the file by path: %s\n", path);
        return nullptr;
    }

    return shaderSrc;
}

MTL::VertexFormat division_attribute_type_to_mtl_format(DivisionShaderVariableType attrType)
{
    switch (attrType)
    {
        case DIVISION_FLOAT:
            return MTL::VertexFormatFloat;
        case DIVISION_INTEGER:
            return MTL::VertexFormatInt;
        case DIVISION_FVEC2:
            return MTL::VertexFormatFloat2;
        case DIVISION_FVEC3:
            return MTL::VertexFormatFloat3;
        case DIVISION_FVEC4:
            return MTL::VertexFormatFloat4;
        case DIVISION_DOUBLE:
        case DIVISION_FMAT4X4:
        default:
            fprintf(stderr, "Unsupported attribute format");
            return (MTL::VertexFormat) 0;
    }
}