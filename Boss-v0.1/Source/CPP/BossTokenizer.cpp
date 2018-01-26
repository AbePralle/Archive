//=============================================================================
//  BossTokenizer.cpp
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#include "Boss.h"

#include <cmath>
using namespace std;

namespace Boss
{

Tokenizer::Tokenizer() : reader(0)
{
}

Tokenizer::~Tokenizer()
{
}

Cmd* Tokenizer::create_symbol_token( TokenType symbol_type, const char* symbol_name )
{
  return new(this) CmdSymbol( symbol_type, symbol_name );
}

void Tokenizer::tokenize( ParseReader* reader )
{
  this->reader = reader;

  while (tokenize_another()) {}
}

Logical Tokenizer::tokenize_another()
{
  reader->consume_whitespace();
  if ( !reader->has_another() ) return false;

  parse_position = reader->position;
  Character ch = reader->peek();

  // EOL
  if (ch == '\n')
  {
    reader->read();
    add( create_symbol_token(TOKEN_EOL,"end of line") );
    return true;
  }

  // Identifier
  if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_')
  {
    tokenize_identifier();
    return true;
  }


  // Literal Integer
  if (ch >= '0' && ch <= '9')
  {
    tokenize_number();
    return true;
  }

  // A decimal like ".1".
  if (ch == '.')
  {
    Character next = reader->peek(1);
    if (next >= '0' && next <= '9')
    {
      tokenize_number();
      return true;
    }
  }

  // Symbol - tokenize_symbol() will throw a syntax error if it doesn't recognize
  // what comes next.
  add( tokenize_symbol() );
  return true;
}

Real Tokenizer::tokenize_integer()
{
  // Used to tokenize regular integers as well as the integer part of a real
  // number; hence the 'Real' return type.
  Real n = 0.0;
  Character ch = reader->peek();
  while (ch >= '0' and ch <= '9')
  {
    Integer intval = reader->read() - '0';
    n = n * 10.0 + intval;
    ch = reader->peek();
  }
  return n;
}

void Tokenizer::tokenize_identifier()
{
  StringBuilder buffer;
  Character ch = reader->peek();
  while ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_')
  {
    buffer.print( reader->read() );
    ch = reader->peek();
  }
  add( (new(this) CmdAccess(vm->consolidate(buffer))) );
}

void Tokenizer::tokenize_number()
{
  Logical is_negative = reader->consume('-');
  Logical is_real = false;

  Real n = tokenize_integer();

  if (reader->peek() == '.')
  {
    Character ch = reader->peek(1);
    if (ch >= '0' && ch <= '9')
    {
      reader->read();
      is_real = true;
      Integer start_index = reader->position.index;
      Real fraction = tokenize_integer();
      n += fraction / pow(10,(reader->position.index-start_index));
    }
    else if (ch == '.' || ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_'))
    {
      // The first period was the start of some other token or expression - stop with what we have.
      if (is_negative) n = -n;
      add( new(this) CmdLiteralInteger((Integer)n) );
      return;
    }
    else
    {
      if (is_negative) n = -n;
      add( new(this) CmdLiteralReal(n) );
      return;
    }
  }

  if (reader->consume('E') || reader->consume('e'))
  {
    is_real = true;
    Logical negative_exponent = reader->consume('-');
    if ( !negative_exponent ) reader->consume('+');
    Real power = tokenize_integer();
    if (negative_exponent) n /= power;
    else                   n *= power;
  }

  if (is_negative) n = -n;

  if (is_real)
  {
    add( new(this) CmdLiteralReal(n) );
  }
  else
  {
    add( new(this) CmdLiteralInteger((Integer)n) );
  }
}

Cmd* Tokenizer::tokenize_symbol()
{
  Character ch = reader->read();

  /*
  if (ch == '!')
  {
    if (reader->consume('=')) return TokenType.symbol_ne
    else              return TokenType.symbol_exclamation_point
  }
  else if (ch == '$' )
  {
    local id = read_identifier
    which (id)
      case "define":   add_new_token( TokenType.directive_define )
      case "include":  add_new_token( TokenType.directive_include )
      case "if":       add_new_token( TokenType.directive_if )
      case "elseIf":   add_new_token( TokenType.directive_elseIf )
      case "else":     add_new_token( TokenType.directive_else )
      case "endIf":    add_new_token( TokenType.directive_endIf )
      others
        add_new_token( TokenType.placeholder_id, "$" + id )
    endWhich
    return null
  }
  else if (ch == '%')
    if     (reader->consume('=')) return TokenType.symbol_percent_equals
    else                  return TokenType.symbol_percent


  elseIf (ch == '&' )
    if     (reader->consume('&')) throw error( "Use 'and' instead of '&&'." )
    else if (reader->consume('=')) return TokenType.symbol_ampersand_equals
    else                  return TokenType.symbol_ampersand
    */

  if (ch == '(')
    return create_symbol_token( TOKEN_OPEN_PAREN, "(" );

  else if (ch == ')')
    return create_symbol_token( TOKEN_CLOSE_PAREN, ")" );

  else if (ch == '*')
    //if     (reader->consume('=')) return TokenType.symbol_times_equals
    //else
    return (new(this) CmdMultiply());

  else if (ch == '+')
  /*
    if     (reader->consume('=')) return TokenType.symbol_plus_equals
    else if (reader->consume('+')) return TokenType.symbol_plus_plus
    else */               return (new(this) CmdAdd());

      /*
  else if (ch == ',')
    return TokenType.symbol_comma
    */

  else if (ch == '-')
  {
    //if     (reader->consume('='))  return TokenType.symbol_minus_equals
    //else if (reader->consume('-'))  return TokenType.symbol_minus_minus
    //else if (reader->peek(0) == '>' and reader->peek(1) == '>') return TokenType.symbol_minus
    //else if (reader->consume('>'))  return TokenType.symbol_arrow
    //else                   return TokenType.symbol_minus
    return (new(this) CmdSubtract());
  }
  /*
  else if (ch == '.' )
    if (reader->consume('.'))
      if (reader->consume('.'))
        # ellipsis
        while (reader->consume(' ')) noAction
        if (not reader->consume('\n')) throw error( "End of line expected after '...'." )
        return null 
        else if (reader->consume('<'))
        return TokenType.symbol_upToLessThan
        else if (reader->consume('>'))
        return TokenType.symbol_downToGreaterThan
      else
        return TokenType.symbol_upTo
      endIf
    else if (reader->consume('='))
      return TokenType.symbol_dot_equals
    else
      return TokenType.symbol_dot
    endIf

  else if (ch == '/')
    if (reader->consume('/'))
      tokenize_alternate_string
      return null
    else if (reader->consume('='))
      return TokenType.symbol_slash_equals
    else
      return TokenType.symbol_slash
    endIf

  else if (ch == ':')
    if (reader->consume(':'))
      return TokenType.symbol_colon_colon
    endIf

    if (reader->consume("<<:"))  return TokenType.symbol_shift_left
    else if (reader->consume(">>:"))  return TokenType.symbol_shift_right
    else if (reader->consume(">>>:")) return TokenType.symbol_shift_right_x
    return TokenType.symbol_colon
    */

  else if (ch == ';')
    return create_symbol_token( TOKEN_SEMICOLON, ";" );

      /*
  else if (ch == '<' )
    if     (reader->consume('<')) return TokenType.symbol_open_specialize
    else if (reader->consume('=')) return TokenType.symbol_le
    else if (reader->consume('>')) return TokenType.symbol_compare
    else                  return TokenType.symbol_lt

  else if (ch == '=' )
    if (reader->consume('='))     return TokenType.symbol_eq
    else if (reader->consume('>')) return TokenType.symbol_fat_arrow
    else                  return TokenType.symbol_equals

  else if (ch == '>' )
    if      (reader->consume('=')) return TokenType.symbol_ge
    else if  (reader->consume('>')) return TokenType.symbol_close_specialize
    else                   return TokenType.symbol_gt

  else if (ch == '?')
    return TokenType.symbol_question_mark

  else if (ch == '@' )
    return TokenType.symbol_at

  else if (ch == '[')
    if (reader->consume(']')) return TokenType.symbol_empty_brackets
    return TokenType.symbol_open_bracket
    #return check_for_directives( TokenType.symbol_open_bracket )

  else if (ch == '\\')
    return TokenType.symbol_backslash

  else if (ch == ']')
    return TokenType.symbol_close_bracket

  else if (ch == '^')
    if     (reader->consume('=')) return TokenType.symbol_caret_equals
    else                  return TokenType.symbol_caret

  else if (ch == '{')
    if (reader->consume('}')) return TokenType.symbol_empty_braces
    return TokenType.symbol_open_brace

  else if  (ch == '|' )
    if     (reader->consume('|')) throw error( "Use 'or' instead of '||'." )
    else if (reader->consume('=')) return TokenType.symbol_vertical_bar_equals
    else                  return TokenType.symbol_vertical_bar

  else if (ch == '}' )
    if     (reader->consume('#')) return TokenType.symbol_close_comment # used for error reporting
    else                  return TokenType.symbol_close_brace

  else if (ch == '~')
    if     (reader->consume('=')) return TokenType.symbol_tilde_equals
    else                  return TokenType.symbol_tilde
  */

  else
  {
    reader->error( "Unexpected character '" ).print( ch ).println( "'." );
    BOSS_THROW( reader->vm, COMPILE_ERROR );
  }
}

}; // namespace Boss
