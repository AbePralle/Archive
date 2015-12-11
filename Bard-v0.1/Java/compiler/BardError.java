package compiler;

import alphavm.BardOp;

public class BardError extends RuntimeException
{
  public String message;
  public String filename;
  public int    line;
  public int    column;

  public BardError( String message )
  {
    super( message );
    this.message = message;
  }

  public BardError( String message, String filename, int line, int column )
  {
    super( message );
    this.message = message;
    this.filename = filename;
    this.line = line;
    this.column = column;
  }

  public String toString()
  {
    if (line > 0) return "ERROR in \"" + filename + "\" line " + line + ", column " + column + ": " + message;
    return message;
  }
}

