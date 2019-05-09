package boss.vm;

import java.io.*;

public class BossTokenizer
{
  public BossVM             vm;
  public BossTokenData      tokenData;
  public BossSymbolLookup   symbolLookup;
  public BossStringIntTable keywordLookup;
  public String             filepath;
  public BossScanner        scanner;
  public BossInt32List      tokens;
  public BossStringBuilder  buffer = new BossStringBuilder();

  public BossTokenizer( BossVM vm )
  {
    this.vm = vm;
    this.tokenData = vm.tokenData;
    this.symbolLookup = vm.symbolLookup;
    this.keywordLookup = vm.keywordLookup;
  }

  public boolean peekIsNumber( int lookahead )
  {
    char ch = scanner.peek( lookahead );
    return (ch >= '0' && ch <= '9');
  }

  public int[] tokenize( String filepath, String content )
  {
    this.filepath = filepath;
    scanner = new BossScanner( filepath, content );
    tokens = new BossInt32List( scanner.count / 4 );
    while (tokenizeAnother()) {}
    return tokens.toArray();
  }

  public int[] tokenize( File file )
  {
    this.filepath = file.getAbsolutePath();
    scanner = new BossScanner( file );
    tokens = new BossInt32List( scanner.count / 4 );
    while (tokenizeAnother()) {}
    return tokens.toArray();
  }

  public boolean tokenizeAnother()
  {
    while (scanner.consume(' ') || scanner.consume('\t')) {}
    tokenData.setPosition( filepath, scanner.line, scanner.column );
    if ( !scanner.hasAnother(1) ) return false;

    char ch = scanner.peek(0);
    if (ch == '\n')
    {
      scanner.read();
      tokens.add( tokenData.createToken(BossTokenType.EOL) );
      return true;
    }

    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_')
    {
      scanIdentifier( buffer );
      int entry = keywordLookup.find( buffer );
      if (entry > 0)
      {
        tokens.add( tokenData.createToken(keywordLookup.data.get(entry+BossStringIntTable.VALUE_OFFSET) ) );
      }
      else
      {
        tokens.add( tokenData.createToken(BossTokenType.IDENTIFIER,buffer) );
      }
      return true;
    }

    if ((ch >= '0' && ch <= '9') || (ch == '.' && peekIsNumber(1)))
    {
      tokenizeNumber();
      return true;
    }

    switch (ch)
    {
      case '#':
        discardComment();
        return true;

      case '"':
        tokenizeStringOrCharacter( '"' );
        return true;

      case '-':
        {
          if (peekIsNumber(1) || (scanner.peek(1)=='.' && peekIsNumber(2)))
          {
            tokenizeNumber();
            return true;
          }
          break;
        }

      case '\'':
        if (scanner.peek(1) == '\'')
        {
          tokenizeAlternateString();
        }
        else
        {
          tokenizeStringOrCharacter( '\'' );
        }
        return true;

    }

    tokenizeSymbol( ch );
    return true;
  }

  public void discardComment()
  {
    buffer.clear();
    scanner.read();  // '#'
    if (scanner.consume((char)123))  // '{'
    {
      int nesting_count = 1;
      while (scanner.hasAnother(1))
      {
        char ch = scanner.read();
        switch (ch)
        {
          case '#':
          {
            buffer.print('#');
            if (scanner.consume('{'))
            {
              buffer.print('{');
              ++nesting_count;
            }
            continue;
          }

          case '}':
          {
            if (scanner.consume('#'))
            {
              --nesting_count;
              if (nesting_count == 0)
              {
                break;
              }
              else
              {
                buffer.print( "}#" );
              }
            }
            else
            {
              buffer.print('}');
            }
            continue;
          }

          default:
            buffer.print( ch );
            continue;
        }
        break;
      }
    }
    else
    {
      while (scanner.hasAnother(1) && scanner.peek(0) != '\n') buffer.print( scanner.read() );
    }
  }

  public void scanIdentifier( BossStringBuilder buffer )
  {
    buffer.clear();

    char ch = scanner.peek(0);
    while ((ch>='a' && ch<='z') || (ch>='A' && ch<='Z') || (ch>='0' && ch<='9') || ch=='_')
    {
      buffer.print( scanner.read() );
      ch = scanner.peek(0);
    }
  }

