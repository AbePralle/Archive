function BardLoader( vm, base64_bc )
{
    this.data     = base64_bc;
    this.position = 0;
    this.x        = 0;
    this.y        = 0;
    this.z        = 0;
    this.buffered = 0;
    this.vm       = vm;
}

BardLoader.prototype.load = function()
{
  this.discard(8);     // "BARDCODE"
  this.read_integer(); // 0x20130601

  // Load code chunk
  var code_size = this.read_integer();
  var vm = this.vm;

  vm.code = new Int32Array(code_size);
  var code = vm.code;
  for (var i=0; i<code_size; ++i)
  {
    code[i] = this.read_integer();
  }

  // Load identifiers
  var identifier_count = this.read_integer();
  vm.identifiers = [];
  for (var i=0; i<identifier_count; ++i)
  {
    vm.identifiers[i] = this.read_id();
  }

  // Load type declarations
  var type_count = this.read_integer();
  vm.types = [];
  vm.type_lookup = {};

  // First create all the type objects
  for (var i=0; i<type_count; ++i) vm.types[i] = new BardType(vm);

  // Now load in basic type info
  for (var i=0; i<type_count; ++i)
  {
    var type = vm.types[i];
    type.name = this.read_indexed_id();
    type.attributes = this.read_integer();

    var tag_count = this.read_integer();
    if (tag_count)
    {
      type.tags = [];
      for (var j=0; j<tag_count; ++j)
      {
        type.tags[j] = this.read_indexed_id();
      }
    }

    // Type Base Types
    var base_type_count = this.read_integer();
    if (base_type_count)
    {
      type.base_types = [];
      for (var j=0; j<base_type_count; ++j)
      {
        type.base_types[j] = this.read_indexed_type();
      }
    }

    // Type Settings
    var settings_count = this.read_integer();
    if (settings_count)
    {
      type.settings = [];
      for (var j=0; j<settings_count; ++j)
      {
        var setting_name = this.read_indexed_id();
        var setting_type = this.read_indexed_type();
        type.settings[j] = new BardProperty( setting_name, setting_type );
      }
    }

    // Type Properties
    var properties_count = this.read_integer();
    if (properties_count)
    {
      type.properties = [];
      for (var j=0; j<properties_count; ++j)
      {
        var property_name = this.read_indexed_id();
        var property_type = this.read_indexed_type();
        type.properties[j] = new BardProperty( property_name, property_type );
      }
    }

    vm.type_lookup[type.name] = type;
  }

  // Main class
  vm.main_class = this.read_indexed_type();

  vm.set_sizes_and_offsets();

  // Method Definitions
  var method_count = this.read_integer();
  vm.methods = [];
  for (var i=0; i<method_count; ++i)
  {
    var m = new BardMethod(vm);
    vm.methods[i] = m;

    m.type_context = this.read_indexed_type();
    m.name = this.read_indexed_id();
    m.parameter_count = this.read_integer();
    m.return_type = this.read_indexed_type();

    m.local_count = this.read_integer();
    m.local_types = [];
    for (var j=0; j<m.local_count; ++j)
    {
      m.local_types[j] = this.read_indexed_type();
    }

    //document.write( "Loaded " );
    //document.write( m.type_context.name );
    //document.write( "::" );
    //document.write( m.name );
    //document.write( "(" );
    //for (var j=0; j<m.parameter_count; ++j)
    //{
    //  if (j > 0) document.write(',');
    //  document.write(m.local_types[j].name);
    //}
    //document.write( ")" );
    //if (m.return_type)
    //{
    //  document.write( "->" );
    //  document.write( m.return_type.name );
    //}
    //document.writeln("<br>");

    m.ip_start = this.read_integer();
    m.ip_limit = this.read_integer();
    m.ip = m.ip_start - 1;
    m.line_info_index = this.read_integer();

    m.attributes = this.read_integer();
    var tag_count = this.read_integer();
    if (tag_count)
    {
      m.tags = [];
      for (var j=0; j<tag_count; ++j)
      {
        m.tags[j] = this.read_indexed_id();
      }
    }

    // Exception Handler Table
    m.exception_handler_count = this.read_integer();
    if (m.exception_handler_count)
    {
      m.exception_handlers = [];
      for (var j=0; j<exception_handler_count; ++j)
      {
        var info = {};
        m.exception_handlers[j] = info;

        info.ip_start = this.read_integer();
        info.ip_limit = this.read_integer();
        info.catch_count = this.read_integer();
        if (info.catch_count)
        {
          info.catches = [];
          for (var k=0; k<info.catch_count; ++k)
          {
            var cur_catch = {};
            info.catches[k] = cur_catch;
            cur_catch.catch_type = this.read_indexed_type();
            cur_catch.handler_ip = this.read_integer() - 1;
            cur_catch.local_slot_index = this.read_integer();
          }
        }
      }
    }

    m.init();
  }

  // Per-type method tables
  type_count = this.read_integer();
  for (var i=0; i<type_count; ++i)
  {
    var type = vm.types[i];

    // Method List
    var method_count = this.read_integer();
    if (method_count)
    {
      type.methods = [];
      for (var j=0; j<method_count; ++j) type.methods[j] = this.read_indexed_method();
    }

    // Aspect Tables
    type.aspect_count = this.read_integer();
    if (type.aspect_count)
    {
      type.aspect_types = [];
      type.aspect_call_tables = [];
      for (var j=0; j<type.aspect_count; ++j)
      {
        type.aspect_types[j] = this.read_indexed_type();
        type.aspect_call_tables[j] = [];
        var m_count = this.read_integer();
        for (var k=0; k<m_count; ++k)
        {
          type.aspect_call_tables[j][k] = this.read_indexed_method();
        }
      }
    }
  }

  // Real literals
  var real_count = this.read_integer();
  vm.literal_reals = new Float64Array( real_count );
  for (var i=0; i<real_count; ++i)
  {
    vm.literal_reals[i] = this.read_real();
  }

  // Filenames
  var filename_count = this.read_integer();
  vm.filenames = [];
  for (var i=0; i<filename_count; ++i)
  {
    vm.filenames[i] = this.read_id();
  }

  // Line info
  var line_info_count = this.read_integer();
  if (line_info_count)
  {
    vm.line_info = new Int8Array( line_info_count );
    for (var i=0; i<line_info_count; ++i)
    {
      vm.line_info[i] = this.read_byte();
    }
  }

  vm.organize();

  // Literal strings
  var literal_string_count = this.read_integer();
  if (literal_string_count)
  {
    vm.literal_strings = [];
    for (var i=0; i<literal_string_count; ++i)
    {
      var count = this.read_integer();
      var characters = new Uint16Array( count );
      for (var j=0; j<count; ++j)
      {
        characters[j] = this.read_integer();
      }
      vm.literal_strings[i] = vm.create_string( characters );
    }

    for (var i=0; i<vm.literal_strings.length; ++i)
    {
    }
  }

  vm.create_singletons();
};

