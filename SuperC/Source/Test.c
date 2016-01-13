#include <stdio.h>
#include "SuperC.h"

void print_int_list( CIntList* list )
{
  int i;
  printf( "[" );
  for (i=0; i<list->count; ++i)
  {
    if (i > 0) printf( "," );
    printf( "%d", list->data[i] );
  }
  printf( "]\n" );
}

int main()
{
  // CIntList
  {
    CIntList* list = CIntList_create();

    CIntList_add( list, 2 );
    CIntList_add( list, 4 );
    CIntList_insert( list, 1, 0 );
    CIntList_insert( list, 3, 2 );
    CIntList_insert( list, 5, 4 );
    print_int_list( list );

    CIntList_remove_at( list, 0 );
    print_int_list( list );
    CIntList_remove_at( list, 1 );
    print_int_list( list );
    CIntList_remove_at( list, 2 );
    print_int_list( list );

    CIntList_destroy( list );
  }

  // CResourceBank
  {
    CResourceBank* bank = CResourceBank_create();
    int gamma = CResourceBank_add( bank, "Gamma" );
    int beta  = CResourceBank_add( bank, "Beta" );
    int delta = CResourceBank_add( bank, "Delta" );
    int alpha = CResourceBank_add( bank, "Alpha" );

    printf( "id %d -> %s\n", alpha, CResourceBank_get(bank, alpha) );
    printf( "id %d -> %s\n", beta,  CResourceBank_get(bank, beta) );
    printf( "id %d -> %s\n", gamma, CResourceBank_get(bank, gamma) );
    printf( "id %d -> %s\n", delta, CResourceBank_get(bank, delta) );

    while (CResourceBank_count(bank))
    {
      printf( "Removing resource %s\n", CResourceBank_remove_another(bank) );
    }

    CResourceBank_destroy( bank );
  }

  return 0;
}

