//=============================================================================
//  BossString.cpp
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#include "Boss.h"

namespace Boss
{

String* String::update_hash_code()
{
  Character* cur = characters - 1;
  Integer code = 0;
  int n = count;
  while (--n >= 0)
  {
    code = ((code << 3) - code) + *(++cur);
  }
  hash_code = code;
  return this;
}

}; // namespace Boss
