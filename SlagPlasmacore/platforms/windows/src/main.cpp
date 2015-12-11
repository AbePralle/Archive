//=============================================================================
// main.cpp
//
// $(PLASMACORE_VERSION) $(DATE)
//
// Customizable main project file for Windows Plasmacore.
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

#include "win_core.h"


void perform_custom_setup()
{
  // See: http://plasmaworks.com/wiki/index.php/Custom_Native_Functionality
}

int WINAPI WinMain( HINSTANCE h_instance, HINSTANCE h_prev_instance,
    LPSTR command_line, int n_cmd_show  )
{
  try
  {
    WinCore_init( h_instance, n_cmd_show );

    perform_custom_setup();

    WinCore_configure();
    int result = WinCore_main_loop();
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

/*
int main(int argc, char *argv[])
{
  try
  {
    WinCore_init( argc, argv );

    perform_custom_setup();

    WinCore_configure();
    WinCore_main_loop();
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
*/

