#include "DivisionOSXAppDelegate.h"

static NSMenu* createMenuBar(DivisionOSXAppDelegate* app_delegate);

@implementation DivisionOSXAppDelegate
- (instancetype)initWithContext:(DivisionContext*)aContext
                       settings:(const DivisionSettings*)aSettings
{
    self = [super init];
    if (self)
    {
        context = aContext;
        window = NULL;
        view = NULL;
        viewDelegate = NULL;
        windowFrame = CGRectMake(0, 0, aSettings->window_width, aSettings->window_height);
        windowTitle = [NSString stringWithUTF8String:aSettings->window_title];
    }

    return self;
}

+ (instancetype)withContext:(DivisionContext*)aContext
                   settings:(const DivisionSettings*)aSettings
{
    return [[self alloc] initWithContext:aContext settings:aSettings];
}

- (void)onAppQuit:(void*)data selector:(SEL)sel sender:(NSObject*)sender
{
    [[NSApplication sharedApplication] terminate:sender];
}

- (void)onWindowClose:(void*)data selector:(SEL)sel sender:(NSObject*)sender
{
    [[[[NSApplication sharedApplication] windows] objectAtIndex:0] close];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender
{
    return YES;
}

- (void)applicationWillFinishLaunching:(NSNotification*)notification
{
    NSMenu* menu = createMenuBar(self);
    NSApplication* app = (NSApplication*)[notification object];
    [app setMainMenu:menu];
    [app setActivationPolicy:NSApplicationActivationPolicyRegular];
}

- (void)applicationDidFinishLaunching:(NSNotification*)notification
{
    window = [[NSWindow alloc]
        initWithContentRect:windowFrame
                  styleMask:NSWindowStyleMaskClosable | NSWindowStyleMaskTitled |
                            NSWindowStyleMaskResizable
                    backing:NSBackingStoreBuffered
                      defer:false];

    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    view = [[MTKView alloc] initWithFrame:windowFrame device:device];
    [view setColorPixelFormat:MTLPixelFormatBGRA8Unorm_sRGB];

    viewDelegate = [DivisionOSXViewDelegate withContext:context device:device];
    [view setDelegate:viewDelegate];

    [window setContentView:view];
    [window setTitle:windowTitle];
    [window makeKeyAndOrderFront:NULL];

    NSApplication* app = [notification object];
    [app activateIgnoringOtherApps:true];

    context->lifecycle.init_callback(context);
}

@end

NSMenu* createMenuBar(DivisionOSXAppDelegate* app_delegate)
{
    NSMenu* mainMenu = [NSMenu new];

    @autoreleasepool
    {
        NSMenuItem* appMenuItem = [NSMenuItem new];
        NSMenu* appMenu = [[NSMenu alloc] initWithTitle:@"Appname"];

        NSString* appName = [[NSRunningApplication currentApplication] localizedName];
        NSString* quitItemName = [@"Quit " stringByAppendingString:appName];

        NSMenuItem* quitItem = [appMenu addItemWithTitle:quitItemName
                                                  action:@selector(onAppQuit:
                                                                    selector:sender:)
                                           keyEquivalent:@"q"];

        [quitItem setKeyEquivalentModifierMask:NSEventModifierFlagCommand];
        [appMenuItem setSubmenu:appMenu];

        NSMenuItem* windowMenuItem = [NSMenuItem new];

        NSMenu* windowMenu = [[NSMenu alloc] initWithTitle:@"window"];

        NSMenuItem* closeWindowItem =
            [windowMenu addItemWithTitle:@"Close window"
                                  action:@selector(onWindowClose:selector:sender:)
                           keyEquivalent:@"w"];
        [closeWindowItem setKeyEquivalentModifierMask:NSEventModifierFlagCommand];

        [windowMenuItem setSubmenu:windowMenu];

        [mainMenu addItem:appMenuItem];
        [mainMenu addItem:windowMenuItem];
    }

    return mainMenu;
}