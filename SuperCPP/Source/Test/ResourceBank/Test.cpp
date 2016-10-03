#include <stdio.h>
#include "SuperCPPResourceBank.h"

using namespace SuperCPP;

int main()
{
  // ResourceBank
  {
    ResourceBank<const char*> bank;
    int gamma = bank.add( "Gamma" );
    int beta  = bank.add( "Beta" );
    int delta = bank.add( "Delta" );
    int alpha = bank.add( "Alpha" );

    printf( "id %d -> %s\n", alpha, bank.get(alpha) );
    printf( "id %d -> %s\n", beta,  bank.get(beta) );
    printf( "id %d -> %s\n", gamma, bank.get(gamma) );
    printf( "id %d -> %s\n", delta, bank.get(delta) );

    while (bank.count())
    {
      printf( "Removing resource %s\n", bank.remove_another() );
    }
  }

  return 0;
}

