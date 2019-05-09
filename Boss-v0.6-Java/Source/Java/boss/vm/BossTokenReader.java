package boss.vm;

public class BossTokenReader
{
  public BossParser    parser;
  public BossTokenData tokenData;
  public int[]         tokens;
  public int           count;
  public int           position;
  public int           tEOI = -1;

  public BossTokenReader( BossParser parser, BossInt32List tokens )
  {
    this.parser    = parser;
    this.tokenData = parser.vm.tokenData;
    this.tokens    = tokens.data;
    this.count     = tokens.count;
  }

  public boolean consume( int tokenType )
  {
    if (tokenData.type(peek()) != tokenType) return false;
    read();
    return true;
  }

  public boolean consumeEOLs()
  {
    boolean consumedAny = false;
    while (consume(BossTokenType.EOL)) consumedAny = true;
    return consumedAny;
  }

  public int eoi()
  {
    if (tEOI != -1) return tEOI;
    if (count > 0)
    {
      int t = tokens[ count-1 ];
      tokenData.setPosition( tokenData.filepath(t), tokenData.line(t), tokenData.column(t) );
    }
    else
    {
      tokenData.setPosition( parser.filepath, 1, 1 );
    }
    tEOI = tokenData.createToken( BossTokenType.EOI );
    return tEOI;
  }

  public boolean hasAnother()
  {
    return (position < count);
  }

  public void mustConsume( int tokenType )
  {
    if (consume(tokenType)) return;
    throw tokenData.error( peek(), tokenData.quotedTypeName(tokenType) + " expected; found " + tokenData.quotedName(peek()) );
  }

  public boolean nextIs( int tokenType )
  {
    return (tokenData.type(peek()) == tokenType);
  }

  public int peek()
  {
    if (position == count) return eoi();
    return tokens[ position ];
  }

  public int peek( int lookahead )
  {
    if (position+lookahead >= count) return eoi();
    return tokens[ position+lookahead ];
  }

  public int read()
  {
    if (position == count)
    {
      throw tokenData.error( eoi(), "Unexpected end of file." );
    }
    else
    {
      return tokens[ position++ ];
    }
  }
}

