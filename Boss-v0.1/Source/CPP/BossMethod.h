//=============================================================================
//  BossMethod.h
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#pragma once

#include "Boss.h"

namespace Boss
{

struct Method
{
  Cmd*    token;
  String* name;
  CmdList statements;
  
  Logical resolved;

  Method( Cmd* token, String* name ) : token(token), name(name), resolved(0) {}

  void resolve( Scope* scope );
};

}; // namespace Boss
