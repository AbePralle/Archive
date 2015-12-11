//=============================================================================
//  BossParser.h
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#pragma once

#include "Boss.h"

namespace Boss
{

struct Parser
{
  VM*         vm;
  Tokenizer   tokenizer;

  Parser( VM* vm );

  Logical consume( TokenType token_type ) { return tokenizer.consume(token_type); }
  Logical consume_eols();
  void    must_consume( TokenType token_type, const char* expected );
  Cmd*    peek() { return tokenizer.peek(); }
  Cmd*    read() { return tokenizer.read(); }

  Logical next_is_statement();

  Logical parse( const char* filepath );
  Logical parse( const char* filepath, const char* utf8_data, Integer utf8_count=-1 );
  Logical parse( const char* filepath, Character* data, Integer count );
  Logical parse( ParseReader& parse_reader );

  void    parse_multi_line_statements( CmdList& commands );
  Cmd*    parse_statement();
  Cmd*    parse_expression();
  Cmd*    parse_add_subtract();
  Cmd*    parse_add_subtract( Cmd* lhs );
  Cmd*    parse_multiply_divide_mod();
  Cmd*    parse_multiply_divide_mod( Cmd* lhs );
  Cmd*    parse_pre_unary();
  Cmd*    parse_term();
};

}; // namespace Boss
