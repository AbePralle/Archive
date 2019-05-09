package boss.vm;

public class BossCmdData
{
  final static public int
    CMD_TYPE = 0,
    TOKEN    = 1,
    NEXT     = 2,

    OPERAND  = 3,
    VALUE    = 3,

    LEFT     = 3,
    RIGHT    = 4,

    ARG1     = 3,
    ARG2     = 4;

  final static public int
    OPERANDS_0 = 0,

    STATEMENTS = 0,
    ARGS       = 1,

    OPERANDS_1 = 10,
    INT32      = 10,
    PRINTLN    = 11,

    OPERANDS_2 = 20,

    ADD        = 20,
    SUBTRACT   = 21,
    MULTIPLY   = 22,
    DIVIDE     = 23;

  public BossVM        vm;
  public BossInt32List data;

  public BossCmdData( BossVM vm )
  {
    this.vm = vm;
    data = new BossInt32List( 16384 );
    data.add( 0 );  // prevent 0 from being a command id
  }

  public int createCmd( int t, int type )
  {
    int size = 3;
    int operand_count;
    if (type < OPERANDS_1)      operand_count = 0;
    else if (type < OPERANDS_2) operand_count = 1;
    else                        operand_count = 2;

    int cmd = data.count;
    data.reserve( size+operand_count );
    data.add( type );
    data.add( t );
    data.add( 0 );
    for (int i=operand_count; --i>=0;) data.add( 0 );
    return cmd;
  }

  public int createCmd( int t, int type, int operand )
  {
    int cmd = createCmd( t, type );
    data.set( cmd+OPERAND, operand );
    return cmd;
  }

  public int createCmd( int t, int type, int a, int b )
  {
    int cmd = createCmd( t, type );
    data.set( cmd+ARG1, a );
    data.set( cmd+ARG2, b );
    return cmd;
  }

  public int cmdType( int cmd )
  {
    return data.get( cmd+CMD_TYPE );
  }

  public int count( int cmd )
  {
    if (cmd == 0) return 0;

    int n = 0;
    cmd = data.get( cmd+NEXT );
    while (cmd > 0)
    {
      ++n;
      cmd = data.get( cmd+NEXT );
    }
    return n;
  }

  public BossError error( int cmd, String message )
  {
    return vm.tokenData.error( data.get(cmd+TOKEN), message );
  }

