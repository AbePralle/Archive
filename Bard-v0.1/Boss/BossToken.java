public class BossToken
{
  final static public int
    // general non-statement
    eof                              = 0,
    eol                              = 1,

    // structure
    symbol_class                     = 2,
    symbol_function                  = 3,
    symbol_else                      = 4,
    symbol_else_eol                  = 5,
    symbol_elseIf                    = 6,
    symbol_endClass                  = 7,
    symbol_endForEach                = 8,
    symbol_endFunction               = 9,
    symbol_endIf                     = 10,
    symbol_endLoop                   = 11,
    symbol_endObject                 = 12,
    symbol_endWhile                  = 13,
    symbol_in                        = 14,
    symbol_METHODS                   = 15,
    symbol_method                    = 16,
    symbol_object                    = 17,
    symbol_of                        = 18,
    symbol_PROPERTIES                = 19,
    symbol_close_brace               = 20,
    symbol_close_bracket             = 21,
    symbol_close_comment             = 22,
    symbol_close_paren               = 23,

    symbol_last_non_statement        = 24,  // End non-statement list

    // general
    comment                          = 25,
    identifier                       = 26,
    command                          = 27,

    // control
    symbol_forEach                   = 28,
    symbol_if                        = 29,
    symbol_loop                      = 30,
    symbol_return                    = 31,
    symbol_throw                     = 32,
    symbol_trace                     = 33,
    symbol_tron                      = 34,
    symbol_troff                     = 35,
    symbol_while                     = 36,

    // expressions
    literal_Real                     = 37,
    literal_Real_pi                  = 38,
    literal_Integer                  = 39,
    literal_Character                = 40,
    literal_Logical_true             = 41,
    literal_Logical_false            = 42,
    literal_String                   = 43,
    symbol_ampersand                 = 44,
    symbol_and                       = 45,
    symbol_as                        = 46,
    symbol_at                        = 47,
    symbol_bitwise_and_with_assign   = 48,
    symbol_bitwise_not               = 49,
    symbol_bitwise_or                = 50,
    symbol_bitwise_or_with_assign    = 51,
    symbol_bitwise_xor               = 52,
    symbol_bitwise_xor_with_assign   = 53,
    symbol_colon                     = 54,
    symbol_comma                     = 55,
    symbol_compare                   = 56,
    symbol_decrement                 = 57,
    symbol_divide                    = 58,
    symbol_divide_with_assign        = 59,
    symbol_dollar                    = 60,
    symbol_downtogreaterthan         = 61,
    symbol_empty_braces              = 62,
    symbol_empty_brackets            = 63,
    symbol_eq                        = 64,
    symbol_equals                    = 65,
    symbol_ge                        = 66,
    symbol_global                    = 67,
    symbol_gt                        = 68,
    symbol_increment                 = 69,
    symbol_instanceOf                = 70,
    symbol_is                        = 71,
    symbol_le                        = 72,
    symbol_lt                        = 73,
    symbol_local                     = 74,
    symbol_member_with_assign        = 75,
    symbol_minus                     = 76,
    symbol_mod_with_assign           = 77,
    symbol_ne                        = 78,
    symbol_noAction                  = 79,
    symbol_not                       = 80,
    symbol_null                      = 81,
    symbol_open_brace                = 82,
    symbol_open_bracket              = 83,
    symbol_open_paren                = 84,
    symbol_or                        = 85,
    symbol_percent                   = 86,
    symbol_period                    = 87,
    symbol_plus                      = 88,
    symbol_plus_equals               = 89,
    symbol_power                     = 90,
    symbol_power_with_assign         = 91,
    symbol_question_mark             = 92,
    symbol_semicolon                 = 93,
    symbol_shl                       = 94,
    symbol_shr                       = 95,
    symbol_shrx                      = 96,
    symbol_subtract_with_assign      = 97,
    symbol_this                      = 98,
    symbol_times                     = 99,
    symbol_times_equals              = 100,
    symbol_upto                      = 101,
    symbol_uptolessthan              = 102,
    
    last_token                       = 200;  // 101, ,

  static public String get_name( int token )
  {
    switch (token)
    {
      case eof:
        return "end of file";
      case eol:
        return "eol";

      case symbol_class:
        return "class";
      case symbol_function:
        return "function";
      case symbol_else:
        return "else";
      case symbol_else_eol:
        return "else";
      case symbol_elseIf:
        return "elseIf";
      case symbol_endClass:
        return "endClass";
      case symbol_endForEach:
        return "endForEach";
      case symbol_endFunction:
        return "endFunction";
      case symbol_endIf:
        return "endIf";
      case symbol_endLoop:
        return "endLoop";
      case symbol_endObject:
        return "endObject";
      case symbol_endWhile:
        return "endWhile";
      case symbol_in:
        return "in";
      case symbol_METHODS:
        return "METHODS";
      case symbol_method:
        return "method";
      case symbol_object:
        return "object";
      case symbol_of:
        return "of";
      case symbol_PROPERTIES:
        return "PROPERTIES";
      case symbol_close_brace:
        return "}";
      case symbol_close_bracket:
        return "]";
      case symbol_close_comment:
        return "}#";
      case symbol_close_paren:
        return ")";

      case comment:
        return "comment";
      case identifier:
        return "identifier";
      case command:
        return "keyword";

      case symbol_forEach:
        return "forEach";
      case symbol_if:
        return "if";
      case symbol_loop:
        return "loop";
      case symbol_return:
        return "return";
      case symbol_throw:
        return "throw";
      case symbol_trace:
        return "trace";
      case symbol_tron:
        return "tron";
      case symbol_troff:
        return "troff";
      case symbol_while:
        return "while";

      case literal_Real:
        return "Real";
      case literal_Real_pi:
        return "pi";
      case literal_Integer:
        return "Integer";
      case literal_Character:
        return "Character";
      case literal_Logical_true:
        return "true";
      case literal_Logical_false:
        return "false";
      case literal_String:
        return "String";
      case symbol_ampersand:
        return "&";
      case symbol_and:
        return "and";
      case symbol_as:
        return "as";
      case symbol_at:
        return "@";
      case symbol_bitwise_and_with_assign:
        return "&=";
      case symbol_bitwise_not:
        return "!";
      case symbol_bitwise_or:
        return "|";
      case symbol_bitwise_or_with_assign:
        return "|=";
      case symbol_bitwise_xor:
        return "~";
      case symbol_bitwise_xor_with_assign:
        return "~=";
      case symbol_colon:
        return ":";
      case symbol_comma:
        return ",";
      case symbol_compare:
        return "<>";
      case symbol_decrement:
        return "--";
      case symbol_divide:
        return "/";
      case symbol_divide_with_assign:
        return "/=";
      case symbol_dollar:
        return "$";
      case symbol_downtogreaterthan:
        return "..>";
      case symbol_empty_braces:
        return "{}";
      case symbol_empty_brackets:
        return "[]";
      case symbol_eq:
        return "==";
      case symbol_equals:
        return "=";
      case symbol_ge:
        return ">=";
      case symbol_global:
        return "global";
      case symbol_gt:
        return ">";
      case symbol_increment:
        return "++";
      case symbol_instanceOf:
        return "instanceOf";
      case symbol_is:
        return "is";
      case symbol_le:
        return "<=";
      case symbol_local:
        return "local";
      case symbol_lt:
        return "<";
      case symbol_member_with_assign:
        return ".=";
      case symbol_minus:
        return "-";
      case symbol_mod_with_assign:
        return "%=";
      case symbol_ne:
        return "!=";
      case symbol_noAction:
        return "noAction";
      case symbol_not:
        return "not";
      case symbol_null:
        return "null";
      case symbol_open_brace:
        return "{";
      case symbol_open_bracket:
        return "[";
      case symbol_open_paren:
        return "(";
      case symbol_or:
        return "or";
      case symbol_percent:
        return "%";
      case symbol_period:
        return ".";
      case symbol_plus:
        return "+";
      case symbol_plus_equals:
        return "+=";
      case symbol_power:
        return "^";
      case symbol_power_with_assign:
        return "^=";
      case symbol_question_mark:
        return "?";
      case symbol_semicolon:
        return ";";
      case symbol_shl:
        return ":<<:";
      case symbol_shr:
        return ":>>:";
      case symbol_shrx:
        return ":>>>:";
      case symbol_subtract_with_assign:
        return "-=";
      case symbol_this:
        return "this";
      case symbol_times:
        return "*";
      case symbol_times_equals:
        return "*=";
      case symbol_upto:
        return "..";
      case symbol_uptolessthan:
        return "..<";
      default:
        return "(Unknown token)";
    }
  }

  public int    type;
  public int    line, column;
  public String filepath;

  public BossToken init( BossScanner scanner, int type )
  {
    this.type = type;
    filepath  = scanner.filepath;
    line      = scanner.line;
    column    = scanner.column;
    return this;
  }

  public BossToken init( BossToken existing )
  {
    this.type     = existing.type;
    this.filepath = existing.filepath;
    this.line     = existing.line;
    this.column   = existing.column;
    return this;
  }

  public String toString() { return getClass().getName(); }

  public BossError error( String message )
  {
    return new BossError( message, filepath, line, column );
  }

  //---------------------------------------------------------------------------

  static public class Identifier extends BossToken
  {
    public String value;

    public Identifier( BossScanner scanner, String value )
    { 
      init( scanner, identifier );
      this.value = value;
    }

    public String toString() { return value; }
  };

  static public class Command extends BossToken
  {
    public BossCommand cmd;

    public Command( BossScanner scanner, BossCommand cmd )
    { 
      init( scanner, command );
      this.cmd = cmd;
    }

    public String toString() { return cmd.keyword; }
  };


  static class LiteralReal extends BossToken
  {
    double value;

    BossToken init( BossScanner scanner, double value )
    {
      init( scanner, literal_Real );
      this.value = value;
      return this;
    }

    BossToken init( BossToken existing, double value )
    {
      init( existing, literal_Real );
      this.value = value;
      return this;
    }

    public String toString() { return ""+value; }
  };

  static class LiteralInteger extends BossToken
  {
    int value;

    public BossToken init( BossScanner scanner, int value )
    {
      super.init( scanner, literal_Integer );
      this.value = value;
      return this;
    }

    public BossToken init( BossToken existing, int value )
    {
      init( existing );
      this.value = value;
      return this;
    }

    public String toString() { return ""+value; }

  };

  static class LiteralCharacter extends BossToken
  {
    char value;

    BossToken init( BossScanner scanner, char value )
    {
      init( scanner, literal_Character );
      this.value = value;
      return this;
    }

    BossToken init( BossToken existing, char value )
    {
      init( existing );
      this.value = value;
      return this;
    }

    public String toString() { return ""+value; }
  };

  static class LiteralString extends BossToken
  {
    String value;

    BossToken init( BossScanner scanner, String value )
    {
      init( scanner, literal_String );
      this.value = value;
      return this;
    }

    BossToken init( BossToken existing, String value )
    {
      init( existing );
      this.value = value;
      return this;
    }

    public String toString() { return ""+value; }
  };
}

