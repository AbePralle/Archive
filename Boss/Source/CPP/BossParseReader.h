//=============================================================================
//  BossParseReader.h
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#pragma once

#include "Boss.h"

namespace Boss
{

//-----------------------------------------------------------------------------
//  ParseReaderPosition
//-----------------------------------------------------------------------------
struct ParseReaderPosition
{
  Integer index;
  Integer line;
  Integer column;

  ParseReaderPosition() : index(0), line(1), column(1) {}
};


//-----------------------------------------------------------------------------
//  ParseReader
//-----------------------------------------------------------------------------
struct ParseReader
{
  // PROPERTIES
  VM*                 vm;
  String*             filepath;
  Character*          data;
  ParseReaderPosition position;
  Integer             count;
  Integer             spaces_per_tab;

  // METHODS
  ParseReader();
  ~ParseReader();

  void init( VM* vm, const char* filepath );
  void init( VM* vm, const char* filepath, const char* utf8_data, Integer utf8_count );
  void init( VM* vm, const char* filepath, Character* data, Integer count );

  void      sanitize_characters();

  Logical   consume( Character value );
  Logical   consume_keyword( const char* keyword );
  Logical   consume_whitespace();
  StringBuilder& error( const char* message );
  Logical   has_another() { return position.index < count; }
  Character peek( Integer lookahead=0 );
  Character read();
};

}; // namespace Boss
