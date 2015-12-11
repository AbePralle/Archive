package vm;

public class BardObject
{
  public BardType     type;
  public BardObject[] properties;
  public BardObject   next_object;
  public boolean      referenced;

  public BardObject( BardType type )
  {
    this.type = type;
    BardVM vm = type.vm;

    BardPropertyList property_info = type.properties;
    int property_count = property_info.size();
    if (property_count > 0) properties = new BardObject[ property_count ];

    for (int i=property_count-1; i>=0; --i)
    {
      properties[i] = property_info.get(i).create_slot_value();
    }
  }

  public BardObject reset()
  {
    BardPropertyList property_info = type.properties;
    for (int i=property_info.size(); --i>=0; )
    {
      properties[i] = property_info.get(i).create_slot_value();
    }
    return this;
  }

  public BardObject collect_references( BardObject list )
  {
    BardObject[] properties = this.properties;
    if (properties != null)
    {
      int i = properties.length;
      while (--i > 0)
      {
        BardObject obj = properties[i];
        if (obj != null && !obj.referenced)
        {
          obj.referenced = true;
          obj.next_object = list;
          list = obj;
        }
      }
    }
    return list;
  }

  public String toString() { return type.name; }

  public BardObject read_Object( int slot )
  {
    return properties[slot];
  }

  public double read_Real( int slot )
  {
    return ((BardValue.RealValue) properties[slot]).value;
  }

  public int read_Integer( int slot )
  {
    return ((BardValue.IntegerValue) properties[slot]).value;
  }

  public int read_Character( int slot )
  {
    return ((BardValue.CharacterValue) properties[slot]).value;
  }

  public int read_Logical( int slot )
  {
    return ((BardValue.LogicalValue) properties[slot]).value;
  }

  public Object read_Native( int slot )
  {
    return ((BardValue.NativeValue) properties[slot]).value;
  }

  public void write_Object( int slot, BardObject new_value )
  {
    properties[slot] = new_value;
  }

  public void write_Real( int slot, double new_value )
  {
    ((BardValue.RealValue) properties[slot]).value = new_value;
  }

  public void write_Integer( int slot, int new_value )
  {
    ((BardValue.IntegerValue) properties[slot]).value = new_value;
  }

  public void write_Character( int slot, int new_value )
  {
    ((BardValue.CharacterValue) properties[slot]).value = new_value;
  }

  public void write_Logical( int slot, int new_value )
  {
    ((BardValue.LogicalValue) properties[slot]).value = new_value;
  }

  public void write_Native( int slot, Object new_value )
  {
    ((BardValue.NativeValue) properties[slot]).value = new_value;
  }
}

