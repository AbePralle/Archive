package boss.vm;


public class BossScanner
{
  /*
  StringBuilder parse_buffer = new StringBuilder();
  HashMap<String,String> consolidation_table = new HashMap<String,String>();

  StringBuilder data;
  int count;
  int position;

  BossScanner( String source )
  {
    count = source.length();
    data = new StringBuilder( count );
    if (count >= 1 && source.charAt(0) == 0xFEFF)
    {
      // Discard Byte Order Mark (BOM)
      for (int i=1; i<count; ++i)
      {
        data.append( source.charAt(i) );
      }
    }
    else if (count >= 3 && source.charAt(0) == 0xEF && source.charAt(1) == 0xBB && source.charAt(2) == 0xBF)
    {
      // Discard Byte Order Mark (BOM)
      for (int i=3; i<count; ++i)
      {
        data.append( source.charAt(i) );
      }
    }
    else
    {
      data.append( source );
    }
  }

  BossScanner( File file )
  {
    try
    {
      count = (int) file.length();
      data = new StringBuilder( count );
      BufferedReader reader = new BufferedReader( new InputStreamReader( new FileInputStream(file.getPath()) ), 1024 );

      int firstCh = reader.read();
      if (firstCh != -1 && firstCh != 0xFEFF)
      {
        // Discard Byte Order Mark (BOM)
        data.append( (char) firstCh );
      }

      for (int ch=reader.read(); ch!=-1; ch=reader.read())
      {
        data.append( (char) ch );
      }
      count = data.length();
      reader.close();
    }
    catch (Exception err)
    {
      data = new StringBuilder();
    }
  }

  String consolidate( String st )
  {
    String consolidated = consolidation_table.get( st );
    if (consolidated != null) return consolidated;
    consolidation_table.put( st, st );
    return st;
  }

  boolean consume( char ch )
  {
    if (position == count) return false;
    if (ch != data.charAt(position)) return false;
    ++position;
    return true;
  }

  boolean consumeEOLs()
  {
    boolean consumed_any = false;
    while (consume('\n') || consume('\r')) consumed_any = true;
    return consumed_any;
  }

  boolean consumeSpaces()
  {
    boolean consumed_any = false;
    while (consume(' ') || consume('\t')) consumed_any = true;
    return consumed_any;
  }

  void consumeSpacesAndEOLs()
  {
    while (consumeSpaces() || consumeEOLs()) {}
  }

  boolean hasAnother()
  {
    return position < count;
  }

  JValue parseValue()
  {
    consumeSpacesAndEOLs();

    if ( !hasAnother() ) return UndefinedValue.singleton;

    char ch = peek();
    if (ch == '{') return parseTable();
    if (ch == '[') return parseList();

    if (ch == '-')              return parseNumber();
    if (ch >= '0' && ch <= '9') return parseNumber();

    if (ch == '"' || ch == '\'')
    {
      String result = parseString();
      if (result.length() == 0) return StringValue.empty_singleton;

      char first_ch = result.charAt( 0 );
      if (first_ch == 't' && result.equals("true"))  return LogicalValue.true_singleton;
      if (first_ch == 'f' && result.equals("false")) return LogicalValue.false_singleton;
      if (first_ch == 'n' && result.equals("null"))  return NullValue.singleton;

      return new StringValue( result );
    }
    else if (nextIsIdentifier())
    {
      String result = parseIdentifier();
      if (result.length() == 0) return StringValue.empty_singleton;

      char first_ch = result.charAt( 0 );
      if (first_ch == 't' && result.equals("true"))  return LogicalValue.true_singleton;
      if (first_ch == 'f' && result.equals("false")) return LogicalValue.false_singleton;
      if (first_ch == 'n' && result.equals("null"))  return NullValue.singleton;

      return new StringValue( result );
    }
    else
    {
      return UndefinedValue.singleton;
    }
  }

  JValue parseTable()
  {
    consumeSpacesAndEOLs();

    if ( !consume('{')) return UndefinedValue.singleton;

    consumeSpacesAndEOLs();

    JValue table = new TableValue();
    if (consume('}')) return table;

    int prev_pos = position;
    boolean first = true;
    while (first || consume(',') || (hasAnother() && peek()!='}' && position>prev_pos))
    {
      first = false;
      prev_pos = position;

      consumeSpacesAndEOLs();

      if (nextIsIdentifier())
      {
        String key = parseIdentifier();
        consumeSpacesAndEOLs();

        if (key.length() > 0)
        {
          if (consume(':'))
          {
            consumeSpacesAndEOLs();
            JValue value = parseValue();
            table.set( key, value );
          }
          else
          {
            table.set( key, JValue.logical(true) );
          }
          consumeSpacesAndEOLs();
        }
      }
    }

    if ( !consume((char)125)) throw new JSONParseError( "'}' expected." );

    return table;
  }

  JValue parseList()
  {
    consumeSpacesAndEOLs();

    if ( !consume('[')) return UndefinedValue.singleton;

    consumeSpacesAndEOLs();

    JValue list = new ListValue();
    if (consume(']')) return list;

    int prev_pos = position;
    boolean first = true;
    while (first || consume(',') || (hasAnother() && peek()!=']' && position>prev_pos))
    {
      first = false;
      prev_pos = position;
      consumeSpacesAndEOLs();
      if (peek() == ']') break;
      list.add( parseValue() );
      consumeSpacesAndEOLs();
    }

    if ( !consume(']')) throw new JSONParseError( "']' expected." );

    return list;
  }

  String parseString()
  {
    consumeSpacesAndEOLs();

    char terminator = '"';
    if      (consume( '"' ))  terminator = '"';
    else if (consume( '\'' )) terminator = '\'';

    if ( !hasAnother()) return "";

    StringBuilder buffer = parse_buffer;
    buffer.setLength( 0 );
    char ch = read();
    while (hasAnother() && ch != terminator)
    {
      if (ch == '\\')
      {
        ch = read();
        if (ch == 'b')     buffer.append( '\b' );
        else if (ch == 'f') buffer.append( '\f' );
        else if (ch == 'n') buffer.append( '\n' );
        else if (ch == 'r') buffer.append( '\r' );
        else if (ch == 't') buffer.append( '\t' );
        else if (ch == 'u') buffer.append( parseHexQuad() );
        else               buffer.append( ch );
      }
      else
      {
        buffer.append( ch );
      }
      ch = read();
    }

    return consolidate( buffer.toString() );
  }

  int hexCharacterToValue( char ch )
  {
    if (ch >= '0' && ch <= '9') return (ch - '0');
    if (ch >= 'A' && ch <= 'Z') return (ch - 'A') + 10;
    if (ch >= 'a' && ch <= 'z') return (ch - 'a') + 10;
    return 0;
  }

  char parseHexQuad()
  {
    int code = 0;
    for (int i=1; i<=4; ++i)
    {
      if (hasAnother())
      {
        code = (code << 4) | hexCharacterToValue( read() );
      }
    }
    return (char) code;
  }

  boolean isIdentifierStart( char ch )
  {
    return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || ch == '_';
  }

  boolean isIdentifierContinuation( char ch )
  {
    return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || ch == '_';
  }

  String parseIdentifier()
  {
    consumeSpacesAndEOLs();

    char ch = peek();
    if (ch == '"' || ch == '\'')
    {
      return parseString();
    }
    else
    {
      StringBuilder buffer = parse_buffer;
      buffer.setLength( 0 );
      boolean finished = false;
      while ( !finished && hasAnother() )
      {
        if (isIdentifierStart(ch))
        {
          read();
          buffer.append( ch );
          ch = peek();
        }
        else
        {
          finished = true;
        }
      }

      if (buffer.length() == 0) throw new JSONParseError( "Identifier expected." );
      return consolidate( buffer.toString() );
    }
  }

  boolean nextIsIdentifier()
  {
    char ch = peek();
    if (isIdentifierContinuation(ch)) return true;
    return (ch == '"' || ch == '\'');
  }

  JValue parseNumber()
  {
    consumeSpacesAndEOLs();

    double sign = 1.0;
    if (consume( '-' ))
    {
      sign = -1.0;
      consumeSpacesAndEOLs();
    }

    double n = 0.0;
    char ch = peek();
    while (hasAnother() && ch >= '0' && ch <= '9')
    {
      read();
      n = n * 10 + (ch - '0');
      ch = peek();
    }

    boolean is_real = false;

    if (consume( '.' ))
    {
      is_real = true;
      double decimal = 0.0;
      double power = 0.0;
      ch = peek();
      while (hasAnother() && ch >= '0' && ch <= '9')
      {
        read();
        decimal = decimal * 10 + (ch - '0');
        power += 1.0;
        ch = peek();
      }
      n += decimal / (Math.pow(10.0,power));
    }

    if (consume( 'e' ) || consume( 'E' ))
    {
      boolean negexp = false;
      if ( !consume('+') && consume('-')) negexp = true;

      double power = 0.0;
      ch = peek();
      while (hasAnother() && ch >= '0' && ch <= '9')
      {
        read();
        power = power * 10.0 + (ch - '0');
        ch = peek();
      }

      if (negexp) n /= Math.pow(10,power);
      else        n *= Math.pow(10,power);
    }

    n = n * sign;

    return JValue.number( n );
  }

  char peek()
  {
    if (position == count) return (char) 0;
    return data.charAt( position );
  }

  char read()
  {
    return data.charAt( position++ );
  }
  */
}
