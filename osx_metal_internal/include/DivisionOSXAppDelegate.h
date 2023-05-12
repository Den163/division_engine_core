#pragma once

#include <Foundation/Foundation.h>
#include <AppKit/AppKit.h>
#include <Metal/MTLDevice.h>

#include "DivisionOSXViewDelegate.h"

#include "division_engine_core/context.h"

@interface DivisionOSXAppDelegate: NSObject<NSApplicationDelegate>
{
    @public DivisionContext* context;
    @public const DivisionSettings* settings;

    @public NSWindow* window;
    @public MTKView* view;
    @public DivisionOSXViewDelegate* viewDelegate;
};

- (instancetype)initWithContext:(DivisionContext*)aContext settings:(const DivisionSettings*)aSettings;

+ (instancetype)withContext:(DivisionContext*)aContext settings:(const DivisionSettings*)aSettings;

- (void)onAppQuit:(void*)data selector:(SEL)sel sender:(NSObject*)sender;
- (void)onWindowClose:(void*)data selector:(SEL)sel sender:(NSObject*)sender;

@end
