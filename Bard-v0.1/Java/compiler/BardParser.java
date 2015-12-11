package compiler;

import alphavm.BardOp;

import java.util.*;

public class BardParser
{
  public BardCompiler      bc;
  public BardModule module;
  public BardScanner scanner;
  public ArrayList<BardLocal>    locals = new ArrayList<BardLocal>();
  public ArrayList<BardProperty> properties = new ArrayList<BardProperty>();
  public boolean immediate_mode = false;
  public boolean parse_ids_as_strings = false;
  public BardType this_type;

  public BardParser( BardCompiler bc )
  {
    this.bc = bc;
    scanner = new BardScanner( bc );
  }

  public BardModule parse( String filepath )
  {
    module = new BardModule( bc, filepath );
    scanner.init( filepath );
    parse();
    return module;
  }

  public BardModule parse( String filepath, String data )
  {
    module = new BardModule( bc, filepath );
    scanner.init( filepath, data );
    parse();
    return module;
  }

  public void parse()
  {
    while (scanner.has_another())
    {
      parse_element();
      consume_eols();
    }
  }

  public boolean next_is( int token )
  {
    if ( !scanner.has_another() ) return false;
    return (scanner.peek() == token);
  }

  public boolean consume( int token )
  {
    if ( !scanner.has_another() ) return false;
    if (scanner.peek() != token) return false;
    scanner.read();
    return true;
  }

  public void must_consume( int token )
  {
    if (consume(token)) return;
    throw scanner.error( "'" + BardToken.get_name(token) + "' expected." );
  }

  public void must_consume( int token, String error_message )
  {
    if (consume(token)) return;
    throw scanner.error( error_message );
  }

  public void must_consume_end_command()
  {
    if (consume(BardToken.eol) || consume(BardToken.symbol_semicolon)) return;
    throw scanner.error( "End Of Line or ';' expected." );
  }

  public boolean consume( String identifier )
  {
    if (scanner.peek() != BardToken.identifier) return false;
    if (scanner.integer_value != bc.id_table.get_id(identifier)) return false;
    scanner.read();
    return true;
  }

  public String scan_identifier()
  {
    if (scanner.read() != BardToken.identifier) throw scanner.error( "Identifier expected." );
    return bc.id_table.get_name( (int) scanner.integer_value );
  }

  public void parse_element()
  {
    parse_ids_as_strings = false;

    consume_eols();
    if (consume(BardToken.symbol_class))
    {
      parse_type_definition( BardToken.symbol_endClass, 0 );
    }
    else if (consume(BardToken.symbol_object))
    {
      parse_type_definition( BardToken.symbol_endObject, BardQualifiers.is_singleton );
    }
    else if (consume(BardToken.symbol_function))
    {
      BardMethod m = parse_method_signature( bc.type_Global );
      if (consume(BardToken.eol))
      {
        parse_multi_line_statements( m.statements );
        must_consume(BardToken.symbol_endFunction);
      }
      else
      {
        parse_single_line_statements( m.statements );
      }
      bc.type_Global.add( m );
    }
    else
    {
      //immediate_mode = true;
      //parse_multi_line_statements( module.global_statements );
      //immediate_mode = false;
      throw scanner.error( "Expected 'class' or 'object' definition." );
    }
  }

  public void parse_type_definition( int terminator, int qualifiers )
  {
    String name;

    name = scan_identifier();
    while (consume(BardToken.symbol_empty_brackets)) name += "[]";

    this_type = module.define_type( name, qualifiers );

    if (consume(BardToken.symbol_colon))
    {
      this_type.base_types.clear();
      this_type.base_types.add( parse_type() );
      while (consume(BardToken.symbol_comma))
      {
        this_type.base_types.add( parse_type() );
      }
    }

    if (consume(BardToken.symbol_open_bracket))
    {
      boolean first = true;
      while (first || consume(BardToken.symbol_comma))
      {
        first = false;
        if (consume("built_in"))    this_type.qualifiers |= BardQualifiers.is_built_in;
        else if (consume("native")) this_type.qualifiers |= BardQualifiers.is_native;
        else                   break;
      }
      must_consume( BardToken.symbol_close_bracket );
    }

    if (consume(BardToken.symbol_semicolon)) return;

    must_consume( BardToken.eol );
    while (parse_section(this_type)) { }

    must_consume( terminator );
  }

