import java.io.*;
import java.util.*;

public class BossScanner
{
  public BossVM vm;
  public BossParseReader reader = new BossParseReader();

  public StringBuilder string_value = new StringBuilder();
  public int    line, column, position;
  public String filepath;

  public ArrayList<BossToken> tokens = new ArrayList<BossToken>();

  public BossScanner( BossVM vm )
  {
    this.vm = vm;
  }

  public void init( String filename )
  {
    this.filepath = filename;
    reader.init( filename );
    init();
  }

  public void init( String filename, String data )
  {
    this.filepath = filename;
    reader.init( filename, data );
    init();
  }

  public void init()
  {
    line = 1;
    column = 1;
    position = 0;
  }

  public BossError error( String message )
  {
    return new BossError( message, filepath, line, column );
  }

  public ArrayList<BossToken> tokenize( String filename )
  {
    init( filename );

    tokens.clear();

    while (scan_next())
    {
      // No action
    }

    tokens.add( new BossToken().init(this,BossToken.eol) );
    tokens.add( new BossToken().init(this,BossToken.eof) );

    return tokens;
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

  public boolean scan_next()
  {
    consume_whitespace();

    if ( !reader.has_another() )
    {
      return false;
    }

    for (;;)
    {
      line = reader.line;
      column = reader.column;
      position = reader.position;

      string_value.setLength( 0 );

      char ch = reader.peek();

      if (ch >= '0' && ch <= '9')
      {
        scan_number();
        return true;
      }

      if (ch == '.' && is_digit(reader.peek(2),10))
      {
        reader.read();
        tokens.add( new BossToken.LiteralReal().init( this, scan_fraction(10) ) );
        return true;
      }

      if ( (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_' )
      {
        scan_identifier("");
        String st = string_value.toString();
        BossCommand cmd = vm.commands.get( st );
        if (cmd != null)
        {
          tokens.add( new BossToken.Command( this, cmd ) );
        }
        else
        {
          tokens.add( new BossToken.Identifier( this, st ) );
        }
        return true;
      }

      switch (ch)
      {
        case '\'':
          scan_string( '\'' );
          String_or_Character();
          return true;

        case '"':
          scan_string( '"' );
          String_or_Character();
          return true;
      }

      scan_symbol();
      return true;
    }
  }

  public void scan_number()
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
        double real_value = 0;
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
        tokens.add( new BossToken.LiteralReal().init(this,real_value) );
        return;
      }

      switch (reader.peek())
      {
        case 'x': base = 16; break;
        case 'd': base = 10; break;
        case 'c': base = 8; break;
        case 'b': base = 2; break;
        default:
          // save the peek value for later; just return the zero
          tokens.add( new BossToken.LiteralInteger().init( this, 0 ) );
          return;
      }
      reader.read();  // discard x, d, c, or b
    }

    boolean is_real = false;
    long integer_value = scan_integer( base );
    double real_value = (double) integer_value;
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

    if (is_real)
    {
      tokens.add( new BossToken.LiteralReal().init(this,real_value) );
    }
    else
    {
      tokens.add( new BossToken.LiteralInteger().init(this,(int)integer_value) );
    }
  }

  public boolean is_digit( char ch, int base )
  {
    int value = -1;

    if (ch >= '0' && ch <= '9') value = ch - '0';
    else if (ch >= 'A' && ch <= 'Z') value = 10 + (ch - 'A');
    else if (ch >= 'a' && ch <= 'z') value = 10 + (ch - 'a');
    else return false;

    return value < base;
  }

  public int digit_value( char ch )
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

