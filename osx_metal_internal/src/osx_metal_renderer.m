#include "division_engine/platform_internal/platform_renderer.h"
#include "division_engine/renderer.h"

#include <Foundation/Foundation.h>
#include <AppKit/AppKit.h>
#include "DivisionOSXAppDelegate.h"
#include "osx_window_context.h"

// TODO: rename to context
bool division_engine_internal_platform_renderer_alloc(DivisionContext* ctx, const DivisionSettings* settings)
{
    @autoreleasepool {
        NSApplication* app = [NSApplication sharedApplication];
        DivisionOSXAppDelegate* app_delegate = [DivisionOSXAppDelegate withContext:ctx settings:settings];
        DivisionOSXWindowContext* window_data = malloc(sizeof(DivisionOSXWindowContext));

        [app setDelegate:app_delegate];

        window_data->app = app;
        window_data->app_delegate = app_delegate;

        ctx->renderer_context->window_data = window_data;

        return true;
    }
}

// TODO: rename to context
void division_engine_internal_platform_renderer_free(DivisionContext* ctx)
{
    DivisionOSXWindowContext* window_data = ctx->renderer_context->window_data;
    window_data->app_delegate = nil;
    window_data->app_delegate = nil;

    free(window_data);
}

void division_engine_internal_platform_renderer_run_loop(DivisionContext* ctx, const DivisionSettings* settings)
{
    @autoreleasepool {
        DivisionOSXWindowContext * window_data = ctx->renderer_context->window_data;
        [window_data->app run];
    }
}
