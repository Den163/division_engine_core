#pragma once

#include <Foundation/Foundation.hpp>

#include "DivisionOSXAppDelegate.h"

typedef struct DivisionWindowContextPlatformInternal_ {
    NS::AutoreleasePool* autorelease_pool;
    NS::Application* app;
    DivisionOSXAppDelegate* app_delegate;
} DivisionOSXWindowContext;