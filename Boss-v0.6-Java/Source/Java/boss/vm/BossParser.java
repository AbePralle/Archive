package boss.vm;

import java.io.*;

public class BossParser
{
  public BossVM           vm;
  public String           filepath;
  /*
  public BossTokenData    tokenData;
  public BossTokenReader  reader;
  public BossCmdData      cmdData;

  public BossParser( BossVM vm, String filepath )
  {
    this.vm = vm;
    this.tokenData    = vm.tokenData;
    this.cmdData      = vm.cmdData;
    this.filepath     = filepath;
  }

  public BossParser( BossVM vm, String filepath, String content )
  {
    this( vm, filepath );

    BossTokenizer tokenizer = new BossTokenizer( vm );
    BossInt32List tokens = tokenizer.tokenize( filepath, content );
    reader = new BossTokenReader( this, tokens );
  }

  public BossParser( BossVM vm, File file )
  {
    this( vm, file.getAbsolutePath() );

    BossTokenizer tokenizer = new BossTokenizer( vm );
    BossInt32List tokens = tokenizer.tokenize( file );
    reader = new BossTokenReader( this, tokens );
  }

  public boolean consume( int tokenType )
  {
    return reader.consume( tokenType );
  }

  public boolean consumeEndOfCommands()
  {
    boolean foundAny = false;
    while (consume(BossTokenType.EOL) || consume(BossTokenType.SYMBOL_SEMICOLON)) foundAny = true;
    return foundAny;
  }

  public boolean consumeEOLs()
  {
    return reader.consumeEOLs();
  }

  public boolean hasAnother()
  {
    return reader.hasAnother();
  }

  public void mustConsume( int tokenType )
  {
    reader.mustConsume( tokenType );
  }

  public boolean nextIs( int tokenType )
  {
    return reader.nextIs( tokenType );
  }

  public boolean nextIsEndOfCommand()
  {
    return reader.nextIs(BossTokenType.EOL) || reader.nextIs(BossTokenType.SYMBOL_SEMICOLON);
  }

  public boolean nextIsStatement()
  {
    if ( !reader.hasAnother() ) return false;
    int t = reader.peek();
    return (!tokenData.isStructural(t));
  }

  public int parseStatements( int statements, int endTokenType )
  {
    if (consumeEOLs())
    {
      statements = parseMultiLineStatements( statements );
      mustConsume( endTokenType );
    }
    else
    {
      statements = parseSingleLineStatements( statements );
      consumeEOLs();
    }
    return statements;
  }

  public int parseMultiLineStatements( int statements )
  {
    int cur = statements;
    consumeEndOfCommands();
    while (nextIsStatement())
    {
      if (statements == 0) cur = statements = cmdData.createCmd( peek(), BossCmdData.STATEMENTS );
      cur = parseStatement( cur, true );
      consumeEndOfCommands();
    }
    return statements;
  }

  public int parseSingleLineStatements( int statements )
  {
    int cur = statements;
    while (nextIsStatement())
    {
      if (statements == 0) cur = statements = cmdData.createCmd( peek(), BossCmdData.STATEMENTS );
      cur = parseStatement( cur, false );
      if ( !consume(BossTokenType.SYMBOL_SEMICOLON) ) return statements;
      while (consume(BossTokenType.SYMBOL_SEMICOLON)) {}

      // Don't let a trailing ';' act as a next-line continuation.
      if (nextIs(BossTokenType.EOL)) break;
    }

    if ( !consume(BossTokenType.EOL) )
    {
      if ( !tokenData.isStructural(peek()) ) mustConsume( BossTokenType.EOL );  // force an error
    }
    return statements;
  }

  public int parseStatement( int statementListTail, boolean allowControlStructures )
  {
    // Returns new list tail
    int expression = parseExpression();
    cmdData.data.set( statementListTail+BossCmdData.NEXT, expression );
    return expression;
  }

  public int parseExpression()
  {
    return parseAddSubtract();
  }

  public int parseAddSubtract()
  {
    return parseAddSubtract( parseMultiplyDivide() );
  }

  public int parseAddSubtract( int left )
  {
    int t = reader.peek();
    if (reader.consume(BossTokenType.SYMBOL_PLUS))
    {
      return parseAddSubtract( cmdData.createCmd(t,BossCmdData.ADD,left,parseMultiplyDivide()) );
    }
    else if (reader.consume(BossTokenType.SYMBOL_MINUS))
    {
      return parseAddSubtract( cmdData.createCmd(t,BossCmdData.SUBTRACT,left,parseMultiplyDivide()) );
    }
    else
    {
      return left;
    }
  }

  public int parseMultiplyDivide()
  {
    return parseMultiplyDivide( parseTerm() );
  }

  public int parseMultiplyDivide( int left )
  {
    int t = reader.peek();
    if (reader.consume(BossTokenType.SYMBOL_ASTERISK))
    {
      return parseAddSubtract( cmdData.createCmd(t,BossCmdData.MULTIPLY,left,parseTerm()) );
    }
    else if (reader.consume(BossTokenType.SYMBOL_SLASH))
    {
      return parseAddSubtract( cmdData.createCmd(t,BossCmdData.DIVIDE,left,parseTerm()) );
    }
    else
    {
      return left;
    }
  }

  public int parseTerm()
  {
    int t = reader.read();
    switch (tokenData.type(t))
    {
      case BossTokenType.INT32:
        return cmdData.createCmd( t, BossCmdData.INT32, tokenData.content(t) );
      case BossTokenType.KEYWORD_PRINTLN:
        return cmdData.createCmd( t, BossCmdData.PRINTLN, parseArgs() );
      default:
        throw tokenData.error( t, "Syntax error - unexpected " + tokenData.quotedName(t) + "." );
    }
  }

  public int parseArgs()
  {
    if (nextIsEndOfCommand()) return 0;

    int t = peek();
    int result = cmdData.createCmd( t, BossCmdData.ARGS );
    int tail   = result;
    boolean hasParens = consume( BossTokenType.SYMBOL_OPEN_PAREN );
    if (hasParens && consume(BossTokenType.SYMBOL_CLOSE_PAREN)) return result;

    boolean isFirst = true;
    while (isFirst || consume(BossTokenType.SYMBOL_COMMA))
    {
      isFirst = false;
      int expr = parseExpression();
      cmdData.data.set( tail+BossCmdData.NEXT, expr );
      tail = expr;
    }

    if (hasParens) mustConsume( BossTokenType.SYMBOL_CLOSE_PAREN );
    return result;
  }

  public int peek()
  {
    return reader.peek();
  }
  */
}

