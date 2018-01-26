//=============================================================================
//  BossCmd.cpp
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#include "Boss.h"

namespace Boss
{

//-----------------------------------------------------------------------------
//  Cmd
//-----------------------------------------------------------------------------
CmdAccess* Cmd::add_arg( Cmd* arg )
{
  error( "Syntax error - end of line or ';' expected." );
  BOSS_THROW( vm, COMPILE_ERROR );
}

StringBuilder& Cmd::error( const char* message )
{
  vm->error_filepath = filepath;
  vm->error_line = line;
  return vm->error_message.print( message );
}

void Cmd::print()
{
  StringBuilder buffer;
  print( buffer );
  buffer.log();
}

void Cmd::println()
{
  StringBuilder buffer;
  print( buffer );
  buffer.println();
  buffer.log();
}

Cmd* Cmd::require_value()
{
  if (type()) return this;

  error( "Value expected." );
  BOSS_THROW( vm, COMPILE_ERROR );
}

void Cmd::trace()
{
  if (size < 0) return;  // already traced
  size = ~size;
}

void* Cmd::operator new( size_t size, VM* vm, String*filepath, Integer line )
{
  Cmd* cmd = (Cmd*) vm->allocator.allocate( (int) size );
  cmd->vm = vm;
  cmd->filepath = filepath;
  cmd->size = (int) size;
  cmd->reference_count = 0;
  cmd->next_allocation = vm->system_objects;
  vm->system_objects = cmd;
  vm->have_new_system_objects = true;
  cmd->line = line;
  return cmd;
}

void* Cmd::operator new( size_t size, Tokenizer* tokenizer )
{
  return operator new( size, tokenizer->vm, tokenizer->reader->filepath, tokenizer->parse_position.line );
}

void* Cmd::operator new( size_t size, Cmd* reference )
{
  return operator new( size, reference->vm, reference->filepath, reference->line );
}

void  Cmd::operator delete( void* cmd )
{
  ((Cmd*)cmd)->vm->allocator.free( cmd, ((Cmd*)cmd)->size );
}


//-----------------------------------------------------------------------------
//  CmdList
//-----------------------------------------------------------------------------
void CmdList::execute()
{
  Cmd**   cur = commands.elements - 1;
  Integer count = commands.count;
  while (--count >= 0)
  {
    (*(++cur))->execute();
  }
}

void CmdList::print( StringBuilder& buffer )
{
  for (int i=0; i<commands.count; ++i)
  {
    commands.elements[i]->print( buffer );
    buffer.println();
  }
}

Cmd* CmdList::resolve( Scope* scope )
{
  for (int i=0; i<commands.count; ++i)
  {
    commands.at(i) = commands.at(i)->resolve(scope);
  }
  return this;
}

void CmdList::trace()
{
  if (size < 0) return;  // already traced
  size = ~size;

  for (int i=commands.count; --i>=0; )
  {
    commands.elements[i]->trace();
  }
}


//-----------------------------------------------------------------------------
//  Cmd Literals
//-----------------------------------------------------------------------------
Type* CmdLiteralReal::type() { return vm->type_Real; }
Type* CmdLiteralInteger::type() { return vm->type_Integer; }

//-----------------------------------------------------------------------------
//  Access
//-----------------------------------------------------------------------------
Cmd* CmdAccess::resolve( Scope* scope )
{
  if (name->equals("println"))
  {
    switch (args.commands.count)
    {
      case 0: return (new(this) CmdPrintln())->resolve(scope);
      case 1: return (new(this) CmdPrintln(args.commands.elements[0]))->resolve(scope);
      default:
        error( "Too many arguments for println." );
        BOSS_THROW( vm, COMPILE_ERROR );
    }
  }

  error( "No such variable or method \"" ).print( name ).print( "\" found." );
  BOSS_THROW( vm, COMPILE_ERROR );
}

//-----------------------------------------------------------------------------
//  Unary Commands
//-----------------------------------------------------------------------------
void CmdUnary::trace()
{
  if (size < 0) return;  // already traced
  size = ~size;

  if (operand) operand->trace();
}

Cmd* CmdEvaluateAndDiscard::resolve( Scope* scope )
{
  operand = operand->resolve(scope);

  Type* operand_type = operand->type();
  if ( !operand_type ) return operand; // no discard necessary

  if (operand_type == vm->type_Real)    return (new(operand) CmdEvaluateAndDiscardReal(operand));
  if (operand_type == vm->type_Long)    return (new(operand) CmdEvaluateAndDiscardLong(operand));
  if (operand_type == vm->type_Integer) return (new(operand) CmdEvaluateAndDiscardInteger(operand));
  if (operand_type == vm->type_Object)  return (new(operand) CmdEvaluateAndDiscardObject(operand));

  error( "Unhandled type '" ).print( operand_type->name ).println( "' in CmdEvaluateAndDiscard::resolve()." );
  BOSS_THROW( vm, COMPILE_ERROR );
}

Cmd* CmdNegate::resolve( Scope* scope )
{
  operand = operand->resolve(scope)->require_value();

  Type* operand_type = operand->type();
  if (operand_type == vm->type_Real)    return (new(this) CmdNegateReal(operand))->resolve(scope);
  if (operand_type == vm->type_Integer) return (new(this) CmdNegateInteger(operand))->resolve(scope);

  error( "Unhandled type '" ).print( operand_type->name ).println( "' in CmdNegate::resolve()." );
  BOSS_THROW( vm, COMPILE_ERROR );
}

//-----------------------------------------------------------------------------
//  Binary Cmds
//-----------------------------------------------------------------------------
void CmdBinaryOp::print( StringBuilder& buffer )
{
  if (left) left->print( buffer );
  buffer.print( symbol() );
  if (right) right->print( buffer );
}

void CmdBinaryOp::trace()
{
  if (size < 0) return;  // already traced
  size = ~size;

  if (left)  left->trace();
  if (right) right->trace();
}

Type* CmdBinaryRealOp::type() { return vm->type_Real; }
Type* CmdBinaryIntegerOp::type() { return vm->type_Integer; }

Cmd* CmdAdd::resolve( Scope* scope )
{
  left  = left->resolve(scope)->require_value();
  right = right->resolve(scope)->require_value();

  // TODO: convert operands to common type
  Type* operand_type = left->type();
  if (operand_type == vm->type_Real)    return (new(this) CmdAddReal(left,right));
  if (operand_type == vm->type_Integer) return (new(this) CmdAddInteger(left,right));

  error( "Unhandled type '" ).print( operand_type->name ).println( "' in CmdAdd::resolve()." );
  BOSS_THROW( vm, COMPILE_ERROR );
}

Cmd* CmdSubtract::resolve( Scope* scope )
{
  left  = left->resolve(scope)->require_value();
  right = right->resolve(scope)->require_value();

  // TODO: convert operands to common type
  Type* operand_type = left->type();
  if (operand_type == vm->type_Real)    return (new(this) CmdSubtractReal(left,right));
  if (operand_type == vm->type_Integer) return (new(this) CmdSubtractInteger(left,right));

  error( "Unhandled type '" ).print( operand_type->name ).println( "' in CmdSubtract::resolve()." );
  BOSS_THROW( vm, COMPILE_ERROR );
}

Cmd* CmdMultiply::resolve( Scope* scope )
{
  left  = left->resolve(scope)->require_value();
  right = right->resolve(scope)->require_value();

  // TODO: convert operands to common type
  Type* operand_type = left->type();
  if (operand_type == vm->type_Real)    return (new(this) CmdMultiplyReal(left,right));
  if (operand_type == vm->type_Integer) return (new(this) CmdMultiplyInteger(left,right));

  error( "Unhandled type '" ).print( operand_type->name ).println( "' in CmdMultiply::resolve()." );
  BOSS_THROW( vm, COMPILE_ERROR );
}


//-----------------------------------------------------------------------------
//  Cmd Statements
//-----------------------------------------------------------------------------
void CmdPrintln::print( StringBuilder& buffer )
{
  if (operand) operand->print( buffer.print("println ") );
}

Cmd* CmdPrintln::resolve( Scope* scope )
{
  if (operand) operand = operand->resolve(scope)->require_value();
  else         return new(this) CmdPrintlnNil();

  Type* operand_type = operand->type();
  if (operand_type == vm->type_Real)    return (new(this) CmdPrintlnReal(operand))->resolve(scope);
  if (operand_type == vm->type_Integer) return (new(this) CmdPrintlnInteger(operand))->resolve(scope);

  error( "Unhandled type '" ).print( operand_type->name ).println( "' in CmdPrintln::resolve()." );
  BOSS_THROW( vm, COMPILE_ERROR );
}

void CmdPrintln::trace()
{
  if (size < 0) return;  // already traced
  size = ~size;

  if (operand) operand->trace();
}

}; // namespace Boss
