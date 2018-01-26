//=============================================================================
//  BossTokenType.h
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#pragma once

#include "Boss.h"

namespace Boss
{

enum TokenType
{
  TOKEN_UNSPECIFIED,

  KEYWORD_METHOD,

  TOKEN_STATEMENT,  // everything after this is a valid start token for a statement

  TOKEN_CLOSE_PAREN,
  TOKEN_EOL,
  TOKEN_IDENTIFIER,
  TOKEN_LITERAL_INTEGER,
  TOKEN_LITERAL_REAL,
  TOKEN_LITERAL_STRING,
  TOKEN_MINUS,
  TOKEN_OPEN_PAREN,
  TOKEN_PLUS,
  TOKEN_PRINTLN,
  TOKEN_SEMICOLON,
  TOKEN_TIMES,
};

}; // namespace Boss
