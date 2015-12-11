//=============================================================================
//  BossString.h
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#pragma once

#include "Boss.h"

namespace Boss
{

struct String : Object
{
  // PROPERTIES
  Integer   count;
  Integer   hash_code;
  Character characters[];

  // METHODS
  String*   update_hash_code();
};

}; // namespace Boss
