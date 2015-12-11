//=============================================================================
//  BossUtility.h
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#include "BossUtility.h"

#include <cmath>
using namespace std;

namespace Boss
{

void real_to_c_string( Real value, char* buffer, int size )
{
  // Suggested buffer size: 512
  if (isnan(value))
  {
    snprintf( buffer, size, "NaN" );
  }
  else if (value == INFINITY)
  {
    snprintf( buffer, size, "+infinity" );
  }
  else if (value == -INFINITY)
  {
    snprintf( buffer, size, "-infinity" );
  }
  else if (floor(value) == value)
  {
    if (value > -1e17 and value < 1e17)
    {
      // Whole number with 16 or fewer digits of precision
      snprintf( buffer, size, "%.0lf", value );
    }
    else
    {
      // Whole number with up to 17+ digits of precision, general format.
      snprintf( buffer, size, "%.16lg", value );
    }
  }
  else if (value > -0.1 and value < 0.1)
  {
    // +/- 0.0xxx - use the long general format (g) specifying 17 digits of
    // decimal precision which will result in either a standard decimal value
    // (%lf) or a value in exponent notation (%le), each with UP TO 17 digits
    // of precision (it may be fewer with %g!).
    snprintf( buffer, size, "%.17lg", value );
  }
  else
  {
    // +/- x.xxxx
    snprintf( buffer, size, "%.16lf", value );

    // Round off
    int len = strlen( buffer );
    if (buffer[len-1] < '5')
    {
      buffer[--len] = 0;
    }
    else
    {
      Logical carry = true;
      buffer[--len] = 0;
      int i = len - 1;
      while (i >= 0)
      {
        char ch = buffer[i];
        if (ch >= '0' && ch <= '9')
        {
          if (ch == '9')
          {
            buffer[i] = '0';
          }
          else
          {
            ++buffer[i];
            carry = false;
            break;
          }
        }
        --i;
      }
      if (carry)
      {
        // Need one more '1' at the front of all this
        i = -1;
        for (;;)
        {
          char ch = buffer[++i];
          if (ch >= '0' && ch <= '9') break;
        }
        memmove( buffer+i+1, buffer+i, len );
        ++len;
        buffer[i] = '1';
      }
    }

    // rescan the rounded-off value
    sscanf( buffer, "%lf", &value );

    // Remove excess digits as long as the reparsed value doesn't change.
    // Without doing this step 3.1415 would be printed as
    // 3.14150000000000018, for instance.
    //
    // With this process we see:
    //   3.14150000000000018 == 3.1415?  TRUE
    //   3.1415000000000001  == 3.1415?  TRUE
    //   3.141500000000000   == 3.1415?  TRUE
    //   ...
    //   3.1415              == 3.1415?  TRUE
    //   3.141               == 3.1415?  FALSE - add the 5 back on and done
    char temp = 0;
    while (len > 0)
    {
      temp = buffer[--len];  // store new temp
      buffer[len] = 0;
      Real n;
      sscanf( buffer, "%lf", &n );
      if (n != value) break;
    }

    // Put back the last character whose absence changed the value.
    buffer[len] = temp;
  }
}

Integer calculate_hash_code( const char* value )
{
  --value;
  Integer hash_code = 0;
  char ch;
  while ((ch = *(++value)))
  {
    hash_code = ((hash_code << 3) - hash_code) + ch;
  }
  return hash_code;
}

Integer calculate_hash_code( Character* characters, Integer count )
{
  --characters;
  Integer hash_code = 0;
  while (--count >= 0)
  {
    hash_code = ((hash_code << 3) - hash_code) + *(++characters);
  }
  return hash_code;
}

};  // namespace Boss
