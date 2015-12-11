package alphavm;

abstract public class BardValue extends BardObject
{
  public BardValue( BardType type )
  {
    super( type );
  }

  static public class NativeValue extends BardValue
  {
    public Object value;

    public NativeValue( BardType type )
    {
      super( type );
    }

    public NativeValue( BardType type, Object value )
    {
      super( type );
      this.value = value;
    }

    public int hashCode() { return value.hashCode(); }

    public boolean equals( Object other )
    {
      if (this == other) return true;

      if (other instanceof NativeValue) return value.equals( ((NativeValue)other).value );

      if (other instanceof CharacterValue && value instanceof String)
      {
        String st = (String) value;
        if (st.length()>0) return st.charAt(0) == ((CharacterValue)other).value;
      }
      return false;
    }

    public String toString() { return value.toString(); }

    public Object read_Native( int slot )
    {
      return value;
    }

    public void write_Native( int slot, Object new_value )
    {
      value = new_value;
    }
  }

  static public class RealValue extends BardValue
  {
    public double value;

    public RealValue( BardType type )
    {
      super( type );
    }

    public RealValue( BardType type, double value )
    {
      super( type );
      this.value = value;
    }

    public int hashCode() { return (int) Double.doubleToLongBits(value); }

    public boolean equals( Object other )
    {
      if ( !(other instanceof RealValue) ) return false;
      return value == ((RealValue)other).value;
    }

    public String toString() { return BardValue.format(value,4); }

    public double read_Real( int slot )
    {
      return value;
    }

    public void write_Real( int slot, double new_value )
    {
      value = new_value;
    }
  }

  static public class IntegerValue extends BardValue
  {
    public int value;

    public IntegerValue( BardType type )
    {
      super( type );
    }

    public IntegerValue( BardType type, int value )
    {
      super( type );
      this.value = value;
    }

    public int hashCode() { return value; }

    public boolean equals( Object other )
    {
      if ( !(other instanceof IntegerValue) ) return false;
      return value == ((IntegerValue)other).value;
    }

    public String toString() { return "" + value; }

    public int read_Integer( int slot )
    {
      return value;
    }

    public void write_Integer( int slot, int new_value )
    {
      value = new_value;
    }
  }

  static public class CharacterValue extends BardValue
  {
    public int value;

    public CharacterValue( BardType type )
    {
      super( type );
    }

    public CharacterValue( BardType type, int value )
    {
      super( type );
      this.value = value;
    }

    public int hashCode() { return value; }

    public boolean equals( Object other )
    {
      if (other instanceof NativeValue)
      {
        String st = (String) ((NativeValue)other).value;
        return (st.length() > 0) && st.charAt(0) == value;
      }

      if (other instanceof CharacterValue) return value == ((CharacterValue)other).value;
      return false;
    }

    public String toString()
    {
      return "" + (char) this.value;
    }

    public int read_Integer( int slot )
    {
      return value;
    }

    public void write_Integer( int slot, int new_value )
    {
      value = new_value;
    }
  }

  static public class LogicalValue extends BardValue
  {
    public int value;

    public LogicalValue( BardType type )
    {
      super( type );
    }

    public LogicalValue( BardType type, int value )
    {
      super( type );
      this.value = value;
    }

    public int hashCode() { return value; }

    public boolean equals( Object other )
    {
      if ( !(other instanceof LogicalValue) ) return false;
      return value == ((LogicalValue)other).value;
    }

    public String toString()
    {
      if (this.value != 0) return "true";
      else                 return "false";
    }

    public int read_Integer( int slot )
    {
      return value;
    }

    public void write_Integer( int slot, int new_value )
    {
      value = new_value;
    }
  }

  static public String format( double n, int decimal_places )
  {
    StringBuilder buffer = new StringBuilder();
    if (Double.isNaN(n) || Double.isInfinite(n))
    {
      buffer.append( n );
    }
    else
    {
      if (n < 0)
      {
        n = -n;
        buffer.append( '-' );
      }

      if (decimal_places == 0)
      {
        buffer.append( Math.round(n) );
      }
      else
      {
        long power_10 = (long) Math.pow( 10.0, decimal_places );
        long long_value = Math.round( n * power_10 );
        buffer.append( long_value / power_10 );
        buffer.append( '.' );
        long_value %= power_10;
        if (long_value == 0)
        {
          for (int i=0; i<decimal_places; ++i)
          {
            buffer.append('0');
          }
        }
        else
        {
          for (long temp=long_value*10; temp<power_10; temp*=10)
          {
            buffer.append('0');
          }
          buffer.append( long_value );
        }
      }
    }
    return buffer.toString();
  }
}

