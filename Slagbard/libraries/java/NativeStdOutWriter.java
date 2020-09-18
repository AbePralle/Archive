package com.plasmaworks.bard;


public class NativeStdOutWriter
{
  static public StringBuilder log_buffer = new StringBuilder();

  static public void write( ClassStdOutWriter obj, char ch )
  {
    if (ch == 10)
    {
      Bard.log(log_buffer.toString());
      log_buffer.setLength(0);
    }
    else
    {
      log_buffer.append(ch);
    }
  }

  static public void print( ClassStdOutWriter obj, char ch )
  {
    if (ch == 10)
    {
      Bard.log( log_buffer.toString() );
      log_buffer.setLength(0);
    }
    else
    {
      log_buffer.append(ch);
    }
  }

  static public void print( ClassStdOutWriter obj, ClassString st )
  {
    Bard.log( st.data );
  }

  static public void flush( ClassStdOutWriter obj )
  {
  }
}

