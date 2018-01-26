//=============================================================================
//  BossAttributes.h
//
//  2015.08.28 by Abe Pralle
//=============================================================================
#pragma once

#include "Boss.h"

namespace Boss
{

enum Attribute
{
  DEFAULT            = 0,
};

struct Attributes
{
  Attribute flags;

  Attributes( Attribute flags=DEFAULT ) : flags(flags) {}
};

}; // namespace Boss
