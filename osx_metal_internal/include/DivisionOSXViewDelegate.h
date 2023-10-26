#pragma once

#include <AppKit/AppKit.h>
#include <Foundation/Foundation.h>
#include <Metal/Metal.h>
#include <MetalKit/MetalKit.h>
#include <GameController/GameController.h>

#include "division_engine_core/context.h"
#include "division_engine_core/input.h"
#include "division_engine_core/render_pass_descriptor.h"
#include "division_engine_core/vertex_buffer.h"
#include "osx_shader_context.h"

#define DIVISION_MTL_VERTEX_DATA_BUFFER_INDEX 29
#define DIVISION_MTL_VERTEX_DATA_INSTANCE_ARRAY_INDEX 30

@interface DivisionOSXViewDelegate : NSResponder <MTKViewDelegate>
{
  @public
    DivisionContext* context;
  @public
    const DivisionSettings* settings;
  @public
    id<MTLDevice> device;
  @public
    id<MTLCommandQueue> commandQueue;
  @public
    GCKeyCode* keycode_map;
};

+ (instancetype)withContext:(DivisionContext*)aContext device:(id)aDevice;

- (instancetype)initWithContext:(DivisionContext*)aContext device:(id)aDevice;

@end