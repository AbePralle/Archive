#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Bard.h"

int main( int argc, const char* argv[] )
{
  BardVM* vm = BardVM_create();

  BardCoreLibrary_configure( vm );

  const char* filename;
  if (argc == 2) filename = argv[1];
  else           filename = "../Test.bc";

  if (BardVM_load_bc_file(vm,filename))
  {
    printf("Loaded %s\n", filename );

    BardVM_create_main_object( vm );

    /*
    printf( "ACTIVE OBJECTS\n" );
    BardObject* active_objects = BardVM_collect_references( vm );
    while (active_objects)
    {
      printf( "%s\n", active_objects->type->name );
      active_objects = active_objects->next_reference;
    }
    */

    while (BardVM_update(vm))
    {
      usleep( 1000000/60 );
    }
  }
  else
  {
    printf("Failed to load %s\n", filename );
  }

  BardVM_free( vm );

  /*
  printf("\n--------------------------------------------------------------------\n");
  printf("Event Queue Test in Bard.c\n");
  printf("--------------------------------------------------------------------\n");
  BardEventQueue* queue = BardEventQueue_create();
  BardEventQueue_begin_event( queue, "update", 0 );

  BardEventQueue_begin_event( queue, "display_change", 2 );
  BardEventQueue_add_integer( queue, "width",  320 );
  BardEventQueue_add_integer( queue, "height", 480 );

  BardEventQueue_print_events( queue );
  BardEventQueue_clear( queue );

  BardEventQueue_begin_event( queue, "key_press", 3 );
  BardEventQueue_add_integer( queue, "keycode",  22 );
  BardEventQueue_add_integer( queue, "unicode",  65 );
  BardEventQueue_add_integer( queue, "is_repeat", 0 );

  BardEventQueue_begin_event( queue, "xyz", 3 );
  BardEventQueue_add_real( queue, "x",  3.5 );
  BardEventQueue_add_real( queue, "y",  4.0 );
  BardEventQueue_add_real( queue, "z",  5.75 );

  BardEventQueue_print_events( queue );
  BardEventQueue_free( queue );
  */

  return 0;
}

