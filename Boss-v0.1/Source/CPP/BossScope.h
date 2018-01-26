//=============================================================================
//  BossScope.h
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#pragma once

#include "Boss.h"

namespace Boss
{

struct Scope
{
  VM*        vm;
  SystemList locals;

  Scope( VM* vm );
};

}; // namespace Boss
