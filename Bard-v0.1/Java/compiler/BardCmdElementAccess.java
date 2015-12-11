package compiler;

import alphavm.BardOp;

public class BardCmdElementAccess extends BardCmd
{
  public BardCmd  context;
  public BardArgs args;

  public BardCmdElementAccess( BardCmd context, BardArgs args )
  {
    this.context = context;
    this.args = args;
  }

  public BardCmd duplicate() { return new BardCmdElementAccess( duplicate(context), duplicate(args) ).set_source_info(this); }

  public BardType type()
  {
    return null;
  }

  public BardCmd resolve( BardMethod method_context )
  {
    BardCompiler bc = info.bc;

    context = context.resolve( method_context ).require_value();

    BardType context_type = context.type();
    if (context.type() == info.bc.type_Variant) 
    {
      context = context.cast_to( bc.type_Object );
      context_type = bc.type_Object;
    }

    if (context_type == bc.type_List)
    {
      switch (args.size())
      {
        case 1:
          return new BardCmdList.Get( context, args.get(0) ).set_source_info(this).resolve( method_context );
      }
    }
    else if (context_type == bc.type_Table)
    {
      args.resolve( method_context );
      switch (args.size())
      {
        case 1:
          return new BardCmdTable.GetKey( context, args.get(0) ).set_source_info(this).resolve( method_context );
      }
    }
    else if (context_type == bc.type_Object)
    {
      context = new BardCmdContextAccess( context, "Collection", null ).set_source_info(context);
      return resolve( method_context );
    }
    //else if (context_type == bc.type_String && args.size() == 1)
    //{
      //return new BardCmdString.Get( context, args.get(0) ).set_source_info(this).resolve( method_context );
    //}

    return new BardCmdContextAccess( context, "get", args ).set_source_info(this).resolve( method_context );
  }

  public BardCmd resolve_assignment( BardMethod method_context, BardCmd new_value )
  {
    context = context.resolve( method_context );
    BardType context_type = context.type();

    BardCompiler bc = info.bc;
    if (context_type == bc.type_List)
    {
      switch (args.size())
      {
        case 1:
          return new BardCmdList.Set( context, args.get(0), new_value ).set_source_info(this).resolve( method_context );
      }
    }
    else if (context_type == bc.type_Variant)
    {
      context = context.cast_to( bc.type_Variant );
      context = new BardCmdContextAccess( context, "Collection", null ).set_source_info(context);
      return resolve_assignment( method_context, new_value );
    }

    args.add( new_value );
    return new BardCmdContextAccess( context, "set", args ).set_source_info(this).resolve( method_context );
  }
}

