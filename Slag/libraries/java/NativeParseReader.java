package com.plasmaworks.slag;

public class NativeParseReader
{
  static public void prep_data( ClassParseReader reader )
  {
    if (reader.property_spaces_per_tab < 0) reader.property_spaces_per_tab = 0;

    int spaces_per_tab = reader.property_spaces_per_tab;

    // Count how many characters we'll have after removing \r and changing
    // each tab into a given number of spaces.
    int read_pos  = -1;
    int remaining = reader.property_remaining + 1;
    int count = 0;
    while (--remaining != 0)
    {
      switch (reader.property_data[++read_pos])
      {
        case  (char)9: count += spaces_per_tab; break;
        case (char)13: break;
        default: ++count; break;
      }
    }

    // Create a new array of the appropriate size.
    char[] new_array = new char[count];

    // Copy the data into it while filtering
    int write_pos = -1;
    read_pos  = -1;
    remaining = reader.property_remaining + 1;
    while (--remaining != 0)
    {
      char ch = reader.property_data[++read_pos];
      switch (ch)
      {
        case  (char)9: 
        {
          int spaces = spaces_per_tab + 1;
          while (--spaces != 0) new_array[++write_pos] = (char) 32;
          break;
        }

        case (char)13: break;

        default:
          new_array[++write_pos] = ch;
          break;
      }
    }

    reader.property_data = new_array;
    reader.property_remaining = count;
  }

  static public boolean has_another( ClassParseReader reader )
  {
    return (reader.property_remaining > 0);
  }

  static public char peek( ClassParseReader reader )
  {
    if (reader.property_remaining == 0) return (char) 0;
    return reader.property_data[reader.property_pos];
  }

  static public char peek( ClassParseReader reader, int num_ahead )
  {
    if (--num_ahead >= reader.property_remaining) return (char) 0;
    return reader.property_data[reader.property_pos + num_ahead];
  }

  static public char read( ClassParseReader reader )
  {
    --reader.property_remaining;
    char ch = reader.property_data[reader.property_pos++];
    switch (ch)
    {
      case (char) 10:
        ++reader.property_line;
        reader.property_column = 1;
        break;

      default:
        ++reader.property_column;
        break;
    }
    return ch;
  }

  static public boolean consume( ClassParseReader reader, char ch )
  {
    if (reader.property_remaining <= 0) return false;

    if (reader.property_data[reader.property_pos] == ch)
    {
      read(reader);
      return true;
    }
    else
    {
      return false;
    }
  }

  static public boolean consume( ClassParseReader reader, ClassString st )
  {
    int count = st.data.length();
    if (count > reader.property_remaining) return false;

    String data1 = st.data;
    char[] data2 = reader.property_data;
    int i1 = count;
    int i2 = reader.property_pos + count;
    while (i1 > 0)
    {
      if (data1.charAt(--i1) != data2[--i2]) return false;
    }

    reader.property_pos += count;
    reader.property_column += count;
    reader.property_remaining -= count;
    return true;
  }
}

