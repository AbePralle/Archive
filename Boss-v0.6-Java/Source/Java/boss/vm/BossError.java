package boss.vm;

public class BossError extends RuntimeException
{
  public String message;
  public String filepath;
  public int    line;
  public int    column;

  public BossError( String message )
  {
    if (message.length() > 0)
    {
      char ch = message.charAt( 0 );
      if (ch >= 'a' && ch <= 'z')
      {
        message = (char)(ch+('A'-'a')) + message.substring(1);
      }
    }
    this.message = message;
  }

  public BossError( String message, String filepath, int line, int column )
  {
    this( message );
    this.filepath = filepath;
    this.line = line;
    this.column = column;
  }

  public void print()
  {
    for (int i=0; i<79; ++i) System.out.print( '=' );
    System.out.println();

    if (filepath != null)
    {
      System.out.print( "ERROR in \"" + filepath + "\"" );
      if (line > 0)
      {
        System.out.print( " line " + line + ", column " + column );
      }
      System.out.println();
      System.out.println();
      System.out.println( message );
    }
    else
    {
      System.out.println( "INTERNAL ERROR" );
      System.out.println();
      printStackTrace();
    }

    for (int i=0; i<79; ++i) System.out.print( '=' );
    System.out.println();
  }

  public String toString()
  {
    return message;
  }
}
