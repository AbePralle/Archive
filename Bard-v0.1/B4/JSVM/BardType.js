function BardType( vm )
{
  this.vm = vm;
}

BardType.prototype.organize = function()
{
  this.m_init_defaults = this.find_method( "init_defaults()" );
}

BardType.prototype.find_property = function( name )
{
  var properties = this.properties;
  if (properties)
  {
    for (var i=0; i<properties.length; ++i)
    {
      var property = properties[i];
      if (property.name == name) return property;
    }
  }
  return null;
};

BardType.prototype.find_method = function( signature )
{
  var call_signature, return_type_name = null;
  if (signature.charCodeAt(signature.length-1) == 41)  // ')'
  {
    call_signature = signature;
  }
  else
  {
    var arrow_index = signature.lastIndexOf(")->") + 1;
    call_signature = signature.substr( 0, arrow_index );
    return_type_name = signature.substr( arrow_index+2 );
  }

  var methods = this.methods;
  var count = methods.length;
  for (var i=0; i<count; ++i)
  {
    var m = methods[i];
    if (m.call_signature == call_signature)
    {
      if ( !return_type_name || return_type_name == m.return_type.name ) return m;
    }
  }

  return null;
};

BardType.prototype.is_class = function()     { return ((this.attributes & BardAttributes.TYPE_MASK) == BardAttributes.CLASS);     }
BardType.prototype.is_aspect = function()    { return ((this.attributes & BardAttributes.TYPE_MASK) == BardAttributes.ASPECT);    }
BardType.prototype.is_reference = function() { return ((this.attributes & BardAttributes.TYPE_MASK) <= BardAttributes.ASPECT);    }
BardType.prototype.is_primitive = function() { return ((this.attributes & BardAttributes.TYPE_MASK) == BardAttributes.PRIMITIVE); }
BardType.prototype.is_compound = function()  { return ((this.attributes & BardAttributes.TYPE_MASK) == BardAttributes.COMPOUND);  }
BardType.prototype.is_native = function()    { return  (this.attributes & BardAttributes.NATIVE);       }
BardType.prototype.is_singleton = function() { return  (this.attributes & BardAttributes.SINGLETON);    }

BardType.prototype.set_slot_count = function()
{
  var vm = this.vm;
  if (this.slot_count) return;  // already set via recursion

  if (this.is_compound())
  {
    // Recursively set the stack slot count for each element while collecting the total slot count.
    this.slot_count = 1; // prevent recursion
    var slot_count = 0;  // working value
    for (var i=0; i<this.properties.length; ++i)
    {
      var property_type = this.properties[i].type
      property_type.set_slot_count();
      slot_count += property_type.slot_count;
    }
    this.slot_count = slot_count;
  }
  else
  {
    this.slot_count = 1;
  }
};

BardType.prototype.configure_settings = function()
{
  var settings = this.settings;
  if ( !settings ) return;

  for (var i=0; i<settings.length; ++i)
  {
    var setting = settings[i];
    setting.data = [];
    setting.init_default( setting.data, 0 );
  }
};

BardType.prototype.set_property_offsets = function()
{
  var properties = this.properties;
  if ( !properties ) return;

  var cur_offset = 0;
  for (var i=0; i<properties.length; ++i)
  {
    var property = properties[i];
    property.offset = cur_offset;
    cur_offset += property.type.slot_count;
  }
};

BardType.prototype.create_object = function()
{
  var result = new BardObject( this );
  return result;
}
