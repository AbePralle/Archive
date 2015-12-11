package compiler;

import alphavm.BardOp;

import java.util.*;

public class BardCmd
{
  static public ArrayList<BardMethod> candidate_methods = new ArrayList<BardMethod>();
  static public ArrayList<BardMethod> matching_methods = new ArrayList<BardMethod>();
  static public ArrayList<BardType>   arg_types = new ArrayList<BardType>();

  public int           line, column;
  public BardFileInfo  info;

  public BardCmd()
  {
  }

  public BardCmd duplicate( BardCmd other )
  {
    if (other == null) return null;
    return other.duplicate();
  }

  public BardArgs duplicate( BardArgs args )
  {
    if (args == null) return null;
    return args.duplicate();
  }

  public BardCmd duplicate() { throw new RuntimeException( getClass().getName()+"::duplicate() is not defined." ); }

  public String toString() { return getClass().getName(); }

  public BardCmd set_source_info( BardCmd other )
  {
    if (other.info == null) throw new RuntimeException( "info is null for " + other.getClass().getName() );
    this.info    = other.info;
    this.line    = other.line;
    this.column  = other.column;
    return this;
  }

  public BardCmd set_source_info( BardScanner scanner )
  {
    this.info   = scanner.file_info;
    this.line   = scanner.previous_line;
    this.column = scanner.previous_column;
    return this;
  }

  public BardError error( String message )
  {
    return new BardError( message, (info != null) ? info.filepath : "[Unknown]", line, column );
  }

  public BardType type() 
  { 
    throw error("Unsupported call to " + getClass().getName() + "::type()." ); 
  }

  public boolean is_literal() { return false; }

  public BardCmd require_Logical()
  {
    BardCompiler bc = info.bc;
    BardType my_type = type();
    if (my_type == info.bc.type_Logical_wrapper) return cast_to( bc.type_Logical );
    if (my_type != info.bc.type_Logical) throw error( "Logical value expected." );
    return this;
  }

  public BardCmd require_value()
  {
    if (type() == null) throw error( "Value expected." );
    return this;
  }

  public BardCmd resolve( BardMethod m )
  { 
    throw new RuntimeException("Unsupported call to " + getClass().getName() + "::resolve()." ); 
  }

  public BardMethod find_method( BardMethod method_context, BardCmd context, String name, BardArgs args, boolean must_find )
  {
    name = info.bc.id_table.consolidate( name );  // allows us to use == instead of equals()

    BardType context_type;
    if (context != null)
    {
      context_type = context.type();
    }
    else
    {
      context_type = method_context.type_context;
    }

    if (context_type == info.bc.type_Variant) context_type = info.bc.type_Object;

    context_type = context_type.object_type();

    if (args != null)
    {
      args.resolve( method_context );

      arg_types.clear();
      int arg_count = args.size();
      for (int i=0; i<arg_count; ++i) arg_types.add( args.get(i).type() );
    }

    BardMethod m = find_method( context_type, name, args );

    if (m == null && must_find) throw unresolved_method_error( context_type, name, args );

    return m;
  }

  public BardMethod find_method( BardType context_type, String name, BardArgs args )
  {
    int arg_count = 0;
    if (args != null)
    {
      arg_count = args.size();
    }

    // Add all methods with matching name
    candidate_methods.clear();
    for (BardMethod m : context_type.methods)
    {
      // Note: '==' is okay here instead of equals()
      if (m.name == name) candidate_methods.add( m );
    }
    if (candidate_methods.size() == 0) return null;

    // Cull out candidates requiring too many or too few arguments.
    matching_methods.clear();
    for (BardMethod m : candidate_methods)
    {
      if (arg_count >= m.min_args && arg_count <= m.parameters.size())
      {
        matching_methods.add( m );
      }
    }
    if (matching_methods.size() == 0) return null;
    if (matching_methods.size() == 1) return matching_methods.get(0);

    candidate_methods.clear();
    candidate_methods.addAll( matching_methods );
    matching_methods.clear();

    // Check for exact parameter match
    for (BardMethod m : candidate_methods)
    {
      boolean match = true;
      for (int i=0; i<arg_count; ++i)
      {
        BardType arg_type = arg_types.get(i).true_type();
        BardType param_type = m.parameters.get(i).type;
        if (arg_type != param_type)
        {
          match = false;
          break;
        }
      }
      if (match) matching_methods.add( m );
    }

    if (matching_methods.size() > 0)
    {
      if (matching_methods.size() == 1) return matching_methods.get(0);
      candidate_methods.clear();
      candidate_methods.addAll( matching_methods );
      matching_methods.clear();
    }

    // Check for numeric match
    for (BardMethod m : candidate_methods)
    {
      boolean match = true;
      for (int i=0; i<arg_count; ++i)
      {
        BardType arg_type = arg_types.get(i).true_type();
        BardType param_type = m.parameters.get(i).type;
        if (arg_type.is_numeric() ^ param_type.is_numeric())
        {
          match = false;
          break;
        }
      }
      if (match) matching_methods.add( m );
    }

    if (matching_methods.size() > 0)
    {
      if (matching_methods.size() == 1) return matching_methods.get(0);
      candidate_methods.clear();
      candidate_methods.addAll( matching_methods );
      matching_methods.clear();
    }

    // Check for compatible match
    for (BardMethod m : candidate_methods)
    {
      boolean match = true;
      for (int i=0; i<arg_count; ++i)
      {
        BardType arg_type = arg_types.get(i).true_type();
        BardType param_type = m.parameters.get(i).type;
        if ( !arg_type.instance_of(param_type) )
        {
          if ( !(args.get(i) instanceof BardCmdLiteral.LiteralNull) || !param_type.instance_of(info.bc.type_Object) )
          {
            match = false;
            break;
          }
        }
      }
      if (match) matching_methods.add( m );
    }


    if (matching_methods.size() > 0)
    {
      if (matching_methods.size() == 1) return matching_methods.get(0);
      candidate_methods.clear();
      candidate_methods.addAll( matching_methods );
      matching_methods.clear();
    }

    return null;
  }

