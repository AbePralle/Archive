#include <cstdio>
#include <cstdlib>
using namespace std;

#include "Cargo.h"
using namespace Cargo;

int main()
{
  Value home_folder = "~";

  Value files = File(home_folder).listing();
  printf( "Here's a listing of your home folder (%d files):\n", files.count() );

  for (int i=0; i<files.count(); ++i)
  {
    File file( home_folder, files[i] );
    file.filepath.print();
    if (file.is_folder()) printf( "  [folder]\n" );
    else                  printf( "  (%lld bytes)\n", file.size() );
  }

  return 0;
}
