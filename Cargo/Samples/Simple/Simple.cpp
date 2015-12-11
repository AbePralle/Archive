#include <cstdio>
using namespace std;

#include "Cargo.h"
using namespace Cargo;

int main()
{
  /*
  Value table = Table();
  table.set( "name", Table().set("first","Abe") );

  printf( "PRINTLN\n" );
  table.println();
  // prints: {name:{first:"Abe"}}

  printf( "\nTO JSON\n" );
  table.to_json().println();
  // prints: {"name":{"first":"Abe"}}

  table.set( "born", 1973 );
  table.get( "name" ).set( "last", "Pralle" );
  
  Value info = List().add( table );
  info.add( JSON("{name:{first:'Max',last:'Rockatansky'}}") );

  //info.add( Table().set( "emoji", JSON("abc\\s01f61cdef") ) );
  //info.to_utf8_string().println();

  // Save the data as JSON.
  File( "Info.json" ).save( info );

  // Load it right back up again - it will load an empty list on an error.
  info = File( "Info.json" ).load_list();
  
  printf( "\nINFO\n" );
  info.println();
  */

  Value st = JSON( "abc\\s01f61cdefdef" );
  printf( "count: %d\n", st.count() );
  st.println();

  return 0;
}
