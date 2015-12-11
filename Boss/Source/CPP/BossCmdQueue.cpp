//=============================================================================
//  BossCmdQueue.cpp
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#include "Boss.h"

namespace Boss
{

CmdQueue::CmdQueue() : count(0), position(0)
{
  capacity = 5000;
  elements = new Cmd*[ capacity ];
}

CmdQueue::~CmdQueue()
{
  delete elements;
}

void CmdQueue::add( Cmd* cmd )
{
  if (count == capacity)
  {
    capacity *= 2;
    Cmd** new_elements = new Cmd*[ capacity ];
    memcpy( new_elements, elements, count*sizeof(Cmd*) );;
    delete elements;
    elements = new_elements;
  }
  elements[ count++ ] = cmd;
}

Logical CmdQueue::consume( TokenType token_type )
{
  Cmd* next = peek();
  if ( !next || next->token_type != token_type ) return false;
  read();
  return true;
}

Logical CmdQueue::next_is( TokenType token_type )
{
  Cmd* next = peek();
  return (next && next->token_type == token_type );
}

Logical CmdQueue::next_is_end_token()
{
  TokenType type = peek_token();
  return (type == TOKEN_EOL || type == TOKEN_SEMICOLON);
}

Cmd* CmdQueue::peek( Integer lookahead )
{
  Integer index;
  if ((index = position+lookahead) < count) return elements[ index ];
  return 0;
}

TokenType CmdQueue::peek_token( Integer lookahead )
{
  Integer index;
  if ((index = position+lookahead) < count) return elements[ index ]->token_type;
  return TOKEN_UNSPECIFIED;
}

Cmd* CmdQueue::read()
{
  if (position == count)
  {
    if (count)
    {
      elements[count-1]->error( "Unexpected end of input." );
    }
    else
    {
      vm->error_message.print( "Unexpected end of input." );
    }
    BOSS_THROW( vm, COMPILE_ERROR );
  }
  return elements[ position++ ];
}

}; // namespace Boss