  public BardError unresolved_method_error( BardType context_type, String name, BardArgs args )
  {
    StringBuilder buffer = BardMethod.signature_buffer;
    buffer.setLength( 0 );
    buffer.append( name );
    buffer.append( '(' );
    boolean first = true;
    if (args != null)
    {
      for (BardCmd arg : args)
      {
        if (first) first = false;
        else       buffer.append( "," );
        buffer.append( arg.type().toString() );
      }
    }
    buffer.append( ')' );

    return error( "No such method " + buffer + " in type " + context_type.toString() + "." );
  }


  public BardCmd resolve_call( BardMethod method_context, BardCmd context, String name, BardArgs args, boolean must_find )
  {
    if (context != null)
    {
      context = context.resolve( method_context ).require_value();
      if (context.type() == info.bc.type_Variant) context = context.cast_to( info.bc.type_Object );
    }

    BardMethod m = find_method( method_context, context, name, args, must_find );
    if (m == null) return null;  // will only be null if 'must_find' is false

    if (context != null)
    {
      return new BardCmdDynamicCall( context, m, args ).set_source_info( this ).resolve( method_context );
    }

    return new BardCmdDynamicCall( m, args ).set_source_info( this ).resolve( method_context );
  }

  public BardCmd resolve_primitive_call( BardMethod method_context, BardCmd context, String name, BardArgs args )
  {
    if (args == null) args = new BardArgs();

    BardType primitive_type = context.type();
    args.add( 0, context );
    BardCmd result = resolve_call( method_context, 
        new BardCmdReadSingleton(primitive_type.object_type()),
        name, args, false );
    if (result != null) return result;

    args.remove( 0 );
    context = context.cast_to( primitive_type.object_type() );
    result = resolve_call( method_context, context, name, args, true );
    return result;
  }

  public BardCmd resolve_assignment( BardMethod method_context, BardCmd new_value )
  {
    throw error( "Invalid assignment '" + this + " = " + new_value + "'." );
  }

  public BardCmd resolve_adjustment( BardMethod method_context, int delta )
  {
    BardCmdBinaryOp adjusted;
    if (delta == 1) adjusted = new BardCmdBinaryOp.Add();
    else            adjusted = new BardCmdBinaryOp.Subtract();
    adjusted.init( this.duplicate(), new BardCmdLiteral.LiteralInteger(1).set_source_info(this) );
    adjusted.set_source_info( this );
    return new BardCmdAssign( this, adjusted ).set_source_info(this).resolve( method_context );
  }

  public void compile( BardCodeWriter writer )
  {
    throw error( getClass().getName() + "::compile() is undefined." );
  }

  public BardType common_type( BardType t1, BardType t2 )
  {
    BardCompiler bc = info.bc;

    // For now objects are converted to primitives for math ops.  Later on
    // we'll call operator methods on the object instead. 
    //if (t1.instance_of(bc.type_Object) || t2.instance_of(bc.type_Object))
    //{
    //  if (t2.instance_of(t1)) return t2;
    //  return t1;
    //}

    if (t1 == bc.type_Variant || t2 == bc.type_Variant) return bc.type_Variant;

    if (promote_Character_to_String())
    {
      if (t1 == bc.type_Character   || t2 == bc.type_Character)   return bc.type_String;
    }

    if (t1 == t2)
    {
      //if (t1 == bc.type_Character) return bc.type_Integer;
      return t1;
    }

    if (t1 == bc.type_String || t2 == bc.type_String) return bc.type_String;

    if (t1 == bc.type_Logical || t2 == bc.type_Logical)
    {
      throw error( "Invalid operation on mix of Logical and non-Logical types." );
    }

    if (t1.instance_of(bc.type_Object) || t2.instance_of(bc.type_Object)) return bc.type_Object;

    if (t1 == bc.type_Real || t2 == bc.type_Real) return bc.type_Real;
    return bc.type_Integer;
  }

  public BardCmd cast_to( BardType to_type )
  {
    BardType from_type = type();
    if (from_type == to_type) return this;
    return new BardCmdCast( this, to_type ).set_source_info(this);
  }

  public String name() { return getClass().getName(); }
  public int    opcode_Variant() { return -1; }
  public int    opcode_Object() { return -1; }
  public int    opcode_Real() { return -1; }
  public int    opcode_Integer()  { return -1; }
  public int    opcode_Character()  { return -1; }
  public int    opcode_Logical(){ return -1; }

  public boolean promote_Character_to_String() { return true; };
}

