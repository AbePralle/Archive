package compiler;

import alphavm.BardOp;

import java.util.*;

public class BardArgs extends ArrayList<BardCmd>
{
  public BardArgs()
  {
  }

  public BardArgs( int capacity )
  {
    super( capacity );
  }

  public BardArgs( BardCmd cmd )
  {
    add( cmd );
  }

  public BardArgs duplicate() 
  { 
    BardArgs result = new BardArgs( size() );
    for (int i=0; i<size(); ++i)
    {
      result.add( get(i).duplicate() );
    }
    return result;
  }

  public String toString()
  {
    StringBuilder buffer = new StringBuilder();
    for (int i=0; i<size(); ++i)
    {
      if (i > 0) buffer.append( "," );
      buffer.append( get(i) );
    }
    return buffer.toString();
  }

  public int count()
  {
    return size();
  }

  public void resolve( BardMethod method_context )
  {
    BardCompiler bc = method_context.type_context.bc;
    for (int i=0; i<size(); ++i)
    {
      set( i, get(i).resolve(method_context) );
    }
  }

  public void compile( BardCodeWriter writer )
  {
    for (BardCmd arg : this) arg.compile( writer );
  }
}