BardLoader.prototype.discard = function(n)
{
  while (--n >= 0) this.read_byte();
};

BardLoader.prototype.read_digit = function(i)
{
  var digit = this.data.charCodeAt(i);
  if (digit == 43)  return 62; // +
  if (digit == 47)  return 63; // /
  if (digit <= 57)  return (digit - 48) + 52; // 0..9
  if (digit <= 90)  return (digit - 65);      // A..Z
  if (digit <= 122) return (digit - 97) + 26; // a..z
};

BardLoader.prototype.read_byte = function()
{
  if (!this.buffered)
  {
    var i = this.position;
    var a = this.read_digit(i);
    var b = this.read_digit(i+1);
    var c = this.read_digit(i+2);
    var d = this.read_digit(i+3);
    this.position += 4;

    x = (a<<2) | (b>>>4);
    y = ((b<<4) | (c>>>2)) & 255;
    z = ((c<<6) | d) & 255;

    this.buffered = 3;
  }

  switch (this.buffered--)
  {
    case 3: return x;
    case 2: return y;
    case 1: return z;
  }
};

BardLoader.prototype.read_integer = function()
{
  // 0: 0xxxxxxx - 0..127
  // 1: 1000xxxx - 1 byte follows (12 bits total, unsigned)
  // 2: 1001xxxx - 2 bytes follow (20 bits total, unsigned)
  // 3: 1010xxxx - 3 bytes follow (28 bits total, unsigned)
  // 4: 10110000 - 4 bytes follow (32 bits total, signed)
  // 5: 11xxxxxx - -64..-1
  var b1 = this.read_byte();

  if (b1 < 128)  return b1;          // encoding 0
  if (b1 >= 192) return (b1 - 256);  // encoding 5

  switch (b1 & 0x30)
  {
    case 0:  // encoding 1
      return ((b1 & 15) << 8) | this.read_byte();

    case 16:  // encoding 2
      {
        var b2 = this.read_byte();
        var b3 = this.read_byte();
        return ((b1 & 15) << 16) | (b2 << 8) | b3;
      }

    case 32:  // encoding 3
      {
        var b2 = this.read_byte();
        var b3 = this.read_byte();
        var b4 = this.read_byte();
        return ((b1 & 15) << 24) | (b2 << 16) | (b3 << 8) | b4;
      }

    case 48:  // encoding 4
      {
        var result = this.read_byte();
        result = (result << 8) | this.read_byte();
        result = (result << 8) | this.read_byte();
        result = (result << 8) | this.read_byte();
        return result;
      }

    default:
      return -1;
  }
};

BardLoader.prototype.read_id = function()
{
  var result = "";
  var count = this.read_integer();
  for (var i=0; i<count; ++i) result += String.fromCharCode( this.read_integer() );
  return result;
};

BardLoader.prototype.read_indexed_id = function()
{
  return this.vm.identifiers[ this.read_integer() ];
};

BardLoader.prototype.read_indexed_type = function()
{
  var index = this.read_integer();
  if (index == -1) return null;
  return this.vm.types[ index ];
};

BardLoader.prototype.read_indexed_method = function()
{
  var index = this.read_integer();
  if (index == -1) return null;
  return this.vm.methods[ index ];
};

BardLoader.prototype.read_real = function()
{
  var high_bits = this.read_integer();
  var low_bits = this.read_integer();
  var conversion_bytes = this.vm.conversion_bytes;
  if (this.vm.low_high_byte_order)
  {
    conversion_bytes[0] = low_bits;
    conversion_bytes[1] = low_bits >> 8;
    conversion_bytes[2] = low_bits >> 16;
    conversion_bytes[3] = low_bits >> 24;
    conversion_bytes[4] = high_bits;
    conversion_bytes[5] = high_bits >> 8;
    conversion_bytes[6] = high_bits >> 16;
    conversion_bytes[7] = high_bits >> 24;
  }
  else
  {
    conversion_bytes[7] = low_bits;
    conversion_bytes[6] = low_bits >> 8;
    conversion_bytes[5] = low_bits >> 16;
    conversion_bytes[4] = low_bits >> 24;
    conversion_bytes[3] = high_bits;
    conversion_bytes[2] = high_bits >> 8;
    conversion_bytes[1] = high_bits >> 16;
    conversion_bytes[0] = high_bits >> 24;
  }
  return this.vm.conversion_reals[0];
};
