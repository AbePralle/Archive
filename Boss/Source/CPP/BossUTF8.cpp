//=============================================================================
//  BossUTF8.cpp
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#include "Boss.h"

namespace Boss
{

Integer UTF8::decoded_count( const char* utf8_data, Integer utf8_count )
{
  if (utf8_count == -1) utf8_count = (int) strlen( utf8_data );

  const char* cur   = utf8_data - 1;
  const char* limit = utf8_data + utf8_count;

  int result_count = 0;
  while (++cur < limit)
  {
    ++result_count;
    int ch = *((unsigned char*)cur);
    if (ch >= 0x80)
    {
      if ((ch & 0xe0) == 0xc0) ++cur;
      else                     cur += 2;
    }
  }

  return result_count;
}

void UTF8::decode( const char* utf8_data, Integer utf8_count,
                       Character* dest_buffer, Integer decoded_count )
{
  Byte*      src  = (Byte*)(utf8_data - 1);
  Character* dest = dest_buffer - 1;

  int remaining_count = decoded_count;
  while (--remaining_count >= 0)
  {
    int ch = *(++src);
    if (ch >= 0x80)
    {
      if ((ch & 0xe0) == 0xc0)
      {
        // 110x xxxx  10xx xxxx
        ch = ((ch & 0x1f) << 6) | (*(++src) & 0x3f);
      }
      else
      {
        // 1110 xxxx  10xx xxxx  10xx xxxx
        ch = ((ch & 0x1f) << 6) | (*(++src) & 0x3f);
        ch = (ch << 6) | (*(++src) & 0x3f);
      }
    }
    *(++dest) = (Character) ch;
  }
}

}; // namespace Boss
