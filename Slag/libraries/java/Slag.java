//=============================================================================
// Bard.java
//
// $(PLASMACORE_VERSION) $(DATE)
//
// Bard standard library Native Layer implementation for Java.
// 
// ----------------------------------------------------------------------------
//
// Copyright 2011 Plasmaworks LLC
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//
//   http://www.apache.org/licenses/LICENSE-2.0 
//
// Unless required by applicable law or agreed to in writing, 
// software distributed under the License is distributed on an 
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
// either express or implied. See the License for the specific 
// language governing permissions and limitations under the License.
//
//=============================================================================

package com.plasmaworks.bard;

import java.util.Arrays;

public class Bard
{
  static public enum CallInit
  {
    FALSE, TRUE
  }

  static public void configure()
  {
    BardSingletons.singleton_ClassGlobal.property_stdout = new ClassStdOutWriter();
  }

  static public void log( String mesg )
  {
    System.out.println( mesg );
  }

  static public ClassString str( String st )
  {
    return new ClassString(st);
  }

  static public String cs_str( ClassString st )
  {
    return st.data;
  }

  static public double mod( double a, double b )
  {
    double q = a / b;
    return (double)(a - Math.floor(q)*b);
  }

  static public float mod( float a, float b )
  {
    float q = a / b;
    return (float)(a - Math.floor(q)*b);
  }

  static public long mod( long a, long b )
  {
    if (b == 1)
    {
      return 0;
    }
    else if ((a^b) < 0)
    {
      long r = a % b;
      return ((r!=0) ? (r+b) : 0);
    }
    else
    {
      return (a % b);
    }
  }

  static public int mod( int a, int b )
  {
    if (b == 1)
    {
      return 0;
    }
    else if ((a^b) < 0)
    {
      int r = a % b;
      return ((r!=0) ? (r+b) : 0);
    }
    else
    {
      return (a % b);
    }
  }

  static public long shr( long n, int count )
  {
    if (count == 0) return n;
    n = (n >> 1) & 0x7FFFffffFFFFffffL;
    if (count > 1)
    {
      n >>= (count-1);
    }
    return n;
  }

  static public int shr( int n, int count )
  {
    if (count == 0) return n;
    n = (n >> 1) & 0x7FFFffff;
    if (count > 1)
    {
      n >>= (count-1);
    }
    return n;
  }

  static public void clear_array( Object[] array, int i1, int i2 )
  {
    Arrays.fill( array, i1, i2+1, null );
  }

  static public void clear_array( double[] array, int i1, int i2 )
  {
    Arrays.fill( array, i1, i2+1, 0.0 );
  }

  static public void clear_array( float[] array, int i1, int i2 )
  {
    Arrays.fill( array, i1, i2+1, 0.0f );
  }

  static public void clear_array( long[] array, int i1, int i2 )
  {
    Arrays.fill( array, i1, i2+1, 0L );
  }

  static public void clear_array( int[] array, int i1, int i2 )
  {
    Arrays.fill( array, i1, i2+1, 0 );
  }

  static public void clear_array( char[] array, int i1, int i2 )
  {
    Arrays.fill( array, i1, i2+1, (char) 0 );
  }

  static public void clear_array( byte[] array, int i1, int i2 )
  {
    Arrays.fill( array, i1, i2+1, (byte) 0 );
  }

  static public void clear_array( boolean[] array, int i1, int i2 )
  {
    Arrays.fill( array, i1, i2+1, false );
  }

  static public Object[] array_duplicate( Object[] array )
  {
    if (array == null) return null;

    int count = array.length;
    Object[] result = new Object[count];
    System.arraycopy( array, 0, result, 0, count );
    return result;
  }

  static public double[] array_duplicate( double[] array )
  {
    if (array == null) return null;

    int count = array.length;
    double[] result = new double[count];
    System.arraycopy( array, 0, result, 0, count );
    return result;
  }

  static public float[] array_duplicate( float[] array )
  {
    if (array == null) return null;

    int count = array.length;
    float[] result = new float[count];
    System.arraycopy( array, 0, result, 0, count );
    return result;
  }

  static public long[] array_duplicate( long[] array )
  {
    if (array == null) return null;

    int count = array.length;
    long[] result = new long[count];
    System.arraycopy( array, 0, result, 0, count );
    return result;
  }

  static public int[] array_duplicate( int[] array )
  {
    if (array == null) return null;

    int count = array.length;
    int[] result = new int[count];
    System.arraycopy( array, 0, result, 0, count );
    return result;
  }

  static public char[] array_duplicate( char[] array )
  {
    if (array == null) return null;

    int count = array.length;
    char[] result = new char[count];
    System.arraycopy( array, 0, result, 0, count );
    return result;
  }

  static public byte[] array_duplicate( byte[] array )
  {
    if (array == null) return null;

    int count = array.length;
    byte[] result = new byte[count];
    System.arraycopy( array, 0, result, 0, count );
    return result;
  }

  static public boolean[] array_duplicate( boolean[] array )
  {
    if (array == null) return null;

    int count = array.length;
    boolean[] result = new boolean[count];
    System.arraycopy( array, 0, result, 0, count );
    return result;
  }

  //static public class FileReaderInfo extends Data
  //{
    //public byte[] data;
    //public int    pos;
    //public FileReaderInfo( String filename )
    //{
      //try
      //{
        //FileInputStream infile = new FileInputStream(filename);
        //data = new byte[infile.available()];
        //infile.read( data );
      ////}
      //catch (IOException err)
      //{
      //}
    //}
  //}

}

