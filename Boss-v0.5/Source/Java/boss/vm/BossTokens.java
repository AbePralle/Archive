package boss.vm;

import java.io.*;
import java.util.*;

public class BossTokens
{
  // DEFINITIONS
  final static int TOKEN_DEF_FLAGS_OFFSET     = 0;
  final static int TOKEN_DEF_NAME_OFFSET      = 1;

  final static int TOKEN_DATA_TYPE_OFFSET     = 0;
  final static int TOKEN_DATA_CONTENT_OFFSET  = 1;
  final static int TOKEN_DATA_FILEPATH_OFFSET = 2;
  final static int TOKEN_DATA_LINE_OFFSET     = 3;
  final static int TOKEN_DATA_COLUMN_OFFSET   = 4;


  // PROPERTIES
  public BossIntList     definitions = new BossIntList( 256 );     // [flags, name string index]
  public BossIntList       tokenData = new BossIntList( 8192 );    // [type, content value/index, filepath index, line, column]
  public BossDoubleList        reals = new BossDoubleList( 512 );
  public BossStringIntLookup strings = new BossStringIntLookup();

  int    nextFilepath;
  int    nextLine;
  int    nextColumn;

  // METHODS
  public BossTokens()
  {
    int isKeyword    = BossTokenType.IS_KEYWORD;
    int isSymbol     = BossTokenType.IS_SYMBOL;
    int isStructural = BossTokenType.IS_STRUCTURAL;

    define( BossTokenType.EOI,        "[end of input]", isStructural );
    define( BossTokenType.EOL,        "[end of line]",  isSymbol );
    define( BossTokenType.IDENTIFIER, "identifier", 0 );
    define( BossTokenType.STRING,     "String", 0 );

    define( BossTokenType.KEYWORD_CLASS,        "class",      isKeyword | isStructural );
    define( BossTokenType.KEYWORD_ELSE,         "else",       isKeyword | isStructural );
    define( BossTokenType.KEYWORD_ELSE_IF,      "elseIf",     isKeyword | isStructural );
    define( BossTokenType.KEYWORD_END_CLASS,    "endClass",   isKeyword | isStructural );
    define( BossTokenType.KEYWORD_END_IF,       "endIf",      isKeyword | isStructural );
    define( BossTokenType.KEYWORD_END_ROUTINE,  "endRoutine", isKeyword | isStructural );
    define( BossTokenType.KEYWORD_END_WHILE,    "endWhile",   isKeyword | isStructural );
    define( BossTokenType.KEYWORD_FALSE,        "false",      isKeyword );
    define( BossTokenType.KEYWORD_GLOBAL,       "global",     isKeyword );
    define( BossTokenType.KEYWORD_IF,           "if",         isKeyword );
    define( BossTokenType.KEYWORD_IMPORT,       "import",     isKeyword );
    define( BossTokenType.KEYWORD_LOCAL,        "local",      isKeyword );
    define( BossTokenType.KEYWORD_METHOD,       "method",     isKeyword | isStructural );
    define( BossTokenType.KEYWORD_METHODS,      "METHODS",    isKeyword | isStructural );
    define( BossTokenType.KEYWORD_NULL,         "null",       isKeyword );
    define( BossTokenType.KEYWORD_PRINTLN,      "println",    isKeyword );
    define( BossTokenType.KEYWORD_PROPERTIES,   "PROPERTIES", isKeyword | isStructural );
    define( BossTokenType.KEYWORD_RETURN,       "return",     isKeyword );
    define( BossTokenType.KEYWORD_ROUTINE,      "routine",    isKeyword | isStructural );
    define( BossTokenType.KEYWORD_THIS_CALL,    "thisCall",   isKeyword | isStructural );
    define( BossTokenType.KEYWORD_TRUE,         "true",       isKeyword );
    define( BossTokenType.KEYWORD_UNDEFINED,    "undefined",  isKeyword );
    define( BossTokenType.KEYWORD_WHILE,        "while",      isKeyword );

    define( BossTokenType.SYMBOL_AMPERSAND,     "&",   isSymbol );
    define( BossTokenType.SYMBOL_ARROW,         "->",  isSymbol );
    define( BossTokenType.SYMBOL_ASTERISK,      "*",   isSymbol );
    define( BossTokenType.SYMBOL_BANG,          "!",   isSymbol );
    define( BossTokenType.SYMBOL_CARET,         "^",   isSymbol );
    define( BossTokenType.SYMBOL_CLOSE_PAREN,   ")",   isSymbol | isStructural );
    define( BossTokenType.SYMBOL_COLON,         ":",   isSymbol );
    define( BossTokenType.SYMBOL_COLON_COLON,   "::",  isSymbol );
    define( BossTokenType.SYMBOL_COMMA,         ",",   isSymbol );
    define( BossTokenType.SYMBOL_DOLLAR,        "$",   isSymbol );
    define( BossTokenType.SYMBOL_EQUALS,        "=",   isSymbol );
    define( BossTokenType.SYMBOL_EQ,            "==",  isSymbol );
    define( BossTokenType.SYMBOL_GE,            ">=",  isSymbol );
    define( BossTokenType.SYMBOL_GT,            ">",   isSymbol );
    define( BossTokenType.SYMBOL_LEFT_SHIFT,    "<<",  isSymbol );
    define( BossTokenType.SYMBOL_LE,            "<=",  isSymbol );
    define( BossTokenType.SYMBOL_LT,            "<",   isSymbol );
    define( BossTokenType.SYMBOL_MINUS,         "-",   isSymbol );
    define( BossTokenType.SYMBOL_MINUS_MINUS,   "--",  isSymbol );
    define( BossTokenType.SYMBOL_NE,            "!=",  isSymbol );
    define( BossTokenType.SYMBOL_OPEN_PAREN,    "(",   isSymbol );
    define( BossTokenType.SYMBOL_PERCENT,       "%",   isSymbol );
    define( BossTokenType.SYMBOL_PERIOD,        ".",   isSymbol );
    define( BossTokenType.SYMBOL_PLUS,          "+",   isSymbol );
    define( BossTokenType.SYMBOL_PLUS_PLUS,     "++",  isSymbol );
    define( BossTokenType.SYMBOL_RIGHT_SHIFT,   ">>",  isSymbol );
    define( BossTokenType.SYMBOL_RIGHT_SHIFT_X, ">>>", isSymbol );
    define( BossTokenType.SYMBOL_SEMICOLON,     ";",   isSymbol );
    define( BossTokenType.SYMBOL_SLASH,         "/",   isSymbol );
    define( BossTokenType.SYMBOL_TILDE,         "~",   isSymbol );
    define( BossTokenType.SYMBOL_VERTICAL_BAR,  "|",   isSymbol );

    setFilepath( "[BossVM]" );
  }

