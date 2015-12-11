package alphavm;

import java.util.*;

public class BardIntTable
{
  public int[]    hash_codes;
  public String[] keys;
  public int[]    values;
  public int      bin_mask;
  public int      count;
  public int      limit;

  public BardIntTable()
  {
    this( 16 );
  }

  public BardIntTable( int capacity )
  {
    int pow2 = 1;
    while (pow2 < capacity) pow2 <<= 1;
    capacity = pow2;

    bin_mask = capacity - 1;
    limit = (capacity * 7) / 10;

    hash_codes = new int[ capacity ];
    keys   = new String[ capacity ];
    values = new int[ capacity ];
  }

  public void clear()
  {
    for (int i=0; i<keys.length; ++i)
    {
      keys[i] = null;
    }
    count = 0;
  }

  public int calculate_hash_code( String key )
  {
    // String and StringBuilder give different hash codes, so use
    // our own better hash code.
    int hash_code = 0;
    for (int i=key.length()-1; i>=0; --i) 
    {
      // code = code * 7 + ch
      hash_code = ((hash_code<<3) - hash_code) + key.charAt(i);  
    }

    return hash_code;
  }

  public int calculate_hash_code( StringBuilder key )
  {
    // String and StringBuilder give different hash codes, so use
    // our own better hash code.
    int hash_code = 0;
    for (int i=key.length()-1; i>=0; --i) 
    {
      // code = code * 7 + ch
      hash_code = ((hash_code<<3) - hash_code) + key.charAt(i);  
    }

    return hash_code;
  }

  public int find( String key )
  {
    return find( key, calculate_hash_code(key) );
  }

  public int find( StringBuilder key )
  {
    return find( key, calculate_hash_code(key) );
  }


  public int find( String key, int hash_code )
  {
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

  public int find( StringBuilder key, int hash_code )
  {
    int index = (hash_code & bin_mask);
    while (keys[index] != null)
    {
      if (hash_codes[index] == hash_code)
      {
        String table_key = keys[index];
        int n = key.length();
        if (n == table_key.length())
        {
          boolean match = true;
          for (int i=n-1; i>=0; --i)
          {
            if (key.charAt(i) != table_key.charAt(i))
            {
              match = false;
              break;
            }
          }
          if (match) return index;
        }
      }
      index = (index + 1) & bin_mask;
    }

    return ~index;  // negative spot where new key can be added
  }
  
  public void set( String key, int value )
  {
    set( key, value, calculate_hash_code( key ) );
  }

  public void set( String key, int value, int hash_code )
  {
    int slot = find( key, hash_code );
    if (slot >= 0)
    {
      values[slot] = value;
    }
    else
    {
      slot = ~slot;
      hash_codes[slot] = hash_code;
      keys[slot] = key;
      values[slot] = value;
      ++count;

      int capacity = keys.length;
      if (count >= limit)
      {
        ensure_capacity( capacity * 2 );
      }
    }
  }

  public void set( StringBuilder key, int value )
  {
    set( key, value, calculate_hash_code( key ) );
  }

  public void set( StringBuilder key, int value, int hash_code )
  {
    int slot = find( key, hash_code );
    if (slot >= 0)
    {
      values[slot] = value;
    }
    else
    {
      slot = ~slot;
      hash_codes[slot] = hash_code;
      keys[slot] = key.toString();
      values[slot] = value;
      ++count;

      int capacity = keys.length;
      if (count >= (capacity - (capacity>>2)))
      {
        ensure_capacity( capacity * 2 );
      }
    }
  }

  @SuppressWarnings({"unchecked"})
  public int get( String key )
  {
    int slot = find( key, calculate_hash_code(key) );
    if (slot >= 0) 
    {
      return (int) values[slot];
    }
    return -1;
  }

  @SuppressWarnings({"unchecked"})
  public int get( StringBuilder key )
  {
    int slot = find( key, calculate_hash_code(key) );
    if (slot >= 0) return (int) values[slot];
    return -1;
  }

  public int first_index()
  {
    if (count == 0) return -1;
    return next_index( values.length );
  }

  public int next_index( int prev_index )
  {
    for (--prev_index; prev_index>=0; --prev_index)
    {
      if (keys[prev_index] != null) return prev_index;
    }
    return -1;
  }

  @SuppressWarnings({"unchecked"})
  public void ensure_capacity( int capacity )
  {
    int pow2 = 1;
    while (pow2 < capacity) pow2 <<= 1;
    capacity = pow2;

    if (capacity <= keys.length) return;
    BardIntTable larger_table = new BardIntTable( capacity );
    for (int i=bin_mask; i>=0; --i)
    {
      if (keys[i] != null)
      {
        larger_table.set( keys[i], (int) values[i], hash_codes[i] );
      }
    }

    hash_codes = larger_table.hash_codes;
    keys       = larger_table.keys;
    values     = larger_table.values;
    bin_mask   = larger_table.bin_mask;
    count      = larger_table.count;
    limit = (capacity * 7) / 10;
  }
}

