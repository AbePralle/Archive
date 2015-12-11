#import <UIKit/UIKit.h>

@class PlasmacoreView;

@interface PlasmacoreAppDelegate : NSObject <UIApplicationDelegate> 
{
    UIWindow *window;
    PlasmacoreView *glView;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet PlasmacoreView *glView;

- (void)alert:(const char*) mesg;

@end

