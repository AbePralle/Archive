#import "PlasmacoreAppDelegate.h"
#import "PlasmacoreView.h"

#include "plasmacore.h"

extern UIApplication*         plasmacore_app;
extern PlasmacoreAppDelegate* plasmacore_app_delegate;
extern PlasmacoreView*        plasmacore_view;
extern UIWindow*              plasmacore_window;

@implementation PlasmacoreAppDelegate

@synthesize window;
@synthesize glView;

void NativeLayer_set_up();
void NativeLayer_configure();

bool plasmacore_running = false;

void perform_custom_setup();

- (void)alert:(const char*) mesg
{
  NSString* st = [NSString stringWithCString:mesg encoding:1];
  UIAlertView *alert_box = [[UIAlertView alloc] initWithTitle:@"Fatal Error" message:st  
	  delegate:nil cancelButtonTitle:@"Exit" otherButtonTitles:nil, nil];
  if (alert_box)
  {
    [alert_box show];
    [alert_box release];	
  }
}

- (void)initializePlasmacore: (UIApplication*) application
{
  try
  {
    NativeLayer_set_up();
    perform_custom_setup();

    NativeLayer_configure();

    plasmacore_launch();
    [glView startAnimation];

    plasmacore_running = true;
  }
  catch (int error_code)
  {
    if (bard_error_message.value) [self alert:bard_error_message.value];
  }
  catch ( const char* mesg )
  {
    [self alert:mesg];
  }
}

- (PlasmacoreView*) createPlasmacoreViewWithFrame:(CGRect) bounds
{
  return [[PlasmacoreView alloc] initWithFrame:bounds];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions   
{
  CGRect bounds = [[UIScreen mainScreen] bounds];
  window = [[UIWindow alloc] initWithFrame:bounds];
  glView = [self createPlasmacoreViewWithFrame:bounds];
  [window addSubview:glView];
  [window makeKeyAndVisible];

  // Assign globals
  plasmacore_app = application;
  plasmacore_app_delegate = self;
  plasmacore_window = window;
  plasmacore_view = glView;

  [self performSelectorOnMainThread:@selector(initializePlasmacore:) withObject:application waitUntilDone:NO];
  return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
  [glView stopAnimation];
  plasmacore_queue_signal( plasmacore.event_suspend );
  plasmacore_raise_pending_signals();
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
  if (plasmacore_running)
  {
    [glView startAnimation];
    plasmacore_queue_signal( plasmacore.event_resume );
    plasmacore_raise_pending_signals();
  }
}

- (void)applicationWillTerminate:(UIApplication *)application
{
  [glView stopAnimation];
  plasmacore_on_exit_request();
  plasmacore_running = false;
}

- (void)dealloc
{
  [window release];
  [glView release];

  [super dealloc];
}

@end