  public void tokenizeNumber()
  {
    boolean isNegative = scanner.consume('-');

    int i = 0;
    if (scanner.hasAnother(i+1) && peekIsNumber(i))
    {
      ++i;
      while (scanner.hasAnother(i+1) && (peekIsNumber(i) || scanner.peek(i) == '_')) ++i;
    }
    char ch = scanner.peek(i);
    boolean isReal = (ch == '.' || ch == 'e' || ch == 'E');

    if (isReal)
    {
      // We have a real number
      double n = scanReal();
      ch = scanner.peek(0);

      if (ch == '.')
      {
        ch = scanner.peek(1);
        if (ch >= '0' && ch <= '9')
        {
          scanner.read();
          int underscoreCount = 0;
          i = 0;
          while (scanner.hasAnother(i+1) && (peekIsNumber(i)) || scanner.peek(i) == '_')
          {
            if (scanner.peek(i) == '_') ++underscoreCount;
            ++i;
          }
          int startPos = scanner.position;
          double fraction = scanReal();
          n += fraction / Math.pow(10.0,((scanner.position - startPos) - underscoreCount));
        }
        else if (ch == '.')
        {
          // Start of range
          if (isNegative) n = -n;
          tokens.add( tokenData.createToken( BossTokenType.INT32, (int)(long)n ) );
          return;
        }
        else if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_')
        {
          // E.g. 5.hash_code is (5).hash_code, not 5.0hash_code
          tokens.add( tokenData.createToken( BossTokenType.INT32, (int)(long)n ) );
          return;
        }
        else
        {
          if (isNegative) n = -n;
          if (n == (int)(long)n) tokens.add( tokenData.createToken( BossTokenType.REAL64_AS_INT32, n ) );
          else                   tokens.add( tokenData.createToken( BossTokenType.REAL64, n ) );
          return;
        }
      }

      if (scanner.consume('E') || scanner.consume('e'))
      {
        boolean isNegativeExponent = scanner.consume('-');
        if ( !isNegativeExponent ) scanner.consume('+');
        double power = scanReal();
        if (isNegativeExponent) n /= Math.pow(10.0,power);
        else                    n *= Math.pow(10.0,power);
      }

      if (isNegative) n = -n;
      if (n == (int)(long)n) tokens.add( tokenData.createToken( BossTokenType.REAL64_AS_INT32, n ) );
      else                   tokens.add( tokenData.createToken( BossTokenType.REAL64, n ) );
    }
    else
    {
      // Integer; make Int32 if possible, else Real64
      long n = scanLong();
      if (isNegative) n = -n;

      if (n == (int)n)
      {
        tokens.add( tokenData.createToken( BossTokenType.INT32, (int)n ) );
      }
      else
      {
        tokens.add( tokenData.createToken( BossTokenType.REAL64, n ) );
      }
    }
  }

  public void tokenizeStringOrCharacter( char terminator )
  {
    buffer.clear();
    scanner.read();  // leading quote

    while (scanner.hasAnother(1))
    {
      char ch = scanner.peek(0);
      if (ch == terminator)
      {
        scanner.read();
        if (terminator == '\'' && buffer.count == 1)
        {
          // Character token
          tokens.add( tokenData.createToken(BossTokenType.CHARACTER,buffer.charAt(0)) );
          return;
        }
        else
        {
          // String token
          tokens.add( tokenData.createToken(BossTokenType.STRING,buffer.toString()) );
        }
        return;
      }
      else
      {
        scanCharacterToBuffer();
      }
    }

    throw scanner.error( "End of input reached while looking for end of string." );
  }

  public void tokenizeAlternateString()
  {
    buffer.clear();
    scanner.read();  // leading quotes
    scanner.read();

    while (scanner.hasAnother(1))
    {
      char ch = scanner.peek(0);
      if (ch == '\'' && scanner.peek(1) == '\'')
      {
        scanner.read();
        scanner.read();
        tokens.add( tokenData.createToken(BossTokenType.STRING,buffer.toString()) );
        return;
      }
      else
      {
        scanCharacterToBuffer();
      }
    }

    throw scanner.error( "End of input reached while looking for end of string." );
  }

