package alphavm;

import java.util.*;

public class BardPropertyList extends ArrayList<BardProperty>
{
  public int write_pos;

  public void save( BardProperty p )
  {
    if (write_pos < size()) set( write_pos, p );
    else                    add( p );
    ++write_pos;
  }

  public void load()
  {
    while (write_pos > size()) remove( size() - 1 );
    write_pos = 0;
  }

  public BardProperty find( int property_id )
  {
    for (int i=size(); --i>=0; )
    {
      BardProperty p = get(i);
      if (p.property_id == property_id) return p;
    }
    return null;
  }
}

