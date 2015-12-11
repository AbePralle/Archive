import java.util.*;

class BossParser
{
  public BossVM vm;

  public BossTokenReader reader;

  public BossParser( BossVM vm )
  {
    this.vm = vm;
  }

  public void parse( String filepath )
  {
    BossScanner scanner = new BossScanner( vm );
    reader = new BossTokenReader( scanner.tokenize(filepath) );
  }

  public BossCode parse_expression()
  {
    return parse_logical_or();
  }

  public BossCode parse_logical_or()
  {
    return parse_logical_or( parse_logical_and() );
  }

  public BossCode parse_logical_or( BossCode lhs )
  {
    return lhs;
  }

  public BossCode parse_logical_and()
  {
    return parse_logical_and( parse_comparison() );
  }

  public BossCode parse_logical_and( BossCode lhs )
  {
    return lhs;
  }

  public BossCode parse_comparison()
  {
    return parse_comparison( parse_bitwise_xor() );
  }

  public BossCode parse_comparison( BossCode lhs )
  {
    return lhs;
  }

  public BossCode parse_bitwise_xor()
  {
    return parse_bitwise_xor( parse_bitwise_or() );
  }

  public BossCode parse_bitwise_xor( BossCode lhs )
  {
    return lhs;
  }

  public BossCode parse_bitwise_or()
  {
    return parse_bitwise_or( parse_bitwise_and() );
  }

  public BossCode parse_bitwise_or( BossCode lhs )
  {
    return lhs;
  }

  public BossCode parse_bitwise_and()
  {
    return parse_bitwise_and( parse_shift() );
  }

  public BossCode parse_bitwise_and( BossCode lhs )
  {
    return lhs;
  }

  public BossCode parse_shift()
  {
    return parse_shift( parse_plus_minus() );
  }

  public BossCode parse_shift( BossCode lhs )
  {
    return lhs;
  }

  public BossCode parse_plus_minus()
  {
    return parse_plus_minus( parse_times_divide_mod() );
  }

  public BossCode parse_plus_minus( BossCode lhs )
  {
    BossToken t = peek();

    if (consume(BossToken.symbol_plus)) return parse_plus_minus( new BossCode.Add().init(t,lhs,parse_times_divide_mod()) );
    
    return lhs;
  }

  public BossCode parse_times_divide_mod()
  {
    return parse_times_divide_mod( parse_power() );
  }

  public BossCode parse_times_divide_mod( BossCode lhs )
  {
    BossToken t = peek();

    if (consume(BossToken.symbol_times)) return parse_times_divide_mod( new BossCode.Multiply().init(t,lhs,parse_power()) );

    return lhs;
  }

  public BossCode parse_power()
  {
    return parse_power( parse_post_unary() );
  }

  public BossCode parse_power( BossCode lhs )
  {
    return lhs;
  }

  public BossCode parse_post_unary()
  {
    return parse_pre_unary();
  }

  public BossCode parse_pre_unary()
  {
    return parse_context_access();
  }

  public BossCode parse_context_access()
  {
    return parse_context_access( parse_term() );
  }

  public BossCode parse_context_access( BossCode context )
  {
    BossToken t = peek();

    if (consume(BossToken.symbol_period))
    {
      return parse_context_access( parse_access(t,context) );
    }
    return context;
  }

  public BossCode parse_access( BossToken t, BossCode context )
  {
    BossCode.Access access = new BossCode.Access( t, context, read_identifier() );
    if (consume(BossToken.symbol_open_paren))
    {
      access.args = new BossCode.Args();
      if ( !consume(BossToken.symbol_close_paren) )
      {
        access.args.add( parse_expression() );
        while (consume(BossToken.symbol_comma))
        {
          access.args.add( parse_expression() );
        }
        must_consume( BossToken.symbol_close_paren );
      }
    }
    return access;
  }

  public BossCode parse_term()
  {
    BossToken t = peek();

    if (consume(BossToken.command))
    {
      return ((BossToken.Command)t).cmd.parse( this, t );
    }
    else if (consume(BossToken.literal_String))
    {
      return new BossCode.LiteralString( t, ((BossToken.LiteralString)t).value );
    }
    else if (consume(BossToken.literal_Integer))
    {
      return new BossCode.LiteralInteger( t, ((BossToken.LiteralInteger)t).value );
    }

    throw t.error( "Syntax error: unexpected '" + t + "'." );
  }

  public BossToken peek() { return reader.peek(); }
  public BossToken read() { return reader.read(); }

  public boolean consume( int token_type )
  {
    if (reader.peek().type != token_type) return false;
    reader.read();
    return true;
  }

  public void must_consume( int token_type )
  {
    if (consume(token_type)) return;

    BossToken t = peek();
    throw t.error( BossToken.get_name(t.type) );
  }

  public String read_identifier()
  {
    BossToken t = reader.read();
    if (t.type != BossToken.identifier) throw t.error( "Identifier expected." );
    return ((BossToken.Identifier)t).value;
  }
}

