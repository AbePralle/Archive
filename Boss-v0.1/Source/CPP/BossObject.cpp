//=============================================================================
//  BossObject.cpp
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#include "Boss.h"

namespace Boss
{

void Object::print()
{
  StringBuilder buffer;
  type->print( this, buffer );
  buffer.log();
}

void Object::println()
{
  StringBuilder buffer;
  type->print( this, buffer );
  buffer.println().log();
}

}; // namespace Boss