  public void scanCharacterToBuffer()
  {
    if ( !scanner.hasAnother(1) ) throw scanner.error( "Character expected." );

    char ch = scanner.peek(0);
    if (ch == '\n') throw scanner.error( "Character expected; found end of line." );

    if (ch == '\\')
    {
      scanner.read();
      if ( !scanner.hasAnother(1) ) throw scanner.error( "Escaped character expected; found end of input." );

      int value = 0;
      if      (scanner.consume('b'))  value = 8;
      else if (scanner.consume('e'))  value = 27;
      else if (scanner.consume('f'))  value = 12;
      else if (scanner.consume('n'))  value = '\n';
      else if (scanner.consume('r'))  value = '\r';
      else if (scanner.consume('t'))  value = '\t';
      else if (scanner.consume('v'))  value = 11;
      else if (scanner.consume('0'))  value = '\0';
      else if (scanner.consume('/'))  value = '/';
      else if (scanner.consume('?'))  value = '?';
      else if (scanner.consume('\'')) value = '\'';
      else if (scanner.consume('\\')) value = '\\';
      else if (scanner.consume('"'))  value = '"';
      else if (scanner.consume('x'))  value = scanHexValue(2);
      else if (scanner.consume('u'))  value = scanHexValue(4);
      else if (scanner.consume('['))  { scanHexSequenceToBuffer(); return; }
      else throw scanner.error( "Invalid escape sequence.  Supported: \\b \\e \\f \\n \\r \\t \\v \\0 \\? \\/ \\' \\\\ \\\" \\" + "xHH \\" + "uHHHH \\" + "[H*]." );

      buffer.print( (char) value );
    }
    else
    {
      buffer.print( scanner.read() );
    }
  }

  public int scanHexValue( int digits )
  {
    int value = 0;
    int i = 1;
    while (i <= digits || digits == -1)
    {
      if ( !scanner.hasAnother(1) ) throw scanner.error( "Hex value expected; found end of file." );
      if ( !nextIsHexDigit() )
      {
        if (digits == -1) return value;
        char ch = scanner.peek(0);
        buffer.clear();
        buffer.print( "Invalid hex digit " );
        if (ch < ' ' || ch == 127) buffer.print( (int) ch );
        else buffer.print( "'" + ch + "'" );
        buffer.print('.');
        throw scanner.error( buffer.toString() );
      }
      int intval = characterToNumber( scanner.read() );
      value = (value << 4) + intval;
      ++i;
    }
    return value;
  }

  public int characterToNumber( char ch )
  {
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'a' && ch <= 'f') return 10 + (ch - 'a');
    if (ch >= 'A' && ch <= 'F') return 10 + (ch - 'A');
    return 0;
  }

  public boolean nextIsHexDigit()
  {
    char ch = scanner.peek(0);
    return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
  }

  public void scanHexSequenceToBuffer()
  {
    boolean first = true;
    while (first || scanner.consume(','))
    {
      first = false;
      int ch = scanHexValue( -1 );
      if (ch >= 0x10000 && ch <= 0x10FFFF)
      {
        // Create surrogate pair to encoded supplementary unicode character
        buffer.print( (char) (0xD800 | ((ch >> 10) & 0x3FF)) );
        buffer.print( (char) (0xDC00 | (ch & 0x3FF)) );
      }
      else
      {
        buffer.print( (char) ch );
      }
    }

    if (scanner.consume(']')) return;
    throw scanner.error( "Closing ']' expected." );
  }

  public void tokenizeSymbol( int ch )
  {
    BossSymbolLookup symbolLookup = this.symbolLookup;
    ch &= 127;
    int start = symbolLookup.firstSymbol[ ch ];
    if (start != -1)
    {
      int count = symbolLookup.symbolCount[ ch ];
      for (int i=0; i<count; ++i)
      {
        if (scanner.consume(symbolLookup.symbols[start+i]))
        {
          tokens.add( tokenData.createToken(symbolLookup.tokenTypes[start+i]) );
          return;
        }
      }
    }

    throw scanner.error( "Syntax error - unexpected '" + scanner.peek(0) + "'." );
  }

  public double scanReal()
  {
    double n = 0.0;
    boolean allowUnderscore = false;
    char ch = scanner.peek(0);
    while ((ch >= '0' && ch <= '9') || (allowUnderscore && ch == '_'))
    {
      if (ch == '_')
      {
        scanner.read();
      }
      else
      {
        int intval = scanner.read() - '0';
        n = n * 10 + intval;
      }
      ch = scanner.peek(0);
      allowUnderscore = true;
    }
    return n;
  }

  public long scanLong()
  {
    long n = 0;
    boolean allowUnderscore = false;
    char ch = scanner.peek(0);
    while ((ch >= '0' && ch <= '9') || (allowUnderscore && ch == '_'))
    {
      if (ch == '_')
      {
        scanner.read();
      }
      else
      {
        int intval = scanner.read() - '0';
        n = n * 10 + intval;
      }
      ch = scanner.peek(0);
      allowUnderscore = true;
    }
    return n;
  }
}

