package compiler;

import alphavm.BardOp;

import java.util.*;

public class BardStatementList
{
  public ArrayList<BardCmd> data;
  public int     write_pos = 0;

  public BardStatementList()
  {
    data = new ArrayList<BardCmd>();
  }

  public BardStatementList( int capacity )
  {
    data = new ArrayList<BardCmd>( capacity );
  }

  public BardStatementList duplicate()
  {
    BardStatementList result = new BardStatementList( data.size() );
    for (int i=0; i<data.size(); ++i)
    {
      result.data.add( data.get(i).duplicate() );
    }
    return result;
  }

  public String toString()
  {
    StringBuilder buffer = new StringBuilder();
    for (BardCmd cmd : data)
    {
      if (cmd != null)
      {
        buffer.append( cmd.toString() );
        buffer.append( '\n' );
      }
    }
    return buffer.toString();
  }

  public void clear()
  {
    data.clear();
  }

  public int count()
  {
    return data.size();
  }

  public void add( BardCmd cmd )
  {
    data.add( cmd );
  }

  public void add( BardStatementList other )
  {
    data.addAll( other.data );
  }

  public void insert( BardCmd cmd )
  {
    data.add( 0, cmd );
  }

  public void save( BardCmd cmd )
  {
    if (write_pos < data.size())
    {
      data.set( write_pos++, cmd );
    }
    else
    {
      data.add( cmd );
      ++write_pos;
    }
  }

  public void load()
  {
    while (data.size() > write_pos) data.remove( data.size() - 1 );
    write_pos = 0;
  }

  public BardCmd get( int index )
  {
    return data.get( index );
  }

  public void resolve( BardMethod m )
  {
    for (BardCmd cmd : data)
    {
      cmd = cmd.resolve( m );
      if (cmd != null)
      {
        save( cmd );
      }
    }
    load();
  }

  public void compile( BardCodeWriter writer )
  {
    for (BardCmd cmd : data)
    {
      cmd.compile( writer );
      if (cmd.type() != null) writer.write( BardOp.pop_discard );
    }
  }
}

