#pragma once

#include <Foundation/Foundation.h>
#include "DivisionOSXAppDelegate.h"

typedef struct DivisionWindowContextPlatformInternal_
{
    __strong NSApplication* app;
    __strong DivisionOSXAppDelegate* app_delegate;
} DivisionOSXWindowContext;