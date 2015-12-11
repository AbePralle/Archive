package vm;

import java.io.*;

public class BardFileReader extends BardObject
{
  public String          filepath;
  public FileInputStream infile;
  public int             next_ch;

  public BardFileReader( BardType type )
  {
    super( type );
  }

  public void init( String filepath )
  {
    this.filepath = filepath;
    try
    {
      infile = new FileInputStream( filepath );
      next_ch = infile.read();
    }
    catch (IOException err)
    {
    }
  }

  public int available()
  {
    try
    {
      return infile.available();
    }
    catch (IOException err)
    {
    }
    return -1;
  }

  public int read()
  {
    try
    {
      int result = next_ch;
      next_ch = infile.read();
      if (next_ch == -1)
      {
        infile.close();
        infile = null;
      }
      return result;
    }
    catch (IOException err)
    {
    }
    return -1;
  }

  public void close()
  {
    try
    {
      if (infile != null) 
      {
        infile.close();
        infile = null;
        next_ch = -1;
      }
    }
    catch (IOException err)
    {
    }
  }
}
