package boss.vm;

import java.io.*;
import java.util.*;
import java.nio.*;

// 1.39s old tokenizer 1,441,792 lines
// Tokenizer + Scanner + SymbolTable = 12,714 bytes

// Froley VM tokenizer:
// 2.3 seconds, 1,441,792 lines
// Class files w/bytecode:   17,714
// Class files w/o bytecode: 10,442 + 5,455 bytecode

public class BossVM
{
  public BossTokenData              tokenData     = new BossTokenData();
  public BossSymbolLookup           symbolLookup  = new BossSymbolLookup( tokenData );
  public BossStringIntTable         keywordLookup = new BossStringIntTable();
  public BossCmdData                cmdData       = new BossCmdData( this );
  public BossMM                     mm            = new BossMM();
  public ArrayList<BossModule>      moduleList    = new ArrayList<BossModule>();
  public HashMap<String,BossModule> moduleLookup  = new HashMap<String,BossModule>();

  public BossType typeInt32;

  public BossInt32List  code  = new BossInt32List();
  public BossReal64List stack = new BossReal64List();

  static public void main( String[] args )
  {
    new BossVM();
  }

  public BossVM()
  {
    try
    {
      for (int i=BossTokenType.FIRST_KEYWORD; i<=BossTokenType.LAST_KEYWORD; ++i)
      {
        keywordLookup.set( tokenData.typeName(i), i );
      }

      code.add( 0 );

      BossModule m = new BossModule( this, "Boss" );
      moduleList.add( m );
      moduleLookup.put( m.name, m );
      typeInt32 = m.defineType( null, "Int32" );
      typeInt32.genericType = BossType.INT32;

      //BossParser parser = new BossParser( this, "[BossVM]", "println 'Hello world!'" );
      //load( "[BossVM]", "println 1\nprintln 2" );
      //load( new File("Build.rogue") );

      //long startTime = System.currentTimeMillis();
      //int[] tokens = new BossTokenizer( this ).tokenize( new File("Test.rogue") );
      //long finishTime = System.currentTimeMillis();
      //System.out.println( (finishTime-startTime)/1000.0 );

      long startTime = System.currentTimeMillis();
      int[] tokens = new BossTokenizerVM( this ).tokenize( new File("Test.rogue") );
      long finishTime = System.currentTimeMillis();
      System.out.println( (finishTime-startTime)/1000.0 );

      System.out.println( "tokens: " + tokens.length );

      //long startTime = System.currentTimeMillis();
      //int[] tokens = new BossTokenizerVM( this ).tokenize( new File("Test2.rogue") );
      //long finishTime = System.currentTimeMillis();
      //System.out.println( (finishTime-startTime)/1000.0 );
      //for (int i=0; i<tokens.length; ++i)
      //{
      //  if (tokenData.type(tokens[i]) == BossTokenType.IDENTIFIER) System.out.print( "ID: " );
      //  System.out.println( tokenData.toString(tokens[i]) );
      //}
    }
    catch (BossError err)
    {
      err.print();
    }
  }

  public void execute( int ip )
  {
    int[] code = this.code.data;
    BossReal64List stack = this.stack;

    for (;;)
    {
      switch (code[ip++])
      {
        case BossOpcode.NOP:
          continue;
        case BossOpcode.RETURN_NIL:
          return;
        //case BossOpcode.RETURN_VALUE:
        //case BossOpcode.RETURN_COMPOUND:
        case BossOpcode.PUSH_INT32:
          stack.add( code[ip++] );
          continue;
        case BossOpcode.PRINT_INT32:
          System.out.print( (int)(long)stack.removeLast() );
          continue;
        case BossOpcode.PRINTLN:
          System.out.println();
          continue;

        default:
          throw new BossError( "[INTERNAL] TODO: BossVM.execute(" + code[ip-1] + ")" );
      }
    }
  }

  /*
  public BossModule load( File file )
  {
    String filepath = file.getAbsolutePath();
    BossModule m = moduleLookup.get( filepath );
    if (m != null) return m;  // already loaded

    m = new BossModule( this, filepath );
    moduleList.add( m );
    moduleLookup.put( filepath, m );
    m.parse( file );
    resolve();
    return m;
  }

  public BossModule load( String filename, String source )
  {
    BossModule m = new BossModule( this, filename );
    moduleList.add( m );
    m.parse( filename, source );
    resolve();
    return m;
  }
  */

  public void resolve()
  {
    for (int i=0; i<moduleList.size(); ++i)
    {
      BossModule m = moduleList.get( i );
      m.resolve();
      m.generateCode();
    }
    for (int i=0; i<moduleList.size(); ++i)
    {
      BossModule m = moduleList.get( i );
      m.launch();
    }
  }
}