  public boolean parse_section( BardType type_context )
  {
    consume_eols();
    if (consume(BardToken.symbol_PROPERTIES))
    {
      consume_eols();
      while (next_is(BardToken.identifier))
      {
        properties.clear();
        boolean first = true;
        while (first || consume(BardToken.symbol_comma))
        {
          consume_eols();
          first = false;
          String name = scan_identifier();
          BardCmd initial_value = null;
          if (consume(BardToken.symbol_equals)) 
          {
            consume_eols();
            initial_value = parse_expression();
          }
          properties.add( new BardProperty(type_context,name,initial_value) );
        }

        if (consume(BardToken.symbol_colon))
        {
          BardType type = parse_type();
          for (BardProperty p : properties)
          {
            p.type = type;
            type_context.add( p );
          }
        }
        else
        {
          for (BardProperty p : properties)
          {
            if (p.initial_value != null) p.type = p.initial_value.type();
            if (p.type == null) p.type = bc.type_Variant;
            type_context.add( p );
          }
        }

        consume_eols();
      }
      return true;
    }
    else if (consume(BardToken.symbol_METHODS))
    {
      consume_eols();
      while (consume(BardToken.symbol_method))
      {
        BardMethod m = parse_method_signature( type_context );
        parse_multi_line_statements( m.statements );
        consume_eols();

        if (m.return_type == null && m.name.equals("init"))
        {
          m.return_type = this_type;
          m.statements.add( new BardCmdReturn().init( new BardCmdThis(this_type).set_source_info(scanner) ).set_source_info(scanner) );
        }

        type_context.add( m );
      }
      return true;
    }
    return false;
  }

  public BardMethod parse_method_signature( BardType type_context )
  {
    BardType return_type = null;
    String method_name;

    method_name = scan_identifier();
    while (consume(BardToken.symbol_empty_brackets))
    {
      method_name += "[]";
    }

    BardMethod m = new BardMethod( type_context, method_name );

    // Parameter list
    if (consume(BardToken.symbol_open_paren))
    {
      boolean first = true;
      while (first || consume(BardToken.symbol_comma))
      {
        first = false;

        BardType type = null;
        String name   = scan_identifier();

        //BardCmd default_value = null;
        //if (consume(BardToken.symbol_equals))
        if (consume(BardToken.symbol_colon))
        {
          type = parse_type();
        }

        BardLocal v = new BardLocal( name, type );
        m.add_parameter( v );
      }

      must_consume( BardToken.symbol_close_paren );
    }

    if (consume(BardToken.symbol_period))
    {
      m.return_type = parse_type();
    }
    else
    {
      m.return_type = return_type;
    }

    if (consume(BardToken.symbol_open_bracket))
    {
      boolean first = true;
      while (first || consume(BardToken.symbol_comma))
      {
        first = false;
        if (consume("built_in"))     m.qualifiers |= BardQualifiers.is_built_in;
        else if (consume("native")) m.qualifiers |= BardQualifiers.is_native;
        else                        break;
      }
      must_consume( BardToken.symbol_close_bracket );
    }

    consume( BardToken.symbol_colon );  // optional ':'

    return m;
  }
  
  public BardType parse_type()
  {
    String name = scan_identifier();
    while (consume(BardToken.symbol_empty_brackets)) name += "[]";

    BardType result = bc.get_type_reference( name );
    if (result.primitive_type != null) return result.primitive_type;
    return result;
  }

  public void consume_eols()
  {
    while (consume(BardToken.eol) || consume(BardToken.symbol_semicolon))
    {
      // no action
    }
  }

  public boolean next_is_end_command()
  {
    if ( !scanner.has_another() ) return true;
    int t = scanner.peek();
    return (t == BardToken.eol || t == BardToken.symbol_semicolon);
  }

  public boolean next_is_statement()
  {
    if ( !scanner.has_another() ) return false;
    return (scanner.peek() > BardToken.symbol_last_non_statement);
  }

  public void parse_multi_line_statements( BardStatementList list )
  {
    consume_eols();
    while (next_is_statement())
    {
      parse_statement( list, true );
      while (consume(BardToken.eol) || consume(BardToken.symbol_semicolon))
      {
        // no action
      }
    }
  }

  public void parse_single_line_statements( BardStatementList list )
  {
    while (next_is_statement())
    {
      parse_statement( list, false );
      while (consume(BardToken.symbol_semicolon))
      {
        // no action
      }
    }
    must_consume( BardToken.eol );
  }

