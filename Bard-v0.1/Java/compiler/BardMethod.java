package compiler;

import alphavm.BardOp;

import java.util.*;

public class BardMethod
{
  static public StringBuilder signature_buffer = new StringBuilder();

  public BardType type_context;
  public String   name;
  public String   signature;
  public ArrayList<BardLocal> parameters = new ArrayList<BardLocal>();
  public ArrayList<BardLocal> locals     = new ArrayList<BardLocal>();
  public BardType return_type;

  public int      qualifiers;
  public int      parameter_count;
  public int      local_count;
  public int      method_id;
  public int      scope_level;
  public int      min_args;

  public int stage = BardCompileStage.unresolved;

  public BardStatementList statements = new BardStatementList();
  public int[] code;

  public String[] String_literals;
  public double[] Real_literals;

  public BardMethod( BardType type_context, String name )
  {
    this.type_context = type_context;
    this.name         = type_context.bc.id_table.consolidate( name );
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
    // Only used to clear statements and locals for immediate-mode method Global::immediate().
    local_count = 0;
    locals.clear();
    statements.clear();
    scope_level = 0;
    stage = BardCompileStage.unresolved;
    type_context.invalidate();
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

    method_id = type_context.bc.id_table.get_id( signature_buffer );
    signature = type_context.bc.id_table.get_name( method_id );
  }

  public void organize()
  {
    if (stage >= BardCompileStage.organized) return;
    stage = BardCompileStage.organized;

    if (return_type == null)
    {
      BardType name_as_type = type_context.bc.find_type( name );
      if (name_as_type != null)
      {
        return_type = name_as_type;
        if (return_type.primitive_type != null) return_type = return_type.primitive_type;
      }
    }

    create_signature();
    scope_level = 0;
  }

  public void resolve()
  {
    if (stage >= BardCompileStage.resolved) return;
    stage = BardCompileStage.resolved;

//System.out.println( "Resolving " + signature );
    if (return_type == null)
    {
      statements.add( new BardCmdReturn().init( null ) );
    }

    statements.resolve( this );
  }

  public void compile( BardCodeWriter writer )
  {
    if (stage >= BardCompileStage.compiled) return;
    stage = BardCompileStage.compiled;

//System.out.println( "Compiling " + signature );
    String_literals = null;
    Real_literals = null;
    writer.reset( this );
    statements.compile( writer );
    writer.write_default_return( return_type );
    writer.finish();
    code = writer.to_code();
//if (code != null)
//{
//System.out.println( signature + " code length:" + code.length);
//for (int i=0; i<code.length; ++i) System.out.print( code[i] + " " );
//System.out.println();
//}
    if (writer.String_literals.count > 0) String_literals = writer.String_literals.to_array();
    if (writer.Real_literals.count > 0) Real_literals = writer.Real_literals.to_array();
  }

  public void open_scope()
  {
    ++scope_level;
  }
  
  public void close_scope()
  {
    // this scoping solution is not the best - needs rework
    --scope_level;
    if (scope_level < 0) throw new RuntimeException( "[Internal] - close_scope() without matching open_scope()." );

    for (BardLocal v : locals)
    {
      if (v.scope_level > scope_level) v.scope_level = -1;
    }
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