  public void generateCode( int cmd, BossVM vm )
  {
    if (cmd == 0) return;

    switch (data.get(cmd+CMD_TYPE))
    {
      case STATEMENTS:
      {
        int cur = data.get( cmd+NEXT );
        while (cur > 0)
        {
          generateCode( cur, vm );
          cur = data.get( cur+NEXT );
        }
        return;
      }

      case ARGS:
      {
        if (cmd > 0)
        {
          int cur = data.get( cmd+NEXT );
          while (cur > 0)
          {
            generateCode( cur, vm );
            cur = data.get( cur+NEXT );
          }
        }
        return;
      }

      case INT32:
        vm.code.add( BossOpcode.PUSH_INT32 );
        vm.code.add( data.get(cmd+VALUE) );
        return;

      case PRINTLN:
        int args = data.get( cmd+OPERAND );
        if (args > 0)
        {
          int cur = data.get( args+NEXT );
          while (cur > 0)
          {
            generateCode( cur, vm );
            int genericType = requireType(cur).genericType;
            switch (genericType)
            {
              case BossType.INT32: vm.code.add( BossOpcode.PRINT_INT32 ); break;
              default:             throw error( cur, "[Internal] TODO: generateCode PRINTLN generic type " + genericType );
            }
            cur = data.get( cur+NEXT );
          }
        }
        vm.code.add( BossOpcode.PRINTLN );
        return;

        /*
      case ADD:
        {
          int left  = requireValue( resolved(data.get(cmd+LEFT),scope) );
          int right = requireValue( resolved(data.get(cmd+RIGHT),scope) );
          int leftCmdType  = data.get( left  + CMD_TYPE );
          int rightCmdType = data.get( right + CMD_TYPE );
          if (leftCmdType == rightCmdType)
          {
            int t = data.get( cmd+TOKEN );
            switch (leftCmdType)
            {
              case INT32:
                return createCmd( t, INT32, data.get(left+VALUE)+data.get(right+VALUE) );
            }
          }
          data.set( cmd+LEFT, left );
          data.set( cmd+RIGHT, right );
        }
        return cmd;

      case SUBTRACT:
        {
          int left  = requireValue( resolved(data.get(cmd+LEFT),scope) );
          int right = requireValue( resolved(data.get(cmd+RIGHT),scope) );
          int leftCmdType  = data.get( left  + CMD_TYPE );
          int rightCmdType = data.get( right + CMD_TYPE );
          if (leftCmdType == rightCmdType)
          {
            int t = data.get( cmd+TOKEN );
            switch (leftCmdType)
            {
              case INT32:
                return createCmd( t, INT32, data.get(left+VALUE)-data.get(right+VALUE) );
            }
          }
          data.set( cmd+LEFT, left );
          data.set( cmd+RIGHT, right );
        }
        return cmd;

      case MULTIPLY:
        {
          int left  = requireValue( resolved(data.get(cmd+LEFT),scope) );
          int right = requireValue( resolved(data.get(cmd+RIGHT),scope) );
          int leftCmdType  = data.get( left  + CMD_TYPE );
          int rightCmdType = data.get( right + CMD_TYPE );
          if (leftCmdType == rightCmdType)
          {
            int t = data.get( cmd+TOKEN );
            switch (leftCmdType)
            {
              case INT32:
                return createCmd( t, INT32, data.get(left+VALUE)*data.get(right+VALUE) );
            }
          }
          data.set( cmd+LEFT, left );
          data.set( cmd+RIGHT, right );
        }
        return cmd;

      case DIVIDE:
        {
          int left  = requireValue( resolved(data.get(cmd+LEFT),scope) );
          int right = requireValue( resolved(data.get(cmd+RIGHT),scope) );
          int leftCmdType  = data.get( left  + CMD_TYPE );
          int rightCmdType = data.get( right + CMD_TYPE );
          if (leftCmdType == rightCmdType)
          {
            int t = data.get( cmd+TOKEN );
            switch (leftCmdType)
            {
              case INT32:
              {
                int a = data.get( left+VALUE );
                int b = data.get( right+VALUE );
                if (b == 0) throw error( cmd, "Illegal division by zero." );
                return createCmd( t, INT32, a/b );
              }
            }
          }
          data.set( cmd+LEFT, left );
          data.set( cmd+RIGHT, right );
        }
        return cmd;
        */

      default:
        throw error( cmd, "TODO: Cmd.generateCode(" + data.get(cmd+CMD_TYPE) + ")" );
    }
  }

  public int operand( int cmd )
  {
    return data.get( cmd+OPERAND );
  }

  public BossType requireType( int cmd )
  {
    switch (data.get(cmd+CMD_TYPE))
    {
      case INT32:
        return vm.typeInt32;

      case ADD:
      case SUBTRACT:
      case MULTIPLY:
      case DIVIDE:
        return requireType( data.get(cmd+LEFT) );

      default:
        throw error( cmd, "Value expected; expression does not result in a value." );
    }
  }

  public int requireValue( int cmd )
  {
    requireType( cmd );
    return cmd;
  }