  public void parse_statement( BardStatementList list, boolean allow_control_statements )
  {
    if (consume("println"))
    {
      // to be removed later
      if (next_is(BardToken.eol))
      {
        list.add( new BardCmdPrintln().set_source_info(scanner) );
      }
      else
      {
        BardCmdPrintln result = (BardCmdPrintln) new BardCmdPrintln().set_source_info(scanner);
        list.add( result.init(parse_expression()) );
      }
      return;
    }

    if (allow_control_statements)
    {
      switch (scanner.peek())
      {
        case BardToken.symbol_if:
          parse_if( list );
          return;

        case BardToken.symbol_forEach:
          parse_forEach( list );
          return;

        case BardToken.symbol_loop:
          parse_loop( list );
          return;

        case BardToken.symbol_while:
          parse_while( list );
          return;

        case BardToken.symbol_local:
          parse_locals( list );
          return;
      }
    }

    switch (scanner.peek())
    {
      case BardToken.symbol_increment:
        scanner.read();
        list.add( new BardCmdAdjust( parse_expression(), 1 ).set_source_info(scanner) );
        return;

      case BardToken.symbol_decrement:
        scanner.read();
        list.add( new BardCmdAdjust( parse_expression(), -1 ).set_source_info(scanner) );
        return;

      case BardToken.symbol_return:
        parse_return( list );
        return;

      case BardToken.symbol_throw:
        parse_throw( list );
        return;

      case BardToken.symbol_trace:
        parse_trace( list );
        return;

      case BardToken.symbol_tron:
        scanner.read();
        list.add( new BardCmdStatement.Tron().set_source_info(scanner) );
        return;

      case BardToken.symbol_troff:
        scanner.read();
        list.add( new BardCmdStatement.Troff().set_source_info(scanner) );
        return;

      case BardToken.symbol_noAction:
        scanner.read();
        return;
    }

    BardCmd expression = parse_expression();

    switch (scanner.peek())
    {
      case BardToken.symbol_increment:
        scanner.read();
        list.add( new BardCmdAdjust( expression, 1 ).set_source_info(scanner) );
        return;

      case BardToken.symbol_decrement:
        scanner.read();
        list.add( new BardCmdAdjust( expression, -1 ).set_source_info(scanner) );
        return;

      case BardToken.symbol_equals:
        {
          scanner.read();
          BardCmd rhs = parse_expression();
          list.add( new BardCmdAssign( expression, rhs ) );
          return;
        }

      case BardToken.symbol_add_with_assign:
        {
          scanner.read();
          BardCmd new_value = new BardCmdBinaryOp.Add().init( expression.duplicate(), parse_expression() ).set_source_info(expression);
          list.add( new BardCmdAssign( expression, new_value ) );
          return;
        }

      case BardToken.symbol_subtract_with_assign:
        {
          scanner.read();
          BardCmd new_value = new BardCmdBinaryOp.Subtract().init( expression.duplicate(), parse_expression() ).set_source_info(expression);
          list.add( new BardCmdAssign( expression, new_value ) );
          return;
        }

      case BardToken.symbol_multiply_with_assign:
        {
          scanner.read();
          BardCmd new_value = new BardCmdBinaryOp.Multiply().init( expression.duplicate(), parse_expression() ).set_source_info(expression);
          list.add( new BardCmdAssign( expression, new_value ) );
          return;
        }

      case BardToken.symbol_divide_with_assign:
        {
          scanner.read();
          BardCmd new_value = new BardCmdBinaryOp.Divide().init( expression.duplicate(), parse_expression() ).set_source_info(expression);
          list.add( new BardCmdAssign( expression, new_value ) );
          return;
        }

      case BardToken.symbol_mod_with_assign:
        {
          scanner.read();
          BardCmd new_value = new BardCmdBinaryOp.Mod().init( expression.duplicate(), parse_expression() ).set_source_info(expression);
          list.add( new BardCmdAssign( expression, new_value ) );
          return;
        }
    }

    if ((expression instanceof BardCmdAccess) && scanner.has_another() && !next_is(BardToken.eol) && !next_is(BardToken.symbol_semicolon))
    {
      // Allow implicit parameters
      BardCmdAccess access = (BardCmdAccess) expression;
      BardArgs args = new BardArgs();
      while (scanner.has_another() && !next_is(BardToken.eol) && !next_is(BardToken.symbol_semicolon))
      {
        args.add( parse_expression() );
        consume( BardToken.symbol_comma );
      }
      access.args = args;
    }

    list.add( expression );
  }

