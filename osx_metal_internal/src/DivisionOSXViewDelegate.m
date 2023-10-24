#include "DivisionOSXViewDelegate.h"

#include "division_engine_core/context.h"
#include "division_engine_core/input.h"
#include "division_engine_core/renderer.h"
#include "division_engine_core/texture.h"
#include "division_engine_core/uniform_buffer.h"
#include "division_engine_core/utility.h"
#include "osx_render_pass.h"
#include "osx_texture.h"
#include "osx_uniform_buffer.h"
#include "osx_vertex_buffer.h"
#include <AppKit/AppKit.h>
#include <Foundation/Foundation.h>
#include <GameController/GameController.h>
#include <MetalKit/MetalKit.h>
#include <stdbool.h>

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
    // TODO: move input to the higher frequency update polling loop
    @autoreleasepool
    {
        DivisionInputSystemContext* input_ctx = context->input_context;
        DivisionMouseInput* mouse = &input_ctx->input.mouse;

        GCMouseInput* mouse_input = [[GCMouse current] mouseInput];
        bool left_button_pressed = [[mouse_input leftButton] isPressed];
        bool right_button_pressed = [[mouse_input rightButton] isPressed];
        bool middle_button_pressed = [[mouse_input middleButton] isPressed];

        NSPoint mouse_location = [NSEvent mouseLocation];

        mouse->pos_x = mouse_location.x;
        mouse->pos_y = mouse_location.y;
        mouse->left_button =
            left_button_pressed ? DIVISION_INPUT_STATE_DOWN : DIVISION_INPUT_STATE_UP;
        mouse->right_button = 
            right_button_pressed ? DIVISION_INPUT_STATE_DOWN : DIVISION_INPUT_STATE_UP;
        mouse->middle_button =
            middle_button_pressed ? DIVISION_INPUT_STATE_DOWN : DIVISION_INPUT_STATE_UP;
    }

    context->lifecycle.draw_callback(context);
}

- (void)mtkView:(nonnull MTKView*)view drawableSizeWillChange:(CGSize)size
{
    DivisionRendererSystemContext* renderer = context->renderer_context;

    renderer->frame_buffer_width = size.width;
    renderer->frame_buffer_height = size.height;
}
@end
