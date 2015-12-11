package compiler;

import alphavm.BardOp;

public class BardCmdDynamicCall extends BardCmd
{
  public BardCmd    context;
  public BardMethod method_info;
  public BardArgs   args;

  public BardCmdDynamicCall( BardCmd context, BardMethod method_info, BardArgs args )
  {
    this.context = context;
    this.method_info = method_info;
    this.args = args;
  }

  public BardCmdDynamicCall( BardMethod method_info, BardArgs args )
  {
    this( null, method_info, args );
  }

  public BardCmd duplicate() { return new BardCmdDynamicCall( duplicate(context), method_info, duplicate(args) ).set_source_info(this); }

  public String toString()
  {
    StringBuilder buffer = new StringBuilder();
    if (context != null)
    {
      buffer.append( context );
      buffer.append( '.' );
    }
    buffer.append( method_info.name );
    buffer.append( "(" );
    if (args != null)
    {
      boolean first = true;
      for (BardCmd arg : args)
      {
        if (first) first = false;
        else       buffer.append(',');
        buffer.append( arg );
      }
    }
    buffer.append( ")" );
    if (method_info.return_type != null)
    {
      buffer.append('.');
      buffer.append( method_info.return_type );
    }
    return buffer.toString();
  }

  public BardType type()
  {
    return method_info.return_type;
  }

  public BardCmd resolve( BardMethod method_context )
  {
    if (context != null) context = context.resolve( method_context );

    if (args != null)
    {
      // Cast each argument to the corresponding parameter type
      for (int i=0; i<args.size(); ++i)
      {
        args.set( i, args.get(i).resolve(method_context).cast_to( method_info.parameters.get(i).type ).resolve(method_context) );
      }
    }

    if (method_info.is_built_in())
    {
      BardCompiler bc = info.bc;
      if (context == null) context = new BardCmdThis( method_context.type_context );

      if (method_info.type_context == bc.type_List)
      {
        if (method_info.signature.equals("count()"))
        {
          return new BardCmdList.Count(context).set_source_info(this);
        }
        else if (method_info.signature.equals("get(Integer)"))
        {
          return new BardCmdList.Get(context,args.get(0)).set_source_info(this);
        }
        else if (method_info.signature.equals("add(Variant)"))
        {
          return new BardCmdList.Add(context,args).set_source_info(this);
        }
        else if (method_info.signature.equals("clear()"))
        {
          return new BardCmdList.Clear(context).set_source_info(this);
        }
        else if (method_info.signature.equals("remove_at(Integer)"))
        {
          return new BardCmdList.RemoveAt(context,args.get(0)).set_source_info(this);
        }
        else if (method_info.signature.equals("insert(Variant)"))
        {
          BardCmd zero = new BardCmdLiteral.LiteralInteger( 0 ).set_source_info( this ); 
          return new BardCmdList.Insert(context,args.get(0),zero).set_source_info(this);
        }
        else if (method_info.signature.equals("insert(Variant,Integer)"))
        {
          return new BardCmdList.Insert(context,args.get(0),args.get(1)).set_source_info(this);
        }
      }
      else if (method_info.type_context == bc.type_Table)
      {
        if (method_info.signature.equals("count()"))
        {
          return new BardCmdTable.Count(context).set_source_info(this);
        }
        else if (method_info.signature.equals("get(Variant)"))
        {
          return new BardCmdTable.GetKey(context,args.get(0)).set_source_info(this);
        }
        else if (method_info.signature.equals("set(Variant,Variant)"))
        {
          return new BardCmdTable.SetKey(context,args.get(0),args.get(1)).set_source_info(this);
        }
        /*
        else if (method_info.signature.equals("add(Variant)"))
        {
          return new BardCmdList.Add(context,args).set_source_info(this);
        }
        else if (method_info.signature.equals("clear()"))
        {
          return new BardCmdList.Clear(context).set_source_info(this);
        }
        else if (method_info.signature.equals("remove_at(Integer)"))
        {
          return new BardCmdList.RemoveAt(context,args.get(0)).set_source_info(this);
        }
        else if (method_info.signature.equals("insert(Variant)"))
        {
          BardCmd zero = new BardCmdLiteral.LiteralInteger( 0 ).set_source_info( this ); 
          return new BardCmdList.Insert(context,args.get(0),zero).set_source_info(this);
        }
        else if (method_info.signature.equals("insert(Variant,Integer)"))
        {
          return new BardCmdList.Insert(context,args.get(0),args.get(1)).set_source_info(this);
        }
        */
      }
    }


    return this;
  }

  public void compile( BardCodeWriter writer )
  {
    if (context == null) writer.write( BardOp.push_Object_this );
    else                 context.compile( writer );
    if (args != null) args.compile( writer );
    writer.write( BardOp.dynamic_call );
    writer.write( method_info.parameter_count, method_info.method_id );
  }
}

