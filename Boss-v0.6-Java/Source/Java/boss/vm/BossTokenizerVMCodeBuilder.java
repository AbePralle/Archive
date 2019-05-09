package boss.vm;

import java.util.*;

public class BossTokenizerVMCodeBuilder
{
  final static public int
    CMD_RETURN                      =  0,
    CMD_SET_SOURCE_POS              =  1,
    CMD_CONSUME_WHITESPACE          =  2,
    CMD_CLEAR_A                     =  3,
    CMD_CLEAR_BUFFER                =  4,
    CMD_CLEAR_C                     =  5,
    CMD_INCREMENT                   =  6,  // ++c; delta = c
    CMD_DECREMENT                   =  7,  // --c; delta = c
    CMD_READ                        =  8,
    CMD_PEEK                        =  9,  // lookahead:Int32
    CMD_COMPARE_TO                  = 10,  // value:Int32
    CMD_JUMP                        = 11,  // address
    CMD_HAS_ANOTHER                 = 14,
    CMD_PRINT_CHAR                  = 15,
    CMD_PRINT_STRING                = 16,  // index:Int32
    CMD_ERROR                       = 17,  // index:Int32
    CMD_IF_TRUE_THEN_TOKEN          = 18,  // tokenType:Int32
    CMD_IS_ALPHA                    = 20,
    CMD_IS_DIGIT                    = 21,
    CMD_READ_TO_BUFFER              = 24,
    CMD_CREATE_STRING_TOKEN         = 25,
    CMD_CREATE_TOKEN                = 26,  // tokenType:Int32
    CMD_CHECK_KEYWORD_TABLE         = 27,  // tableSize:Int32 (followed by table[tableSize]:Int32)
    CMD_JUMP_IF_EQ                  = 28,  // address:Int32
    CMD_JUMP_IF_NE                  = 29,  // address:Int32
    CMD_CONSUME                     = 30,  // value:Int32, EQ/TRUE on success
    CMD_CREATE_INT32_TOKEN          = 31,  // base:Int32
    CMD_CREATE_REAL64_TOKEN         = 32,
    CMD_TRACE                       = 33,
    CMD_CALL                        = 34,  // address:Int32
    CMD_ACCEPT_AND_RETURN           = 35,  // checkFor:Int32 mapTo:Int32

    CMD_JUMP_IF_TRUE                = CMD_JUMP_IF_EQ,  // address:Int32
    CMD_JUMP_IF_FALSE               = CMD_JUMP_IF_NE;  // address:Int32

  static public class Backpatch
  {
    public String label;
    public int    address;
    public Backpatch (String label, int address)
    {
      this.label = label;
      this.address = address;
    }
  }

  static public class ScanNode
  {
    int     index;

    char    character;
    int     acceptTokenType;  // accept token type else 0
    ArrayList<ScanNode> links = new ArrayList<ScanNode>();

    ScanNode( char ch )
    {
      character = ch;
    }

    void collectNodes( ArrayList<ScanNode> nodes )
    {
      nodes.add( this );
      for (ScanNode node : links)
      {
        node.collectNodes( nodes );
      }
    }

    ScanNode link( char ch )
    {
      for (int i=links.size(); --i>=0;)
      {
        ScanNode cur = links.get( i );
        if (cur.character == ch) return cur;
      }
      ScanNode result = new ScanNode( ch );
      links.add( result );
      return result;
    }

    int size()
    {
      // 2 for acceptTokenType & linkCount + 2 per link
      return 2 + links.size() * 2;
    }
  }

  public BossInt32List           code = new BossInt32List();
  public HashMap<String,Integer> labels = new HashMap<String,Integer>();
  public ArrayList<Backpatch>    backpatchList = new ArrayList<Backpatch>();

