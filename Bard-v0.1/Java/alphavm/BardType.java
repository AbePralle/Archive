package alphavm;

import java.util.*;

public class BardType
{
  final static public int
    variant_Object      = 0,
    variant_Object_null = 1,  // This lets us avoid null checks in variant operations
    variant_Real        = 2,
    variant_Integer     = 3,
    variant_Character   = 4,
    variant_Logical     = 5;

  final static public int[] variant_stack_slots = { 1, 1, 1, 1, 1, 1 };

  static public BardMethodList method_collection_list = new BardMethodList();

  public BardVM     vm;

  public String name;
  public BardPropertyList properties = new BardPropertyList();
  public BardMethodList   methods = new BardMethodList();

  public BardType     primitive_type;  // if non-null then this type is a wrapper
  public BardType     wrapper_type;  // wrapper type that corresponds to this built-in type

  public BardTypeList base_types = new BardTypeList();
  public BardObject   singleton;
  public BardObject   free_list;

  public BardObjectFactory native_factory;  // for native objects

  public boolean visiting;
  public boolean defined;
  public int     stage = BardLinkStage.unresolved;
  public int     qualifiers;
  public int     index;
  public int     variant_type;

  public BardMethod m_init_object;


  public BardType( BardVM vm, String name )
  {
    this.vm = vm;
    this.name = vm.id_table.consolidate( name );
  }

  public void reset()
  {
    properties.clear();
    methods.clear();
    singleton = null;
    defined = false;
    stage = BardLinkStage.unresolved;
  }

  public String toString()
  {
    if (is_primitive()) return name.substring( name.indexOf(':')+1 );
    return name;
  }

  public boolean is_built_in() { return (qualifiers & BardQualifiers.is_built_in) != 0; }
  public boolean is_singleton() { return (qualifiers & BardQualifiers.is_singleton) != 0; }
  public boolean is_primitive() { return (qualifiers & BardQualifiers.is_primitive) != 0; }

  public boolean is_Object()  { return this == vm.type_Object; }
  public boolean is_String()  { return this == vm.type_String; }
  public boolean is_Real()  { return this == vm.type_Real; }
  public boolean is_Integer()   { return this == vm.type_Integer; }
  public boolean is_Character()    { return this == vm.type_Character; }
  public boolean is_Logical() { return this == vm.type_Logical; }

  public boolean is_numeric()
  {
    return (this == vm.type_Real)
      || (this == vm.type_Integer);
  }

  public boolean instance_of( BardType ancestor )
  {
    if (this == ancestor) return true;
    for (int i=base_types.size(); --i>=0; )
    {
      BardType base_type = base_types.get(i);
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
    return properties.find( vm.id_table.get_id(name) );
  }

  public BardProperty must_find_property( String name )
  {
    BardProperty result = properties.find( vm.id_table.get_id(name) );
    if (result != null) return result;
    throw new RuntimeException( "No such property: " + name );
  }

  public BardMethod find_method( String signature )
  {
    return methods.find( vm.id_table.get_id( signature ) );
  }

  public BardMethod must_find_method( String signature )
  {
    BardMethod result = methods.find( vm.id_table.get_id( signature ) );
    if (result != null) return result;
    throw new RuntimeException( "No such method: " + signature );
  }

  public void add( BardProperty p )
  {
    p.index = properties.size();
    properties.add( p );
  }

  public void add( BardMethod m )
  {
    if (m.name.equals("init_object")) m_init_object = m;
    methods.add( m );
  }

  public void invalidate()
  {
    stage = BardLinkStage.unresolved;
  }

  public void organize( BardType starting_type )
  {
    if (stage >= BardLinkStage.organized) return;
    stage = BardLinkStage.organized;

    if (((qualifiers & BardQualifiers.is_native) != 0) && native_factory == null && ((qualifiers & BardQualifiers.is_built_in) == 0))
    {
      throw new BardError( "No object factory is defined for native type " + name + "." ); 
    }

    if (visiting) throw new BardError( starting_type + " base type specification creates cyclic inheritance." );
    visiting = true;

    if (!is_built_in() && base_types.size() == 0 && this != vm.type_Object)
    {
      base_types.add( vm.type_Object );
    }

    // Organize base types
    for (BardType base_type : base_types) base_type.organize( starting_type );

    // Define method signatures
    for (int i=0; i<methods.count; ++i)
    {
      methods.get(i).organize();
    }

    // Inherit base type methods
    method_collection_list.clear();
    for (BardType base_type : base_types) base_type.collect_methods( method_collection_list );
    collect_methods( method_collection_list );
    methods.clear();
    for (int i=0; i<method_collection_list.count; ++i)
    {
      methods.add( method_collection_list.get(i) );
    }
//System.out.println( name + ": " + methods );
//for (BardType base_type : base_types) System.out.println( "  : " + base_type + ": " + base_type.methods );

    visiting = false;
  }

  public void collect_methods( BardMethodList list )
  {
    for (int i=0; i<methods.count; ++i)
    {
      BardMethod m = methods.get(i);
      int index = list.index_of( m.method_id );
      if (index != -1) list.set( index, m );
      else             list.add( m );
    }
  }

  public void resolve()
  {
    if (stage >= BardLinkStage.resolved) return;
    stage = BardLinkStage.resolved;

//System.out.println( "Resolving " + name );
    for (int i=0; i<methods.count; ++i)
    {
      methods.get(i).resolve();
    }
  }

  public BardObject create_object()
  {
    if (free_list != null)
    {
      BardObject result = free_list;
      free_list = free_list.next_object;
      vm.active_objects.add( result );
      return result.reset();
    }
    else
    {
//System.out.println("Creating a " + name );
      ++vm.objects_created;
      BardObject result;
      if (native_factory != null) result = native_factory.create( this );
      else                        result = new BardObject( this );
      vm.active_objects.add( result );
      return result;
    }
  }

  public BardObject create_singleton()
  {
    if (singleton == null)
    {
      singleton = create_object();
    }
    return singleton;
  }
}

