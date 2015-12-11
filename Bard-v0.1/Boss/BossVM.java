import java.util.*;

class BossVM
{
  final static public int status_return = 0;

  public int stack_size = 4096;

  public int[]    index_stack   = new int[ stack_size ];

  public Object[] object_stack  = new Object[ stack_size ];
  public int[]    integer_stack = new int[ stack_size ];
  public double[] real_stack    = new double[ stack_size ];

  public int[]    frame_stack   = new int[ stack_size ];

  public int   sp      = 0;
  public int   dp      = 0;
  public int   fp      = 0;
  public int   call_sp = 0;

  public BossInteger type_Integer = new BossInteger();

  public HashMap<String,BossCommand> commands = new HashMap<String,BossCommand>();

  public BossVM()
  {
    addCommand( 
        new BossCommand("five")
        {
          public BossCode parse( BossParser parser, BossToken t )
          {
            return new BossCode(t)
            {
              public String toString()
              {
                return "5";
              }

              public boolean execute( BossVM vm )
              {
                vm.pushInteger( 5 );
                return true;
              }
            };
          }
        }
      );
  }

  public void addCommand( BossCommand command )
  {
    commands.put( command.keyword, command );
  }

  public boolean load( String filename )
  {
    try
    {
      BossParser parser = new BossParser( this );
      parser.parse( filename );

      execute( parser.parse_expression() );
      System.out.println( popInteger() );

      return true;
    }
    catch (BossError err)
    {
      System.out.println( err );
      return false;
    }
  }

  public void pushNull()
  {
    index_stack[ sp++ ]  = dp;
    object_stack[ dp++ ] = null;
  }

  public void pushObject( Object obj )
  {
    index_stack[ sp++ ] = dp;
    object_stack[ dp++ ]  = obj;
  }

  public void pushInteger( int value )
  {
    index_stack[ sp++ ]   = dp;
    object_stack[ dp ]    = type_Integer;
    integer_stack[ dp++ ] = value;
  }

  public BossVariant peekVariant()
  {
    Object obj = object_stack[ index_stack[sp] ];
    if (obj == null)         return BossVariant.type_Null;
    if (obj == type_Integer) return BossVariant.type_Integer;
    return BossVariant.type_Object;
  }

  public Object toObject( int index_pos ) 
  {
    int pos = index_stack[ index_pos ];
    Object obj = object_stack[ pos ];
    if (obj == null)         return null;
    if (obj == type_Integer) return new Integer( integer_stack[pos] );
    return obj;
  }

  public String toString( int index_pos ) 
  {
    int pos = index_stack[ index_pos ];
    Object obj = object_stack[ pos ];
    if (obj == null)         return "";
    if (obj == type_Integer) return "" + integer_stack[pos];
    return obj.toString();
  }

  public int toInteger( int index_pos ) 
  {
    int pos = index_stack[ index_pos ];
    Object obj = object_stack[ pos ];
    if (obj == null)           return 0;
    if (obj == type_Integer)   return integer_stack[pos];
    if (obj instanceof Number) return ((Number)obj).intValue();
    if (obj instanceof BossInterface)
    {
      ((BossInterface)obj).access(this,"Integer",0);
      return popInteger();
    }
    return BossString.toInteger( obj.toString() );
  }

  public void popDiscard()
  {
    dp = index_stack[ --sp ];
  }

  public Object popObject() 
  {
    return toObject( --sp );
  }

  public String popString()
  {
    Object obj = popObject();
    if (obj == null) return "";
    return obj.toString();
  }

  public int popInteger() 
  { 
    return toInteger( --sp );
  }

  public String argAsString( int offset )
  {
    return toString( fp+offset );
  }

  public int argAsInteger( int offset )
  {
    return toInteger( fp+offset );
  }

  public void execute( BossCode cmd )
  {
    cmd.execute( this );
  }

  public boolean call( String name, int arg_count )
  {
    frame_stack[ call_sp++ ] = fp;

    fp = sp - (arg_count+1);

    Object obj = toObject( fp );
    if (obj == null)
    {
      pushNull();
    }
    else if (obj instanceof BossInterface)
    {
      BossInterface boss_obj = (BossInterface) obj;
      if (boss_obj != null) boss_obj.access( this, name, arg_count );
    }
    else if (obj instanceof String)
    {
      BossString.access( (String)obj, this, name, arg_count );
    }
    else
    {
      pushNull();
    }

    int result_dp = index_stack[ sp - 1 ];

    sp = fp;
    dp = index_stack[ sp ];

    fp = frame_stack[ --call_sp ];

    ++sp;
    obj = object_stack[ result_dp ];
    if (obj == null)
    {
      object_stack[ dp++ ] = null;
    }
    else
    {
      object_stack[ dp ] = obj;
      if (obj == type_Integer) 
      {
        integer_stack[ dp ] = integer_stack[ result_dp ];
      }
      ++dp;
    }

    return true;
  }
}

