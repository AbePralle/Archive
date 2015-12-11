function BardVM( base64_bc )
{
  var bc_loader = new BardLoader( this, base64_bc );
  bc_loader.load();
}

BardVM.prototype.conversion_bytes = new Int8Array(8);
BardVM.prototype.conversion_reals = new Float64Array( BardVM.prototype.conversion_bytes.buffer );

BardVM.prototype.conversion_reals[0] = 1.0;
BardVM.prototype.low_high_byte_order = (BardVM.prototype.conversion_bytes[7] != 0);

BardVM.prototype.must_find_type = function( name )
{
  var type = this.type_lookup[ name ];
  if (type) return type;
  alert( "Can not find required type: " + name );
};

BardVM.prototype.set_sizes_and_offsets = function()
{
  this.type_Real      = this.must_find_type( "Real" );
  this.type_Integer   = this.must_find_type( "Integer" );
  this.type_Character = this.must_find_type( "Character" );
  this.type_Byte      = this.must_find_type( "Byte" );
  this.type_Logical   = this.must_find_type( "Logical" );

  this.type_Object    = this.must_find_type( "Object" );
  this.type_String    = this.must_find_type( "String" );
  this.type_StringBuilder = this.must_find_type( "StringBuilder" );

  this.type_Array_of_Logical   = this.type_lookup[ "Array<<Logical>>" ];
  this.type_Array_of_Byte      = this.type_lookup[ "Array<<Byte>>" ];
  this.type_Array_of_Character = this.type_lookup[ "Array<<Character>>" ];
  this.type_Array_of_Integer   = this.type_lookup[ "Array<<Integer>>" ];
  this.type_Array_of_Real      = this.type_lookup[ "Array<<Real>>" ];
  this.type_Array_of_Object    = this.type_lookup[ "Array<<Object>>" ];

  this.type_ByteList      = this.type_lookup[ "Byte[]" ];
  this.type_LogicalList   = this.type_lookup[ "Logical[]" ];
  this.type_CharacterList = this.type_lookup[ "Character[]" ];
  this.type_IntegerList   = this.type_lookup[ "Integer[]" ];
  this.type_RealList      = this.type_lookup[ "Real[]" ];
  this.type_ObjectList    = this.type_lookup[ "Object[]" ];

  for (var i=0; i<this.types.length; ++i) this.types[i].set_slot_count();

  for (var i=0; i<this.types.length; ++i)
  {
    var type = this.types[i];
    type.configure_settings();
    type.set_property_offsets();
  }

  // Collect various methods and property offsets
  this.List_data_offset  = this.type_ObjectList.find_property( "data" ).offset;
  this.List_count_offset = this.type_ObjectList.find_property( "count" ).offset;
  this.String_characters_offset  = this.type_String.find_property( "characters" ).offset;
  this.String_hash_code_offset = this.type_String.find_property( "hash_code" ).offset;

  // Add special array constructors
  if (this.type_Array_of_Logical)
  {
    this.type_Array_of_Logical.create = function(count)
    {
      var array = new BardObject( this );
      array.data = new Uint8Array( count );
      array.element_type = this.vm.type_Logical;
      return array;
    }
  }

  if (this.type_Array_of_Byte)
  {
    this.type_Array_of_Byte.create = function(count)
    {
      var array = new BardObject( this );
      array.data = new Uint8Array( count );
      array.element_type = this.vm.type_Byte;
      return array;
    }
  }

  if (this.type_Array_of_Character)
  {
    this.type_Array_of_Character.create = function(count)
    {
      var array = new BardObject( this );
      array.data = new Uint16Array( count );
      array.element_type = this.vm.type_Character;
      return array;
    }
  }

  if (this.type_Array_of_Integer)
  {
    this.type_Array_of_Integer.create = function(count)
    {
      var array = new BardObject( this );
      array.data = new Int32Array( count );
      array.element_type = this.vm.type_Integer;
      return array;
    }
  }

  if (this.type_Array_of_Real)
  {
    this.type_Array_of_Real.create = function(count)
    {
      var array = new BardObject( this );
      array.data = new Float64Array( count );
      array.element_type = this.vm.type_Real;
      return array;
    }
  }

  if (this.type_CharacterList)
  {
    //this.type_CharacterList.

    this.type_CharacterList.create = function( initial_capacity )
    {
      var vm = this.vm;
      var list = new BardObject( vm.type_CharacterList );
      list.properties[ vm.List_data_offset ] = vm.type_Array_of_Character.create( initial_capacity );
      list.properties[ vm.List_count_offset ] = 0;
      return list
    }
  }
};

BardVM.prototype.organize = function()
{
  // Types
  for (var i=0; i<this.types.length; ++i) this.types[i].organize();

  // Methods
  for (var i=0; i<this.methods.length; ++i) this.methods[i].organize();

  // Bridge native methods
  for (var i=0; i<this.methods.length; ++i)
  {
    var m = this.methods[i];
    if (m.is_native())
    {
      m.native_method = BardProcessor_on_unhandled_native_method;
      //this.code[m.ip+1] = Bard
    }
  }
  document.writeln( "TODO: assign native methods" );
};

BardVM.prototype.create_Array_of_Object = function( count )
{
  var array = new BardObject( this.type_Array_of_Object );
  array.data = [];

  var data = array.data;
  for (var i=0; i<count; ++i) data[i] = null;

  return array;
}

BardVM.prototype.create_string = function( content )
{
  var count = content.length;
  var st = new BardObject( this.type_String );
  var characters;
  var data;

  if (content.indexOf)
  {
    characters = this.type_CharacterList.create( count );

    data = characters.properties[ this.List_data_offset ].data;
    for (var i=0; i<count; ++i) data[i] = content.charCodeAt(i);
  }
  else
  {
    characters = new BardObject( this.type_CharacterList );
    var array = new BardObject( this.type_Array_of_Character );
    array.data = content;
    array.element_type = this.type_Character;
    characters.properties[ this.List_data_offset ] = array;
    data = content;
  }
  characters.properties[ this.List_count_offset ] = count;
  st.properties[ this.String_characters_offset ] = characters;

  var hash_code = 0;
  for (var i=0; i<count; ++i) hash_code = hash_code * 7 + data[i];
  st.properties[ this.String_hash_code_offset ] = hash_code & 0xffffFFFF;

  return st;
};

BardVM.prototype.string_to_js_string = function( st )
{
  return String.fromCharCode.apply( null, st.properties[this.String_characters_offset].properties[this.List_data_offset].data );
}

BardVM.prototype.register_native_method = function( type_name, signature, fn )
{
  this.native_methods = this.native_methods || [];
  this.native_methods.push( {"type_name":type_name, "signature":signature, "fn":fn} );
}

BardVM.prototype.create_singletons = function()
{
  this.singletons = [];

  // Create singleton objects
  for (var i=0; i<this.types.length; ++i)
  {
    var type = this.types[i];
    var singleton = type.create_object();
    type.singleton = singleton;
    this.singletons[i] = singleton;
  }
}
