#include <stdio.h>
#include "SuperCPP.h"
#include "SuperCPPBuilder.h"

using namespace SuperCPP;

int main()
{
  // Builder 1
  {
    Builder<Int32> builder;
    for (int i=0; i<256; ++i)
    {
      builder.add( i );
    }
    for (int i=0; i<256; ++i)
    {
      if (builder[i] != i) printf( "Builder 1 - Invalid number stored at %d\n", i );
    }
    printf( "Builder 1 using 1k buffer, no dynamic allocation (should be 1): %d\n", builder.data == builder.internal_buffer );
  }

  // Builder 2
  {
    Builder<Int32> builder;
    for (int i=0; i<257; ++i)
    {
      builder.add( i );
    }
    for (int i=0; i<257; ++i)
    {
      if (builder[i] != i) printf( "Builder 2 - Invalid number stored at %d\n", i );
    }
    printf( "Builder 2 using 1k internal buffer, no dynamic allocation (should be 0): %d\n", builder.data == builder.internal_buffer );
  }

  return 0;
}

