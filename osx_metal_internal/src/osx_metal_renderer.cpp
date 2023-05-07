#include "division_engine/platform_internal/platform_renderer.h"
#include "division_engine/renderer.h"

#include "DivisionOSXAppDelegate.h"
#include "osx_window_context.h"

// TODO: rename to context
bool division_engine_internal_platform_renderer_alloc(DivisionContext* ctx, const DivisionSettings* settings)
{
    NS::AutoreleasePool* autoreleasePool = NS::AutoreleasePool::alloc()->init();

    auto* appDelegate = new DivisionOSXAppDelegate(settings, ctx);
    auto* window_data = static_cast<DivisionOSXWindowContext*>(malloc(sizeof(DivisionOSXWindowContext)));

    NS::Application* app = NS::Application::sharedApplication();
    app->setDelegate(appDelegate);

    window_data->autorelease_pool = autoreleasePool;
    window_data->app = app;
    window_data->app_delegate = appDelegate;

    ctx->renderer_context->window_data = window_data;

    return true;
}

// TODO: rename to context
void division_engine_internal_platform_renderer_free(DivisionContext* ctx)
{
    auto* window_data = static_cast<DivisionOSXWindowContext*>(ctx->renderer_context->window_data);
    window_data->autorelease_pool->release();
    delete window_data->app_delegate;
    free(window_data);
}

void division_engine_internal_platform_renderer_run_loop(DivisionContext* ctx, const DivisionSettings* settings)
{
    auto* window_data = static_cast<DivisionOSXWindowContext*>(ctx->renderer_context->window_data);
    window_data->app->run();
}
