package alphavm;

public class BardTable extends BardObject
{
  public BardObject[]    keys;
  public BardObject[]    values;
  public int[]           hash_codes;
  public int[]           order;

  public int capacity; // number of bins (power of 2)
  public int limit;    // 70% of capacity
  public int count;    // 0..limit

  public BardObject test_key;

  public BardTable( BardType type )
  {
    super( type );
    init( 8 );
  }

  public BardTable( BardType type, int initial_capacity )
  {
    super( type );
    init( initial_capacity );
  }

  public BardTable init( int initial_capacity )
  {
    if (capacity < initial_capacity)
    {
      // Make capacity a power of 2
      capacity = 1;
      while (capacity < initial_capacity) capacity <<= 1;

      limit = (capacity * 7) / 10;
      count = 0;

      keys   = BardMM.replace_Object_array( keys, capacity );
      values = BardMM.replace_Object_array( values, capacity );
      hash_codes = BardMM.replace_Integer_array( hash_codes, capacity );
      order      = BardMM.replace_Integer_array( order, limit+1 );
    }
    return this;
  }

  public BardObject collect_references( BardObject list )
  {
    list = super.collect_references( list );

    BardObject[] keys   = this.keys;
    BardObject[] values = this.values;
    int i = capacity;
    while (--i >= 0)
    {
      BardObject obj = keys[i];
      if (obj != null && !obj.referenced)
      {
        obj.referenced = true;
        obj.next_object = list;
        list = obj;
      }

      obj = values[i];
      if (obj != null && !obj.referenced)
      {
        obj.referenced = true;
        obj.next_object = list;
        list = obj;
      }
    }

    return list;
  }

  public BardObject reset()
  {
    clear();
    return this;
  }

  public void clear()
  {
    for (int i=0; i<capacity; ++i)
    {
      keys[i] = null;
      values[i] = null;
    }
    count = 0;
  }

  public int find( BardObject key )
  {
    return find( key, key.hashCode() );
  }

  public int find( BardObject key, int hash_code )
  {
    int bin_mask = capacity - 1;
    int index = (hash_code & bin_mask);
    while (keys[index] != null)
    {
      if (hash_codes[index] == hash_code && key.equals(keys[index]))
      {
        return index;
      }
      index = (index + 1) & bin_mask;
    }

    return ~index;  // negative spot where new key can be added
  }

  public void set( BardObject key, BardObject value )
  {
    set( key, value, key.hashCode() );
  }

  public void set( BardObject key, BardObject value, int hash_code )
  {
    int slot = find( key, hash_code );
    if (slot >= 0)
    {
      // Replace existing value
      values[slot] = value;
    }
    else
    {
      // New key
      slot = ~slot;
      hash_codes[slot] = hash_code;
      keys[slot] = key;
      values[slot] = value;
      order[count] = slot;
      ++count;

      if (count >= limit)
      {
        ensure_capacity( capacity * 2 );
      }
    }
  }

  public BardObject key( int i )
  {
    return keys[ order[i] ];
  }

  public BardObject get( BardObject key )
  {
    if (key == null) return null;

    int slot = find( key, key.hashCode() );
    if (slot >= 0) 
    {
      return values[slot];
    }
    return null;
  }

  public void ensure_capacity( int requested_capacity )
  {
    if (requested_capacity <= capacity) return;

    BardObject[] original_keys = keys;
    BardObject[] original_values = values;
    int[]        original_hash_codes = hash_codes;
    int[]        original_order = order;
    int          original_count = count;

    // Prevent original data from being released just yet
    keys = null;
    values = null;
    hash_codes = null;
    order = null;

    // Reinitialize with larger arrays.
    init( requested_capacity );

    // Copy over original data
    for (int i=0; i<original_count; ++i)
    {
      int index = original_order[i];
      set( original_keys[index], original_values[index], original_hash_codes[index] );
    }

    BardMM.release( original_keys );
    BardMM.release( original_values );
    BardMM.release( original_hash_codes );
    BardMM.release( original_order );
  }

  public String toString()
  {
    StringBuilder buffer = new StringBuilder();
    buffer.append('{');

    boolean first = true;
    for (int i=0; i<count; ++i)
    {
      if (first) first = false;
      else       buffer.append(',');

      int index = order[i];
      buffer.append( keys[index] );
      buffer.append( ':' );
      buffer.append( values[index] );
    }

    buffer.append('}');
    return buffer.toString();
  }
}