  public byte[] toArray( BossTokenData tokenData )
  {
    // Keyword scan table
    ScanNode keywordScanTree = new ScanNode( (char)0 );
    for (int k=BossTokenType.FIRST_KEYWORD; k<=BossTokenType.LAST_KEYWORD; ++k)
    {
      ScanNode cur = keywordScanTree;
      String keyword = tokenData.typeName( k );
      for (int i=0; i<keyword.length(); ++i)
      {
        cur = cur.link( keyword.charAt(i) );
      }
      cur.acceptTokenType = k;
    }

    ArrayList<ScanNode> allNodes = new ArrayList<ScanNode>();
    keywordScanTree.collectNodes( allNodes );

    int tableSize = 0;
    for (ScanNode node : allNodes)
    {
      node.index = tableSize;
      tableSize += node.size();
    }
    int[] keywordScanTable = new int[ tableSize ];
    for (ScanNode node : allNodes)
    {
      int index = node.index;
      keywordScanTable[ index   ] = node.acceptTokenType;
      keywordScanTable[ index+1 ] = node.links.size();
      for (int i=0; i<node.links.size(); ++i)
      {
        keywordScanTable[ index+2+i*2   ] = node.links.get( i ).character;
        keywordScanTable[ index+2+i*2+1 ] = node.links.get( i ).index;
      }
    }

    // Symbol scan table
    ScanNode symbolScanTree = new ScanNode( (char)0 );
    for (int k=BossTokenType.FIRST_SYMBOL; k<=BossTokenType.LAST_SYMBOL; ++k)
    {
      ScanNode cur = symbolScanTree;
      String symbol = tokenData.typeName( k );
      for (int i=0; i<symbol.length(); ++i)
      {
        cur = cur.link( symbol.charAt(i) );
      }
      cur.acceptTokenType = k;
    }

    allNodes.clear();
    symbolScanTree.collectNodes( allNodes );

    tableSize = 0;
    for (ScanNode node : allNodes)
    {
      node.index = tableSize;
      tableSize += node.size();
    }
    int[] symbolScanTable = new int[ tableSize ];
    for (ScanNode node : allNodes)
    {
      int index = node.index;
      symbolScanTable[ index   ] = node.acceptTokenType;
      symbolScanTable[ index+1 ] = node.links.size();
      for (int i=0; i<node.links.size(); ++i)
      {
        symbolScanTable[ index+2+i*2   ] = node.links.get( i ).character;
        symbolScanTable[ index+2+i*2+1 ] = node.links.get( i ).index;
      }
    }

    write( CMD_JUMP, "while (reader.hasAnother)" );
    label( "while (reader.hasAnother) body" );

    write( CMD_CONSUME, 10 );
    write( CMD_IF_TRUE_THEN_TOKEN, BossTokenType.EOL );

    // check for identifier
    write( CMD_PEEK, 0 );
    write( CMD_IS_ALPHA );
    write( CMD_JUMP_IF_FALSE, "not identifier" );

    // scan identifier
    write( CMD_CLEAR_BUFFER );
    label( "read id loop" );
    write( CMD_READ_TO_BUFFER );
    write( CMD_PEEK, 0 );
    write( CMD_IS_ALPHA );
    write( CMD_JUMP_IF_TRUE, "read id loop" );
    write( CMD_IS_DIGIT );
    write( CMD_JUMP_IF_TRUE, "read id loop" );

    // See if buffered id is a keyword
    write( CMD_CHECK_KEYWORD_TABLE, keywordScanTable.length );
    for (int i=0; i<keywordScanTable.length; ++i)
    {
      write( keywordScanTable[i] );
    }

    label( "not identifier" );

    // Check for number
    write( CMD_PEEK, 0 );
    write( CMD_IS_DIGIT );
    write( CMD_JUMP_IF_TRUE, "tokenize number" );
    write( CMD_COMPARE_TO, '.' );
    write( CMD_JUMP_IF_NE, "not number" );
    write( CMD_PEEK, 1 );
    write( CMD_IS_DIGIT );
    write( CMD_JUMP_IF_NE, "not number" );

    label( "tokenize number" );
    write( CMD_CLEAR_BUFFER );

    // read whole number
    write( CMD_JUMP, "has another whole" );
    label( "read another whole" );
    write( CMD_READ_TO_BUFFER );
    label( "has another whole" );
    write( CMD_CONSUME, '_' );
    write( CMD_JUMP_IF_TRUE, "has another whole" );
    write( CMD_PEEK, 0 );
    write( CMD_IS_DIGIT );
    write( CMD_JUMP_IF_TRUE, "read another whole" );

    write( CMD_PEEK, 0 );
    write( CMD_COMPARE_TO, '.' );
    write( CMD_JUMP_IF_FALSE, "create int32 token" );
    write( CMD_PEEK, 1 );
    write( CMD_IS_DIGIT );
    write( CMD_JUMP_IF_TRUE, "continue reading decimal" );

    // stop with our integer
    label( "create int32 token" );
    write( CMD_CREATE_INT32_TOKEN, 10 );

    label( "continue reading decimal" );
    write( CMD_READ_TO_BUFFER ); // .

    write( CMD_JUMP, "has another decimal" );
    label( "read another decimal" );
    write( CMD_READ_TO_BUFFER );
    label( "has another decimal" );
    write( CMD_CONSUME, '_' );
    write( CMD_JUMP_IF_TRUE, "has another decimal" );
    write( CMD_PEEK, 0 );
    write( CMD_IS_DIGIT );
    write( CMD_JUMP_IF_TRUE, "read another decimal" );

    // e/E +/- ...
    write( CMD_PEEK, 0 );
    write( CMD_COMPARE_TO, 'e' );
    write( CMD_JUMP_IF_EQ, "scan exponent +/-" );
    write( CMD_COMPARE_TO, 'E' );
    write( CMD_JUMP_IF_NE, "create real64 token" );

    label( "scan exponent +/-" );
    write( CMD_READ_TO_BUFFER );  // e/E
    write( CMD_PEEK, 0 );
    write( CMD_COMPARE_TO, '+' );
    write( CMD_JUMP_IF_EQ, "copy exponent sign" );
    write( CMD_COMPARE_TO, '-' );
    write( CMD_JUMP_IF_NE, "scan exponent value" );

    label( "copy exponent sign" );
    write( CMD_READ_TO_BUFFER );  // +/-

    label( "scan exponent value" );
    write( CMD_JUMP, "has another exponent digit" );
    label( "read another exponent digit" );
    write( CMD_READ_TO_BUFFER );
    label( "has another exponent digit" );
    write( CMD_CONSUME, '_' );
    write( CMD_JUMP_IF_TRUE, "has another exponent digit" );
    write( CMD_PEEK, 0 );
    write( CMD_IS_DIGIT );
    write( CMD_JUMP_IF_TRUE, "read another exponent digit" );

    label( "create real64 token" );
    write( CMD_CREATE_REAL64_TOKEN );

    // optional decimal point and decimal portion
    //write( CMD_JUMP, "has another whole" );

    label( "not number" );

    // discard comment
    write( CMD_CONSUME, '#' );
    write( CMD_JUMP_IF_NE, "not comment" );
    write( CMD_CONSUME, '{' );
    write( CMD_JUMP_IF_EQ, "multiline comment" );

    label( "single line comment" );
    write( CMD_HAS_ANOTHER );
    write( CMD_JUMP_IF_FALSE, 0 );  // restart
    write( CMD_PEEK, 0 );
    write( CMD_COMPARE_TO, '\n' );
    write( CMD_JUMP_IF_TRUE, 0 );  // restart
    write( CMD_READ );
    write( CMD_JUMP, "single line comment" );

    label( "multiline comment" );
    write( CMD_CLEAR_C );
    write( CMD_INCREMENT );
    label( "read multiline comment" );
    write( CMD_HAS_ANOTHER );
    write( CMD_JUMP_IF_FALSE, "missing close comment" );
    write( CMD_READ );
    write( CMD_COMPARE_TO, '#' );
    write( CMD_JUMP_IF_FALSE, "multiline check close comment" );
    write( CMD_PEEK, 0 );
    write( CMD_COMPARE_TO, '{' );
    write( CMD_JUMP_IF_FALSE, "read multiline comment" );
    write( CMD_INCREMENT );
    label( "multiline check close comment" );
    write( CMD_COMPARE_TO, '}' );
    write( CMD_JUMP_IF_FALSE, "read multiline comment" );
    write( CMD_PEEK, 0 );
    write( CMD_COMPARE_TO, '#' );
    write( CMD_JUMP_IF_FALSE, "read multiline comment" );
    write( CMD_DECREMENT );
    write( CMD_JUMP_IF_NE, "read multiline comment" );
    write( CMD_JUMP, 0 );  // restart tokenizer program

    label( "missing close comment" );
    write( CMD_CLEAR_BUFFER    );
    write( CMD_PRINT_STRING, 2 );
    write( CMD_ERROR );

    label( "not comment" );

    write( CMD_CONSUME, '"' );
    write( CMD_JUMP_IF_NE, "not double quote string" );

    write( CMD_CLEAR_BUFFER );
    label( "read double quote character" );
    write( CMD_HAS_ANOTHER );
    write( CMD_JUMP_IF_FALSE, "missing close double quote" );
    write( CMD_READ );
    write( CMD_COMPARE_TO, '"' );
    write( CMD_JUMP_IF_EQ, "string ready" );
    write( CMD_CALL, "read character" );
    write( CMD_PRINT_CHAR );
    write( CMD_JUMP, "read double quote character" );

    label( "read character" );
    write( CMD_COMPARE_TO, '\\' );
    write( CMD_JUMP_IF_NE, "return character" );
    write( CMD_READ );
    write( CMD_ACCEPT_AND_RETURN, 'n', '\n' );
    write( CMD_ACCEPT_AND_RETURN, 'r', '\r' );
    write( CMD_ACCEPT_AND_RETURN, 't', '\t' );
    write( CMD_ACCEPT_AND_RETURN, '"', '"' );

    label( "return character" );
    write( CMD_RETURN );

    label( "string ready" );
    write( CMD_CREATE_STRING_TOKEN );

    label( "missing close double quote" );
    write( CMD_CLEAR_BUFFER    );
    write( CMD_PRINT_STRING, 3 );
    write( CMD_ERROR );

    label( "not double quote string" );

    write( CMD_CLEAR_BUFFER    );
    write( CMD_PEEK, 0 );
    write( CMD_PRINT_STRING, 0 );
    write( CMD_PRINT_CHAR    );
    write( CMD_PRINT_STRING, 1 );
    write( CMD_ERROR );

    label( "while (reader.hasAnother)" );
    write( CMD_CONSUME_WHITESPACE );
    write( CMD_SET_SOURCE_POS );
    write( CMD_HAS_ANOTHER );
    write( CMD_JUMP_IF_TRUE, "while (reader.hasAnother) body" );
    write( CMD_RETURN );

    System.out.println( "Original code count: " + code.count );
    int byteSize = 0;
    for (int i=0; i<code.count; ++i)
    {
      int value = code.get( i );
      if (value >= 0 && value <= 254) ++byteSize;
      else                            byteSize += 5;
    }
    byte[] bytes = new byte[ byteSize ];
    byteSize = 0;
    for (int i=0; i<code.count; ++i)
    {
      int value = code.get(i);
      if (value >= 0 && value <= 254)
      {
        bytes[ byteSize++ ] = (byte) value;
      }
      else
      {
        bytes[ byteSize++ ] = (byte) -1;
        bytes[ byteSize++ ] = (byte)(value >> 24);
        bytes[ byteSize++ ] = (byte)(value >> 16);
        bytes[ byteSize++ ] = (byte)(value >> 8);
        bytes[ byteSize++ ] = (byte)value;
      }
    }
    return bytes;
  }

  public void label( String text )
  {
    if (labels.get(text) != null) throw new RuntimeException( "Duplicate label definition" );
    labels.put( text, code.count );
//System.out.println( "[" + text + " @" + code.count + "]" );

    for (int i=backpatchList.size(); --i>=0;)
    {
      Backpatch patch = backpatchList.get( i );
      if (patch.label.equals(text))
      {
        code.set( patch.address, code.count );
        backpatchList.remove( i );
      }
    }
  }

  public void write( int opcode )
  {
    code.add( opcode );
  }

  public void write( String label )
  {
    if (labels.get(label) != null)
    {
      write( (int)labels.get(label) );
    }
    else
    {
      backpatchList.add( new Backpatch(label,code.count) );
      write( 0 );
    }
  }

  public void write( int opcode, int arg1 )
  {
    code.add( opcode );
    code.add( arg1 );
  }

  public void write( int opcode, String label )
  {
    code.add( opcode );
    write( label );
  }

  public void write( int opcode, int arg1, int arg2 )
  {
    code.add( opcode );
    code.add( arg1 );
    code.add( arg2 );
  }

  public void write( int opcode, int arg1, String label )
  {
    code.add( opcode );
    code.add( arg1 );
    write( label );
  }
}
