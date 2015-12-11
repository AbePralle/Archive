import java.util.*;

abstract public class BossCode
{
  public BossToken t;

  public BossCode() { }

  public BossCode( BossToken t )
  {
    this.t = t;
  }

  public BossCode init( BossToken t )
  {
    this.t = t;
    return this;
  }

  abstract public boolean execute( BossVM vm );

  //--------------------------------------------------------------------------

  static class Args
  {
    ArrayList<BossCode> data = new ArrayList<BossCode>();

    public Args()
    {
    }

    public Args( BossCode cmd )
    {
      add( cmd );
    }

    public Args add( BossCode cmd )
    {
      data.add( cmd );
      return this;
    }

    public int count() { return data.size(); }

    public BossCode get( int index )
    {
      return data.get( index );
    }
  }

  //--------------------------------------------------------------------------

  static class LiteralString extends BossCode
  {
    public String value;

    public LiteralString( BossToken t, String value )
    {
      this.t = t;
      this.value = value;
    }

    public String toString() { return value; }

    public boolean execute( BossVM vm ) 
    { 
      vm.pushObject( value );
      return true;
    }
  }

  static class LiteralInteger extends BossCode
  {
    public int value;

    public LiteralInteger( BossToken t, int value )
    {
      this.t = t;
      this.value = value;
    }

    public String toString() { return ""+value; }

    public boolean execute( BossVM vm ) 
    { 
      vm.pushInteger( value );
      return true;
    }
  }

  //--------------------------------------------------------------------------

  abstract static public class BinaryOp extends BossCode
  {
    BossCode lhs, rhs;

    public BossCode init( BossToken t, BossCode lhs, BossCode rhs )
    {
      init( t );
      this.lhs = lhs;
      this.rhs = rhs;
      return this;
    }

    abstract public String name();

    public String toString()
    {
      return lhs + " " + name() + " " + rhs;
    }

    public RuntimeException unhandled_operand_type_error()
    {
      return new RuntimeException( "'" + name() + "' operation undefined with operand types used." );
    }
  }

  static public class Add extends BinaryOp
  {
    public String name() { return "+"; }

    public boolean execute( BossVM vm )
    {
      if ( !lhs.execute(vm) ) return false;
      if ( !rhs.execute(vm) ) return false;

      switch (vm.peekVariant())
      {
        case type_Integer:
        {
          int rhs_value = vm.popInteger();
          switch (vm.peekVariant())
          {
            case type_Integer:
              {
                int lhs_value = vm.popInteger();
                vm.pushInteger( lhs_value + rhs_value );
                return true;
              }
          }
          break;
        }
      }

      throw unhandled_operand_type_error();
    }
  }

  static public class Multiply extends BinaryOp
  {
    public String name() { return "*"; }

    public boolean execute( BossVM vm )
    {
      if ( !lhs.execute(vm) ) return false;
      if ( !rhs.execute(vm) ) return false;

      switch (vm.peekVariant())
      {
        case type_Integer:
        {
          int rhs_value = vm.popInteger();
          switch (vm.peekVariant())
          {
            case type_Integer:
              {
                int lhs_value = vm.popInteger();
                vm.pushInteger( lhs_value * rhs_value );
                return true;
              }
          }
          break;
        }
      }

      throw unhandled_operand_type_error();
    }
  }

  static public class Access extends BossCode
  {
    public BossCode context;
    public String  name;
    public Args    args;

    public Access( BossToken t, BossCode context, String name )
    {
      this.t = t;
      this.context = context;
      this.name = name;
    }

    public String toString()
    {
      StringBuilder buffer = new StringBuilder();
      if (context != null) 
      {
        buffer.append( context.toString() );
        buffer.append('.');
      }
      buffer.append( name );
      return buffer.toString();
    }

    public boolean execute( BossVM vm )
    {
      if (context != null)
      {
        if ( !context.execute(vm) ) return false;

        int arg_count = 0;
        if (args != null)
        {
          arg_count = args.count();
          for (int i=0; i<arg_count; ++i)
          {
            if ( !args.get(i).execute(vm) ) return false;
          }
        }

        return vm.call( name, arg_count );
      }
      else
      {
        throw t.error( "Unhandled implicit-context access." );
      }
    }
  }
}

