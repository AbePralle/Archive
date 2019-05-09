package boss.vm;

public class BossTokenType
{
  final static public int IS_KEYWORD    = 1;
  final static public int IS_SYMBOL     = 2;
  final static public int IS_STRUCTURAL = 4;

  final static public int EOI                  =  0;
  final static public int EOL                  =  1;
  final static public int IDENTIFIER           =  2;
  final static public int STRING               =  3;
  final static public int REAL64               =  4;
  final static public int REAL64_AS_INT32      =  5;
  final static public int INT32                =  7;
  final static public int CHARACTER            =  8;

  final static public int KEYWORD_CLASS        =  9;
  final static public int KEYWORD_ELSE         = 10;
  final static public int KEYWORD_ELSE_IF      = 11;
  final static public int KEYWORD_END_CLASS    = 12;
  final static public int KEYWORD_END_IF       = 13;
  final static public int KEYWORD_END_ROUTINE  = 14;
  final static public int KEYWORD_END_WHILE    = 15;
  final static public int KEYWORD_FALSE        = 16;
  final static public int KEYWORD_GLOBAL       = 17;
  final static public int KEYWORD_IF           = 18;
  final static public int KEYWORD_IMPORT       = 19;
  final static public int KEYWORD_LOCAL        = 20;
  final static public int KEYWORD_METHOD       = 21;
  final static public int KEYWORD_METHODS      = 22;
  final static public int KEYWORD_NULL         = 23;
  final static public int KEYWORD_PRINTLN      = 24;
  final static public int KEYWORD_PROPERTIES   = 25;
  final static public int KEYWORD_RETURN       = 26;
  final static public int KEYWORD_ROUTINE      = 27;
  final static public int KEYWORD_THIS_CALL    = 28;
  final static public int KEYWORD_TRUE         = 29;
  final static public int KEYWORD_UNDEFINED    = 30;
  final static public int KEYWORD_WHILE        = 31;

  final static public int SYMBOL_AMPERSAND     = 32;
  final static public int SYMBOL_ARROW         = 33;
  final static public int SYMBOL_ASTERISK      = 34;
  final static public int SYMBOL_BANG          = 35;
  final static public int SYMBOL_CARET         = 36;
  final static public int SYMBOL_CLOSE_PAREN   = 37;
  final static public int SYMBOL_COLON         = 38;
  final static public int SYMBOL_COLON_COLON   = 39;
  final static public int SYMBOL_COMMA         = 40;
  final static public int SYMBOL_DOLLAR        = 41;
  final static public int SYMBOL_EQUALS        = 42;
  final static public int SYMBOL_EQ            = 43;
  final static public int SYMBOL_GE            = 44;
  final static public int SYMBOL_GT            = 45;
  final static public int SYMBOL_LEFT_SHIFT    = 46;
  final static public int SYMBOL_LE            = 47;
  final static public int SYMBOL_LT            = 48;
  final static public int SYMBOL_MINUS         = 49;
  final static public int SYMBOL_MINUS_MINUS   = 50;
  final static public int SYMBOL_NE            = 51;
  final static public int SYMBOL_OPEN_PAREN    = 52;
  final static public int SYMBOL_PERCENT       = 53;
  final static public int SYMBOL_PERIOD        = 54;
  final static public int SYMBOL_PLUS          = 55;
  final static public int SYMBOL_PLUS_PLUS     = 56;
  final static public int SYMBOL_RIGHT_SHIFT   = 57;
  final static public int SYMBOL_RIGHT_SHIFT_X = 58;
  final static public int SYMBOL_SEMICOLON     = 59;
  final static public int SYMBOL_SLASH         = 60;
  final static public int SYMBOL_TILDE         = 61;
  final static public int SYMBOL_VERTICAL_BAR  = 62;
}