  public void scan_identifier( String prefix )
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
  }

  public void scan_symbol()
  {
    int type = 0;

    for (;;)
    {
      char ch = reader.read();
      switch (ch)
      {
        case '\n' : 
          type = BossToken.eol;
          break;

          /*
        case '!' : 
          if (reader.consume('=')) return BossToken.symbol_ne;
          else                     return BossToken.symbol_bitwise_not;
          */

        case '#':
          if (reader.consume('{')) scan_multi_line_comment();
          else                     scan_single_line_comment();
          return;

          /*
        case '$' : 
          return BossToken.symbol_dollar;

        case '%':
          if     (reader.consume('=')) return BossToken.symbol_mod_with_assign;
          else                         return BossToken.symbol_percent;

        case '&' :
          if      (reader.consume('&')) throw error( "Use 'and' instead of '&&'." );
          else if (reader.consume('=')) return BossToken.symbol_bitwise_and_with_assign;
          else                          return BossToken.symbol_ampersand;
          */

        case '(':
          type = BossToken.symbol_open_paren;
          break;

        case ')':
          type = BossToken.symbol_close_paren;
          break;

        case '*':
          if     (reader.consume('=')) type = BossToken.symbol_times_equals;
          else                         type = BossToken.symbol_times;
          break;

        case '+':
          if     (reader.consume('='))  type = BossToken.symbol_plus_equals;
          else if (reader.consume('+')) type = BossToken.symbol_increment;
          else                          type = BossToken.symbol_plus;
          break;

        case ',':
          type = BossToken.symbol_comma;
          break;

          /*
        case '-':
          if      (reader.consume('=')) return BossToken.symbol_subtract_with_assign;
          else if (reader.consume('-')) return BossToken.symbol_decrement;
          else                          return BossToken.symbol_minus;
          */

        case '.' :
          /*
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
              return BossToken.symbol_uptolessthan;
            }
            else if (reader.consume('>'))
            {
              return BossToken.symbol_downtogreaterthan;
            }
            else 
            {
              return BossToken.symbol_upto;
            }
          }
          else if (reader.consume('='))
          {
            return BossToken.symbol_member_with_assign;
          }
          else
          */
          {
            type = BossToken.symbol_period;
          }
          break;

          /*
        case '/':
          if (reader.consume('/'))
          {
            scan_alternate_string();
            return String_or_Character();
          }
          else if (reader.consume('='))
          {
            return BossToken.symbol_divide_with_assign;
          }
          else
          {
            return BossToken.symbol_divide;
          }

        case ':' :
          if (reader.consume("<<:"))      return BossToken.symbol_shl;
          else if (reader.consume(">>:")) return BossToken.symbol_shr;
          else if (reader.consume(">>>:")) return BossToken.symbol_shrx;
          return BossToken.symbol_colon;

        case ';':
          return BossToken.symbol_semicolon;

        case '<' :
          if      (reader.consume('=')) return BossToken.symbol_le;
          else if (reader.consume('>')) return BossToken.symbol_compare;
          else                          return BossToken.symbol_lt;

        case '=' :
          if (reader.consume('=')) return BossToken.symbol_eq;
          else                     return BossToken.symbol_equals;

        case '>' :
          if      (reader.consume('=')) return BossToken.symbol_ge;
          else                          return BossToken.symbol_gt;

        case '?':
          return BossToken.symbol_question_mark;

        case '@' :
          if (reader.consume('|'))
          {
            return scan_actual_string();
          }
          return BossToken.symbol_at;

        case '[':
          if (reader.consume(']')) return BossToken.symbol_empty_brackets;
          return BossToken.symbol_open_bracket;

        case ']':
          return BossToken.symbol_close_bracket;

        case '^':
          if     (reader.consume('=')) return BossToken.symbol_power_with_assign;
          else                         return BossToken.symbol_power;

        case '{': 
          if (reader.consume('}')) return BossToken.symbol_empty_braces;
          return BossToken.symbol_open_brace;

        case '|' :
          if     (reader.consume('='))  return BossToken.symbol_bitwise_or_with_assign;
          else if (reader.consume('|')) return BossToken.eol;
          else                          return BossToken.symbol_bitwise_or;

        case '}' :
          if     (reader.consume('#')) return BossToken.symbol_close_comment; // used for error reporting
          else                         return BossToken.symbol_close_brace;

        case '~':
          if     (reader.consume('=')) return BossToken.symbol_bitwise_xor_with_assign;
          else                         return BossToken.symbol_bitwise_xor;
          */

        default:
          if (ch < 32 || ch > 126) throw error( "Unrecognized symbol: Character(" + (int)ch + ")." );
          else                     throw error( "Unrecognized symbol '" + ch + "'." );
      }
      tokens.add( new BossToken().init(this,type) );
      return;
    }
  }

  public void String_or_Character()
  {
    // We have a String already; convert it to a Character if it's length 1.
    /*
    if (string_value.length() == 1)
    {
      char value = string_value.charAt(0);
      tokens.add( new BossToken.LiteralCharacter().init(this,value) );
    }
    else
    */
    {
      tokens.add( new BossToken.LiteralString().init(this,string_value.toString()) );
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
      if (reader.consume(terminator)) 
      {
        return;
      }
      string_value.append( scan_char(terminator) );
    }

    throw error( "Missing closing quote (" + terminator + ") on string literal." );
  }

/*
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

  int scan_actual_string()
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
  */

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

