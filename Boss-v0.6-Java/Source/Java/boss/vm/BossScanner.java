package boss.vm;

import java.io.*;

public class BossScanner
{
  //public BossStringBuilder parseBuffer = new BossStringBuilder();

  public String filepath;
  public BossStringBuilder data;
  public int count;
  public int position;
  public int line;
  public int column;

  public BossScanner( String filepath, String source )
  {
    this.filepath = filepath;
    count = source.length();
    data = new BossStringBuilder( count );
    if (count >= 1 && source.charAt(0) == 0xFEFF)
    {
      // Discard Byte Order Mark (BOM)
      for (int i=1; i<count; ++i)
      {
        data.print( source.charAt(i) );
      }
    }
    else if (count >= 3 && source.charAt(0) == 0xEF && source.charAt(1) == 0xBB && source.charAt(2) == 0xBF)
    {
      // Discard Byte Order Mark (BOM)
      for (int i=3; i<count; ++i)
      {
        data.print( source.charAt(i) );
      }
    }
    else
    {
      data.print( source );
    }
    line = 1;
    column = 1;
  }

  public BossScanner( File file )
  {
    filepath = file.getAbsolutePath();
    try
    {
      count = (int) file.length();
      data = new BossStringBuilder( count );
      BufferedReader reader = new BufferedReader( new InputStreamReader( new FileInputStream(file.getPath()) ), 1024 );

      int firstCh = reader.read();
      if (firstCh != -1 && firstCh != 0xFEFF)
      {
        // Discard Byte Order Mark (BOM)
        data.print( (char) firstCh );
      }

      for (int ch=reader.read(); ch!=-1; ch=reader.read())
      {
        data.print( (char) ch );
      }
      count = data.length();
      reader.close();
    }
    catch (Exception err)
    {
      data = new BossStringBuilder();
    }
    line = 1;
    column = 1;
  }

  public boolean consume( char ch )
  {
    if (position == count) return false;
    if (ch != data.charAt(position)) return false;
    read();
    return true;
  }

  public boolean consume( String value )
  {
    int n = value.length();
    if ( !hasAnother(n) ) return false;
    for (int i=n; --i>=0;)
    {
      if (peek(i) != value.charAt(i)) return false;
    }
    for (int i=n; --i>=0;) read();
    return true;
  }

  public BossError error( String message )
  {
    return new BossError( message, filepath, line, column );
  }

  public boolean hasAnother( int additional )
  {
    return (position+additional <= count);
  }

  public char peek( int lookahead )
  {
    if (position+lookahead >= count) return (char) 0;
    return data.charAt( position+lookahead );
  }

  public char read()
  {
    char ch = data.charAt( position++ );
    switch (ch)
    {
      case '\t': column += 4; return ch;
      case '\n': ++line; column=1; return ch;
      default:   ++column; return ch;
    }
  }
}
