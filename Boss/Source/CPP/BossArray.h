//=============================================================================
//  BossArray.h
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#pragma once

#include "Boss.h"

namespace Boss
{

struct Array : Object
{
  Integer count;
  union
  {
    Real        reals[];
    Float       floats[];
    Long        longs[];
    Integer     integers[];
    Character   characters[];
    Byte        bytes[];
    Logical     logicals[];
    Object*     objects[];
  };
};

}; // namespace Boss
