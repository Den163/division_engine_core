#include "DivisionOSXViewDelegate.h"

#include "division_engine_core/renderer.h"
#include "division_engine_core/texture.h"
#include "division_engine_core/uniform_buffer.h"
#include "division_engine_core/utility.h"
#include "osx_render_pass.h"
#include "osx_texture.h"
#include "osx_uniform_buffer.h"
#include "osx_vertex_buffer.h"
#include <MetalKit/MetalKit.h>

@implementation DivisionOSXViewDelegate
- (instancetype)initWithContext:(DivisionContext*)aContext device:(id)aDevice
{
    self = [super init];
    if (self)
    {
        context = aContext;
        device = aDevice;
        commandQueue = [device newCommandQueue];
    }

    return self;
}

+ (instancetype)withContext:(DivisionContext*)aContext device:(id)aDevice
{
    return [[self alloc] initWithContext:aContext device:aDevice];
}

- (void)drawInMTKView:(nonnull MTKView*)view
{
    context->lifecycle.ready_to_draw_callback(context);
}

- (void)mtkView:(nonnull MTKView*)view drawableSizeWillChange:(CGSize)size
{
    DivisionRendererSystemContext* renderer = context->renderer_context;

    renderer->frame_buffer_width = size.width;
    renderer->frame_buffer_height = size.height;
}
@end
