package compiler;

import alphavm.BardOp;

public class BardCmdContextAccess extends BardCmdAccess
{
  public BardCmd  context;

  public BardCmdContextAccess( BardCmd context, String name, BardArgs args )
  {
    super( name, args );
    this.context = context;
  }

  public BardCmd duplicate() { return new BardCmdContextAccess( context.duplicate(), name, duplicate(args) ).set_source_info(this); }

  public String toString()
  {
    return context.toString() + "." + super.toString();
  }

  public BardType type()
  {
    throw error( "Type requested from unresolved part access." );
  }

  public BardCmd resolve( BardMethod method_context )
  {
    context = context.resolve( method_context );

    BardType context_type = context.type();
    BardCmd special_result = resolve_special_context( method_context, context_type );
    if (special_result != null) return special_result.resolve( method_context );

    int arg_count = 0;
    if (args != null) arg_count = args.size();

    if (arg_count == 0)
    {
      int part_id = info.bc.id_table.get_id( name );
      BardProperty p = context_type.properties.find( part_id );
      if (p != null) return (new BardCmdReadProperty( context, p )).set_source_info( this );
    }
    else if (arg_count == 1)
    {
      int part_id = info.bc.id_table.get_id( name );
      BardProperty p = context_type.properties.find( part_id );
      if (p != null) return (new BardCmdWriteProperty( context, p, args.get(0) )).set_source_info( this ).resolve( method_context );
    }

    BardCmd result = resolve_call( method_context, context, name, args, false );
    if (result != null) return result;

    result = resolve_call( method_context, context, name, args, true );
    return result;
  }

  public BardCmd resolve_special_context( BardMethod method_context, BardType context_type )
  {
    BardCompiler bc = info.bc;

    int arg_count = 0;
    if (args != null) arg_count = args.size();
    if (context_type.is_primitive())
    {
      BardCmd result = resolve_primitive_call( method_context, context, name, args );
      return result;
    }
    else if (context_type == bc.type_List)
    {
      if (name.equals("add"))
      {
        return new BardCmdList.Add( context, args ).set_source_info(this).resolve( method_context );
      }
    }
    else if (context_type == bc.type_Table)
    {
      // table.unknown_part -> table["unknown_part"]
      BardMethod m = find_method( method_context, context, name, args, false );
      if (m == null)
      {
        return new BardCmdTable.GetKey( context, 
            new BardCmdLiteral.LiteralString(name).set_source_info(this) ).set_source_info(this).resolve(method_context);
      }
    }
    else if (context_type == bc.type_Variant)
    {
      if (args == null)
      {
        BardType cast_type = bc.find_type( name );
        if (cast_type != null)
        {
          // Type conversion: Variant.TypeName
          return new BardCmdCast( context, cast_type.true_type() ).set_source_info(this).resolve( method_context );
        }
      }
    }

    if (args == null)
    {
      if (context_type == bc.type_Table || context_type == bc.type_List || context_type == bc.type_Object || context_type == bc.type_Variant)
      {
        // obj.part -> obj.get("part")
        BardMethod m = find_method( method_context, context, name, null, false );
        if (m == null && !name.equals("get"))
        {
          BardArgs args = new BardArgs( new BardCmdLiteral.LiteralString(name).set_source_info(this) );
          return new BardCmdContextAccess( context, "get", args ).set_source_info(this).resolve(method_context);
        }
      }
    }

    return null;
  }

  public BardCmd resolve_assignment( BardMethod method_context, BardCmd new_value )
  {
    if (args != null)
    {
      throw error( "Invalid assignment." );
    }

    context = context.resolve( method_context );
    BardType context_type = context.type();

    int part_id = info.bc.id_table.get_id( name );
    BardProperty p = context_type.properties.find( part_id );
    if (p != null) 
    {
      return new BardCmdContextAccess( context, name, new BardArgs(new_value) ).set_source_info(this).resolve( method_context );
    }

    BardCompiler bc = info.bc;
    if (context_type == bc.type_Table || context_type == bc.type_Object || context_type == bc.type_Variant)
    {
      // table.unknown_part = x -> table.set( "unknown_part", x )
      BardArgs args = new BardArgs( new_value );
      BardMethod m = find_method( method_context, context, name, args, false );
      if (m == null)
      {
        args.add( 0, new BardCmdLiteral.LiteralString(name).set_source_info(this) );
        return new BardCmdContextAccess( context, 
            "set", args ).set_source_info(this).resolve(method_context);
      }
    }

    throw new RuntimeException( "TODO (" + getClass().getName() + "): "+ this );
  }
}

