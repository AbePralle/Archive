package compiler;

import alphavm.BardOp;

import java.io.*;
import java.util.*;

// TODO:
// - methods/functions have filename, cmds have line number
// - statically typed

public class BardScanner
{
  public BardCompiler          bc;
  public BardParseReader reader = new BardParseReader();
  public BardIntTable    keywords = new BardIntTable();

  public BardFileInfo    file_info;
  public StringBuilder   string_value = new StringBuilder();
  public double          real_value;
  public long            integer_value;
  public int             line, column, position;
  public int             previous_line, previous_column;
  public int             prepped_token, prepped_position;
  public boolean         need_final_eol;

  public BardScanner( BardCompiler bc )
  {
    this.bc = bc;
  }

  public BardScanner( BardCompiler bc, String filename )
  {
    this.bc = bc;
    init( filename );
  }

  public BardScanner( BardCompiler bc, String filename, String data )
  {
    this.bc = bc;
    init( filename, data );
  }

  public void init( String filename )
  {
    this.file_info = new BardFileInfo( bc, filename );
    reader.init( filename );
    init();
  }

  public void init( String filename, String data )
  {
    this.file_info = new BardFileInfo( bc, filename );
    reader.init( filename, data );
    init();
  }

  public void init()
  {
    line = 1;
    column = 1;
    position = 0;
    previous_line = 1;
    previous_column = 1;
    prepped_token = -1;
    prepped_position = 0;
    need_final_eol = true;

    if (keywords.count == 0)
    {
      keywords.set( "and",         BardToken.symbol_and );
      keywords.set( "as",          BardToken.symbol_as );
      keywords.set( "class",       BardToken.symbol_class );
      keywords.set( "else",        BardToken.symbol_else );
      keywords.set( "elseIf",      BardToken.symbol_elseIf );
      keywords.set( "endClass",    BardToken.symbol_endClass );
      keywords.set( "endForEach",  BardToken.symbol_endForEach );
      keywords.set( "endFunction", BardToken.symbol_endFunction );
      keywords.set( "endIf",       BardToken.symbol_endIf );
      keywords.set( "endObject",   BardToken.symbol_endObject );
      keywords.set( "endLoop",     BardToken.symbol_endLoop );
      keywords.set( "endWhile",    BardToken.symbol_endWhile );
      keywords.set( "false",       BardToken.literal_Logical_false );
      keywords.set( "forEach",     BardToken.symbol_forEach );
      keywords.set( "function",    BardToken.symbol_function );
      keywords.set( "global",      BardToken.symbol_global );
      keywords.set( "if",          BardToken.symbol_if );
      keywords.set( "in",          BardToken.symbol_in );
      keywords.set( "instanceOf",  BardToken.symbol_instanceOf );
      keywords.set( "is",          BardToken.symbol_is );
      keywords.set( "local",       BardToken.symbol_local );
      keywords.set( "loop",        BardToken.symbol_loop );
      keywords.set( "METHODS",     BardToken.symbol_METHODS );
      keywords.set( "method",      BardToken.symbol_method );
      keywords.set( "noAction",    BardToken.symbol_noAction );
      keywords.set( "not",         BardToken.symbol_not );
      keywords.set( "null",        BardToken.symbol_null );
      keywords.set( "object",      BardToken.symbol_object );
      keywords.set( "of",          BardToken.symbol_of );
      keywords.set( "or",          BardToken.symbol_or );
      keywords.set( "pi",          BardToken.literal_Real_pi );
      keywords.set( "PROPERTIES",  BardToken.symbol_PROPERTIES );
      keywords.set( "return",      BardToken.symbol_return );
      keywords.set( "this",        BardToken.symbol_this );
      keywords.set( "throw",       BardToken.symbol_throw );
      keywords.set( "trace",       BardToken.symbol_trace );
      keywords.set( "tron",        BardToken.symbol_tron );
      keywords.set( "troff",       BardToken.symbol_troff );
      keywords.set( "true",        BardToken.literal_Logical_true );
      keywords.set( "while",       BardToken.symbol_while );
    }
  }

