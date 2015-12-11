package compiler;

import alphavm.BardOp;

import java.io.*;

public class BardParseReader
{
  public String        filepath;
  public StringBuilder data = new StringBuilder();
  public int           position;
  public int           count;
  public int           line;
  public int           column;

  public BardParseReader()
  {
  }

  public BardParseReader( String filename, String data )
  {
    init( filename, data );
  }

  public BardParseReader( String filename )
  {
    init( filename );
  }

  public void init( String filename, String data )
  {
    this.filepath = filename;

    count = data.length();
    this.data.setLength( 0 );
    this.data.ensureCapacity( count );

    for (int i=0; i<count; ++i)
    {
      this.data.append( data.charAt(i) );
    }

    line = 1;
    column = 1;
    position = 0;
  }

  public void init( String filename )
  {
    this.filepath = filename;
    try
    {
      FileInputStream infile = new FileInputStream( filename );

      data.setLength( 0 );
      data.ensureCapacity( infile.available() );

      for (int ch=infile.read(); ch!=-1; ch=infile.read())
      {
        data.append( (char) ch );
      }

      count = data.length();

      line = 1;
      column = 1;
      position = 0;
    }
    catch (IOException err)
    {
      throw new BardError( "Cannot open \"" + filename + "\" for reading." );
    }
  }

  public boolean has_another()
  {
    return (position < count);
  }

  public char peek()
  {
    if (position == count) return (char)0;
    return data.charAt( position );
  }

  public char peek( int num_ahead )
  {
    --num_ahead;
    if (position+num_ahead >= count) return 0;
    return data.charAt( position+num_ahead );
  }

  public char read()
  {
    char result = data.charAt( position++ );
    if (result == '\n')
    {
      ++line;
      column = 1;
    }
    else
    {
      ++column;
    }
    return result;
  }

  //public String read_line()
  //{
  //  StringBuilder buffer = new StringBuilder();
  //  while (has_another())
  //  {
  //    char ch = read();
  //    if (ch == '\n') return buffer.toString();
  //    buffer.append( ch );
  //  }
  //  return buffer.toString();
  //}

  public boolean consume( char ch )
  {
    if (position == count || data.charAt( position ) != ch) return false;
    read();
    return true;
  }

  public boolean consume( String st )
  {
    int count = st.length();
    for (int i=0; i<count; ++i)
    {
      if (peek(i+1) != st.charAt(i)) return false;
    }

    ++count;
    while (--count > 0) read();  // necessary to adjust the line & column

    return true;
  }
}

