#include "DivisionOSXViewDelegate.h"

#include "division_engine_core/context.h"
#include "division_engine_core/input.h"
#include "division_engine_core/keycode.h"
#include "division_engine_core/renderer.h"
#include "division_engine_core/texture.h"
#include "division_engine_core/uniform_buffer.h"
#include "division_engine_core/utility.h"
#include "osx_keycode_map.h"
#include "osx_render_pass.h"
#include "osx_texture.h"
#include "osx_uniform_buffer.h"
#include "osx_vertex_buffer.h"
#include <AppKit/AppKit.h>
#import <Carbon/Carbon.h>
#include <Foundation/Foundation.h>
#include <GameController/GameController.h>
#include <MetalKit/MetalKit.h>
#include <objc/objc.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

static inline void handle_inputs(
    MTKView* view, DivisionContext* ctx, GCKeyCode* keycode_map
);

#define DIVISION_DEBUG_CHECK_PRESSED_KEY()                                               \
    GCKeyboardInput* __kb = [[GCKeyboard coalescedKeyboard] keyboardInput];              \
    for (NSString * s in [__kb buttons])                                                 \
    {                                                                                    \
        GCControllerButtonInput* b = [[__kb buttons] objectForKey:s];                    \
        if ([b isPressed])                                                               \
        {                                                                                \
            NSLog(@"%@", s);                                                             \
        }                                                                                \
    }

@implementation DivisionOSXViewDelegate
- (instancetype)initWithContext:(DivisionContext*)aContext device:(id)aDevice
{
    self = [super init];
    if (self)
    {
        context = aContext;
        device = aDevice;
        commandQueue = [device newCommandQueue];
        keycode_map = osx_keycode_map_alloc();
    }

    return self;
}

+ (instancetype)withContext:(DivisionContext*)aContext device:(id)aDevice
{
    return [[self alloc] initWithContext:aContext device:aDevice];
}

- (void)drawInMTKView:(nonnull MTKView*)view
{
    handle_inputs(view, context, keycode_map);
    context->lifecycle.draw_callback(context);
}

- (void)mtkView:(nonnull MTKView*)view drawableSizeWillChange:(CGSize)size
{
    DivisionRendererSystemContext* renderer = context->renderer_context;

    renderer->frame_buffer_width = size.width;
    renderer->frame_buffer_height = size.height;
}

- (void)keyDown:(NSEvent*)event
{
    // Silent beeps
}

- (void)mouseMoved:(NSEvent*)event
{
    DivisionMouseInput* mouse_input = &self->context->input_context->input.mouse;
}

- (void)dealloc
{
    osx_keycode_map_free(keycode_map);
}
@end

void handle_inputs(MTKView* view, DivisionContext* ctx, GCKeyCode* keycode_map)
{
    @autoreleasepool
    {
        DivisionInputSystemContext* input_ctx = ctx->input_context;
        DivisionMouseInput* mouse = &input_ctx->input.mouse;
        DivisionKeyboardInput* keyboard = &input_ctx->input.keyboard;
        GCMouseInput* osx_mouse_input = [[GCMouse current] mouseInput];
        NSPoint mouse_pos =
            [[view window] convertPointFromScreen:[NSEvent mouseLocation]];
        mouse->pos_x = mouse_pos.x;
        mouse->pos_y = mouse_pos.y;

        bool left_button_pressed = [[osx_mouse_input leftButton] isPressed];
        bool right_button_pressed = [[osx_mouse_input rightButton] isPressed];
        bool middle_button_pressed = [[osx_mouse_input middleButton] isPressed];

        GCKeyboardInput* keyboardInput = [[GCKeyboard coalescedKeyboard] keyboardInput];

        bool mouse_buttons_is_pressed[] = {
            [DIVISION_INPUT_MOUSE_LEFT] = left_button_pressed,
            [DIVISION_INPUT_MOUSE_RIGHT] = right_button_pressed,
            [DIVISION_INPUT_MOUSE_MIDDLE] = middle_button_pressed,
        };

        mouse->mouse_button_state_mask = 0;
        for (int i = 0; i < DIVISION_INPUT_MOUSE_BUTTON_COUNT; i++)
        {
            bool is_pressed = mouse_buttons_is_pressed[i];
            DIVISION_INPUT_SET_MOUSE_KEY_STATE(
                mouse->mouse_button_state_mask, i, is_pressed
            );
        }

        memset(keyboard->key_state_mask, 0, sizeof(uint32_t[4]));
        for (int i = 1; i < DIVISION_KEYCODE_COUNT; i++)
        {
            GCKeyCode keycode = keycode_map[i];
            GCControllerButtonInput* button = [keyboardInput buttonForKeyCode:keycode];
            bool is_button_pressed = [button isPressed];

            DIVISION_INPUT_SET_KEYBOARD_KEY_STATE(
                keyboard->key_state_mask, i, is_button_pressed
            );
        }
    }
}