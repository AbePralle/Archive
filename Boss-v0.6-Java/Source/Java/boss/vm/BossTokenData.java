package boss.vm;

import java.io.*;
import java.util.*;

public class BossTokenData
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
  public BossInt32List        definitions = new BossInt32List( 256 );     // [flags, name string index]
  public BossInt32List        tokenData   = new BossInt32List( 8192 );    // [type, content value/index, filepath index, line, column]
  public BossReal64List     reals       = new BossReal64List( 512 );
  public BossStringIntTable strings     = new BossStringIntTable();

  String nextFilepath;
  int    nextFilepathIndex;
  int    nextLine;
  int    nextColumn;

  // METHODS
  public BossTokenData()
  {
    int isKeyword    = BossTokenType.IS_KEYWORD;
    int isSymbol     = BossTokenType.IS_SYMBOL;
    int isStructural = BossTokenType.IS_STRUCTURAL;

    define( BossTokenType.EOI,        "[end of input]", isStructural );
    define( BossTokenType.EOL,        "EOL",  0 );
    define( BossTokenType.IDENTIFIER, "identifier", 0 );
    define( BossTokenType.STRING,     "String", 0 );

    define( BossTokenType.REAL64,          "(Real64)", 0 );
    define( BossTokenType.REAL64_AS_INT32, "(Real64 as Int32)", 0 );
    define( BossTokenType.INT32,           "(Int32)", 0 );
    define( BossTokenType.CHARACTER,       "(Character)", 0 );

    define( BossTokenType.KEYWORD_CLASS,         "class",      isKeyword | isStructural );
    define( BossTokenType.KEYWORD_ELSE,          "else",       isKeyword | isStructural );
    define( BossTokenType.KEYWORD_ELSE_IF,       "elseIf",     isKeyword | isStructural );
    define( BossTokenType.KEYWORD_END_CLASS,     "endClass",   isKeyword | isStructural );
    define( BossTokenType.KEYWORD_END_IF,        "endIf",      isKeyword | isStructural );
    define( BossTokenType.KEYWORD_END_ROUTINE,   "endRoutine", isKeyword | isStructural );
    define( BossTokenType.KEYWORD_END_WHILE,     "endWhile",   isKeyword | isStructural );
    define( BossTokenType.KEYWORD_FALSE,         "false",      isKeyword );
    define( BossTokenType.KEYWORD_GLOBAL,        "global",     isKeyword );
    define( BossTokenType.KEYWORD_IF,            "if",         isKeyword );
    define( BossTokenType.KEYWORD_IMPORT,        "import",     isKeyword );
    define( BossTokenType.KEYWORD_LOCAL,         "local",      isKeyword );
    define( BossTokenType.KEYWORD_METHOD,        "method",     isKeyword | isStructural );
    define( BossTokenType.KEYWORD_METHODS,       "METHODS",    isKeyword | isStructural );
    define( BossTokenType.KEYWORD_NULL,          "null",       isKeyword );
    define( BossTokenType.KEYWORD_PRINTLN,       "println",    isKeyword );
    define( BossTokenType.KEYWORD_PROPERTIES,    "PROPERTIES", isKeyword | isStructural );
    define( BossTokenType.KEYWORD_RETURN,        "return",     isKeyword );
    define( BossTokenType.KEYWORD_ROUTINE,       "routine",    isKeyword | isStructural );
    define( BossTokenType.KEYWORD_THIS_CALL,     "thisCall",   isKeyword | isStructural );
    define( BossTokenType.KEYWORD_TRUE,          "true",       isKeyword );
    define( BossTokenType.KEYWORD_UNDEFINED,     "undefined",  isKeyword );
    define( BossTokenType.KEYWORD_WHILE,         "while",      isKeyword );

    define( BossTokenType.SYMBOL_AMPERSAND,      "&",   isSymbol );
    define( BossTokenType.SYMBOL_ARROW,          "->",  isSymbol );
    define( BossTokenType.SYMBOL_ASTERISK,       "*",   isSymbol );
    define( BossTokenType.SYMBOL_AT,             "@",   isSymbol );
    define( BossTokenType.SYMBOL_BANG,           "!",   isSymbol );
    define( BossTokenType.SYMBOL_CARET,          "^",   isSymbol );
    define( BossTokenType.SYMBOL_CLOSE_BRACE,    "}",   isSymbol | isStructural );
    define( BossTokenType.SYMBOL_CLOSE_BRACKET,  "]",   isSymbol | isStructural );
    define( BossTokenType.SYMBOL_CLOSE_PAREN,    ")",   isSymbol | isStructural );
    define( BossTokenType.SYMBOL_COLON,          ":",   isSymbol );
    define( BossTokenType.SYMBOL_COLON_COLON,    "::",  isSymbol );
    define( BossTokenType.SYMBOL_COMMA,          ",",   isSymbol );
    define( BossTokenType.SYMBOL_DOLLAR,         "$",   isSymbol );
    define( BossTokenType.SYMBOL_EQUALS,         "=",   isSymbol );
    define( BossTokenType.SYMBOL_EQ,             "==",  isSymbol );
    define( BossTokenType.SYMBOL_GE,             ">=",  isSymbol );
    define( BossTokenType.SYMBOL_GT,             ">",   isSymbol );
    define( BossTokenType.SYMBOL_LEFT_SHIFT,     "<<",  isSymbol );
    define( BossTokenType.SYMBOL_LE,             "<=",  isSymbol );
    define( BossTokenType.SYMBOL_LT,             "<",   isSymbol );
    define( BossTokenType.SYMBOL_MINUS,          "-",   isSymbol );
    define( BossTokenType.SYMBOL_MINUS_MINUS,    "--",  isSymbol );
    define( BossTokenType.SYMBOL_NE,             "!=",  isSymbol );
    define( BossTokenType.SYMBOL_OPEN_BRACE,     "{",   isSymbol );
    define( BossTokenType.SYMBOL_OPEN_BRACKET,   "[",   isSymbol );
    define( BossTokenType.SYMBOL_OPEN_PAREN,     "(",   isSymbol );
    define( BossTokenType.SYMBOL_PERCENT,        "%",   isSymbol );
    define( BossTokenType.SYMBOL_PERIOD,         ".",   isSymbol );
    define( BossTokenType.SYMBOL_PLUS,           "+",   isSymbol );
    define( BossTokenType.SYMBOL_PLUS_PLUS,      "++",  isSymbol );
    define( BossTokenType.SYMBOL_QUESTION,       "?",   isSymbol );
    define( BossTokenType.SYMBOL_QUESTION_COLON, "?:",  isSymbol );
    define( BossTokenType.SYMBOL_RIGHT_SHIFT,    ">>",  isSymbol );
    define( BossTokenType.SYMBOL_RIGHT_SHIFT_X,  ">>>", isSymbol );
    define( BossTokenType.SYMBOL_SEMICOLON,      ";",   isSymbol );
    define( BossTokenType.SYMBOL_SLASH,          "/",   isSymbol );
    define( BossTokenType.SYMBOL_TILDE,          "~",   isSymbol );
    define( BossTokenType.SYMBOL_VERTICAL_BAR,   "|",   isSymbol );

    setPosition( "[BossVM]", 1, 1 );
  }

  public int column( int token )
  {
    return tokenData.get( token+TOKEN_DATA_COLUMN_OFFSET );
  }

  public int content( int token )
  {
    return tokenData.get( token+TOKEN_DATA_CONTENT_OFFSET );
  }

  public double contentReal64( int token )
  {
    return reals.get( content(token) );
  }

  public String contentString( int token )
  {
    return string( content(token) );
  }

  public int createToken( int type )
  {
    return createToken( type, 0 );
  }

  public int createToken( int type, int content )
  {
    BossInt32List tokenData = this.tokenData;
    tokenData.reserve( 5 );
    int index = tokenData.count;
    tokenData.add( type );
    tokenData.add( content );
    tokenData.add( nextFilepathIndex );
    tokenData.add( nextLine );
    tokenData.add( nextColumn );
    return index;
  }

  public int createToken( int type, double content )
  {
    reals.add( content );
    return createToken( type, reals.count - 1 );
  }

  public int createToken( int type, String content )
  {
    return createToken( type, stringIndex(content) );
  }

  public int createToken( int type, BossStringBuilder content )
  {
    return createToken( type, stringIndex(content) );
  }

  public void define( int tokenType, String name, int flags )
  {
    int index = tokenType * 2;
    definitions.expandToCount( index + 2 );

    definitions.set( index, flags );
    definitions.set( index+1, stringIndex(name) );
  }

  public BossError error( int token, String message )
  {
    return new BossError( message, filepath(token), line(token), column(token) );
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
    if (type(token) == BossTokenType.IDENTIFIER) return contentString( token );
    return string( definitions.get( tokenData.get(token)*2 + TOKEN_DEF_NAME_OFFSET ) );
  }

  public String quotedName( int token )
  {
    if (type(token) == BossTokenType.IDENTIFIER)     return "'" + contentString( token ) + "'";
    else if (type(token) <= BossTokenType.CHARACTER) return name(token);
    else                                             return "'" + name(token) + "'";
  }

  public String quotedTypeName( int tokenType )
  {
    if (tokenType <= BossTokenType.CHARACTER) return typeName(tokenType);
    else                                      return "'" + typeName(tokenType) + "'";
  }

  public void setPosition( String nextFilepath, int line, int column )
  {
    if (this.nextFilepath != nextFilepath && (this.nextFilepath == null || !this.nextFilepath.equals(nextFilepath)))
    {
      this.nextFilepath = nextFilepath;
      this.nextFilepathIndex = stringIndex( nextFilepath );
    }
    this.nextLine = line;
    this.nextColumn = column;
  }

  public String string( int index )
  {
    return strings.keyAt( index );
  }

  public int stringIndex( String value )
  {
    return strings.add( value );
  }

  public int stringIndex( BossStringBuilder value )
  {
    return strings.add( value );
  }

  public BossSourceInfo toSourceInfo( int token )
  {
    return new BossSourceInfo( filepath(token), line(token), column(token) );
  }

  public String toString( int token )
  {
    switch (type(token))
    {
      case BossTokenType.IDENTIFIER:      return contentString(token);
      case BossTokenType.STRING:          return "\"" + contentString(token) + "\"";
      case BossTokenType.REAL64:          return "" + contentReal64(token);
      case BossTokenType.REAL64_AS_INT32: return "" + content(token);
      case BossTokenType.INT32:           return "" + content(token);
      case BossTokenType.CHARACTER:       return "" + (char) content(token);
      default:                            return name(token);
    }
  }

  public int type( int index )
  {
    return tokenData.get( index );
  }

  public boolean typeIsSymbol( int tokenType )
  {
    return (definitions.get(tokenType*2) & BossTokenType.IS_SYMBOL) != 0;
  }

  public String typeName( int tokenType )
  {
    return string( definitions.get(tokenType*2+TOKEN_DEF_NAME_OFFSET) );
  }
}