  public BardError error( String message )
  {
    return new BardError( message, reader.filepath, line, column );
  }

  public void consume_whitespace()
  {
    while (reader.has_another())
    {
      char ch = reader.peek();
      if (ch == 10 || ch > ' ' && ch <= 126) return;
      reader.read();
    }
  }

  public boolean has_another()
  {
    return (peek() != BardToken.eof);
  }

  public int source_position()
  {
    if (prepped_token != -1) return prepped_position;
    return reader.position;
  }

  public String source_between( int pos1, int pos2 )
  {
    string_value.setLength(0);

    int old_position = reader.position;
    int old_line = reader.line;
    int old_column = reader.column;

    reader.position = pos1;
    while (reader.position <= pos2)
    {
      string_value.append( (char) reader.read() );
    }

    reader.position = old_position;
    reader.line = old_line;
    reader.column = old_column;

    return string_value.toString();
  }

  public int peek()
  {
    if (prepped_token != -1) return prepped_token;

    int cur_pos = position;
    prepped_token = read();
    prepped_position = position;
    position = cur_pos;

    return prepped_token;
  }

  public int read()
  {
    if (prepped_token != -1)
    {
      int result = prepped_token;
      prepped_token = -1;
      position = prepped_position;
      return result;
    }

    int t = scan_next();
    while (t == BardToken.comment)
    {
      t = scan_next();
    }
    return t;
  }

