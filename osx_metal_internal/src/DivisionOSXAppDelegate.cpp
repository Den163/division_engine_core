#include <DivisionOSXAppDelegateCpp.h>
#include "division_engine/renderer.h"


DivisionOSXAppDelegate::DivisionOSXAppDelegate(const DivisionSettings* settings, DivisionContext* context) :
    settings(settings), context(context), _window(nullptr), _view(nullptr), _device(nullptr), viewDelegate(nullptr)
{
}

DivisionOSXAppDelegate::~DivisionOSXAppDelegate()
{
    _view->release();
    _window->release();
    _device->release();
    delete viewDelegate;
}

NS::Menu* DivisionOSXAppDelegate::createMenuBar()
{
    using NS::StringEncoding::UTF8StringEncoding;

    NS::Menu* mainMenu = NS::Menu::alloc()->init();
    NS::MenuItem* appMenuItem = NS::MenuItem::alloc()->init();
    NS::Menu* appMenu = NS::Menu::alloc()->init(NS::String::string("Appname", UTF8StringEncoding));

    NS::String* appName = NS::RunningApplication::currentApplication()->localizedName();
    NS::String* quitItemName = NS::String::string("Quit ", UTF8StringEncoding)->stringByAppendingString(appName);
    SEL quitCallback = NS::MenuItem::registerActionCallback("appQuit", [](void*, SEL, const NS::Object* pSender)
    {
        NS::Application::sharedApplication()->terminate(pSender);
    });

    NS::MenuItem* quitItem = appMenu->addItem(
        quitItemName, quitCallback, NS::String::string("q", UTF8StringEncoding));
    quitItem->setKeyEquivalentModifierMask(NS::EventModifierFlagCommand);

    appMenuItem->setSubmenu(appMenu);

    NS::MenuItem* windowMenuItem = NS::MenuItem::alloc()->init();
    NS::Menu* windowMenu = NS::Menu::alloc()->init(NS::String::string("window", UTF8StringEncoding));
    SEL closeWindowCallback = NS::MenuItem::registerActionCallback("windowClose", [](void*, SEL, const NS::Object*)
    {
        NS::Application::sharedApplication()->windows()->object<NS::Window>(0)->close();
    });
    NS::MenuItem* closeWindowItem = windowMenu->addItem(
        NS::String::string("Close window", UTF8StringEncoding),
        closeWindowCallback,
        NS::String::string("w", UTF8StringEncoding));
    closeWindowItem->setKeyEquivalentModifierMask(NS::EventModifierFlagCommand);

    windowMenuItem->setSubmenu(windowMenu);

    mainMenu->addItem(appMenuItem);
    mainMenu->addItem(windowMenuItem);

    appMenuItem->release();
    windowMenuItem->release();
    appMenu->release();
    windowMenu->release();

    return mainMenu->autorelease();
}

void DivisionOSXAppDelegate::applicationWillFinishLaunching(NS::Notification* pNotification)
{
    auto* menu = createMenuBar();
    auto* app = reinterpret_cast<NS::Application*>(pNotification->object());
    app->setMainMenu(menu);
    app->setActivationPolicy(NS::ActivationPolicyRegular);
}

void DivisionOSXAppDelegate::applicationDidFinishLaunching(NS::Notification* pNotification)
{
    CGRect windowFrame = CGRectMake(0, 0, settings->window_width, settings->window_height);

    _window = NS::Window::alloc()->init(
        windowFrame,
        NS::WindowStyleMaskClosable | NS::WindowStyleMaskTitled | NS::WindowStyleMaskResizable,
        NS::BackingStoreBuffered,
        false
    );

    _device = MTL::CreateSystemDefaultDevice();
    _view = MTK::View::alloc()->init(windowFrame, _device);
    _view->setColorPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
    viewDelegate = new DivisionOSXViewDelegate(_device, settings, context);
    _view->setDelegate(viewDelegate);
    _window->setContentView(_view);
    _window->setTitle(NS::String::string(settings->window_title, NS::StringEncoding::UTF8StringEncoding));
    _window->makeKeyAndOrderFront(nullptr);

    auto* app = reinterpret_cast<NS::Application*>(pNotification->object());
    app->activateIgnoringOtherApps(true);

    settings->init_callback(context);
}

bool DivisionOSXAppDelegate::applicationShouldTerminateAfterLastWindowClosed(NS::Application* pSender)
{
    return true;
}

