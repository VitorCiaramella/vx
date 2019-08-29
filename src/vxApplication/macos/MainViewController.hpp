#import <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>

#pragma mark -
#pragma mark MainViewController

/** The main view controller for the main storyboard. */
@interface MainViewController : NSViewController
@end

#pragma mark -
#pragma mark MainWindowController

/** The main window controller for the main storyboard. */
@interface MainWindowController : NSWindowController
@end

#pragma mark -
#pragma mark MainView

/** The Metal-compatibile view for the main storyboard. */
@interface MainView : NSView
@end
