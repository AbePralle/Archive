package compiler;

import alphavm.BardOp;

import java.util.*;

public class BardMethodList extends ArrayList<BardMethod>
{
  public int write_pos;

  public void save( BardMethod m )
  {
    if (write_pos < size()) set( write_pos, m );
    else                    add( m );
    ++write_pos;
  }

  public void load()
  {
    while (write_pos > size()) remove( size() - 1 );
    write_pos = 0;
  }

  public int index_of( int method_id )
  {
    int i = 0;
    for (BardMethod m : this)
    {
      if (m.method_id == method_id) return i;
      ++i;
    }
    return -1;
  }

  public BardMethod find( int method_id )
  {
    for (BardMethod m : this)
    {
      if (m.method_id == method_id) return m;
    }
    return null;
  }

  public String toString()
  {
    StringBuilder buffer = new StringBuilder();
    buffer.append('{');
    boolean first = true;
    for (BardMethod m : this)
    {
      if (first) first = false;
      else       buffer.append( ',' ); 
      buffer.append( m.signature ); 
    }
    buffer.append('}');
    return buffer.toString();
  }
}

