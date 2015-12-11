package compiler;

import alphavm.BardOp;

import java.util.*;

public class BardType
{
  final static public int
    variant_Object      = 0,
    variant_Object_null = 1,
    variant_Real        = 2,
    variant_Integer     = 3,
    variant_Character        = 4,
    variant_Logical     = 5;

  final static public int[] variant_stack_slots = { 1, 1, 1, 1, 1, 1, 1 };

  static public BardPropertyList property_collection_list = new BardPropertyList();
  static public BardMethodList   method_collection_list = new BardMethodList();

  public BardCompiler     bc;

  public String name;
  public BardPropertyList properties = new BardPropertyList();
  public BardMethodList   methods = new BardMethodList();

  public BardType     primitive_type;  // if non-null then this type is a wrapper
  public BardType     wrapper_type;  // wrapper type that corresponds to this built-in type

  public BardTypeList base_types = new BardTypeList();

  public boolean visiting;
  public boolean defined;
  public int     stage = BardCompileStage.unresolved;
  public int     qualifiers;
  public int     index;
  public int     variant_type;

  public BardMethod m_init_object;

  public BardType( BardCompiler bc, String name )
  {
    this.bc = bc;
    this.name = bc.id_table.consolidate( name );
    m_init_object = new BardMethod( this, "init_object" );
    m_init_object.return_type = this;
    add( m_init_object );
  }

  public void reset()
  {
    properties.clear();
    methods.clear();
    defined = false;
    stage = BardCompileStage.unresolved;

    add( m_init_object );
  }

  public String toString()
  {
    if (is_primitive()) return name.substring( name.indexOf(':')+1 );
    return name;
  }

  public boolean is_built_in() { return (qualifiers & BardQualifiers.is_built_in) != 0; }
  public boolean is_primitive() { return (qualifiers & BardQualifiers.is_primitive) != 0; }

  public boolean is_Object()  { return this == bc.type_Object; }
  public boolean is_String()  { return this == bc.type_String; }
  public boolean is_Real()  { return this == bc.type_Real; }
  public boolean is_Integer()   { return this == bc.type_Integer; }
  public boolean is_Character()    { return this == bc.type_Character; }
  public boolean is_Logical() { return this == bc.type_Logical; }
  public boolean is_Variant() { return this == bc.type_Variant; }

  public boolean is_numeric()
  {
    return (this == bc.type_Real)
      || (this == bc.type_Integer);
  }

  public boolean instance_of( BardType ancestor )
  {
    if (ancestor == bc.type_Variant) return true;

    if (this == ancestor) return true;

    for (BardType base_type : base_types)
    {
      if (base_type.instance_of(ancestor)) return true;
    }
    return false;
  }

  public BardType true_type()
  {
    if (primitive_type != null) return primitive_type;
    return this;
  }

  public BardType object_type()
  {
    if (wrapper_type != null) return wrapper_type;
    return this;
  }

  public BardProperty find_property( String name )
  {
    return properties.find( bc.id_table.get_id(name) );
  }

  public BardMethod find_method( String signature )
  {
    return methods.find( bc.id_table.get_id( signature ) );
  }

  public void add( BardProperty p )
  {
    p.index = properties.size();
    properties.add( p );
  }

  public void add( BardMethod m )
  {
    methods.add( m );
  }

  public void invalidate()
  {
    stage = BardCompileStage.unresolved;
  }

  public void organize( BardType starting_type )
  {
    if (stage >= BardCompileStage.organized) return;
    stage = BardCompileStage.organized;

    if (visiting) throw new BardError( starting_type + " base type specification creates cyclic inheritance." );
    visiting = true;

    if (!is_built_in() && base_types.size() == 0)
    {
      base_types.add( bc.type_Object );
    }

    // Organize base types
    for (BardType base_type : base_types) base_type.organize( starting_type );

    // Inherit base type properties
    property_collection_list.clear();
    for (BardType base_type : base_types) base_type.collect_properties( property_collection_list );
    collect_properties( property_collection_list );
    properties.clear();
    properties.addAll( property_collection_list );

    // Define method signatures
    for (BardMethod m : methods) m.organize();

    // Inherit base type methods
    method_collection_list.clear();
    for (BardType base_type : base_types) base_type.collect_methods( method_collection_list );
    collect_methods( method_collection_list );
    methods.clear();
    methods.addAll( method_collection_list );
//System.out.println( name + ": " + methods );
//for (BardType base_type : base_types) System.out.println( "  : " + base_type + ": " + base_type.methods );

    visiting = false;
  }

  public void collect_properties( BardPropertyList list )
  {
    for (BardProperty p : properties)
    {
      int index = list.index_of( p.property_id );
      if (index != -1)
      {
        list.set( index, p );
        p.index = index;
      }
      else
      {
        p.index = list.size();
        list.add( p );
      }
    }
  }

  public void collect_methods( BardMethodList list )
  {
    for (BardMethod m : methods)
    {
      int index = list.index_of( m.method_id );
      if (index != -1) list.set( index, m );
      else             list.add( m );
    }
  }

  public void resolve()
  {
    if (stage >= BardCompileStage.resolved) return;
    stage = BardCompileStage.resolved;

    m_init_object.statements.clear();
    for (BardProperty p : properties)
    {
      if (p.initial_value != null)
      {
        m_init_object.statements.add( new BardCmdWriteProperty(null,p,p.initial_value) );
      }
    }
    m_init_object.statements.add( new BardCmdReturn().init(new BardCmdThis(this)) );

//System.out.println( "Resolving " + name );
    for (BardMethod m : methods) m.resolve();
  }

  public void compile( BardCodeWriter code_writer )
  {
    if (stage >= BardCompileStage.compiled) return;
    stage = BardCompileStage.compiled;

    for (BardMethod m : methods) m.compile( code_writer );
  }
}

