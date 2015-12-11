package alphavm;

import java.util.*;

// TODO: never remove types from list for incremental link - messes up singleton index

public class BardVM
{
  public BardIDTable id_table = new BardIDTable();

  public ArrayList<BardType>      type_list  = new ArrayList<BardType>();
  public HashMap<String,BardType> type_table = new HashMap<String,BardType>();

  public ArrayList<BardObject>    singletons = new ArrayList<BardObject>();

  public BardVMObjectList   active_objects = new BardVMObjectList(256);
  public int objects_created;

  public ArrayList<BardNativeMethod> native_methods = new ArrayList<BardNativeMethod>();

  public ArrayList<BardType>      implicit_context_list = new ArrayList<BardType>();

  public BardProcessor main_processor;

  public BardType type_Real;
  public BardType type_Integer;
  public BardType type_Character;
  public BardType type_Logical;
  public BardType type_NullObject;

  public BardType type_Real_wrapper;
  public BardType type_Integer_wrapper;
  public BardType type_Character_wrapper;
  public BardType type_Logical_wrapper;

  public BardType type_Variant;
  public BardType type_Object;
  public BardType type_Value;
  public BardType type_Collection;
  public BardType type_List;
  public BardType type_Table;

  public BardType type_String;
  public BardType type_IntegerList;
  public BardType type_CharacterList;
  public BardType type_ByteList;

  public BardType type_Global;

  public BardObject NullObjectVariant_singleton;
  public BardObject RealVariant_singleton;
  public BardObject IntegerVariant_singleton;
  public BardObject CharacterVariant_singleton;
  public BardObject LogicalVariant_singleton;

  public BardMethod m_global_immediate;

  public boolean resolved;

  public void organize()
  {
    type_Object      = must_find_type( "Object" );
    type_Variant     = must_find_type( "Variant" );

    type_Real        = must_find_type( "Real" );
    type_Integer     = must_find_type( "Integer" );
    type_Character        = must_find_type( "Character" );
    type_Logical     = must_find_type( "Logical" );
    type_NullObject  = must_find_type( "null" );

    type_Real.variant_type    = BardType.variant_Real;
    type_Integer.variant_type = BardType.variant_Integer;
    type_Character.variant_type    = BardType.variant_Character;
    type_Logical.variant_type = BardType.variant_Logical;
    type_NullObject.variant_type = BardType.variant_Object_null;

    NullObjectVariant_singleton = new BardValue.NativeValue( type_NullObject );
    RealVariant_singleton = new BardValue.RealValue( type_Real );
    IntegerVariant_singleton = new BardValue.IntegerValue( type_Integer );
    CharacterVariant_singleton = new BardValue.CharacterValue( type_Character );
    LogicalVariant_singleton = new BardValue.LogicalValue( type_Logical );

    type_Real_wrapper    = must_find_type( "RealObject" );
    type_Integer_wrapper = must_find_type( "IntegerObject" );
    type_Character_wrapper    = must_find_type( "CharacterObject" );
    type_Logical_wrapper = must_find_type( "LogicalObject" );

    type_Real_wrapper.primitive_type = type_Real;
    type_Integer_wrapper.primitive_type = type_Integer;
    type_Character_wrapper.primitive_type = type_Character;
    type_Logical_wrapper.primitive_type = type_Logical;

    type_Real.wrapper_type = type_Real_wrapper;
    type_Integer.wrapper_type = type_Integer_wrapper;
    type_Character.wrapper_type = type_Character_wrapper;
    type_Logical.wrapper_type = type_Logical_wrapper;

    type_String      = must_find_type( "String" );
    type_IntegerList = must_find_type( "Integer[]" );
    type_CharacterList = must_find_type( "Character[]" );
    type_ByteList = must_find_type( "Byte[]" );

    type_Value      = must_find_type( "Value" );
    type_Global     = must_find_type( "Global" );
    type_Collection = must_find_type( "Collection" );
    type_List       = must_find_type( "List" );
    type_Table      = must_find_type( "Table" );

    type_Real_wrapper.native_factory = new BardObjectFactory()
    {
      public BardObject create( BardType of_type )
      {
        return new BardValue.RealValue( of_type, 0 );
      }
    };

    type_Integer_wrapper.native_factory = new BardObjectFactory()
    {
      public BardObject create( BardType of_type )
      {
        return new BardValue.IntegerValue( of_type, 0 );
      }
    };

    type_Character_wrapper.native_factory = new BardObjectFactory()
    {
      public BardObject create( BardType of_type )
      {
        return new BardValue.CharacterValue( of_type, 0 );
      }
    };

    type_Logical_wrapper.native_factory = new BardObjectFactory()
    {
      public BardObject create( BardType of_type )
      {
        return new BardValue.LogicalValue( of_type, 0 );
      }
    };

    type_Real.native_factory = type_Real_wrapper.native_factory;
    type_Integer.native_factory = type_Integer_wrapper.native_factory;
    type_Character.native_factory = type_Character_wrapper.native_factory;
    type_Logical.native_factory = type_Logical_wrapper.native_factory;

    type_String.native_factory = new BardObjectFactory()
    {
      public BardObject create( BardType of_type )
      {
        return new BardString( of_type, 0 );
      }
    };

    type_CharacterList.native_factory = new BardObjectFactory()
    {
      public BardObject create( BardType of_type )
      {
        return new BardCharacterList( of_type, 0 );
      }
    };

    type_IntegerList.native_factory = new BardObjectFactory()
    {
      public BardObject create( BardType of_type )
      {
        return new BardIntegerList( of_type, 0 );
      }
    };

    type_List.native_factory = new BardObjectFactory()
    {
      public BardObject create( BardType of_type )
      {
        return new BardList( of_type, 0 );
      }
    };

    type_Table.native_factory = new BardObjectFactory()
    {
      public BardObject create( BardType of_type )
      {
        return new BardTable( of_type );
      }
    };

    //type_Value.base_types.add( type_Object );
    //type_Real.base_types.add( type_Value );
    //type_Integer.base_types.add( type_Value );
    //type_Character.base_types.add( type_Value );
    //type_Logical.base_types.add( type_Value );
    //type_Collection.base_types.add( type_Value );

    m_global_immediate = new BardMethod( type_Global, "immediate" );
    type_Global.add( m_global_immediate );

    implicit_context_list.add( type_Global );

    main_processor = new BardProcessor( this, 4096 );

    BardStandardLibrary.install( this );

    for (int i=0; i<type_list.size(); ++i) type_list.get(i).index = i;

    for (BardType type : type_list) type.organize( type );
    for (BardType type : type_list) type.resolve();

    // install native methods
    for (BardNativeMethod native_method : native_methods)
    {
      BardType type = find_type( native_method.type_name );
      if (type != null)
      {
        BardMethod m = type.find_method( native_method.signature );
        if (m != null)
        {
          m.native_method = native_method;
        }
      }
    }

    // Create singleton objects
    singletons.clear();
    for (BardType type : type_list) singletons.add( type.create_singleton() );

    // Call init_object on all singletons.
    for (BardType type : type_list) 
    {
      BardObject obj = type.singleton;
      main_processor.push_Object( obj );
      if (type.m_init_object != null) main_processor.invoke( type.m_init_object );
    }

    // Call init() on all 'object' singletons that have it.
    for (BardType type : type_list) 
    {
      if (type.is_singleton())
      {
        BardMethod m = type.find_method( "init()" );
        if (m != null)
        {
          BardObject obj = type.singleton;
          main_processor.push_Object( obj );
          main_processor.invoke( m );
        }
      }
    }
  }