  public void parse_if( BardStatementList list )
  {
    scanner.read();
    boolean has_parens = consume( BardToken.symbol_open_paren );
    BardCmdIf cmd_if = new BardCmdIf();
    cmd_if.init( parse_expression() );
    if (has_parens) must_consume( BardToken.symbol_close_paren );

    if (consume(BardToken.eol))
    {
      parse_multi_line_statements( cmd_if.body );
      consume_eols();

      while (consume(BardToken.symbol_elseIf))
      {
        BardCmdIf.ElseIf else_if = new BardCmdIf.ElseIf( parse_expression() );
        else_if.set_source_info( scanner );
        cmd_if.else_ifs.add( else_if );
        must_consume_end_command();
        parse_multi_line_statements( else_if.body );
        consume_eols();
      }

      if (consume(BardToken.symbol_else_eol))
      {
        must_consume_end_command();
        cmd_if.else_body = new BardStatementList();
        parse_multi_line_statements( cmd_if.else_body );
        consume_eols();
      }

      must_consume( BardToken.symbol_endIf );
    }
    else
    {
      parse_single_line_statements( cmd_if.body );

      while (next_is(BardToken.symbol_elseIf))
      {
        if (elseIf_has_eol_after_condition())
        {
          // Not a single-line elseIf
          break;
        }
        consume( BardToken.symbol_elseIf );
        must_consume( BardToken.symbol_open_paren );
        BardCmdIf.ElseIf else_if = new BardCmdIf.ElseIf( parse_expression() );
        else_if.set_source_info( scanner );
        must_consume( BardToken.symbol_close_paren );
        cmd_if.else_ifs.add( else_if );
        parse_single_line_statements( else_if.body );
        consume_eols();
      }

      if (consume(BardToken.symbol_else))
      {
        cmd_if.else_body = new BardStatementList();
        parse_single_line_statements( cmd_if.else_body );
        consume_eols();
      }

    }
    list.add( cmd_if );
  }

  public boolean elseIf_has_eol_after_condition()
  {
    // kludgy
    BardParseReader reader = scanner.reader;

    boolean found_open_paren = false;
    int num_ahead = 1;
    int paren_count = 0;
    char ch = reader.peek( num_ahead );
    while (ch != 0)
    {
      if (found_open_paren)
      {
        if (ch == '(')
        {
          ++paren_count;
        }
        else if (ch == ')')
        {
          if (--paren_count == 0)
          {
            while (reader.peek(++num_ahead) == ' ') { }
            if (reader.peek(num_ahead) == '#') return true;
            return (reader.peek(num_ahead) == '\n');
          }
        }
        else if (ch == '\n') return false;  // syntax error
      }
      else
      {
        if (ch == '(')
        {
          paren_count = 1;
          found_open_paren = true;
        }
        else if (ch == '\n') return false;  // syntax error
      }
      ch = reader.peek( ++num_ahead );
    }
    return false;
  }

  public void parse_forEach( BardStatementList list )
  {
    scanner.read();  // 'forEach'

    boolean has_parens = consume( BardToken.symbol_open_paren );
    String element_name = scan_identifier();

    BardCmdForEach cmd_forEach;
    if (consume(BardToken.symbol_in))
    {
      cmd_forEach = new BardCmdForEach.ForEachIn().init( element_name, parse_expression() );
    }
    else
    {
      must_consume( BardToken.symbol_of, "'in' or 'of' expected." );
      cmd_forEach = new BardCmdForEach.ForEachOf().init( element_name, parse_expression() );
    }
    cmd_forEach.set_source_info( scanner );
    if (has_parens) must_consume( BardToken.symbol_close_paren );

    if (consume(BardToken.eol))
    {
      parse_multi_line_statements( cmd_forEach.body );
      consume_eols();
      must_consume( BardToken.symbol_endForEach );
    }
    else
    {
      parse_single_line_statements( cmd_forEach.body );
    }
    list.add( cmd_forEach );
  }

  public void parse_loop( BardStatementList list )
  {
    scanner.read();
    BardCmdLoop cmd_loop = new BardCmdLoop();

    if ( !consume(BardToken.eol) ) cmd_loop.init( parse_expression() );

    parse_multi_line_statements( cmd_loop.body );
    consume_eols();
    must_consume( BardToken.symbol_endLoop );
    must_consume( BardToken.eol );

    list.add( cmd_loop );
  }

  public void parse_while( BardStatementList list )
  {
    scanner.read();
    BardCmdWhile cmd_while = new BardCmdWhile();
    cmd_while.init( parse_expression() );

    if (consume(BardToken.eol))
    {
      parse_multi_line_statements( cmd_while.body );
      consume_eols();
      must_consume( BardToken.symbol_endWhile );
      must_consume( BardToken.eol );
    }
    else
    {
      parse_single_line_statements( cmd_while.body );
    }
    list.add( cmd_while );
  }

  public void parse_locals( BardStatementList list )
  {
    if (immediate_mode) throw scanner.error( "Use 'global' instead of 'local' in globally-scoped statements." );

    scanner.read();
    locals.clear();
    boolean first = true;
    while (first || consume(BardToken.symbol_comma))
    {
      consume_eols();
      first = false;
      BardLocal v = new BardLocal( scan_identifier() );
      if (consume(BardToken.symbol_equals))
      {
        consume_eols();
        v.initial_value = parse_expression();
      }
      locals.add( v );
    }

    if (consume(BardToken.symbol_colon))
    {
      BardType type = parse_type();
      for (BardLocal v : locals)
      {
        v.type = type;
      }
    }

    for (BardLocal v : locals)
    {
      list.add( new BardCmdLocalDeclaration(v).set_source_info(scanner) ); // FIXME: info is wrong
      list.add( new BardCmdWriteLocal( v, v.initial_value ).set_source_info( scanner ) );
    }
  }

