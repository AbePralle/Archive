//=============================================================================
//  BossMethod.cpp
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#include "Boss.h"

namespace Boss
{

void Method::resolve( Scope* scope )
{
  if (resolved) return;
  resolved = true;

  statements.resolve( scope );
}

}; // namespace Boss