  public int resolved( int cmd, BossScope scope )
  {
    if (cmd == 0) return 0;

    switch (data.get(cmd+CMD_TYPE))
    {
      case STATEMENTS:
      {
        int prev = cmd;
        int cur = data.get( cmd+NEXT );
        while (cur > 0)
        {
          int next = data.get( cur+NEXT );
          cur = resolved( cur, scope );
          data.set( prev+NEXT, cur );
          prev = cur;
          cur = next;
        }
        return cmd;
      }

      case ARGS:
      {
        int prev = cmd;
        int cur = data.get( cmd+NEXT );
        while (cur > 0)
        {
          int next = data.get( cur+NEXT );
          cur = resolved( cur, scope );
          data.set( prev+NEXT, cur );
          prev = cur;
          cur = next;
        }
        return cmd;
      }

      case INT32:
        return cmd;

      case PRINTLN:
        resolved( data.get(cmd+OPERAND), scope );
        return cmd;

      case ADD:
        {
          int left  = requireValue( resolved(data.get(cmd+LEFT),scope) );
          int right = requireValue( resolved(data.get(cmd+RIGHT),scope) );
          int leftCmdType  = data.get( left  + CMD_TYPE );
          int rightCmdType = data.get( right + CMD_TYPE );
          if (leftCmdType == rightCmdType)
          {
            int t = data.get( cmd+TOKEN );
            switch (leftCmdType)
            {
              case INT32:
                return createCmd( t, INT32, data.get(left+VALUE)+data.get(right+VALUE) );
            }
          }
          data.set( cmd+LEFT, left );
          data.set( cmd+RIGHT, right );
        }
        return cmd;

      case SUBTRACT:
        {
          int left  = requireValue( resolved(data.get(cmd+LEFT),scope) );
          int right = requireValue( resolved(data.get(cmd+RIGHT),scope) );
          int leftCmdType  = data.get( left  + CMD_TYPE );
          int rightCmdType = data.get( right + CMD_TYPE );
          if (leftCmdType == rightCmdType)
          {
            int t = data.get( cmd+TOKEN );
            switch (leftCmdType)
            {
              case INT32:
                return createCmd( t, INT32, data.get(left+VALUE)-data.get(right+VALUE) );
            }
          }
          data.set( cmd+LEFT, left );
          data.set( cmd+RIGHT, right );
        }
        return cmd;

      case MULTIPLY:
        {
          int left  = requireValue( resolved(data.get(cmd+LEFT),scope) );
          int right = requireValue( resolved(data.get(cmd+RIGHT),scope) );
          int leftCmdType  = data.get( left  + CMD_TYPE );
          int rightCmdType = data.get( right + CMD_TYPE );
          if (leftCmdType == rightCmdType)
          {
            int t = data.get( cmd+TOKEN );
            switch (leftCmdType)
            {
              case INT32:
                return createCmd( t, INT32, data.get(left+VALUE)*data.get(right+VALUE) );
            }
          }
          data.set( cmd+LEFT, left );
          data.set( cmd+RIGHT, right );
        }
        return cmd;

      case DIVIDE:
        {
          int left  = requireValue( resolved(data.get(cmd+LEFT),scope) );
          int right = requireValue( resolved(data.get(cmd+RIGHT),scope) );
          int leftCmdType  = data.get( left  + CMD_TYPE );
          int rightCmdType = data.get( right + CMD_TYPE );
          if (leftCmdType == rightCmdType)
          {
            int t = data.get( cmd+TOKEN );
            switch (leftCmdType)
            {
              case INT32:
              {
                int a = data.get( left+VALUE );
                int b = data.get( right+VALUE );
                if (b == 0) throw error( cmd, "Illegal division by zero." );
                return createCmd( t, INT32, a/b );
              }
            }
          }
          data.set( cmd+LEFT, left );
          data.set( cmd+RIGHT, right );
        }
        return cmd;

      default:
        throw error( cmd, "TODO: Cmd.resolved(" + data.get(cmd+CMD_TYPE) + ")" );
    }
  }

  public String toString( int cmd )
  {
    return toString( cmd, new BossStringBuilder() ).toString();
  }

  public BossStringBuilder toString( int cmd, BossStringBuilder builder )
  {
    int type = data.get( cmd+CMD_TYPE );
    switch (type)
    {
      case STATEMENTS:
      {
        int cur = data.get( cmd+NEXT );
        while (cur > 0)
        {
          toString( cur, builder );
          builder.print( '\n' );
          cur = data.get( cur+NEXT );
        }
        break;
      }
      case ARGS:
      {
        builder.print( '(' );
        int cur = data.get( cmd+NEXT );
        boolean isFirst = true;
        while (cur > 0)
        {
          if (isFirst) isFirst = false;
          else         builder.print( ',' );
          toString( cur, builder );
          cur = data.get( cur+NEXT );
        }
        builder.print( ')' );
        break;
      }
      case INT32:
        builder.print( data.get(cmd+VALUE) );
        break;
      case PRINTLN:
        builder.print( "println" );
        toString( operand(cmd), builder );
        break;

      case ADD:
        builder.print( '(' );
        toString( data.get(cmd+LEFT), builder );
        builder.print( " + " );
        toString( data.get(cmd+RIGHT), builder );
        builder.print( ')' );
        break;
      case SUBTRACT:
        builder.print( '(' );
        toString( data.get(cmd+LEFT), builder );
        builder.print( " - " );
        toString( data.get(cmd+RIGHT), builder );
        builder.print( ')' );
        break;
      case MULTIPLY:
        builder.print( '(' );
        toString( data.get(cmd+LEFT), builder );
        builder.print( " * " );
        toString( data.get(cmd+RIGHT), builder );
        builder.print( ')' );
        break;
      case DIVIDE:
        builder.print( '(' );
        toString( data.get(cmd+LEFT), builder );
        builder.print( " / " );
        toString( data.get(cmd+RIGHT), builder );
        builder.print( ')' );
        break;
      default:
        throw error( cmd, "[Internal] TODO: no toString() for cmd type " + type );
    }
    return builder;
  }
}
