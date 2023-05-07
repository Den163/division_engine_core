#pragma once

#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>
#include <DivisionOSXViewDelegate.h>

#include "division_engine/settings.h"
#include "division_engine/context.h"
#include "../../include/division_engine/context.h"

class DivisionOSXAppDelegate : public NS::ApplicationDelegate {
public:
    DivisionOSXAppDelegate(const DivisionSettings* settings, DivisionContext* context);
    ~DivisionOSXAppDelegate() override;

    void applicationWillFinishLaunching(NS::Notification* pNotification) override;
    void applicationDidFinishLaunching(NS::Notification* pNotification) override;
    bool applicationShouldTerminateAfterLastWindowClosed(NS::Application* pSender) override;

    const DivisionSettings* settings;
    DivisionContext* context;
    DivisionOSXViewDelegate* viewDelegate;

private:
    static NS::Menu* createMenuBar();

    NS::Window* _window;
    MTL::Device* _device;
    MTK::View* _view;
};