package alphavm;

public class BardProgram
{
  public BardVM vm;

  public int[] code;

  public BardMethod working_method;

  public BardProgram()
  {
  }

  public BardVM create_vm()
  {
    return new BardVM();
  }

  public void define_type( String name, int qualifiers, int property_count )
  {
    BardType type = vm.define_type( name, qualifiers );
    type.properties.ensureCapacity( property_count );
  }

  public void add_base_type( BardType type, String base_type_name )
  {
    type.base_types.add( vm.must_find_type(base_type_name) );
  }

  public void add_base_type( String type_name, String base_type_name )
  {
    add_base_type( vm.must_find_type(type_name), base_type_name );
  }

  public void add_property( BardType type, int i, String name, String type_name )
  {
    BardProperty p = new BardProperty( type, name );
    p.type = vm.must_find_type( type_name );
    type.add( p );
  }

  public void add_property( String type_context_name, int i, String name, String type_name )
  {
    add_property( vm.must_find_type(type_context_name), i, name, type_name );
  }

  public BardMethod add_method( String type_context_name, String name, int qualifiers, 
      int parameter_count, int local_count, String return_type_name,
      int[] code, int ip )
  {
    return add_method( type_context_name, name, qualifiers, parameter_count, local_count, return_type_name, code, ip+1, 0 );
  }

  public BardMethod add_method( String type_context_name, String name, int qualifiers, 
      int parameter_count, int local_count, String return_type_name,
      int[] code, int ip, int String_count )
  {
    // NOTE: New BardCompiler calls this method directly, old BardCompiler calls this method indirectly
    //       (hence the +1/-1 ip irregularity)
    BardType type = vm.must_find_type( type_context_name );
    BardMethod m = new BardMethod( type, name );
    m.qualifiers = qualifiers;
    m.parameters.ensureCapacity( parameter_count );
    m.locals.ensureCapacity( local_count );
    if (return_type_name != null) m.return_type = vm.must_find_type( return_type_name );

    m.code = code;
    m.initial_ip = ip - 1;

    type.add( m );

    working_method = m;

    if (String_count > 0) m.String_literals = new BardString[ String_count ];

    return m;
  }

  public void add_String_literal( int index, BardString st )
  {
    working_method.String_literals[index] = st;
  }

  public void add_parameter( BardMethod m, int i, String name, String type_name )
  {
    BardType type = vm.must_find_type( type_name );
    m.add_parameter( new BardLocal(name,type) );
  }

  public void add_local( BardMethod m, int i, String name, String type_name )
  {
    BardType type = vm.must_find_type( type_name );
    m.add_local( new BardLocal(name,type) );
  }

  public void add_parameter( int i, String name, String type_name )
  {
    add_parameter( working_method, i, name, type_name );
  }

  public void add_local( int i, String name, String type_name )
  {
    add_local( working_method, i, name, type_name );
  }

  public double[] to_double_array( long[] values )
  {
    double[] result = new double[ values.length ];
    for (int i=0; i<result.length; ++i)
    {
      result[i] = Double.longBitsToDouble( values[i] );
    }
    return result;
  }


  public void launch()
  {
    vm.organize();

    vm.push_Object( vm.must_find_type("Main").singleton );
    if (BardMethod.use_return_types_in_method_signatures)
    {
      vm.main_processor.invoke( "init().Main", 0 );
    }
    else
    {
      vm.main_processor.invoke( "init()", 0 );
    }

    if (vm.main_processor.fatal_error)
    {
      System.exit(1);
    }

    vm.mark_referenced_objects();

    //vm = null;
    //BardMM.release_all();
    //System.gc();

    if (false)
    {
      for (int i=0; i<10; ++i)
      {
        System.out.println( "---------------------------------------------" );
        vm.push_Object( vm.must_find_type("Main").singleton );
        vm.main_processor.invoke( "init()", 0 );

        vm.mark_referenced_objects();

        System.out.println( "Arrays allocated: " + BardMM.arrays_allocated );

        int count = 0;
        BardObject cur = vm.type_CharacterList.free_list;
        while (cur != null)
        {
          ++count;
          cur = cur.next_object;
        }
        System.out.println( "Free strings: " + count );
      }

      System.out.println( vm.active_objects.count + " active objects." );
      System.out.println( vm.objects_created + " objects created." );
    }
  }

  public void set_up_code( String[] code_strings, int decoded_size )
  {
    code = new int[ decoded_size ];

    int dest_pos = -1;
    for (String st : code_strings)
    {
      int len = st.length();
      for (int i=0; i<len; )
      {
        i = decode_int( st, i, ++dest_pos );
      }
    }
  }

  public int decode_int( String st, int i, int dest_pos )
  {
    int b;
    int b1 = st.charAt(i++);
    if (b1 <= 127)
    {
      b = b1;
    }
    else
    {
      switch (b1 & 0xf0)
      {
        case 0x80:  // %1000 xxxx yyyyyyyy
          {
            int b2 = st.charAt(i++);
            b = ((b1 & 15) << 8) | b2;
          }
          break;

        case 0x90:  // %1001 xxxx yyyyyyyy zzzzzzzz
          {
            int b2 = st.charAt(i++);
            int b3 = st.charAt(i++);
            b = ((b1 & 15) << 16) | (b2 << 8) | b3;
          }
          break;

        case 0xa0:  // %1010 wwww xxxxxxxx yyyyyyyy zzzzzzzz
          {
            int b2 = st.charAt(i++);
            int b3 = st.charAt(i++);
            int b4 = st.charAt(i++);
            b = ((b1 & 15) << 24) | (b2 << 16) | (b3 << 8) | b4;
          }
          break;

        case 0xb0:  // %1011 0000 wwwwwwww xxxxxxxx yyyyyyyy zzzzzzzz
          {
            b1 = st.charAt(i++);
            int b2 = st.charAt(i++);
            int b3 = st.charAt(i++);
            int b4 = st.charAt(i++);
            b = (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
          }
          break;

        default:  // %11xx xxxx
          {
            b = (b1 & 63) - 64;
          }
      }
    }
//System.out.println( b + " / " + Integer.toString(b,16) );
    code[dest_pos] = b;
    return i;
  }
}

