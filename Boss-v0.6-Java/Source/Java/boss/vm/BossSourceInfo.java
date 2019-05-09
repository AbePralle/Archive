package boss.vm;

// Source code tokens are stored efficiently in BossTokenData. However,
// that data is cleared after parsing.  TokenData tokens are converted to
// BossSourceInfo for long-term retention in types and methods.

public class BossSourceInfo
{
  public String filepath;
  public int    line;
  public int    column;

  public BossSourceInfo( String filepath, int line, int column )
  {
    this.filepath = filepath;
    this.line = line;
    this.column = column;
  }

  public BossError error( String message )
  {
    return new BossError( message, filepath, line, column );
  }
}
