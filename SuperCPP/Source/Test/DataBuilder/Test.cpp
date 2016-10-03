  // 😀
  // π
#include <cstdio>
#include <cmath>
using namespace std;

#include "SuperCPP.h"
#include "SuperCPPDataBuilder.h"
#include "SuperCPPDataReader.h"

using namespace SuperCPP;

void test_int64( Int64 n )
{
  DataBuilder builder;
  builder.write_int64( n );
  builder.write_int64x( n );

  printf( "%lld\n", n );
  DataReader reader( builder.data, builder.count );
  if (reader.read_int64() != n)
  {
    printf( "Error encoding or decoding int64 %lld\n", n );
  }
  if (reader.read_int64x() != n)
  {
    printf( "Error encoding or decoding int64x %lld\n", n );
  }
}

int main()
{
  DataBuilder builder;
  builder.write_string( "ABCD😀 πEFG" );
  builder.write_string( "HIJKLMNOP" );
  builder.write_byte( 255 );
  builder.write_int32( 1234578900 );
  builder.write_int32x( 1234578900 );
  builder.write_real64( acos(-1) );
  builder.write_real32( (float) acos(-1) );
  builder.write_logical( true );
  builder.write_logical( false );

  StringBuilder buffer;
  DataReader reader( builder.data, builder.count );
  printf( "Read %d characters: %s\n", reader.read_string(buffer.clear()), buffer.as_c_string() );
  printf( "Read %d characters: %s\n", reader.read_string(buffer.clear()), buffer.as_c_string() );
  printf( "%d\n", reader.read_byte() );
  printf( "%d\n", reader.read_int32() );
  printf( "%d\n", reader.read_int32x() );
  printf( "%lf\n", reader.read_real64() );
  printf( "%lf\n", reader.read_real32() );
  printf( "%d\n", reader.read_logical() );
  printf( "%d\n", reader.read_logical() );

  buffer.clear().print( "pi:" ).println( acos(-1) );

  buffer.println( -3 );
  buffer.println( -3.1 );
  buffer.println( -3.01 );
  buffer.println( -3.001 );
  buffer.println( -3.0001 );
  buffer.println( -3.00001 );
  buffer.println( acos(-1), 4 );
  buffer.println( acos(-1), 5 );
  buffer.println( acos(-1), 6 );
  buffer.println( acos(-1), 7 );
  buffer.println( acos(-1) );
  buffer.println( true );
  buffer.println( false );

  printf( "%s\n", buffer.as_utf8_string() );

  printf( "Tests complete\n" );

  return 0;
}