  public void parse_return( BardStatementList list )
  {
    scanner.read();
    BardCmdReturn cmd_return = (BardCmdReturn) new BardCmdReturn().set_source_info( scanner );
    if (next_is_end_command()) list.add( cmd_return );
    else                       list.add( cmd_return.init( parse_expression() ) );
  }

  public void parse_throw( BardStatementList list )
  {
    scanner.read(); // 'throw
    BardCmdThrow cmd_throw = (BardCmdThrow) new BardCmdThrow().set_source_info( scanner );
    list.add( cmd_throw.init( parse_expression() ) );
  }

  public void parse_trace( BardStatementList list )
  {
    consume( BardToken.symbol_trace );

    int pos1 = scanner.source_position();
    BardCmd result = null;
    while ( !next_is_end_command() )
    {
      BardCmd expr = parse_expression();

      if (expr.is_literal())
      {
        expr = new BardCmdCast( expr, bc.type_String ).set_source_info( expr );

        if (result == null) result = expr;
        else                result = new BardCmdBinaryOp.Add().init( result, expr ).set_source_info(expr);
      }
      else
      {
        int pos2 = scanner.source_position() - 1;
        String literal = scanner.source_between( pos1, pos2 );
//System.out.println( "LITERAL: " + literal );

        if (literal.indexOf('#') >= 0) literal = literal.substring( 0, literal.indexOf('#') );
        literal = literal.trim();
        while (literal.startsWith(",")) literal = literal.substring(1).trim();
        while (literal.endsWith(",")) literal = literal.substring(0,literal.length()-1).trim();
        literal += ":";
        BardCmd st = new BardCmdLiteral.LiteralString( literal ).set_source_info(expr);
        BardCmd term = new BardCmdBinaryOp.Add().init( st, expr ).set_source_info(expr);

        if (result == null) result = term;
        else                result = new BardCmdBinaryOp.Add().init( result, term ).set_source_info(expr);
      }

      if (consume(BardToken.symbol_comma))
      {
        BardCmd st = new BardCmdLiteral.LiteralString( ", " ).set_source_info(expr);

        if (result == null) result = st;
        else                result = new BardCmdBinaryOp.Add().init( result, st ).set_source_info(result);
      }

      pos1 = scanner.source_position();
    }

    list.add( new BardCmdPrintln().init( result ).set_source_info( result ) );
  }

  public BardCmd parse_expression()
  {
    return parse_logical_or();
  }

  public BardCmd parse_logical_or()
  {
    BardCmd cmd = parse_logical_and();
    return parse_logical_or( cmd );
  }

  public BardCmd parse_logical_or( BardCmd lhs )
  {
    if (consume(BardToken.symbol_or))
    {
      BardCmdCmp cmd = (BardCmdCmp) new BardCmdCmp.Or().set_source_info( scanner );
      return parse_logical_or( cmd.init(lhs,parse_logical_and()) );
    }
    return lhs;
  }

  public BardCmd parse_logical_and()
  {
    BardCmd cmd = parse_comparison();
    return parse_logical_and( cmd );
  }

  public BardCmd parse_logical_and( BardCmd lhs )
  {
    if (consume(BardToken.symbol_and))
    {
      BardCmdCmp cmd = (BardCmdCmp) new BardCmdCmp.And().set_source_info( scanner );
      return parse_logical_and( cmd.init(lhs,parse_comparison()) );
    }
    return lhs;
  }

  public BardCmd parse_comparison()
  {
    BardCmd cmd = parse_bitwise_xor();
    return parse_comparison( cmd );
  }