  public int scan_next()
  {
    consume_whitespace();
    if ( !reader.has_another() )
    {
      if (need_final_eol)
      {
        need_final_eol = false;
        return BardToken.eol;
      }
      return BardToken.eof;
    }

    for (;;)
    {
      previous_line   = line;
      previous_column = column;
      line = reader.line;
      column = reader.column;
      position = reader.position;

      string_value.setLength( 0 );

      char ch = reader.peek();

      if (ch >= '0' && ch <= '9')
      {
        return scan_number();
      }

      if (ch == '.' && is_digit(reader.peek(2),10))
      {
        reader.read();
        real_value = scan_fraction(10);
        return BardToken.literal_Real;
      }

      if ( (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_' )
      {
        scan_identifier("");
        int keyword = keywords.get( string_value );
        if (keyword != -1)
        {
          if (keyword == BardToken.symbol_else)
          {
            consume_whitespace();
            if (reader.peek() == '#' || reader.peek() == 10) keyword = BardToken.symbol_else_eol;
          }
          return keyword;
        }
        return BardToken.identifier;
      }

      switch (ch)
      {
        case '\'':
          scan_string( '\'' );
          return String_or_Character();

        case '"':
          scan_string( '"' );
          return String_or_Character();
      }

      return scan_symbol();
    }
  }

  public int scan_number()
  {
    int base = 10;
    if (reader.peek() == '0')
    {
      // Either just literal "0", the start of a decimal number
      // "0.xxx", or the start of a base specifier:
      // 0x, 0d, 0c, 0b (hex, decimal, octal, binary)
      reader.read();
      if (reader.peek() == '.')
      {
        real_value = 0;
        char ch2 = reader.peek(2);
        if (ch2 >= '0' && ch2 <= '9')
        {
          reader.read();  // discard '.'
          real_value = scan_fraction(10);
        }
        else
        {
          reader.read();  // discard '.'
        }
        return BardToken.literal_Real;
      }

      switch (reader.peek())
      {
        case 'x': base = 16; break;
        case 'd': base = 10; break;
        case 'c': base = 8; break;
        case 'b': base = 2; break;
        default:
          // save the peek value for later; just return the zero
          integer_value = 0;
          return BardToken.literal_Integer;
      }
      reader.read();  // discard x, d, c, or b
    }

    boolean is_real = false;
    integer_value = scan_integer( base );
    real_value = (double) integer_value;
    if (reader.peek() == '.' && is_digit(reader.peek(2),base))
    {
      reader.read();
      double fraction = scan_fraction( base );
      real_value += fraction;
      is_real = true;
    }

    if (reader.consume('E') || reader.consume('e'))
    {
      is_real = true;
      real_value = Math.pow( real_value, (double) scan_integer(base) );
    }

    if (is_real) return BardToken.literal_Real;

    return BardToken.literal_Integer;
  }

  boolean is_digit( char ch, int base )
  {
    int value = -1;

    if (ch >= '0' && ch <= '9') value = ch - '0';
    else if (ch >= 'A' && ch <= 'Z') value = 10 + (ch - 'A');
    else if (ch >= 'a' && ch <= 'z') value = 10 + (ch - 'a');
    else return false;

    return value < base;
  }

  int digit_value( char ch )
  {
    int value = -1;

    if (ch >= '0' && ch <= '9') value = ch - '0';
    else if (ch >= 'A' && ch <= 'Z') value = 10 + (ch - 'A');
    else if (ch >= 'a' && ch <= 'z') value = 10 + (ch - 'a');

    return value;
  }

  public long scan_integer( int base )
  {
    long n = 0;
    int digit = digit_value( reader.peek() );
    while (digit >= 0 && digit < base)
    {
      n = n * base + digit;
      reader.read();
      digit = digit_value( reader.peek() );
    }
   
    return n;
  }

  public double scan_fraction( int base )
  {
    double n = 0;
    double count = 0;

    int digit = digit_value( reader.peek() );
    while (digit >= 0 && digit < base)
    {
      n = n * base + digit;
      ++count;
      reader.read();
      digit = digit_value( reader.peek() );
    }

    // Ex: .1001 = 9 / (2^4)
    return (n / Math.pow(base, count));
  }

  public int scan_identifier( String prefix )
  {
    // Requires: already determined to start with A-Za-z_
    string_value.setLength( 0 );
    string_value.append( prefix );
    char ch = reader.peek();
    for (;;)
    {
      if ( (ch >= 'a' && ch <= 'z')
         || (ch >= 'A' && ch <= 'Z')
         || (ch >= '0' && ch <= '9')
         || (ch == '_') )
      {
        string_value.append( reader.read() );
      }
      else
      {
        break;
      }

      if ( !reader.has_another() ) break;
      ch = reader.peek();
    }

    integer_value = bc.id_table.get_id( string_value );

    return BardToken.identifier;
  }

  public int scan_symbol()
  {
    for (;;)
    {
      char ch = reader.read();
      switch (ch)
      {
        case '\n' : 
          return BardToken.eol;

        case '!' : 
          if (reader.consume('=')) return BardToken.symbol_ne;
          else                     return BardToken.symbol_bitwise_not;

        case '#':
          if (reader.consume('{')) scan_multi_line_comment();
          else                     scan_single_line_comment();
          return BardToken.comment;

        case '$' : 
          return BardToken.symbol_dollar;

        case '%':
          if     (reader.consume('=')) return BardToken.symbol_mod_with_assign;
          else                         return BardToken.symbol_percent;

        case '&' :
          if      (reader.consume('&')) throw error( "Use 'and' instead of '&&'." );
          else if (reader.consume('=')) return BardToken.symbol_bitwise_and_with_assign;
          else                          return BardToken.symbol_ampersand;

        case '(':
          return BardToken.symbol_open_paren;

        case ')':
          return BardToken.symbol_close_paren;

        case '*':
          if     (reader.consume('=')) return BardToken.symbol_multiply_with_assign;
          else                         return BardToken.symbol_times;

        case '+':
          if     (reader.consume('='))  return BardToken.symbol_add_with_assign;
          else if (reader.consume('+')) return BardToken.symbol_increment;
          else                          return BardToken.symbol_plus;

        case ',':
          return BardToken.symbol_comma;

        case '-':
          if      (reader.consume('=')) return BardToken.symbol_subtract_with_assign;
          else if (reader.consume('-')) return BardToken.symbol_decrement;
          else                          return BardToken.symbol_minus;

        case '.' :
          if (reader.consume('.'))
          {
            if (reader.consume('.')) 
            {
              // ellipsis
              consume_whitespace();
              if ( !reader.consume('\n') ) throw error( "End of line expected after '...'." );
              continue;
            }
            else if (reader.consume('<'))
            {
              return BardToken.symbol_uptolessthan;
            }
            else if (reader.consume('>'))
            {
              return BardToken.symbol_downtogreaterthan;
            }
            else 
            {
              return BardToken.symbol_upto;
            }
          }
          else if (reader.consume('='))
          {
            return BardToken.symbol_member_with_assign;
          }
          else
          {
            return BardToken.symbol_period;
          }

        case '/':
          if (reader.consume('/'))
          {
            scan_alternate_string();
            return String_or_Character();
          }
          else if (reader.consume('='))
          {
            return BardToken.symbol_divide_with_assign;
          }
          else
          {
            return BardToken.symbol_divide;
          }

        case ':' :
          if (reader.consume("<<:"))      return BardToken.symbol_shl;
          else if (reader.consume(">>:")) return BardToken.symbol_shr;
          else if (reader.consume(">>>:")) return BardToken.symbol_shrx;
          return BardToken.symbol_colon;

        case ';':
          return BardToken.symbol_semicolon;

        case '<' :
          if      (reader.consume('=')) return BardToken.symbol_le;
          else if (reader.consume('>')) return BardToken.symbol_compare;
          else                          return BardToken.symbol_lt;

        case '=' :
          if (reader.consume('=')) return BardToken.symbol_eq;
          else                     return BardToken.symbol_equals;

        case '>' :
          if      (reader.consume('=')) return BardToken.symbol_ge;
          else                          return BardToken.symbol_gt;

        case '?':
          return BardToken.symbol_question_mark;

        case '@' :
          if (reader.consume('|'))
          {
            return scan_actual_string();
          }
          return BardToken.symbol_at;

        case '[':
          if (reader.consume(']')) return BardToken.symbol_empty_brackets;
          return BardToken.symbol_open_bracket;

        case ']':
          return BardToken.symbol_close_bracket;

        case '^':
          if     (reader.consume('=')) return BardToken.symbol_power_with_assign;
          else                         return BardToken.symbol_power;

        case '{': 
          if (reader.consume('}')) return BardToken.symbol_empty_braces;
          return BardToken.symbol_open_brace;

        case '|' :
          if     (reader.consume('='))  return BardToken.symbol_bitwise_or_with_assign;
          else if (reader.consume('|')) return BardToken.eol;
          else                          return BardToken.symbol_bitwise_or;

        case '}' :
          if     (reader.consume('#')) return BardToken.symbol_close_comment; // used for error reporting
          else                         return BardToken.symbol_close_brace;

        case '~':
          if     (reader.consume('=')) return BardToken.symbol_bitwise_xor_with_assign;
          else                         return BardToken.symbol_bitwise_xor;

        default:
          if (ch < 32 || ch > 126) throw error( "Unrecognized symbol: Character(" + (int)ch + ")." );
          else                     throw error( "Unrecognized symbol '" + ch + "'." );
      }
    }
  }

  public int String_or_Character()
  {
    // We have a String already; convert it to a Character if it's length 1.
    if (string_value.length() == 1)
    {
      integer_value = string_value.charAt(0);
      return BardToken.literal_Character;
    }
    else
    {
      return BardToken.literal_String;
    }
  }

  public char scan_char( char terminator )
  {
    String error_mesg = "Character expected; found end of file.";
    if (terminator == '"') error_mesg = "String terminator (\") expected; found end of file.";
    else if (terminator == '\'') error_mesg = "String terminator (') expected; found end of file.";
    else if (terminator == '/')  error_mesg = "String terminator (//) expected; found end of file.";

    if ( !reader.has_another() ) throw error( error_mesg );

    char ch = reader.peek();
    switch (ch)
    {
      case 10:
        throw error( error_mesg );

      case '\\':
        reader.read();
        if ( !reader.has_another() ) throw error( error_mesg );
        switch (reader.read())
        {
          case 'b':  return (char)8;
          case 'f':  return (char)12;
          case 'n':  return (char)10;
          case 'r':  return (char)13;
          case 't':  return (char)9;
          case '0':  return (char)0;
          case '/':  return '/';
          case '\'': return '\'';
          case '\\': return '\\';
          case '"':  return '"';
          case 'x':
            return read_hex_value(2);
          case 'u':
            return read_hex_value(4);

          default:
            throw error( "Invalid escape sequence.  Supported: \\n \\r \\t \\0 \\/ \\' \\\\ \\\" \\" + "uXXXX \\" + "xXX." );
        }

      default:
        reader.read();  // ch
        if ((ch & 0x80) != 0)
        {
          // Handle UTF8 encoding
          char ch2 = reader.read();

          if ((ch & 0x20) == 0)
          {
            // %110xxxxx 10xxxxxx
            ch  &= 0x1f;
            ch2 &= 0x3f;
            return (char)((ch << 6) | ch2);
          }
          else
          {
            // %1110xxxx 10xxxxxx 10xxxxxx
            char ch3 = reader.read();
            ch  &= 15;
            ch2 &= 0x3f;
            ch3 &= 0x3f;
            return (char) ((ch << 12) | (ch2 << 6) | ch3);
          }
        }
        return ch;
    }
  }

  public char read_hex_value( int digits )
  {
    int value = 0;
    for (int i=0; i<digits; ++i)
    {
      if ( !reader.has_another() ) throw error( digits + "-digit hex value expected; found end of file." );
      if ( !is_digit(reader.peek(),16) )
      {
        char ch = reader.peek();
        String str;
        if (ch < 32 || ch == 127) str = "" + (int) ch;
        else str = "'" + ch + "'";
        throw error( "Invalid hex digit " + str + "." );
      }
      value = (value << 4) + digit_value( reader.read() );
    }
    return (char) value;
  }

  public void scan_string( char terminator )
  {
    string_value.setLength(0);

    reader.read();  // we know the first character is open quote
    while (reader.has_another())
    {
      if (reader.consume(terminator)) return;
      string_value.append( scan_char(terminator) );
    }

    throw error( "Missing closing quote (" + terminator + ") on string literal." );
  }

  public void scan_alternate_string()
  {
    string_value.setLength(0);
    while (reader.has_another())
    {
      char ch;
      if (reader.peek() == '/')
      {
        ch = reader.read();
        if (reader.consume('/')) return;
      }
      else
      {
        ch = (char) scan_char('/');
      }
      string_value.append( ch );
    }

    throw error( "Missing closing quote (//) on string literal." );
  }

  public int scan_actual_string()
  {
    string_value.setLength( 0 );
    while (reader.has_another())
    {
      int ch = reader.read();
      string_value.append( (char) ch );
      if (ch == 10)
      {
        consume_whitespace();
        if ( !reader.consume('|') )
        {
          string_value.setLength( string_value.length() - 1 );  // remove the last \n
          break;
        }
      }
    }
    return String_or_Character();
  }

  public void scan_single_line_comment()
  {
    string_value.setLength(0);
    while (reader.has_another())
    {
      char ch = reader.peek();
      if (ch == '\n') break;
      string_value.append(reader.read());
    }
  }

  public void scan_multi_line_comment()
  {
    int stack_count = 1;
    string_value.setLength(0);
    while (reader.has_another())
    {
      char ch = reader.read();
      if (ch == '#')
      {
        if (reader.peek() == '{')
        {
          ++stack_count;
          reader.read();
          string_value.append( "#{" );
          continue;
        }
      }
      else if (ch == '}')
      {
        if (reader.peek() == '#')
        {
          --stack_count;
          reader.read();
          if (stack_count > 0)
          {
            string_value.append( "}#" );
            continue;
          }
          else
          {
            return;
          }
        }
      }
      string_value.append(ch);
    }
    throw error( "'}" + "#' expected; found end of file." );
  }
}

