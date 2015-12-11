//=============================================================================
//  bard.cpp
//
//  Bard VM main function
//
//  $(BARD_VERSION)
//  ---------------------------------------------------------------------------
//
//  Copyright 2008-2010 Plasmaworks LLC
//
//  Licensed under the Apache License, Version 2.0 (the "License"); 
//  you may not use this file except in compliance with the License. 
//  You may obtain a copy of the License at 
//
//    http://www.apache.org/licenses/LICENSE-2.0 
//
//  Unless required by applicable law or agreed to in writing, 
//  software distributed under the License is distributed on an 
//  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
//  either express or implied. See the License for the specific 
//  language governing permissions and limitations under the License.
//
//  ---------------------------------------------------------------------------
//
//  History:
//    2010.12.24 / Abe Pralle - v3.2 revamp
//    2008.12.17 / Abe Pralle - Created
//=============================================================================

#include "bard_version.h"
#define USAGE "Bard Virtual Machine " VERSION "\nUsage: bard filename.etc [args]\n"

#include "bard.h"

int main( int argc, char** argv )
{
  try
  {
    bard_set_raw_exe_filepath( argv[0] );
    if (argc < 2) { fprintf( stderr, USAGE ); return 1; }

    BardLoader loader;
    loader.load(argv[1]);

    bard_configure();
    bard_set_command_line_args( argv+2, argc-2 );
    bard_launch();
    bard_shut_down();
  }
  catch (int code)
  {
    if (bard_error_message.value)
    {
      fprintf( stderr, "%s\n", bard_error_message.value );
    }
    return code;
  }
  return 0;
}

void bard_adjust_filename_for_os( char* filename, int len )
{
}