  public BardCmd parse_comparison( BardCmd lhs )
  {
    if (consume(BardToken.symbol_eq))
    {
      BardCmdCmp cmd = (BardCmdCmp) new BardCmdCmp.EQ().set_source_info( scanner );
      return parse_comparison( cmd.init(lhs,parse_bitwise_xor()) );
    }
    else if (consume(BardToken.symbol_ne))
    {
      BardCmdCmp cmd = (BardCmdCmp) new BardCmdCmp.NE().set_source_info( scanner );
      return parse_comparison( cmd.init(lhs,parse_bitwise_xor()) );
    }
    else if (consume(BardToken.symbol_ge))
    {
      BardCmdCmp cmd = (BardCmdCmp) new BardCmdCmp.GE().set_source_info( scanner );
      return parse_comparison( cmd.init(lhs,parse_bitwise_xor()) );
    }
    else if (consume(BardToken.symbol_le))
    {
      BardCmdCmp cmd = (BardCmdCmp) new BardCmdCmp.LE().set_source_info( scanner );
      return parse_comparison( cmd.init(lhs,parse_bitwise_xor()) );
    }
    else if (consume(BardToken.symbol_gt))
    {
      BardCmdCmp cmd = (BardCmdCmp) new BardCmdCmp.GT().set_source_info( scanner );
      return parse_comparison( cmd.init(lhs,parse_bitwise_xor()) );
    }
    else if (consume(BardToken.symbol_lt))
    {
      BardCmdCmp cmd = (BardCmdCmp) new BardCmdCmp.LT().set_source_info( scanner );
      return parse_comparison( cmd.init(lhs,parse_bitwise_xor()) );
    }
    else if (consume(BardToken.symbol_is))
    {
      BardCmdCmp cmd = (BardCmdCmp) new BardCmdCmp.Is().set_source_info( scanner );
      return parse_comparison( cmd.init(lhs,parse_bitwise_xor()) );
    }
    else if (consume(BardToken.symbol_instanceOf))
    {
      BardCmdTypeCheck.InstanceOf result = new BardCmdTypeCheck.InstanceOf();
      result.set_source_info( scanner );
      result.init( lhs, parse_type() );
      return result;
    }
    else if (consume(BardToken.symbol_as))
    {
      BardCmdCast.As result = new BardCmdCast.As( lhs );
      result.set_source_info( scanner );
      result.to_type = parse_type();
      return result;
    }
    return lhs;
  }

  public BardCmd parse_bitwise_xor()
  {
    BardCmd cmd = parse_bitwise_or();
    return parse_bitwise_xor( cmd );
  }

  public BardCmd parse_bitwise_xor( BardCmd lhs )
  {
    if (consume(BardToken.symbol_bitwise_xor))
    {
      BardCmdBinaryOp cmd = new BardCmdBinaryOp.BitwiseXor();
      cmd.set_source_info( scanner );
      return parse_bitwise_xor( cmd.init(lhs,parse_bitwise_or()) );
    }
    return lhs;
  }

  public BardCmd parse_bitwise_or()
  {
    BardCmd cmd = parse_bitwise_and();
    return parse_bitwise_or( cmd );
  }

  public BardCmd parse_bitwise_or( BardCmd lhs )
  {
    if (consume(BardToken.symbol_bitwise_or))
    {
      BardCmdBinaryOp cmd = new BardCmdBinaryOp.BitwiseOr();
      cmd.set_source_info( scanner );
      return parse_bitwise_or( cmd.init(lhs,parse_bitwise_and()) );
    }
    return lhs;
  }

  public BardCmd parse_bitwise_and()
  {
    BardCmd cmd = parse_shift();
    return parse_bitwise_and( cmd );
  }

  public BardCmd parse_bitwise_and( BardCmd lhs )
  {
    if (consume(BardToken.symbol_ampersand))
    {
      BardCmdBinaryOp cmd = new BardCmdBinaryOp.BitwiseAnd();
      cmd.set_source_info( scanner );
      return parse_bitwise_and( cmd.init(lhs,parse_shift()) );
    }
    return lhs;
  }

  public BardCmd parse_shift()
  {
    BardCmd cmd = parse_plus_minus();
    return parse_shift( cmd );
  }

  public BardCmd parse_shift( BardCmd lhs )
  {
    if (consume(BardToken.symbol_shl))
    {
      BardCmdBinaryOp cmd = new BardCmdBinaryOp.SHL();
      cmd.set_source_info( scanner );
      return parse_shift( cmd.init(lhs,parse_plus_minus()) );
    }
    else if (consume(BardToken.symbol_shr))
    {
      BardCmdBinaryOp cmd = new BardCmdBinaryOp.SHR();
      cmd.set_source_info( scanner );
      return parse_shift( cmd.init(lhs,parse_plus_minus()) );
    }
    else if (consume(BardToken.symbol_shrx))
    {
      BardCmdBinaryOp cmd = new BardCmdBinaryOp.SHRX();
      cmd.set_source_info( scanner );
      return parse_shift( cmd.init(lhs,parse_plus_minus()) );
    }
    return lhs;
  }


  public BardCmd parse_plus_minus()
  {
    BardCmd cmd = parse_times_divide_mod();
    return parse_plus_minus( cmd );
  }

