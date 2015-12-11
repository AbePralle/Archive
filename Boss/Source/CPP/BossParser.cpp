//=============================================================================
//  BossParser.cpp
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#include "Boss.h"

namespace Boss
{

Parser::Parser( VM* vm ) : vm(vm)
{
  tokenizer.vm = vm;
}

Logical Parser::consume_eols()
{
  if ( !consume(TOKEN_EOL) ) return false;
  while (consume(TOKEN_EOL)) {}
  return true;
}

void Parser::must_consume( TokenType token_type, const char* expected )
{
  if (consume(token_type)) return;

  vm->error_message.print( "Expected " ).print( expected ).print( ", found " );
  if (peek())
  {
    vm->error_message.print( "'" );
    peek()->print( vm->error_message );
    vm->error_message.print( "'." );
  }
  else
  {
    vm->error_message.print( "end of input." );
  }
  BOSS_THROW( vm, COMPILE_ERROR );
}

Logical Parser::next_is_statement()
{
  return (tokenizer.peek_token() >= TOKEN_STATEMENT);
}

Logical Parser::parse( const char* filepath )
{
  BOSS_TRY(vm)
  {
    ParseReader parse_reader;
    parse_reader.init( vm, filepath );
    return parse( parse_reader );
  }
  BOSS_CATCH_ANY
  {
    vm->log_error();
    return false;
  }
  BOSS_END_TRY(vm)
}

Logical Parser::parse( const char* filepath, const char* utf8_data, Integer utf8_count )
{
  if (utf8_count == -1) utf8_count = strlen( utf8_data );
  ParseReader parse_reader;
  parse_reader.init( vm, filepath, utf8_data, utf8_count );
  return parse( parse_reader );
}

Logical Parser::parse( const char* filepath, Character* data, Integer count )
{
  ParseReader parse_reader;
  parse_reader.init( vm, filepath, data, count );
  return parse( parse_reader );
}

Logical Parser::parse( ParseReader& parse_reader )
{
  BOSS_TRY(vm)
  {
    tokenizer.tokenize( &parse_reader );
    parse_multi_line_statements( vm->main_routine->statements );
    return true;
  }
  BOSS_CATCH_ANY
  {
    vm->log_error();
    return false;
  }
  BOSS_END_TRY(vm)
}

void Parser::parse_multi_line_statements( CmdList& commands )
{
  consume_eols();
  while (next_is_statement())
  {
    Cmd* statement = parse_statement();
    if (statement)
    {
      commands.add( statement );
    }
    while (consume_eols() || consume(TOKEN_SEMICOLON)) {}
  }
}

Cmd* Parser::parse_statement()
{
  Cmd* result = parse_expression();

  while (tokenizer.has_another() && !tokenizer.next_is_end_token())
  {
    result->add_arg( parse_expression() );
  }

  result = new(result) CmdEvaluateAndDiscard( result );
  return result;
}

Cmd* Parser::parse_expression()
{
  consume_eols();
  return parse_add_subtract();
}

Cmd* Parser::parse_add_subtract()
{
  return parse_add_subtract( parse_multiply_divide_mod() );
}

Cmd* Parser::parse_add_subtract( Cmd* lhs )
{
  switch (tokenizer.peek_token())
  {
    case TOKEN_PLUS:
    {
      CmdAdd* cmd_add = (CmdAdd*) tokenizer.read();
      cmd_add->left = lhs;
      cmd_add->right = parse_multiply_divide_mod();
      consume_eols();
      return parse_add_subtract( cmd_add );
    }

    case TOKEN_MINUS:
    {
      CmdSubtract* cmd_add = (CmdSubtract*) tokenizer.read();
      cmd_add->left = lhs;
      cmd_add->right = parse_multiply_divide_mod();
      consume_eols();
      return parse_add_subtract( cmd_add );
    }

    default:
      return lhs;
  }
}

Cmd* Parser::parse_multiply_divide_mod()
{
  return parse_multiply_divide_mod( parse_pre_unary() );
}

Cmd* Parser::parse_multiply_divide_mod( Cmd* lhs )
{
  switch (tokenizer.peek_token())
  {
    case TOKEN_TIMES:
    {
      CmdMultiply* cmd_multiply = (CmdMultiply*) tokenizer.read();
      cmd_multiply->left = lhs;
      cmd_multiply->right = parse_pre_unary();
      consume_eols();
      return parse_multiply_divide_mod( cmd_multiply );
    }

    default:
      return lhs;
  }
}

Cmd* Parser::parse_pre_unary()
{
  Cmd* cmd = tokenizer.peek();
  if (tokenizer.consume(TOKEN_MINUS))
  {
    return new(cmd) CmdNegate( parse_pre_unary() );
  }
  else
  {
    return parse_term();
  }
}

Cmd* Parser::parse_term()
{
  Cmd* cmd = tokenizer.read();
  switch (cmd->token_type)
  {
    case TOKEN_IDENTIFIER:
      return cmd;

    case TOKEN_LITERAL_INTEGER:
    case TOKEN_LITERAL_REAL:
    case TOKEN_LITERAL_STRING:
      return cmd;

    case TOKEN_OPEN_PAREN:
    {
      Cmd* result = parse_expression();
      must_consume( TOKEN_CLOSE_PAREN, "')'" );
      return result;
    }

    default:
      break;
  }

  cmd->error( "Unexpected '" );
  cmd->print( vm->error_message );
  vm->error_message.print( "'." );
  BOSS_THROW( vm, COMPILE_ERROR );
}

}; // namespace Boss
