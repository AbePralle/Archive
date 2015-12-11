package vm;

import java.io.*;

public class BardFileWriter extends BardObject
{
  public String       filepath;
  public OutputStream outfile;

  public BardFileWriter( BardType type )
  {
    super( type );
  }

  public void init( String filepath )
  {
    this.filepath = filepath;
    try
    {
      outfile = new FileOutputStream( filepath );
    }
    catch (IOException err)
    {
    }
  }

  public void write( int ch )
  {
    try
    {
      outfile.write( ch );
    }
    catch (IOException err)
    {
      try
      {
        outfile.close();
        outfile = null;
      }
      catch (IOException oh_well)
      {
      }
    }
  }

  public void close()
  {
    try
    {
      outfile.close();
      outfile = null;
    }
    catch (IOException err)
    {
    }
  }
}
