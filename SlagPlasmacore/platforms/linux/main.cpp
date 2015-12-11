//=============================================================================
// main.cpp
//
// $(PLASMACORE_VERSION) $(DATE)
//
// http://plasmaworks.com/plasmacore
//
// Launcher for Linux Plasmacore.  Designed to easily allow custom native
// features to be added to Plasmacore - see the following web page for 
// more information:
//
//   http://plasmaworks.com/wiki/index.php/Custom_Native_Functionality
//
// The GoGo build system creates this file if it's missing, but it is (handily)
// NOT overwritten during an upgrade.
//
// ----------------------------------------------------------------------------
//
// $(COPYRIGHT)
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
#include "plasmacore.h"

void LinuxCore_init(int argc, char * argv[]);
void LinuxCore_configure();
void LinuxCore_main_loop();

void perform_custom_setup()
{
  // See: http://plasmaworks.com/wiki/index.php/Custom_Native_Functionality
}

void NativeLayer_sleep( int ms )
{
  // Kill time between update cycles for the given number of milliseconds.
  // Add custom tasks as desired.
  SDL_Delay(ms);
}


int main(int argc, char *argv[])
{
  try
  {
    LinuxCore_init(argc, argv);

    perform_custom_setup();

    LinuxCore_configure();
    LinuxCore_main_loop();
  }
  catch (int error_code)
  {
    if (bard_error_message.value) NativeLayer_alert( bard_error_message.value );
    NativeLayer_shut_down();
    SDL_Quit();
    return error_code;
  }
  catch ( const char* mesg )
  {
    NativeLayer_alert( mesg );
    SDL_Quit();
    return 1;
  }

  // Cleanup
  NativeLayer_shut_down();
  SDL_Quit();

  return 0;
}