  public void register_native( String type_name, String signature, BardNativeMethod m )
  {
    m.type_name = type_name;
    m.signature = signature;
    native_methods.add( m );
  }

  public void register_native( String type_name, BardObjectFactory factory )
  {
    BardType type = find_type( type_name );
    if (type != null) type.native_factory = factory;
  }

  public BardType define_type( String name, int qualifiers )
  {
    BardType type = get_type_reference( name );
    type.reset();
    type.defined  = true;
    type.qualifiers = qualifiers;
    return type;
  }


  public BardType get_type_reference( String name )
  {
    BardType type = type_table.get( name );
    if (type != null) return type;

    type = new BardType( this, this.id_table.consolidate(name) );
    type_table.put( name, type );
    type_list.add( type );
    return type;
  }

  public BardType find_type( String name )
  {
    return type_table.get( name );
  }

  public BardType must_find_type( String name )
  {
    BardType result = type_table.get( name );
    if (result != null) return result;
    throw new BardError( "Required type is undefined: " + name );
  }

  public void push_Object( BardObject obj )
  {
    main_processor.push_Object( obj );
  }

  public void push_String( String value )
  {
    main_processor.push_String( value );
  }

  public void push_Real( double value )
  {
    main_processor.push_Real( value );
  }

  public void push_Integer( int value )
  {
    main_processor.push_Integer( value );
  }

  public void push_Character( int value )
  {
    main_processor.push_Character( value );
  }

  public void push_Logical( boolean value )
  {
    main_processor.push_Logical( value );
  }

  public BardList create_List_object( int capacity )
  {
    return new BardList( type_List, capacity );
  }

  public BardCharacterList create_CharacterList_object( int capacity )
  {
    return new BardCharacterList( type_CharacterList, capacity );
  }

  public BardByteList create_ByteList_object( int capacity )
  {
    return new BardByteList( type_ByteList, capacity );
  }

  public BardString create_String_object( String st )
  {
    return new BardString( type_String, st );
  }

  public BardString create_String( String st )
  {
    return new BardString( type_String, st );
  }

  public void mark_referenced_objects()
  {
//System.out.println ( "(" + active_objects.count + " active objects)" );
    // Start by adding all singletons to the queue.
    BardObject queue = null;
    for (int i=singletons.size()-1; i>=0; --i)
    {
      BardObject obj = singletons.get(i);
      if (obj != null)
      {
        obj.referenced = true;
        obj.next_object = queue;
        queue = obj;
      }
    }

    // Iteratively visit all visible objects.
    while (queue != null)
    {
      BardObject obj = queue;
      queue = queue.next_object;

      if (obj.type.native_factory != null)
      {
        // Let the native object figure out how to collect references.
        queue = obj.collect_references( queue );
      }
      else
      {
        BardObject[] properties = obj.properties;
        if (properties != null)
        {
          int i = properties.length;
          while (--i >= 0)
          {
            BardObject p = properties[i];
            if (p != null && !p.referenced)
            {
              p.referenced = true;
              p.next_object = queue;
              queue = p;
            }
          }
        }
      }
    }

    // Link free objects to per-type object pools and remove from active list
    for (int i=active_objects.count; --i>=0; )
    {
      BardObject cur = active_objects.data[i];
      if (cur.referenced)
      {
        cur.referenced = false;
      }
      else
      {
        active_objects.data[i] = active_objects.data[--active_objects.count];
        active_objects.data[ active_objects.count ] = null;

        cur.next_object = cur.type.free_list;
        cur.type.free_list = cur;
      }
    }
  }
}

