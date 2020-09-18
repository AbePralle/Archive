package com.plasmaworks.bard;

public class NativeString
{
  static public boolean opEQ( ClassString st1, ClassString st2 )
  {
    if (st1 == st2)  return true;
    if (st2 == null) return false;

    if (st1.hash_code != st2.hash_code) return false;

    int count = st1.data.length();
    if (count != st2.data.length()) return false;

    String data1 = st1.data;
    String data2 = st2.data;

    for (int i=0; i<count; ++i)
    {
      if (data1.charAt(i) != data2.charAt(i)) return false;
    }

    return true;
  }

  static public int opCMP( ClassString st1, ClassString st2 )
  {
    if (st1 == st2)  return 0;
    if (st2 == null) return 1;

    int count = st1.data.length();
    if (count > st2.data.length()) count = st2.data.length();

    String data1 = st1.data;
    String data2 = st2.data;

    for (int i=0; i<count; ++i)
    {
      if (data1.charAt(i) != data2.charAt(i))
      {
        if (data1.charAt(i) < data2.charAt(i)) return -1;
        else                                   return  1;
      }
    }

    // Equal so far
    if (count < st1.data.length()) return  1;
    if (count < st2.data.length()) return -1;
    return 0;
  }

  static public ClassString substring( ClassString st, int i1, int i2 )
  {
    return new ClassString( st.data.substring(i1,i2+1) );
  }

  static public char[] to_Array( ClassString bard_st )
  {
    String st = bard_st.data;

    int count = st.length();
    char[] result = new char[ count ];
    for (int i=0; i<count; ++i)
    {
      result[i] = st.charAt(i);
    }
    return result;
  }
}