  public BardCmd parse_plus_minus( BardCmd lhs )
  {
    if (consume(BardToken.symbol_plus))
    {
      BardCmdBinaryOp cmd_add = new BardCmdBinaryOp.Add();
      cmd_add.set_source_info( scanner );
      return parse_plus_minus( cmd_add.init(lhs,parse_times_divide_mod()) );
    }
    else if (consume(BardToken.symbol_minus))
    {
      BardCmdBinaryOp cmd_subtract = new BardCmdBinaryOp.Subtract();
      cmd_subtract.set_source_info( scanner );
      return parse_plus_minus( cmd_subtract.init(lhs,parse_times_divide_mod()) );
    }
    return lhs;
  }

  public BardCmd parse_times_divide_mod()
  {
    BardCmd cmd = parse_power();
    return parse_times_divide_mod( cmd );
  }

  public BardCmd parse_times_divide_mod( BardCmd lhs )
  {
    if (consume(BardToken.symbol_times))
    {
      BardCmdBinaryOp cmd = (BardCmdBinaryOp) new BardCmdBinaryOp.Multiply().set_source_info( scanner );
      return parse_times_divide_mod( cmd.init(lhs,parse_power()) );
    }
    else if (consume(BardToken.symbol_divide))
    {
      BardCmdBinaryOp cmd = (BardCmdBinaryOp) new BardCmdBinaryOp.Divide().set_source_info( scanner );
      return parse_times_divide_mod( cmd.init(lhs,parse_power()) );
    }
    else if (consume(BardToken.symbol_percent))
    {
      BardCmdBinaryOp cmd = (BardCmdBinaryOp) new BardCmdBinaryOp.Mod().set_source_info( scanner );
      return parse_times_divide_mod( cmd.init(lhs,parse_power()) );
    }
    return lhs;
  }

  public BardCmd parse_power()
  {
    BardCmd cmd = parse_post_unary();
    return parse_power( cmd );
  }

  public BardCmd parse_power( BardCmd lhs )
  {
    if (consume(BardToken.symbol_power))
    {
      BardCmdBinaryOp cmd = (BardCmdBinaryOp) new BardCmdBinaryOp.Power().set_source_info( scanner );
      return parse_power( cmd.init(lhs,parse_post_unary()) );
    }
    return lhs;
  }

  public BardCmd parse_post_unary()
  {
    BardCmd operand = parse_pre_unary();

    boolean logicalize = false;
    while (consume(BardToken.symbol_question_mark)) logicalize = true;
    if (logicalize)
    {
      return new BardCmdUnaryOp.Logicalize().init(operand).set_source_info(scanner);
    }
    return operand;
  }


  public BardCmd parse_pre_unary()
  {
    /*
    if (consume(BardToken.symbol_bitwise_not))
    {
      return new BardCmdBitwiseNot( parse_pre_unary() );
    }
    else if (consume(BardToken.symbol_not))
    {
      return new BardCmdLogicalNot( parse_pre_unary() );
    }
    */
    if (consume(BardToken.symbol_minus))
    {
      BardCmdUnaryOp cmd = new BardCmdUnaryOp.Negate();
      cmd.set_source_info( scanner );
      return cmd.init( parse_pre_unary() );
    }
    else if (consume(BardToken.symbol_not))
    {
      BardCmdUnaryOp cmd = new BardCmdUnaryOp.Not();
      cmd.set_source_info( scanner );
      return cmd.init( parse_pre_unary() );
    }

    return parse_context_access();
  }

  public BardCmd parse_context_access()
  {
    return parse_context_access( parse_term() );
  }

  public BardCmd parse_context_access( BardCmd context )
  {
    if (consume(BardToken.symbol_period))
    {
      return parse_context_access( parse_access(context) );
    }
    else if (consume(BardToken.symbol_open_bracket))
    {
      BardArgs args = new BardArgs();
      BardCmdElementAccess result = new BardCmdElementAccess( context, args );
      result.set_source_info( scanner );

      consume_eols();
      args.add( parse_expression() );
      consume_eols();
      while (consume(BardToken.symbol_comma))
      {
        consume_eols();
        args.add( parse_expression() );
        consume_eols();
      }
      must_consume( BardToken.symbol_close_bracket );
      return parse_context_access( result );
    }
    return context;
  }