  public int column( int token )
  {
    return tokenData.get( token+TOKEN_DATA_COLUMN_OFFSET );
  }

  public int content( int token )
  {
    return tokenData.get( token+TOKEN_DATA_CONTENT_OFFSET );
  }

  public String contentString( int token )
  {
    return string( content(token) );
  }

  public void define( int tokenType, String name, int flags )
  {
    int index = tokenType * 2;
    definitions.expandToCount( index + 2 );

    definitions.set( index, flags );
    definitions.set( index+1, stringIndex(name) );
  }

  public String filepath( int token )
  {
    return string( tokenData.get(token+TOKEN_DATA_FILEPATH_OFFSET) );
  }

  public boolean isKeyword( int token )
  {
    return (definitions.get( tokenData.get(token)*2 ) & BossTokenType.IS_KEYWORD) != 0;
  }

  public boolean isStructural( int token )
  {
    return (definitions.get( tokenData.get(token)*2 ) & BossTokenType.IS_STRUCTURAL) != 0;
  }

  public boolean isSymbol( int token )
  {
    return (definitions.get( tokenData.get(token)*2 ) & BossTokenType.IS_SYMBOL) != 0;
  }

  public int line( int token )
  {
    return tokenData.get( token+TOKEN_DATA_LINE_OFFSET );
  }

  public String name( int token )
  {
    return string( definitions.get( tokenData.get(token)*2 + TOKEN_DEF_NAME_OFFSET ) );
  }

  public void setFilepath( String nextFilepath )
  {
    this.nextFilepath = stringIndex( nextFilepath );
  }

  public void setSource( int line, int column )
  {
    this.nextLine = line;
    this.nextColumn = column;
  }

  public String string( int index )
  {
    return strings.get( index );
  }

  public int stringIndex( String value )
  {
    return strings.add( value );
  }

  public int stringIndex( BossStringBuilder value )
  {
    return strings.add( value );
  }

  public int token( int type )
  {
    return token( type, 0 );
  }

  public int token( int type, int content )
  {
    BossIntList tokenData = this.tokenData;
    tokenData.reserve( 5 );
    int index = tokenData.count;
    tokenData.add( type );
    tokenData.add( content );
    tokenData.add( nextFilepath );
    tokenData.add( nextLine );
    tokenData.add( nextColumn );
    return index;
  }

  public int token( int type, double content )
  {
    reals.add( content );
    return token( type, reals.count - 1 );
  }

  public int token( int type, String content )
  {
    return token( type, stringIndex(content) );
  }

  public int type( int index )
  {
    return tokenData.get( index );
  }

}

