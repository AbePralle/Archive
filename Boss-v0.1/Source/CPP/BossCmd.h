//=============================================================================
//  BossCmd.h
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#pragma once

#include "Boss.h"

namespace Boss
{

//-----------------------------------------------------------------------------
//  Cmd
//-----------------------------------------------------------------------------
struct Cmd : SystemObject
{
  // PROPERTIES
  String*   filepath;
  TokenType token_type;
  Integer   line;

  // METHODS
  Cmd() : token_type(TOKEN_UNSPECIFIED) {}
  Cmd( TokenType token_type ) : token_type(token_type) {}
  virtual ~Cmd() {}

  virtual CmdAccess* add_arg( Cmd* arg );
  StringBuilder&     error( const char* mesg );
  virtual void       execute() {}
  virtual Real       execute_real() { return 0.0; }
  virtual Long       execute_long() { return 0; }
  virtual Integer    execute_integer() { return 0; }
  virtual Object*    execute_object() { return 0; }
  void               print();
  virtual void       print( StringBuilder& buffer ) = 0;
  void               println();
  Cmd*               require_value();
  virtual Cmd*       resolve( Scope* scope ) { return this; }
  virtual void       trace();
  virtual Type*      type() { return 0; }

  // ROUTINES
  static void* operator new( size_t size, VM* vm, String* filepath, Integer line ); 
  static void* operator new( size_t size, Tokenizer* tokenizer ); 
  static void* operator new( size_t size, Cmd* reference ); 
  static void  operator delete( void *cmd );
};

//-----------------------------------------------------------------------------
//  CmdList
//-----------------------------------------------------------------------------
struct CmdList : Cmd
{
  CmdQueue commands;

  CmdList() {}
  CmdList( Cmd* cmd ) { commands.add(cmd); }
  CmdList( Cmd* cmd1, Cmd* cmd2 ) { commands.add(cmd1); commands.add(cmd2); }

  CmdList* add( Cmd* cmd ) { commands.add(cmd); return this; }
  CmdList* clear() { commands.clear(); return this; }
  void     execute();
  void     print( StringBuilder& buffer );
  Cmd*     resolve( Scope* scope );
  void     trace();
};


//-----------------------------------------------------------------------------
//  Cmd Tokens
//-----------------------------------------------------------------------------
struct CmdSymbol : Cmd
{
  const char* symbol;

  CmdSymbol( TokenType token_type, const char* symbol )
    : Cmd(token_type), symbol(symbol) {}

  void print( StringBuilder& buffer ) { buffer.print(symbol); }
};

struct CmdLiteralInteger : Cmd
{
  Integer value;

  CmdLiteralInteger( Integer value ) : Cmd(TOKEN_LITERAL_INTEGER), value(value) {}

  Integer execute_integer() { return value; }
  void    print( StringBuilder& buffer ) { buffer.print(value); }
  Type*   type();
};

struct CmdLiteralReal : Cmd
{
  Real value;

  CmdLiteralReal( Real value ) : Cmd(TOKEN_LITERAL_REAL), value(value) {}

  Real  execute_real() { return value; }
  void  print( StringBuilder& buffer ) { buffer.print(value); }
  Type* type();
};

struct CmdAccess : Cmd
{
  String* name;
  CmdList args;

  CmdAccess() : Cmd(TOKEN_IDENTIFIER) {}
  CmdAccess( String* name ) : Cmd(TOKEN_IDENTIFIER), name(name) {}

  CmdAccess* add_arg( Cmd* arg ) { args.add(arg); return this; }
  void       execute() { StringBuilder buffer; buffer.println(name).log(); }
  void       print( StringBuilder& buffer ) { buffer.print(name); }

  Cmd*       resolve( Scope* scope );

  Type* type() { return 0; }
};

struct CmdUnary : Cmd
{
  Cmd* operand;

