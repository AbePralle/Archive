//=============================================================================
// mac_core.mm
//
// $(PLASMACORE_VERSION) $(DATE)
//
// Plasmacore Native Layer implementation for Mac.
// 
// ----------------------------------------------------------------------------
//
// $(COPYRIGHT)
//
//   http://plasmaworks.com/plasmacore
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//
//   http://www.apache.org/licenses/LICENSE-2.0 
//
// Unless required by applicable law or agreed to in writing, 
// software distributed under the License is distributed on an 
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
// either express or implied. See the License for the specific 
// language governing permissions and limitations under the License.
//
//=============================================================================
#import <Cocoa/Cocoa.h>

#include "plasmacore.h"

//=============================================================================
//  PlasmacoreWindow
//=============================================================================
@interface PlasmacoreWindow : NSWindow
{
}
- (BOOL) acceptsMouseMovedEvents;
@end

//=============================================================================
//  PlasmacoreView
//=============================================================================
@interface PlasmacoreView : NSOpenGLView
{
}
- (void) setDefaults;
- (void) drawRect: (NSRect) bounds;
@end

//=============================================================================
//  PlasmacoreAppDelegate
//=============================================================================
@interface PlasmacoreAppDelegate : NSObject <NSApplicationDelegate> {
@private
}

- (void) performCustomSetup;
- (PlasmacoreView*) createPlasmacoreView:(NSRect)bounds withPixelFormat:(NSOpenGLPixelFormat*)pixel_format;

- (void) createWindow;
- (void) createFullscreenWindow;
- (void) initPlasmacore;
- (void) configurePlasmacore;
- (void) update;

@end


//=============================================================================
//  Globals
//=============================================================================
extern PlasmacoreWindow* plasmacore_window;
extern PlasmacoreView*   plasmacore_view;

