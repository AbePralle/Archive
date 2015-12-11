package compiler;

import alphavm.BardOp;

public class BardCmdAccess extends BardCmd
{
  public String   name;
  public BardArgs args;

  public BardCmdAccess( String name, BardArgs args )
  {
    this.name = name;
    this.args = args;
  }

  public BardCmd duplicate()
  {
    return new BardCmdAccess( name, duplicate( args ) ).set_source_info(this);
  }

  public String toString()
  {
    StringBuilder buffer = new StringBuilder();
    buffer.append( name );
    if (args != null)
    {
      buffer.append( "(" );
      boolean first = true;
      for (BardCmd arg : args)
      {
        if (first) first = false;
        else       buffer.append(',');
        buffer.append( arg );
      }
      buffer.append( ")" );
    }
    return buffer.toString();
  }

  public BardType type()
  {
    throw error( "Type requested from unresolved access." );
  }

  public BardCmd resolve( BardMethod method_context )
  {
    BardCompiler bc = info.bc;

    // local var
    if (args == null || args.size() == 0)
    {
      BardLocal v = method_context.find_local( name );
      if (v != null)
      {
        return new BardCmdReadLocal( v ).set_source_info(this);
      }
    }

    // singleton or new object
    BardType type = bc.find_type( name );
    if (type != null)
    {
      if (args == null)
      {
        return new BardCmdReadSingleton( type ).set_source_info( this );
      }
      else
      {
        return new BardCmdContextAccess(
            new BardCmdCreateObject(type).set_source_info(this), 
            "init", args ).set_source_info(this).resolve( method_context ).cast_to(type);
      }
    }
    else
    {
      BardType type_context = method_context.type_context;

      int arg_count = 0;
      if (args != null) arg_count = args.size();

      if (arg_count == 0)
      {
        int part_id = bc.id_table.get_id( name );
        BardProperty p = type_context.properties.find( part_id );
        if (p != null) return (new BardCmdReadProperty( null, p )).set_source_info( this );
      }
      else if (arg_count == 1)
      {
        int part_id = bc.id_table.get_id( name );
        BardProperty p = type_context.properties.find( part_id );
        if (p != null) return (new BardCmdWriteProperty( null, p, args.get(0) )).set_source_info( this ).resolve( method_context );
      }
    }

    return resolve_call( method_context, null, name, args, true );
  }

  public BardCmd resolve_assignment( BardMethod method_context, BardCmd new_value )
  {
    if (args != null)
    {
      throw error( "Invalid assignment." );
    }

    BardLocal v = method_context.find_local( name );
    if (v != null)
    {
      return new BardCmdWriteLocal( v, new_value.resolve(method_context) ).resolve( method_context );
    }

    args = new BardArgs( new_value );
    return resolve( method_context );
  }

  public BardCmd resolve_adjustment( BardMethod method_context, int delta )
  {
    if (args != null)
    {
      throw error( "Invalid target for ++/-- adjustment." );
    }

    BardLocal v = method_context.find_local( name );
    if (v != null)
    {
      if (delta == 1) return new BardCmdAdjust.IncrementLocal().init( v ).set_source_info( this ).resolve( method_context );
      else            return new BardCmdAdjust.DecrementLocal().init( v ).set_source_info( this ).resolve( method_context );
    }

    BardCmd one = new BardCmdLiteral.LiteralInteger( delta ).set_source_info( this );
    BardCmd this_plus_one = new BardCmdBinaryOp.Add().init( duplicate(), one ).set_source_info( this );
    return new BardCmdAssign( this, this_plus_one ).set_source_info( this ).resolve( method_context );

    //BardProperty p = type_context.find_property( name );
    //if (p != null)
    //{
    //  if (delta == 1) return new BardCmdAdjust.IncrementProperty( p ).set_source_info( this );
    //  else            return new BardCmdAdjust.DecrementProperty( p ).set_source_info( this );
    //}

    //throw error( "No such variable '" + name + "'." );
  }

  public void compile( BardCodeWriter writer )
  {
    throw error( "[Internal] Unresolved '" + name + "' access." );
  }
}

