#ifndef WIN_CORE_H
#define WIN_CORE_H
//=============================================================================
// win_core.h
//
// $(PLASMACORE_VERSION) $(DATE)
//
// Plasmacore Native Layer implementation for Windows.
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
//
//----Windows Debug---------------------------------------------------
// The order of the next 3 lines should be preserved
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//----Windows---------------------------------------------------------
#define WINVER 0x0400
#include <winsock2.h>
#include <windows.h>
#include <shlwapi.h>
#include <direct.h>


//----DirectX includes------------------------------------------------
#include <d3d9.h>


//----Plasmacore includes---------------------------------------------
#include "plasmacore.h"

void WinCore_init( HINSTANCE h_instance, int n_cmd_show );
void WinCore_configure();
int  WinCore_main_loop();

#endif // WIN_CORE_H

