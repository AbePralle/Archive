package com.plasmaworks.bard;

public class NativeStringManager
{
  static public ClassString create_from( ClassObject context, char[] array, int count )
  {
    if (count == -1) count = array.length;
    StringBuilder builder = new StringBuilder(count);
    for (int i=0; i<count; ++i) builder.append( array[i] );
    return new ClassString( builder.toString() );
  }

  static public ClassString create_from( ClassObject context, char ch )
  {
    return new ClassString( ""+ch );
  }
}