  CmdUnary( Cmd* operand ) : operand(operand) {}
  CmdUnary( TokenType token_type, Cmd* operand ) : Cmd(token_type), operand(operand) {}
  Cmd*  resolve( Scope* scope ) { operand = operand->resolve(scope)->require_value(); return this; }
  void  trace();
  Type* type() { return operand->type(); }
};

struct CmdEvaluateAndDiscard : CmdUnary
{
  CmdEvaluateAndDiscard( Cmd* operand ) : CmdUnary(operand) {}
  void print( StringBuilder& buffer ) { operand->print(buffer); }
  Cmd* resolve( Scope* scope );
};

struct CmdEvaluateAndDiscardReal : CmdEvaluateAndDiscard
{
  CmdEvaluateAndDiscardReal( Cmd* operand ) : CmdEvaluateAndDiscard(operand) {}
  void execute() { operand->execute_real(); }
};

struct CmdEvaluateAndDiscardLong : CmdEvaluateAndDiscard
{
  CmdEvaluateAndDiscardLong( Cmd* operand ) : CmdEvaluateAndDiscard(operand) {}
  void execute() { operand->execute_long(); }
};

struct CmdEvaluateAndDiscardInteger : CmdEvaluateAndDiscard
{
  CmdEvaluateAndDiscardInteger( Cmd* operand ) : CmdEvaluateAndDiscard(operand) {}
  void execute() { printf("%d\n", operand->execute_integer() ); }
};

struct CmdEvaluateAndDiscardObject : CmdEvaluateAndDiscard
{
  CmdEvaluateAndDiscardObject( Cmd* operand ) : CmdEvaluateAndDiscard(operand) {}
  void execute() { operand->execute_object(); }
};

struct CmdNegate : CmdUnary
{
  CmdNegate( Cmd* operand ) : CmdUnary(TOKEN_MINUS,operand) {}

  void  print( StringBuilder& buffer ) { operand->print( buffer.print('-') ); }
  Cmd*  resolve( Scope* scope );
};

struct CmdNegateReal : CmdNegate
{
  CmdNegateReal( Cmd* operand ) : CmdNegate(operand) {}

  Real  execute_real() { return -operand->execute_real(); }
  Cmd*  resolve( Scope* scope ) { return this; }
};

struct CmdNegateInteger : CmdNegate
{
  CmdNegateInteger( Cmd* operand ) : CmdNegate(operand) {}

  Integer execute_integer() { return -operand->execute_integer(); }
  Cmd*    resolve( Scope* scope ) { return this; }
};

struct CmdBinaryOp : Cmd
{
  Cmd*    left;
  Cmd*    right;

  CmdBinaryOp() {}
  CmdBinaryOp( TokenType token_type ) : Cmd(token_type), left(0), right(0) {}
  CmdBinaryOp( Cmd* left, Cmd* right ) : left(left), right(right) {}

  void    print( StringBuilder& buffer );
  virtual const char* symbol() { return "?"; }
  void    trace();
};

struct CmdBinaryRealOp : CmdBinaryOp
{
  CmdBinaryRealOp( Cmd* left, Cmd* right ) : CmdBinaryOp(left,right) {}

  Cmd*  resolve( Scope* scope ) { left = left->resolve(scope); right = right->resolve(scope); return this; }
  Type* type();
};

struct CmdBinaryIntegerOp : CmdBinaryOp
{
  CmdBinaryIntegerOp( Cmd* left, Cmd* right ) : CmdBinaryOp(left,right) {}

  Cmd*    resolve( Scope* scope ) { left = left->resolve(scope); right = right->resolve(scope); return this; }
  Type*   type();
};

struct CmdAdd : CmdBinaryOp
{
  CmdAdd() : CmdBinaryOp(TOKEN_PLUS) {}

  Cmd*        resolve( Scope* scope );
  const char* symbol() { return "+"; }
};

struct CmdAddReal : CmdBinaryRealOp
{
  CmdAddReal( Cmd* left, Cmd* right ) : CmdBinaryRealOp(left,right) {}

