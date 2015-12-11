//=============================================================================
//  BossUTF8.h
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#pragma once

#include "Boss.h"

namespace Boss
{

struct UTF8
{
  static Integer decoded_count( const char* utf8_data, Integer utf8_count );
  static void    decode( const char* utf8_data, Integer utf8_count,
                         Character* dest_buffer, Integer decoded_count );
};

}; // namespace Boss
