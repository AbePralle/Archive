package alphavm;

import java.util.*;

public class BardMM
{
  static public ObjectArrayList[]  Object_array_pool = new ObjectArrayList[32];
  static public IntegerArrayList[] Integer_array_pool = new IntegerArrayList[32];
  // Slot [n] contains arrays of size 2^n, n = 0..31

  static int arrays_allocated = 0;

  static
  {
    for (int i=0; i<=31; ++i)
    {
      Object_array_pool[i] = new ObjectArrayList();
      Integer_array_pool[i] = new IntegerArrayList();
    }
  }

  static void release_all()
  {
    Object_array_pool = null;
    Integer_array_pool = null;
  }

  static public BardObject[] acquire_Object_array( int count )
  {
    int slot, slot_count;
    if (count == 0)
    {
      slot = 0;
      slot_count = 0;
    }
    else
    {
      slot = 1;
      slot_count = 1;
      while (slot_count < count)
      {
        ++slot;
        slot_count <<= 1;
      }
    }

    int size = Object_array_pool[slot].size();
    if (size > 0)
    {
      BardObject[] result = Object_array_pool[slot].remove( size - 1 );
      for (int i=result.length; --i>=0; )
      {
        result[i] = null;
      }
      return result;
    }
    else
    {
++arrays_allocated;
      return new BardObject[ slot_count ];
    }
  }

  static public int[] acquire_Integer_array( int count )
  {
    int slot, slot_count;
    if (count == 0)
    {
      slot = 0;
      slot_count = 0;
    }
    else
    {
      slot = 1;
      slot_count = 1;
      while (slot_count < count)
      {
        ++slot;
        slot_count <<= 1;
      }
    }

    int size = Integer_array_pool[slot].size();
    if (size > 0)
    {
      int[] result = Integer_array_pool[slot].remove( size - 1 );
      for (int i=result.length; --i>=0; )
      {
        result[i] = 0;
      }
      return result;
    }
    else
    {
++arrays_allocated;
      return new int[ slot_count ];
    }
  }

  static public void release( BardObject[] array )
  {
    if (array.length == 0)
    {
      Object_array_pool[0].add( array );
    }
    else
    {
      int length = array.length;
      int slot = 1;
      int slot_count = 1;
      while (slot_count < length)
      {
        ++slot;
        slot_count <<= 1;
      }
      Object_array_pool[slot].add( array );
    }
  }

  static public void release( int[] array )
  {
    if (array.length == 0)
    {
      Integer_array_pool[0].add( array );
    }
    else
    {
      int length = array.length;
      int slot = 1;
      int slot_count = 1;
      while (slot_count < length)
      {
        ++slot;
        slot_count <<= 1;
      }
      Integer_array_pool[slot].add( array );
    }
  }

  static public BardObject[] replace_Object_array( BardObject[] old_array, int new_count )
  {
    BardObject[] result = acquire_Object_array( new_count );
    if (old_array != null) release( old_array );
    return result;
  }

  static public int[] replace_Integer_array( int[] old_array, int new_count )
  {
    int[] result = acquire_Integer_array( new_count );
    if (old_array != null) release( old_array );
    return result;
  }

  static public BardObject[] resize_Object_array( BardObject[] old_array, int old_count, int new_count )
  {
    BardObject[] new_array = acquire_Object_array( new_count );
    System.arraycopy( old_array, 0, new_array, 0, old_count );
    release( old_array );
    return new_array;
  }

  static public int[] resize_Integer_array( int[] old_array, int old_count, int new_count )
  {
    int[] new_array = acquire_Integer_array( new_count );
    System.arraycopy( old_array, 0, new_array, 0, old_count );
    release( old_array );
    return new_array;
  }

  static public class ObjectArrayList extends ArrayList<BardObject[]>
  {
  }

  static public class IntegerArrayList extends ArrayList<int[]>
  {
  }
}

