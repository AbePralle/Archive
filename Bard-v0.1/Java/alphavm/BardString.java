package alphavm;

import java.util.*;

public class BardString extends BardObject
{
  public char[] data;
  public int    count;
  public int    hash_code;
  public int    write_pos;

  public BardString( BardType type, int capacity )
  {
    super( type );
//System.out.println( "New String" );
    if (capacity > 0)
    {
      data = new char[ capacity ];
//System.out.println( "New Character[]" );
    }
  }

  public BardString( BardType type, String value )
  {
    super( type );

    count = value.length();
    data = new char[ count ];
//System.out.println( "New String, new data[" +count+"] (" +value+")" );
    for (int i=0; i<count; ++i)
    {
      char ch = value.charAt(i);
      data[i] = ch;
      hash_code = ((hash_code<<3) - hash_code) + ch;  // code = code * 7 + ch
    }
  }

  public BardString substring( int i1, int i2 )
  {
    int new_count = (i2 - i1) + 1;

    if (new_count == 0) return new BardString( type, "" );
    if (new_count == this.count) return this;

    BardString result = new BardString( type, new_count );
    result.count = new_count;

    for (int i=new_count; --i>=0; )
    {
      result.data[i] = data[i+i1];
    }
    
    result.set_hash_code();
    return result;
  }

  public void set_hash_code()
  {
    hash_code = 0;
    for (char ch : data)
    {
      hash_code = ((hash_code<<3) - hash_code) + ch;  // code = code * 7 + ch
    }
  }

  public int hashCode() { return hash_code; }

  public boolean equals( Object other_object )
  {
    if (other_object instanceof BardString)
    {
      BardString other = (BardString) other_object;

      if (hash_code != other.hash_code) return false;
      int c = this.count;
      if (c != other.count) return false;

      char[] data1 = this.data;
      char[] data2 = other.data;
      while (--c >= 0)
      {
        if (data1[c] != data1[c]) return false;
      }
      return true;
    }
    else if (other_object instanceof String)
    {
      String other = (String) other_object;
      int c = this.count;
      if (c != other.length()) return false;

      char[] this_data = this.data;
      while (--c >= 0)
      {
        if (this_data[c] != other.charAt(c)) return false;
      }
      return true;
    }
    else
    {
      return false;
    }
  }

  public String toString()
  {
    StringBuilder buffer = new StringBuilder();
    for (int i=0; i<count; ++i)
    {
      buffer.append( data[i] );
    }
    return buffer.toString();
  }
}
