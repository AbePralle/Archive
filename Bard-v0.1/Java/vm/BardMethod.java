package vm;

import java.util.*;

public class BardMethod
{
  static public StringBuilder signature_buffer = new StringBuilder();
  static public boolean       use_return_types_in_method_signatures;

  public BardType type_context;
  public String   name;
  public String   signature;
  public ArrayList<BardLocal> parameters = new ArrayList<BardLocal>();
  public ArrayList<BardLocal> locals     = new ArrayList<BardLocal>();
  public BardType return_type;

  public int      qualifiers;
  public int      parameter_count;
  public int      expected_stack_delta;
  public int      local_count;
  public int      method_id;
  public int      scope_level;
  public int      min_args;

  public int stage = BardLinkStage.unresolved;

  public int initial_ip = -1;
  public int[] code;
  public BardNativeMethod  native_method;

  public BardString[] String_literals;
  public double[]     Real_literals;

  public BardMethod( BardType type_context, String name )
  {
    this.type_context = type_context;
    this.name         = type_context.vm.id_table.consolidate( name );
  }

  public boolean is_built_in()
  {
    return (qualifiers & BardQualifiers.is_built_in) != 0;
  }

  public boolean is_native()
  {
    return (qualifiers & BardQualifiers.is_native) != 0;
  }

  public void clear()
  {
  }

  public void create_signature()
  {
    signature_buffer.setLength( 0 );
    signature_buffer.append( name );
    signature_buffer.append( '(' );
    boolean first = true;
    for (BardLocal p : parameters)
    {
      if (first) first = false;
      else       signature_buffer.append(',');
      signature_buffer.append( p.type.toString() );
    }
    signature_buffer.append( ')' );

    signature = type_context.vm.id_table.consolidate( signature_buffer );

    if (use_return_types_in_method_signatures)
    {
      if (return_type != null)
      {
        BardVM vm = type_context.vm;
        signature_buffer.append('.');
        if (return_type == vm.type_Real) signature_buffer.append( "Real" );
        else if (return_type == vm.type_Integer) signature_buffer.append( "Integer" );
        else if (return_type == vm.type_Character) signature_buffer.append( "Character" );
        else if (return_type == vm.type_Logical) signature_buffer.append( "Logical" );
        else  signature_buffer.append( return_type.name );
      }
      method_id = type_context.vm.id_table.get_id( signature_buffer );
    }
    else
    {
      method_id = type_context.vm.id_table.get_id( signature_buffer );
    }
  }

  public void organize()
  {
    if (stage >= BardLinkStage.organized) return;
    stage = BardLinkStage.organized;

    if (return_type == null)
    {
      BardType name_as_type = type_context.vm.find_type( name );
      if (name_as_type != null)
      {
        return_type = name_as_type;
        if (return_type.primitive_type != null) return_type = return_type.primitive_type;
      }
    }

    expected_stack_delta = (parameter_count + 1);
    if (return_type != null) --expected_stack_delta;

    create_signature();
    scope_level = 0;
  }

  public void resolve()
  {
    if (stage >= BardLinkStage.resolved) return;
    stage = BardLinkStage.resolved;
  }

  public void add_parameter( BardLocal v )
  {
    v.scope_level = scope_level;
    parameters.add( v );
    locals.add( v );
    v.index = -locals.size();
    ++parameter_count;
    ++local_count;
    min_args = parameter_count;
  }

  public void add_local( BardLocal v )
  {
    v.scope_level = scope_level;
    locals.add( v );
    v.index = -locals.size();
    ++local_count;
  }

  public BardLocal find_local( String name )
  {
    for (int i=locals.size()-1; i>=0; --i)
    {
      BardLocal v = locals.get(i);
      if (v.scope_level >= 0 && v.name.equals(name)) return v;
    }
    return null;
  }
}

