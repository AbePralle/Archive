//=============================================================================
// main.mm
//
// $(PLASMACORE_VERSION) $(DATE)
//
// Customizable main project file for Mac Plasmacore.
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

#import "mac_core.h"

//=============================================================================
//  CustomPlasmacoreAppDelegate
//  This is the main app delegate.  Override methods as desired.
//=============================================================================
@interface CustomPlasmacoreAppDelegate : PlasmacoreAppDelegate
@end

@implementation CustomPlasmacoreAppDelegate

- (void) performCustomSetup
{
  // See: http://plasmaworks.com/wiki/index.php/Custom_Native_Functionality
}

@end


//-----------------------------------------------------------------------------
//  main
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  try
  {
    int result = NSApplicationMain(argc, (const char **)argv);
    NativeLayer_shut_down();
    return result;
  }
  catch (int error_code)
  {
    if (bard_error_message.value) NativeLayer_alert( bard_error_message.value );
    NativeLayer_shut_down();
    return error_code;
  }
  catch ( const char* mesg )
  {
    NativeLayer_alert( mesg );
    NativeLayer_shut_down();
    return 1;
  }
}

