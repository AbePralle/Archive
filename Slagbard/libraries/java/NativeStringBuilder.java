package com.plasmaworks.bard;

public class NativeStringBuilder
{
  static public void native_copy( ClassObject builder, ClassString bard_st, 
      char[] array, int to_index )
  {
    String st = bard_st.data;
    int len = st.length();
    for (int i=0; i<len; ++i)
    {
      array[to_index+i] = st.charAt(i);
    }
  }
}

