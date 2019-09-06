#import "MainViewController.hpp"
#import <QuartzCore/CAMetalLayer.h>
#import <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>

#include <MoltenVK/mvk_vulkan.h>
#include <vxApplication/vxApplication.hpp>


//#include "../../Vulkan-Tools/cube/cube.c"


#pragma mark -
#pragma mark MainViewController

@implementation MainViewController {
	CVDisplayLinkRef	_displayLink;
	spt(VxApplicationInstance) spVxApplicationInstance;
}

-(void) dealloc {
    //vxApp.destroy();
	CVDisplayLinkRelease(_displayLink);
	[super dealloc];
}

bool vxGetWindowSize(void* rpWindowHandle, uint32_t & width, uint32_t & height)
{
    if (rpWindowHandle==nullptr) return false;
    
    auto layer = (CALayer*)(rpWindowHandle);    
    width = layer.frame.size.width * layer.contentsScale;
    height = layer.frame.size.height * layer.contentsScale;
    return true;
}

-(void) viewDidLoad {
	[super viewDidLoad];

	self.view.wantsLayer = YES;// Back the view with a layer created by the makeBackingLayer method.

    
    
    auto userHomeDirectory = NSHomeDirectory();
    auto debugFilePath = [NSString stringWithFormat:@"%@/.vxApplication/Debug.txt", userHomeDirectory];
    vxInitDebugInstance(std::string([debugFilePath UTF8String]));

    auto mainBundle = [NSBundle mainBundle];

    auto spCreateInfo = nsp<VxApplicationInstanceCreateInfo>();
	spCreateInfo->rpMainWindowHandle = self.view.layer;
    spCreateInfo->resourcePath = std::string([mainBundle.resourcePath UTF8String]);
	auto vxResult = vxCreateApplicationInstance(spCreateInfo, spVxApplicationInstance);

    //TODO add assert
    if (vxResult==VxResult::VX_SUCCESS)
    {
        spVxApplicationInstance->spVxGraphicsInstance->spMainVxGraphicsWindow->rpVxGetWindowSize = vxGetWindowSize;
        
        CVDisplayLinkCreateWithActiveCGDisplays(&_displayLink);
        CVDisplayLinkSetOutputCallback(_displayLink, &DisplayLinkCallback, NULL);
        CVDisplayLinkSetOutputCallback(_displayLink, &DisplayLinkCallback, spVxApplicationInstance.get());
        CVDisplayLinkStart(_displayLink);
    }
}


#pragma mark Display loop callback function

/** Rendering loop callback function for use with a CVDisplayLink. */
static CVReturn DisplayLinkCallback(CVDisplayLinkRef displayLink,
									const CVTimeStamp* now,
									const CVTimeStamp* outputTime,
									CVOptionFlags flagsIn,
									CVOptionFlags* flagsOut,
									void* target)
{
    auto spVxApplicationInstance = (VxApplicationInstance*)target;
    spVxApplicationInstance->draw();
	return kCVReturnSuccess;
}

@end

#pragma mark -
#pragma mark MainWindowController

@implementation MainWindowController {
    
}

- (void)windowDidLoad
{
    [super windowDidLoad];

    /*
    [self.window deminiaturize:self];
    [self.window makeMainWindow];
    [self.window makeKeyAndOrderFront:self];
    [self.window orderFrontRegardless];
     */
}

@end

#pragma mark -
#pragma mark MainView

@implementation MainView

/** Indicates that the view wants to draw using the backing layer instead of using drawRect:.  */
-(BOOL) wantsUpdateLayer { return YES; }

-(BOOL) acceptsFirstResponder { return YES; }

/** Returns a Metal-compatible layer. */
+(Class) layerClass { return [CAMetalLayer class]; }

/** If the wantsLayer property is set to YES, this method will be invoked to return a layer instance. */
-(CALayer*) makeBackingLayer {
	CALayer* layer = [self.class.layerClass layer];
	CGSize viewScale = [self convertSizeToBacking: CGSizeMake(1.0, 1.0)];
	layer.contentsScale = MIN(viewScale.width, viewScale.height);
	return layer;
}

@end
