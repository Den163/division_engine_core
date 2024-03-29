#pragma once

#include <CoreFoundation/CoreFoundation.h>
#include <Foundation/Foundation.h>
#include <AppKit/AppKit.h>
#include <Metal/MTLDevice.h>
#include <objc/NSObject.h>

#include "DivisionOSXViewDelegate.h"

#include "division_engine_core/context.h"

@interface DivisionOSXAppDelegate: NSObject<NSApplicationDelegate>
{
    @public DivisionContext* context;
    @public CGRect windowFrame;
    @public NSString* windowTitle;

    @public NSWindow* window;
    @public MTKView* view;
    @public DivisionOSXViewDelegate* viewDelegate;
};

- (instancetype)initWithContext:(DivisionContext*)aContext settings:(const DivisionSettings*)aSettings;

+ (instancetype)withContext:(DivisionContext*)aContext settings:(const DivisionSettings*)aSettings;

- (void)applicationWillTerminate:(NSNotification *)notification NS_SWIFT_UI_ACTOR;

@end
