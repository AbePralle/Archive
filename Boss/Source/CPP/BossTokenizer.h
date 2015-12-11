//=============================================================================
//  BossTokenizer.h
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#pragma once

#include "Boss.h"

namespace Boss
{

struct Tokenizer : CmdQueue
{
  // PROPERTIES
  ParseReader* reader;
  ParseReaderPosition parse_position;

  // METHODS
  Tokenizer();
  ~Tokenizer();

  Cmd*    create_symbol_token( TokenType symbol_type, const char* symbol_name );

  void    tokenize( ParseReader* reader );
  Logical tokenize_another();
  void    tokenize_identifier();
  Real    tokenize_integer();
  void    tokenize_number();
  Cmd*    tokenize_symbol();
};

}; // namespace Boss
