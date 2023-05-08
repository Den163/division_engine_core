#include "DivisionOSXAppDelegate.h"

static NSMenu* createMenuBar(DivisionOSXAppDelegate* app_delegate);


@implementation DivisionOSXAppDelegate
- (instancetype)initWithContext:(DivisionContext*)aContext settings:(const DivisionSettings*)aSettings {
    self = [super init];
    if (self)
    {
        context = aContext;
        settings = aSettings;
        window = NULL;
        view = NULL;
        viewDelegate = NULL;
    }

    return self;
}

+ (instancetype)withContext:(DivisionContext*)aContext settings:(const DivisionSettings*)aSettings {
    return [[self alloc] initWithContext:aContext settings:aSettings];
}

- (void)onAppQuit:(void*)data selector:(SEL)sel sender:(NSObject*)sender {
    [[NSApplication sharedApplication] terminate:sender];
}

- (void)onWindowClose:(void*)data selector:(SEL)sel sender:(NSObject*)sender {
    [[[[NSApplication sharedApplication] windows] objectAtIndex:0] close];
}


- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
    return YES;
}

- (void)applicationWillFinishLaunching:(NSNotification*)notification {
    NSMenu* menu = createMenuBar(self);
    NSApplication* app = (NSApplication*) [notification object];
    [app setMainMenu:menu];
    [app setActivationPolicy:NSApplicationActivationPolicyRegular];
}

- (void)applicationDidFinishLaunching:(NSNotification*)notification {
    CGRect windowFrame = CGRectMake(0, 0, settings->window_width, settings->window_height);

    window = [[NSWindow alloc]
                        initWithContentRect:windowFrame
                                  styleMask:NSWindowStyleMaskClosable | NSWindowStyleMaskTitled |
                                            NSWindowStyleMaskResizable
                                    backing:NSBackingStoreBuffered
                                      defer:false
    ];

    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    view = [[MTKView alloc] initWithFrame:windowFrame device: device];
    [view setColorPixelFormat:MTLPixelFormatBGRA8Unorm_sRGB];

    viewDelegate = [DivisionOSXViewDelegate withContext:context settings:settings device:device];
    [view setDelegate: viewDelegate];

    [window setContentView:view];
    [window setTitle:[NSString stringWithUTF8String:settings->window_title]];
    [window makeKeyAndOrderFront:NULL];

    NSApplication* app = [notification object];
    [app activateIgnoringOtherApps:true];

    settings->init_callback(context);
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

        NSMenuItem* quitItem = [appMenu
            addItemWithTitle:quitItemName
                      action:@selector(onAppQuit:selector:sender:)
               keyEquivalent:@"q"
        ];

        [quitItem setKeyEquivalentModifierMask:NSEventModifierFlagCommand];
        [appMenuItem setSubmenu:appMenu];

        NSMenuItem* windowMenuItem = [NSMenuItem new];

        NSMenu* windowMenu = [[NSMenu alloc] initWithTitle:@"window"];

        NSMenuItem* closeWindowItem = [windowMenu
            addItemWithTitle:@"Close window"
                      action:@selector(onWindowClose:selector:sender:)
               keyEquivalent:@"w"
        ];
        [closeWindowItem setKeyEquivalentModifierMask:NSEventModifierFlagCommand];

        [windowMenuItem setSubmenu:windowMenu];

        [mainMenu addItem:appMenuItem];
        [mainMenu addItem:windowMenuItem];
    }

    return mainMenu;
}