  Real  execute_real() { return left->execute_real() + right->execute_real(); }
  const char* symbol() { return "+"; }
};

struct CmdAddInteger : CmdBinaryIntegerOp
{
  CmdAddInteger( Cmd* left, Cmd* right ) : CmdBinaryIntegerOp(left,right) {}

  Integer execute_integer() { return left->execute_integer() + right->execute_integer(); }
  const char* symbol() { return "+"; }
};

struct CmdSubtract : CmdBinaryOp
{
  CmdSubtract() : CmdBinaryOp(TOKEN_MINUS) {}

  Cmd*    resolve( Scope* scope );
  void    print( StringBuilder& buffer ) { buffer.print('-'); }
  const char* symbol() { return "-"; }
};

struct CmdSubtractReal : CmdBinaryRealOp
{
  CmdSubtractReal( Cmd* left, Cmd* right ) : CmdBinaryRealOp(left,right) {}

  Real  execute_real() { return left->execute_real() - right->execute_real(); }
  const char* symbol() { return "-"; }
};

struct CmdSubtractInteger : CmdBinaryIntegerOp
{
  CmdSubtractInteger( Cmd* left, Cmd* right ) : CmdBinaryIntegerOp(left,right) {}

  Integer     execute_integer() { return left->execute_integer() - right->execute_integer(); }
  const char* symbol() { return "-"; }
};

struct CmdMultiply : CmdBinaryOp
{
  CmdMultiply() : CmdBinaryOp(TOKEN_TIMES) {}
  CmdMultiply( Cmd* left, Cmd* right ) : CmdBinaryOp(left,right) {}

  Cmd*        resolve( Scope* scope );
  const char* symbol() { return "*"; }
};

struct CmdMultiplyReal : CmdBinaryRealOp
{
  CmdMultiplyReal( Cmd* left, Cmd* right ) : CmdBinaryRealOp(left,right) {}

  Real  execute_real() { return left->execute_real() * right->execute_real(); }
  const char* symbol() { return "*"; }
};

struct CmdMultiplyInteger : CmdBinaryIntegerOp
{
  CmdMultiplyInteger( Cmd* left, Cmd* right ) : CmdBinaryIntegerOp(left,right) {}

  Integer execute_integer() { return left->execute_integer() * right->execute_integer(); }
  const char* symbol() { return "*"; }
};

struct CmdStatement : Cmd
{
  CmdStatement() {}
  CmdStatement( TokenType token_type ) : Cmd(token_type) {}
};

struct CmdPrintln : CmdStatement
{
  Cmd* operand;

  CmdPrintln() : operand(0) { this->token_type = TOKEN_PRINTLN; }
  CmdPrintln( Cmd* operand ) : CmdStatement(TOKEN_PRINTLN), operand(operand) {}
  void    print( StringBuilder& buffer );
  Cmd*    resolve( Scope* scope );
  void    trace();
};

struct CmdPrintlnNil : CmdPrintln
{
  CmdPrintlnNil() {}

  void    execute() { printf("\n"); }
};

struct CmdPrintlnType : CmdPrintln
{
  CmdPrintlnType( Cmd* operand ) : CmdPrintln(operand) {}

  Cmd* resolve( Scope* scope ) { operand = operand->resolve(scope)->require_value(); return this; }
};

struct CmdPrintlnReal : CmdPrintlnType
{
  CmdPrintlnReal( Cmd* operand ) : CmdPrintlnType(operand) {}

  void execute() { printf( "%.4lf\n", operand->execute_real() ); }
};

struct CmdPrintlnInteger : CmdPrintlnType
{
  CmdPrintlnInteger( Cmd* operand ) : CmdPrintlnType(operand) {}

  void execute() { printf( "%d\n", operand->execute_integer() ); }
};

//-----------------------------------------------------------------------------
//  Local Variable Declaration
//-----------------------------------------------------------------------------

}; // namespace Boss
