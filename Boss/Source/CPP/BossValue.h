//=============================================================================
//  BossValue.h
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#pragma once

#include "Boss.h"

namespace Boss
{

union Value
{
  Real         as_real;
  Float        as_float;
  Long         as_long;
  Integer      as_integer;
  Character    as_character;
  Byte         as_byte;
  Logical      as_logical;
  Object*      as_object;
};

}; // namespace Boss
