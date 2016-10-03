#include <stdio.h>
#include "SuperCPPList.h"

using namespace SuperCPP;

void print_int_list( List<int>* list )
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
  // List<int>
  {
    List<int> list;

    list.add( 2 );
    list.add( 4 );
    list.insert( 1, 0 );
    list.insert( 3, 2 );
    list.insert( 5, 4 );
    print_int_list( &list );

    list.remove_at( 0 );
    print_int_list( &list );
    list.remove_at( 1 );
    print_int_list( &list );
    list.remove_at( 2 );
    print_int_list( &list );
  }

  return 0;
}