  public BardCmd parse_term()
  {
    int t = scanner.peek();

    switch (t)
    {
      case BardToken.identifier:
        if (parse_ids_as_strings) return new BardCmdLiteral.LiteralString( scan_identifier() ).set_source_info(scanner);
        return parse_access( null );

      case BardToken.symbol_this:
        scanner.read();
        return new BardCmdThis( this_type ).set_source_info( scanner );

      case BardToken.literal_Real:
        scanner.read();
        return new BardCmdLiteral.LiteralReal( scanner.real_value ).set_source_info(scanner);

      case BardToken.literal_Real_pi:
        scanner.read();
        return new BardCmdLiteral.LiteralReal( Math.PI ).set_source_info(scanner);

      case BardToken.literal_Integer:
        scanner.read();
        return new BardCmdLiteral.LiteralInteger( (int) scanner.integer_value ).set_source_info(scanner);

      case BardToken.literal_Character:
        scanner.read();
        return new BardCmdLiteral.LiteralCharacter( (int) scanner.integer_value ).set_source_info(scanner);

      case BardToken.literal_Logical_true:
        scanner.read();
        return new BardCmdLiteral.LiteralLogical( true ).set_source_info(scanner);

      case BardToken.literal_Logical_false:
        scanner.read();
        return new BardCmdLiteral.LiteralLogical( false ).set_source_info(scanner);

      case BardToken.literal_String:
        scanner.read();
        return new BardCmdLiteral.LiteralString( scanner.string_value.toString() ).set_source_info(scanner);

      case BardToken.symbol_open_paren:
        {
          scanner.read();
          BardCmd result = parse_expression();
          must_consume( BardToken.symbol_close_paren );
          return result;
        }

      case BardToken.symbol_empty_brackets:
        {
          scanner.read();
          return new BardCmdLiteral.LiteralList( new BardArgs() ).set_source_info( scanner );
        }

      case BardToken.symbol_open_bracket:
        {
          scanner.read();

          BardArgs args = new BardArgs();
          BardCmdLiteral.LiteralList result = new BardCmdLiteral.LiteralList( args );
          result.set_source_info( scanner );

          consume_eols();
          if ( !consume(BardToken.symbol_close_bracket) )
          {
            args.add( parse_expression() );
            consume_eols();
            while (consume(BardToken.symbol_comma))
            {
              consume_eols();
              args.add( parse_expression() );
              consume_eols();
            }
            must_consume( BardToken.symbol_close_bracket );
          }
          return result;
        }

      case BardToken.symbol_at:
        {
          scanner.read();
          switch (scanner.peek())
          {
            case BardToken.symbol_empty_braces:
              return parse_term();

            case BardToken.symbol_empty_brackets:
              return parse_term();

            case BardToken.symbol_open_brace:
            case BardToken.symbol_open_bracket:
              {
                parse_ids_as_strings = true;
                BardCmd result = parse_term();
                parse_ids_as_strings = false;
                return result;
              }

            default:
              throw scanner.error( "Syntax error." );
          }
        }

      case BardToken.symbol_empty_braces:
        {
          scanner.read();
          return new BardCmdLiteral.LiteralTable( new BardArgs(), new BardArgs() ).set_source_info( scanner );
        }

      case BardToken.symbol_open_brace:
        {
          scanner.read();  // '{'

          BardArgs keys   = new BardArgs();
          BardArgs values = new BardArgs();
          BardCmdLiteral.LiteralTable cmd_table = new BardCmdLiteral.LiteralTable( keys, values );
          cmd_table.set_source_info( scanner );

          consume_eols();
          if ( !consume(BardToken.symbol_close_brace) )
          {
            boolean first = true;
            while (first || consume(BardToken.symbol_comma))
            {
              first = false;
              consume_eols();
              keys.add( parse_expression() );
              consume_eols();
              must_consume( BardToken.symbol_colon );
              consume_eols();
              values.add( parse_expression() );
              consume_eols();
            }
            must_consume( BardToken.symbol_close_brace );
          }
          return cmd_table;
        }
      case BardToken.symbol_null:
        scanner.read();
        return new BardCmdLiteral.LiteralNull().set_source_info( scanner );
    }

    if (scanner.peek() == BardToken.identifier)
    {
      throw scanner.error( "Syntax error - unexpected '" + scanner.string_value + "'." );
    }
    else
    {
      throw scanner.error( "Syntax error - unexpected '" + BardToken.get_name(scanner.peek()) + "'." );
    }
  }

  public BardCmdAccess parse_access( BardCmd context )
  {
    String name = scan_identifier();
    BardArgs args = null;
    if (consume(BardToken.symbol_open_paren))
    {
      args = new BardArgs();

      consume_eols();
      if ( !consume(BardToken.symbol_close_paren) )
      {
        boolean first = true;
        while (first || consume(BardToken.symbol_comma))
        {
          first = false;
          consume_eols();
          args.add( parse_expression() );
          consume_eols();
        }

        consume_eols();
        must_consume( BardToken.symbol_close_paren );
      }
    }
    else if (consume(BardToken.symbol_empty_brackets))
    {
      name += "[]";
      args = new BardArgs();
    }

    BardCmdAccess result;
    if (context != null) result = new BardCmdContextAccess( context, name, args );
    else                 result = new BardCmdAccess( name, args );
    result.set_source_info( scanner );

    return result;
  }
}

