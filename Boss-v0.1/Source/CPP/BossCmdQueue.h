//=============================================================================
//  BossCmdQueue.h
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#pragma once

#include "Boss.h"

namespace Boss
{

struct CmdQueue
{
  // PROPERTIES
  VM*          vm;
  Cmd**        elements;
  Integer      count;
  Integer      capacity;
  Integer      position;

  // METHODS
  CmdQueue();
  ~CmdQueue();

  void      add( Cmd* cmd );
  Cmd*&     at( int i ) { return elements[i]; }
  void      clear() { count = 0; }
  Logical   consume( TokenType token_type );
  Logical   has_another() { return position < count; }
  Logical   next_is( TokenType token_type );
  Logical   next_is_end_token();
  Cmd*      peek( Integer lookahead=0 );
  TokenType peek_token( Integer lookahead=0 );
  Cmd*      read();
  void      rewind() { position = 0; }
};

}; // namespace Boss
